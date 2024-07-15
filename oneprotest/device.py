import struct
import serial as Serial
import config
import time
import math

# port = config.PORT
# 配置端口和比特率
port = config.PORT
bps = config.BPS
# 设置好相应的端口波特率和停止位，读超时时间设置，无校验位，停止位1bit
serial = Serial.Serial(port, int(bps), timeout=1, parity=Serial.PARITY_NONE, stopbits=1)

# 定义数据发送函数，传入数据和延迟时间
def send(data, Time):
    # 将数据转成ASCII码的形式发送
    serial.write(data)
    # 等待数据发送完毕
    serial.flush()
    # 等待下位机对数据做出处理
    time.sleep(Time)

# 这个电机是传动结构用来将机械臂推出
class motor:
    def __init__(self, port):
        self.comma_head = bytes.fromhex('33 01') #选择电机
        self.comma_trail = bytes.fromhex('61')
        self.port_str = '{:02x}'.format(port)

    def shoot(self, speed, Time):
        # tobytes 占1个字节，大端(大端表示高位在前，小端表示低位在前)，有符号
        data = self.comma_head + bytes.fromhex("00 {}",format(self.port_str)) + speed.to_bytes(2, byteorder='big', signed=True) + self.comma_trail
        #serial.write(data)
        #serial.flush()
        send(data,Time)

class Light:
    def __init__(self):
        pass
    # 灯光工作发送对应指令
    def work(self, color):
        if color == 'red':
            self.data = bytes.fromhex('33 03 0D 00 00 61')
        elif color == 'yellow':
            self.data = bytes.fromhex('33 03 0E 00 00 61')
        elif color == 'green':
            self.data = bytes.fromhex('33 03 0F 00 00 61')
        elif color == 'blue':
            self.data = bytes.fromhex('33 03 10 00 00 61')
        elif color == 'white':
            self.data = bytes.fromhex('33 03 11 00 00 61')
        elif color == 'black':
            self.data = bytes.fromhex('33 03 12 00 00 61')
        send(self.data, config.sleep_time)

class Beep:
    def __init__(self):
        pass
    # 蜂鸣器工作发送对应指令
    def work(self, state):
        if state == "ON":
            self.data = bytes.fromhex('33 04 12 01 00 61')
        elif state == "OFF":
            self.data = bytes.fromhex('33 04 12 00 00 61')
        send(self.data, config.sleep_time)

class Button:
    def __init__(self):
        pass
    # 按到下位机的按键回传给上位机
    def clicked(self):
        response = serial.read() # 从串口读一个字节的数据
        # print(response)
        # print(len(response))
        buttonclick = "no" # 默认按键值为空
        button_value = response
        if len(response) == 1 : #长度为1个字节
            if button_value == bytes.fromhex("13"):
                buttonclick = "LEFT"
            elif button_value == bytes.fromhex("14"):
                buttonclick = "UP"
            elif button_value == bytes.fromhex("15"):
                buttonclick = "RIGHT"
            elif button_value == bytes.fromhex("16"):
                buttonclick = "DOWN"
        return buttonclick



