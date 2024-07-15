import serial as ser
import time
import yaml
import os
import config

# path_dir = os.path.abspath(os.path.dirname(__file__))
# yaml_path = os.path.join(path_dir, "config_communication.yaml")
#
# with open(yaml_path, 'r') as stream:
#     config = yaml.safe_load(stream)
#     # print(config)
#     port = config["PORT"]
#     bps = config["BPS"]
#     velocity = config["velocity"]
#     kx = config["kx"]

port = config.PORT
bps = config.BPS
velocity = config.velocity
kx = config.kx

# 配置端口和比特率
# port = config.PORT
# bps = config.BPS
# 设置好相应的端口波特率和停止位，读超时时间设置，无校验位，停止位1bit
serial = ser.Serial(port, int(bps), timeout=1, parity=ser.PARITY_NONE, stopbits=1)


# 定义数据发送函数，传入数据和延迟时间
def send(data, w_time):
    # 将数据转成ASCII码的形式发送
    serial.write(data)
    # 等待数据发送完毕
    serial.flush()
    # 等待下位机对数据做出处理
    time.sleep(w_time)


# 计算校验和
def getCheckSum(frame):
    return sum(frame) & 0xff


instructionMode = {
    'Asynchronous': 1,
    'Synchronous': 2
}

'''
class Motor:
    def __init__(self, port):
        self.comma_head = bytes.fromhex('33')  # 帧头
        self.comma_tail = bytes.fromhex('61')  # 帧尾
        self.sort = 1  # 选择控制电机
        self.port = port  # 选择哪个电机
        self.length = 8  # 帧长度
        self.temp = []  # 用于计算校验和
        self.checksum = 0  # 定义校验和
        self.instructionMode = 'Asynchronous'  # 指令模式

    def work(self, speed):
        # to_bytes 占几个字节，大端(大端表示高位在前，小端表示低位在前)，有无符号
        self.instructionMode = mode2num(self.instructionMode)
        self.temp = [self.length, self.sort, self.instructionMode, self.port, speed]
        checksum = getCheckSum(self.temp)

        frame = self.comma_head + \
                self.length.to_bytes(1, byteorder='big', signed=False) + \
                self.sort.to_bytes(1, byteorder='big', signed=False) + \
                self.instructionMode.to_bytes(1, byteorder='big', signed=False) + \
                self.port.to_bytes(1, byteorder='big', signed=False) + \
                speed.to_bytes(1, byteorder='big', signed=True) + \
                checksum.to_bytes(1, byteorder='big', signed=False) + \
                self.comma_tail
        send(frame, config.sleep_time)
'''


class Cart:

    def __init__(self):
        self.comma_head = bytes.fromhex('33')  # 帧头
        self.comma_tail = bytes.fromhex('61')  # 帧尾
        self.sort = 1  # 选择控制电机
        self.length = None  # 帧长度
        self.temp = []  # 用于计算校验和以及帧长
        self.checksum = 0  # 定义校验和
        self.velocity = velocity
        self.kx = kx
        self.instructionMode = 'Synchronous'  # 指令模式
        self.Check_mode = 'NotCheck'  # 支持 Check 和 NotCheck

    def Run_steer(self, angle):
        speed = self.velocity
        # angle > 0 左转， angle < 0 右转
        lf_speed = speed * 1.0
        lr_speed = speed * 1.0
        rf_speed = speed * 1.0
        rr_speed = speed * 1.0
        k = self.kx

        lf_speed = int(lf_speed - angle * k)
        lr_speed = int(lr_speed - angle * k)
        rf_speed = int(rf_speed + angle * k)
        rr_speed = int(rr_speed + angle * k)

        # if angle >= 0:
        #     lf_speed = int(lf_speed - angle * k)
        #     lr_speed = int(lr_speed - angle * k)
        #
        # else:
        #     rf_speed = int(rf_speed + angle * k)
        #     rr_speed = int(rr_speed + angle * k)
        # print(lf_speed, lr_speed, rf_speed, rr_speed)
        self.move([lf_speed, lr_speed, rf_speed, rr_speed])

    def stop(self):
        self.move([0, 0, 0, 0])

    def move(self, speeds):
        left_front = int(speeds[0])
        left_rear = int(speeds[1])
        right_front = int(speeds[2])
        right_rear = int(speeds[3])
        # 撰写数据帧
        # self.instructionMode = instructionMode[self.instructionMode]
        self.instructionMode = 2
        self.temp = [self.length, self.sort, self.instructionMode, left_front, right_front,
                     left_rear, right_rear, self.comma_head, self.comma_tail]
        self.length = len(self.temp)
        # 计算校验和
        if self.Check_mode == 'Check':
            self.temp = [self.length, self.sort, self.instructionMode, left_front, right_front,
                         left_rear, right_rear]
            checksum = getCheckSum(self.temp)
            frame = self.comma_head + \
                self.length.to_bytes(1, byteorder='big', signed=False) + \
                self.sort.to_bytes(1, byteorder='big', signed=False) + \
                self.instructionMode.to_bytes(1, byteorder='big', signed=False) + \
                left_front.to_bytes(1, byteorder='big', signed=True) + \
                left_rear.to_bytes(1, byteorder='big', signed=True) + \
                right_front.to_bytes(1, byteorder='big', signed=True) + \
                right_rear.to_bytes(1, byteorder='big', signed=True) + \
                checksum.to_bytes(1, byteorder='big', signed=False) + \
                self.comma_tail
        else:
            frame = self.comma_head + \
                self.length.to_bytes(1, byteorder='big', signed=False) + \
                self.sort.to_bytes(1, byteorder='big', signed=False) + \
                self.instructionMode.to_bytes(1, byteorder='big', signed=False) + \
                left_front.to_bytes(1, byteorder='big', signed=True) + \
                left_rear.to_bytes(1, byteorder='big', signed=True) + \
                right_front.to_bytes(1, byteorder='big', signed=True) + \
                right_rear.to_bytes(1, byteorder='big', signed=True) + \
                self.comma_tail
        # print(frame)
        # 发送适当延时防止下位机未来得及处理
        send(frame, config.sleep_time)


if __name__ == "__main__":
    myCar = Cart()
    # myCar.Run_steer(0)
    myCar.stop()

