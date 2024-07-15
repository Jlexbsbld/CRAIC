from typing import List
from utils_tools import preprocess
from paddle.inference import Config, create_predictor
from paddle.inference import PrecisionType

import cv2
import numpy as np
import glob, time
import os, sys

# 添加上两层目录
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
#from ernie_bot import HumAttrPrompt

def get_current_dir():
    current_dir = os.path.dirname(os.path.abspath(__file__))  # 返回绝对路径
    return current_dir


# 多目标跟踪（MOT）：单目标跟踪（VOT/SOT）、目标检测（detection）、行人重识别（Re-ID）
# ①检测 ②特征提取、运动预测 ③相似度计算 ④数据关联。
# SORT作为一个粗略的框架，核心就是两个算法：卡尔曼滤波和匈牙利匹配。
# DeepSORT的优化主要就是基于匈牙利算法里的这个代价矩阵。它在IOU Match之前做了一次额外的级联匹配，利用了外观特征和马氏距离。
#
#
class DetectResult:
    def __init__(self, category_id, label, score, bbox, object_id=0) -> None:
        self.class_id = int(category_id)
        self.object_id = int(object_id)
        self.label_name = label
        self.score = float(score)  # score
        self.bbox = bbox.astype(np.int32)

        # 当前bbox中心
        self.center = [self.bbox[0] + self.bbox[2] / 2, self.bbox[1] + self.bbox[3] / 2]
        # 整个图片中心
        self.middle = [320, 240]

    def pos_from_center(self):
        error_x = self.center[0] - self.middle[0]
        error_y = self.center[1] - self.middle[1]
        return [error_x, error_y]

    def get_pos(self):
        error_x = (self.bbox[0] + self.bbox[2]) / 2 - self.middle[0]
        error_y = (self.bbox[1] + self.bbox[3]) / 2 - self.middle[1]
        return [error_x, error_y]

    def tolist(self):
        return [self.class_id, self.object_id, self.label_name, self.score] + self.bbox.tolist()

    # 归一化结果
    def tolist_nomoralize(self, size):
        mid_x = size[0] / 2
        mid_y = size[1] / 2
        pt_mid = [mid_x, mid_y]
        # 归一化中心值
        normalized_x = float((self.bbox[0] + self.bbox[2]) / 2 - pt_mid[0]) / pt_mid[0]
        normalized_y = float((self.bbox[1] + self.bbox[3]) / 2 - pt_mid[1]) / pt_mid[1]
        normalized_w = float(self.bbox[2] - self.bbox[0]) / pt_mid[0]
        normalized_h = float(self.bbox[3] - self.bbox[1]) / pt_mid[1]
        return [self.class_id, self.object_id, self.label_name, self.score] + [normalized_x, normalized_y, normalized_w,
                                                                               normalized_h]

    def pos_from_pos(self, pos):
        error_x = self.center[0] - pos[0]
        error_y = self.center[1] - pos[1]
        return [error_x, error_y]

    def __repr__(self) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return "cls_id:{} obj_id:{} label:{} score:{:.3f} bbox:{}".format(
            self.class_id, self.object_id, self.label_name, self.score, self.bbox
        )


class InferInterface:
    def __init__(self, model_dir: str) -> None:
        self.model_dir = os.path.join(get_current_dir(), model_dir)  # 连接路径
        self.predictor = None

    def get_model_path(self):
        model_path = glob.glob(self.model_dir + "/*.pdmodel")[0]
        params_path = glob.glob(self.model_dir + "/*.pdiparams")[0]
        return model_path, params_path

    def get_path_abs(self, path_relative):
        return os.path.join(get_current_dir(), path_relative)

    def load_cfg(self):
        yml_path = os.path.join(self.model_dir, "infer_cfg.yml")
        with open(yml_path) as f:
            import yaml
            yml_conf = yaml.safe_load(f)
            self.threshold = yml_conf['draw_threshold']
            self.label_list = yml_conf['label_list']

    def __call__(self, *args, **kwds):
        return self.predict(*args, **kwds)

    def predict(self, image, normalize_out=False, visualize=False):
        pass

    def close(self):
        pass