class Servo:
    def __init__(self):
        pass
    def work(self, Servo, angle, kind):
        # 所有舵机的标号，传入的Servo与相应的舵机标号对应则发送对应的控制指令
        Servo_1 = 0x05
        Servo_2 = 0x06
        Servo_3 = 0x07
        Servo_4 = 0x08
        Servo_5 = 0x09
        Servo_6 = 0x0A
        Servo_7 = 0x0B
        Servo_8 = 0x0C
        # 角度占两个字节，高位在前，带符号
        if kind==0: # 180度舵机
            angle = angle
        elif kind==1: # 270度舵机
            angle = (int)(angle * config.Servo_270_k)
        angle = self.exchange(angle,kind)
        angle = angle.to_bytes(2, byteorder='big', signed=True)

        if Servo == Servo_1:
            data = bytes.fromhex('33 02 05') + angle + bytes.fromhex('61')
        elif Servo == Servo_2:
            data = bytes.fromhex('33 02 06') + angle + bytes.fromhex('61')
        elif Servo == Servo_3:
            data = bytes.fromhex('33 02 07') + angle + bytes.fromhex('61')
        elif Servo == Servo_4:
            data = bytes.fromhex('33 02 08') + angle + bytes.fromhex('61')
        elif Servo == Servo_5:
            data = bytes.fromhex('33 02 09') + angle + bytes.fromhex('61')
        elif Servo == Servo_6:
            data = bytes.fromhex('33 02 0A') + angle + bytes.fromhex('61')
        elif Servo == Servo_7:
            data = bytes.fromhex('33 02 0B') + angle + bytes.fromhex('61')
        elif Servo == Servo_8:
            data = bytes.fromhex('33 02 0C') + angle + bytes.fromhex('61')
        send(data, config.sleep_time)
    def exchange(self,angle,kind): #对舵机数据限幅
        if(kind == 1):# 270
            if(angle > 270):
                angle = 270
            elif(angle < 0):
                angle = 0
            else:
                angle = angle
        else:# 180
            if(angle > 180):
                angle = 180
            elif(angle < 0):
                angle = 0
            else:
                angle = angle
        return angle


