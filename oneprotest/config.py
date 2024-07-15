PORT = "/dev/ttyUSB0"
# PORT = "COM3"
BPS = 115200
sleep_time = 0.01  # 0.01

# Drive
VELOCITY = 48   # 极限速度 50  45  50 60
RATIO = 0.8  # 弯道速率限制
P = 0.9  # 横向移动系数
KP = 0.33  # 前后移动系数
KX = 30 # 车的转弯系数，值越大越可以过大弯道 50  30  42 50
back_speed = 0  # 后轮最大速度  40  25

FRONT_CAMERA = 0  # 摄像头
SIDE_CAMERA = 1  # 摄像头

cruiser_black_flag = True  # 黑白图片巡线

landmark_list = {
    "background": 0,
    "greenlight": 1,
    "redlight": 2,
    "speedlimit": 3,
    "limitlifted": 4,
    "sidewalk": 5,
    "sidewalk2": 6,
    "stop": 7,
    "turnleft": 8,
    "turnright": 9
}

label_task = {
    0: "background",
    1: "greenlight",
    2: "redlight",
    3: "speedlimit",
    4: "limitlifted",
    5: "sidewalk",
    6: "sidewalk2",
    7: "stop",
    8: "turnleft",
    9: "turnright",
}
task_confidence = {
    "background": 10,
    "greenlight": 0.7,
    "redlight": 0.75,
    "speedlimit": 0.4,
    "limitlifted": 0.95,
    "sidewalk": 1.8,
    "sidewalk2": 0.5, # 0.9
    "stop": 0.8,
    "turnleft": 1.7, # 0.7
    "turnright": 0.5
}