class LaneInfer(InferInterface):
    def __init__(self, model_lane, pptrt = False) -> None:
        #model_lane = "lane_model"
        super().__init__(model_lane)
        model_path, params_path = self.get_model_path()
        self.config = Config(model_path, params_path)

        self.config.enable_use_gpu(1000, 0)
        self.config.switch_ir_optim()
        self.config.enable_memory_optim()
        if pptrt == True:
            self.config.enable_tensorrt_engine(workspace_size=1 << 30, precision_mode=PrecisionType.Float32, max_batch_size=1,
                                           min_subgraph_size=5, use_static=True, use_calib_mode=False)
        self.predictor = create_predictor(self.config)

        self.img_size = (128, 128)
        self.mean = np.array([1.0, 1.0, 1.0])

        self.std = None

    # 归一化处理
    def normalize(self, img):
        img = img.astype(np.float32) / 127.5
        if self.mean is not None:
            img -= self.mean
        if self.std is not None:
            img /= self.std
        # img = (img - self.mean) / self.std
        # 为什么上面的前两个可以，后面的那个不行,
        # 下面这个是操作后赋值了新的变量，前面的是变量没有更改
        return img

    def preprocess(self, img):
        # 更改分辨率
        img = cv2.resize(img, self.img_size)
        img = self.normalize(img)
        # img = self.resize(img, self.img_size)
        # bgr-> rgr
        img = img[:, :, ::-1].astype('float32')  # bgr -> rgb
        img = img.transpose((2, 0, 1))  # hwc -> chw
        return img[np.newaxis, :]

    def predict(self, img, normalize_out=False):
        # copy img data to input tensor
        img = self.preprocess(img)
        input_names = self.predictor.get_input_names()

        input_tensor = self.predictor.get_input_handle(input_names[0])
        # input_tensor.reshape(img.shape)
        input_tensor.copy_from_cpu(img)
        # do the inference
        self.predictor.run()
        # get out data from output tensor
        output_names = self.predictor.get_output_names()

        output_tensor = self.predictor.get_output_handle(output_names[0])
        output_data = output_tensor.copy_to_cpu()[0]
        if normalize_out:
            output_data = output_data.tolist()
        return output_data

class YoloeInfer(InferInterface):
    def __init__(self, model_dir, pptrt=False, run_mode="paddle") -> None:
        # model_dir = "task_model3"
        super().__init__(model_dir)
        print(model_dir, run_mode)

        model_path, params_path = self.get_model_path()
        #yaml_path = glob.glob(self.model_dir + "/*.yml")[0]
        self.config = Config(model_path,params_path)
        self.config.switch_ir_optim()
        self.config.enable_memory_optim()
        self.config.enable_use_gpu(1000,0)
        if pptrt == True:
            self.config.enable_tensorrt_engine(workspace_size=1 << 30, max_batch_size=1, min_subgraph_size=5,
                                        precision_mode=PrecisionType.Float32, use_static=True, use_calib_mode=False)
            #self.config.enable_tensorrt_dla(0)
        self.predictor = create_predictor(self.config)
        self.img_size = 256
        self.im_shape = np.array([self.img_size, self.img_size]).reshape((1, 2)).astype(np.float32)

        self.scale_factor = (np.array([self.img_size * 1.0 / 480, self.img_size * 1.0 / 640]).reshape(
            (1, 2)).astype(np.float32))
        #super().load_cfg()

    def predict(self, image, normalize_out=False) -> List[DetectResult]:
        #image = preprocess(image, self.img_size)
        #input_names = self.predictor.get_input_names()
        #input_info = {}
        #input_info['image'] = image  # 输入图片的numpy格式
        #input_info['scale_factor'] = np.array([1., 1.], dtype=np.float32)
        #for i in range(len(input_names)):
        #   input_tensor = self.predictor.get_input_handle(input_names[i])
        #   input_tensor.copy_from_cpu(input_info[input_names[i]])

        data = preprocess(image, self.img_size)
        #print(image.shape[0])
        #print(image.shape[1])
        image = data
        #image = [self.im_shape, data, self.scale_factor]
        input_names = self.predictor.get_input_names()
        for i, name in enumerate(input_names):
            input_tensor = self.predictor.get_input_handle(name)
            input_tensor.reshape(image[i].shape)
            input_tensor.copy_from_cpu(image[i])

        #image = preprocess(image, self.img_size)
        #input_names = self.predictor.get_input_names()
        #input_tensor = self.predictor.get_input_handle(input_names[0])
        #input_tensor.reshape(image.shape)
        #input_tensor.copy_from_cpu(image)

        self.predictor.run()

        results = []
        output_names = self.predictor.get_output_names()
        for i, name in enumerate(output_names):
            output_tensor = self.predictor.get_output_handle(name)
            output_data = output_tensor.copy_to_cpu()
            results.append(output_data)
        return results

    def close(self):
        super.close()



def cam_infer_test():
    cap = cv2.VideoCapture(1)
    infer = LaneInfer()
    time_start = time.time()
    while True:
        ret, img = cap.read()
        if ret:
            response = infer(img)
            print(response)
            fps = 1 / (time.time() - time_start)
            print("fps:", fps)
            time_start = time.time()
            # cv2.putText(img, "fps:{}".format(fps), (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            # cv2.imshow("img", img)
            key = cv2.waitKey(10)
            if key == ord('q'):
                break