class Cart:
    def __init__(self):
        # 速度
        self.velocity = config.VELOCITY
        self.Kx = config.KX  # 运动系数,修改可提速，本质改的是转弯角速度
        self.p = config.P  # 横向移动的速度系数
        self.kp = config.KP  # 目标对准的速度调整系数
        self.slow_ratio = config.RATIO  # 当angle大于一定值时，速度变小的系数
        self.slow_angle = 0.5  # 速度变慢的阈值
        self.back_max = config.back_speed
        '''
        # 麦轮
        self.wheel_R = config.Mec_wheel_R #麦轮的半径
        self.wheel_t = config.Mec_t # 车身的长一半
        self.wheel_b = config.Mec_b # 车身的宽一半
        self.angle_to_w = config.Mec_w
        self.PI = 3.1415926
        '''
        # 祖传延时
        time.sleep(1)

    # 十八届草稿

    def Run_steer(self, angle, mode=0):
        speed = int(self.velocity)
        if abs(angle) > self.slow_angle:
            speed = int(self.velocity*self.slow_ratio)
        # angle > 0 左转， angle < 0 右转
        leftfront = speed * 1.0
        leftrear = speed * 1.0
        rightfront = speed * 1.0
        rightrear = speed * 1.0

        k = self.Kx

        if angle > 0:
            leftfront = int(leftfront - angle * k)
            leftrear = int(leftrear - angle * k)
            rightfront = int(rightfront + angle * k)
            rightrear = int(rightrear + angle * k)
        elif angle < 0:
            leftfront = int(leftfront - angle * k)
            leftrear = int(leftrear - angle * k)
            rightfront = int(rightfront + angle * k)
            rightrear = int(rightrear + angle * k)
        else:
            pass
        back_wheel = -self.back_max*angle
        '''
        back_wheel = 0
        if angle > 0:
            back_wheel = -self.back_max*angle
        elif angle < 0:
            back_wheel = -self.back_max*angle
        '''
        # print(leftfront, leftrear, rightfront,rightrear)
        if mode == 0:
            self.move([self.exchange(leftfront), self.exchange(rightrear), self.exchange(-rightfront), self.exchange(-leftrear)])
        elif mode == 1:  # 右横向移动
            speed = self.velocity * self.p
            self.move([0, 0, -30, 0])
        elif mode == 2:  # 左横向移动
            speed = self.velocity * self.p
            self.move([0, 0, 30, 0])
        elif mode == 3:  # 前进，用于对准目标
            speed = self.velocity * self.kp
            self.move([speed, 0, 0, speed])
        else:  # 后退，用于对准目标
            speed = self.velocity * self.kp
            self.move([-speed, 0, 0, -speed])






    # 十七届祖传差速模型
    def steer(self,angle,mode = 0):
        speed = int(self.velocity)
        if abs(angle) > self.slow_angle:
            speed = int(self.velocity * self.slow_ratio)
        # angle = angle * 0.9
        angle = angle * self.Kx * -1
        delta = angle - 0

        leftwheel = speed*1.0
        rightwheel = speed*1.0
        leftwheel_back = speed*1.0
        rightwheel_back = speed*1.0

        scale = 1
        if (delta < 0):
            leftwheel = int((1 + delta * scale) * speed)
            rightwheel = int((1 - delta * scale) * speed)
            if rightwheel == speed:
                R = 1e4
                leftwheel_back = leftwheel
                rightwheel_back = rightwheel
                # print('equls!!!')
            else:
                R = speed * 0.1 / (rightwheel - speed)
                rl = R - 0.1
                rr = R + 0.1
                leftwheel_back = int(speed / R * math.sqrt(rl * rl + 0.0256))
                rightwheel_back = int(speed / R * math.sqrt(rr * rr + 0.0256))
        if (delta > 0):
            leftwheel = int((1 + delta * scale) * speed)
            rightwheel = int((1 - delta * scale) * speed)
            if leftwheel == speed:
                R = 1e4
                leftwheel_back = leftwheel
                rightwheel_back = rightwheel
                # print('equls!!!')
            else:
                R = speed * 0.1 / (leftwheel - speed)
                rl = R + 0.1
                rr = R - 0.1
                leftwheel_back = int(speed / R * math.sqrt(rl * rl + 0.0256))
                rightwheel_back = int(speed / R * math.sqrt(rr * rr + 0.0256))
        # print([leftwheel, rightwheel, leftwheel_back, rightwheel_back])
        kp = 3.6
        if mode == 0:
            self.move([self.exchange(leftwheel*0.8), self.exchange(rightwheel*1.2), self.exchange(leftwheel_back*0.8), self.exchange(rightwheel_back*1.2)])
        elif mode ==2:
            self.move([-leftwheel, -rightwheel, -leftwheel_back, -rightwheel_back])
        elif mode ==3:
            self.move([leftwheel/kp, rightwheel/kp, leftwheel/kp, rightwheel/kp])
        elif mode ==4:
            self.move([-leftwheel/kp, -rightwheel/kp, -leftwheel/kp, -rightwheel/kp])
        else:
            self.move([leftwheel, rightwheel, leftwheel, rightwheel])

    def exchange(self, speed):  # 速度限幅下位机也有限幅，不过为了防止指令数据转换出错还是要加
        if speed > 100:
            speed = 100
        elif speed < -100:
            speed = -100
        else:
            speed = speed
        return speed

    def move(self, speeds):
        left_front = int(speeds[0])
        right_front = int(speeds[1])
        left_rear = int(speeds[2])
        right_rear = int(speeds[3])
        self.min_speed = int(min(speeds))
        # 定义帧头和帧尾
        comma_head = bytes.fromhex('33 01') # 选择电机
        comma_trail = bytes.fromhex('61')
        # 撰写数据帧
        left_front_kl = comma_head + bytes.fromhex('01 00') + left_front.to_bytes(1, byteorder='big', signed=True) + comma_trail
        right_front_kl = comma_head + bytes.fromhex('02 00') + right_front.to_bytes(1, byteorder='big', signed=True) + comma_trail
        left_rear_kl = comma_head + bytes.fromhex('03 00') + left_rear.to_bytes(1, byteorder='big', signed=True) + comma_trail
        right_rear_kl = comma_head + bytes.fromhex('04 00') + right_rear.to_bytes(1, byteorder='big', signed=True) +comma_trail

        All_kl = left_front_kl + right_front_kl + left_rear_kl + right_rear_kl

        # 发送适当延时防止下位机未来得及处理
        '''
        send(left_front_kl, config.sleep_time)
        send(right_front_kl, config.sleep_time)
        send(left_rear_kl, config.sleep_time)
        send(right_rear_kl, config.sleep_time)
        '''
        send(All_kl, config.sleep_time)

    # 停车
    def stop(self):
        self.move([0, 0, 0, 0])
        # serial.close()
        # time.sleep(0.01)
        # serial.open()

if __name__ == "__main__":
    car = Cart()
    car.steer(0)
    time.sleep(3)
    car.stop()
