import numpy as np
import cruiser_infer
import cv2 as cv
import sys
import os
import config
import time


def BGR2BW(img_BGR):
    lower_green = np.array([0, 125, 93])  # 绿色阈值下界[45, 50, 50]
    higher_green = np.array([62, 255, 255])  # 绿色阈值上界[90, 255, 255]
    if img_BGR is not None:
        hsv = cv.cvtColor(img_BGR, cv.COLOR_BGR2HSV)  # 转化成HSV图像
        inRange_hsv = cv.inRange(hsv, lower_green, higher_green)  # 阈值内的变成白色，提取绿色变白色
        # retval, dst = cv.threshold(inRange_hsv, 50, 255, cv.THRESH_BINARY_INV)  # 黑白反转 retval为阈值,白色变黑色
        dst = cv.cvtColor(inRange_hsv, cv.COLOR_GRAY2RGB)  # 转化为RGB三通道，会重复灰度图3次来得到一个3通道的图像，
        return dst
    else:
        return img_BGR


def deal(input):
    if len(input[0][0]) != 6:
        return 0, 0, [0, 0, 0, 0]
    else:
        max_index = 0
        max_confidence = 0
        out = np.array(input[0])
        length = len(out[:, 0])

        result_det = []
        for i in range(10):
            result_det.append([0,0])

        # for x in range(length):
        #     if out[x, 1] > config.task_confidence[config.label_task[out[x, 0]]] and out[x, 1] > max_confidence:
        #         max_index = x
        #         max_confidence = out[x, 1]
        for x in range(length):
            if out[x,1] > result_det[int(out[x,0])][0]:
                result_det[int(out[x,0])][0] = out[x,1]
                result_det[int(out[x,0])][1] = x
        for i in range(10):
            if result_det[i][0] <= config.task_confidence[config.label_task[i]]:
                result_det[i][1] = None
        check = 0
        for i in range(10):
            if result_det[i][1] != None:
                check+=1
                max_index = result_det[i][1]
        # print('check',check)
        # print('turnleft',result_det[8][0])
        # print('turnright', result_det[9][0])
        if check == 0:
            return 0, 0, [0, 0, 0, 0]
        res = (out[:, 0].astype('int32'))[max_index]

        # if out[max_index, 1] < config.task_confidence[config.label_task[res]]:  # 置信度小于阈值
        #     return 0, 0, [0, 0, 0, 0]

        xl = out[max_index, 2].astype('float32')
        xr = out[max_index, 4].astype('float32')
        yl = out[max_index, 3].astype('float32')
        yr = out[max_index, 5].astype('float32')

        xl = xl * 500
        xr = xr * 500
        yl = yl * 380 +100
        yr = yr * 380 +100

        box = [int(xl), int(yl), int(xr), int(yr)]
        max_confidence = out[max_index, 1]

        return res, max_confidence, box


def deal_normalization(input):
    if len(input[0][0]) != 6:
        return 0, 0, [0, 0, 0, 0]
    else:
        max_index = 0
        max_confidence = 0
        out = np.array(input[0])
        out_save = out.copy()
        length = len(out[:, 0])
        for x in range(length):
            out[x, 1] = out[x, 1] / config.task_confidence[config.label_task[out[x, 0]]]
        for x in range(length):
            if out[x, 1] > max_confidence:
                max_index = x
                max_confidence = out[x, 1]

        res = (out[:, 0].astype('int32'))[max_index]

        if out[max_index, 1] < 1:  # 置信度小于阈值
            return 0, 0, [0, 0, 0, 0]

        xl = out[max_index, 2].astype('float32')
        xr = out[max_index, 4].astype('float32')
        yl = out[max_index, 3].astype('float32')
        yr = out[max_index, 5].astype('float32')

        xl = xl * 640
        xr = xr * 640
        yl = yl * 480
        yr = yr * 480

        box = [int(xl), int(yl), int(xr), int(yr)]
        max_confidence_original = out_save[max_index, 1]

        return res, max_confidence_original, box


def CameraFRONT(queue_front,queue_front_infer):
    """前面摄像头读取图片"""
    stream_front = cv.VideoCapture(config.FRONT_CAMERA)  # 默认格式为BGR
    stream_front.set(cv.CAP_PROP_FRAME_WIDTH, 640)
    stream_front.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
    stream_front.set(cv.CAP_PROP_FPS, 30)
    stream_det = cv.VideoCapture(config.SIDE_CAMERA)  # 默认格式为BGR
    stream_det.set(cv.CAP_PROP_FRAME_WIDTH, 640)
    stream_det.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
    stream_det.set(cv.CAP_PROP_FPS, 30)
    lane_time = time.time()+0.01
    det_time = time.time()+0.16
    try:
        while True:
            if time.time() > lane_time:
                lane_time = time.time() + 0.01
                val, frame = stream_front.read()
                if val:
                    queue_front.put(frame)
            if time.time() > det_time:
                det_time = time.time() + 0.125
                val, frame = stream_det.read()
                if val:
                    queue_front_infer.put(frame)
            if queue_front.full():
                queue_front.get()
            if queue_front_infer.full():
                queue_front_infer.get()
    except KeyboardInterrupt:
        stream_front.release()
        sys.exit()
    except:
        stream_front.release()
        os.system('pkill python3')
    finally:
        print("carema front stop!")


def Indetify(queue_front,angle_message,start_flag_cruiser):
    cnt_lane = 0
    time_clear = time.time() + 1
    line = cruiser_infer.LaneInfer("lane_model", pptrt=True)  # 创建巡线类
    try:
        while True:
            if not queue_front.empty():
                frame = queue_front.get()
                if config.cruiser_black_flag:  # 黑白巡线
                    frame = BGR2BW(frame)
                angle_message.value = line(frame)
                start_flag_cruiser.value = 1
                cnt_lane += 1
                # print('angle_message.value',angle_message.value)
                if time.time() > time_clear:
                    time_clear = time.time() + 1
                    print("lane", cnt_lane)
                    cnt_lane = 0
    except KeyboardInterrupt:
        sys.exit()
    except:
        os.system('pkill python3')
    finally:
        print("front stop!")


def detect(queue_front_infer,front_sign_message,start_flag_detect):
    cnt_det = 0
    time_clear = time.time() + 1
    infer = cruiser_infer.YoloeInfer("MobileNetSSD", pptrt=True)
    try:
        while True:
            if not queue_front_infer.empty():
                frame = queue_front_infer.get()
                frame = frame[100:, :500]
                response = infer(frame)
                # print(response)
                start_flag_detect.value = 1
                try:
                    front_sign_message[0],front_sign_message[1],front_sign_message[2] = deal(response)
                except Exception as e:
                    print(e)
                    front_sign_message[0], front_sign_message[1], front_sign_message[2] = -1, -1, [0, 0, 0, 0]
                cnt_det += 1
                # print('front_sign_message',front_sign_message[0],front_sign_message[1],front_sign_message[2])
                if time.time() > time_clear:
                    time_clear = time.time() + 1
                    print("det", cnt_det)
                    cnt_det = 0
                # print(response)

    except KeyboardInterrupt:
        sys.exit()
    except:
        os.system('pkill python3')
    finally:
        print("front stop!")