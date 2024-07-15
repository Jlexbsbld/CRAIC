import multiprocessing as mp
import gather
import time
import sys
import os
import cv2 as cv
import device
import config
import cruiser_infer
import sys,select,termios,tty

settings = termios.tcgetattr(sys.stdin)
def getKey():
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
    else:
        key = ''
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


def main():
    spend = time.time()
    stream_front = cv.VideoCapture(config.FRONT_CAMERA)  # 默认格式为BGR
    stream_front.set(cv.CAP_PROP_FRAME_WIDTH, 640)
    stream_front.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
    stream_front.set(cv.CAP_PROP_FPS, 30)
    stream_det = cv.VideoCapture(config.SIDE_CAMERA)  # 默认格式为BGR
    stream_det.set(cv.CAP_PROP_FRAME_WIDTH, 640)
    stream_det.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
    stream_det.set(cv.CAP_PROP_FPS, 30)

    line = cruiser_infer.LaneInfer("lane_model", pptrt=True)  # 创建巡线类
    infer = cruiser_infer.YoloeInfer("MobileNetSSD", pptrt=True)
    cart = device.Cart()

    while True:
        val1, img1 = stream_front.read()
        val2, img2 = stream_det.read()
        if val1 and val2:
            break
    num = 100
    while num:
        _, _ = stream_front.read()
        _, _ = stream_det.read()
        num -= 1
    num = 20
    while num:
        line(img1)
        infer(img2)
        num -= 1
    front_sign_message = [0,0,[0,0,0,0]]

    save_front_message = -1
    speed_limit_flag = True
    red_stop_flag = False
    redlight_num = 0
    limit_num = 0
    time_identify = time.time()
    time_proc = 5
    state = None
    lane_time = time.time()
    det_time = time.time()

    time_clear = time.time() + 1
    lane = 0
    det = 0
    turn_flag = False
    print("spend_time",time.time()-spend)
    # cart.velocity = 40
    # cart.Kx = 40*0.875
    start_time = time.time()
    print_flag = False
    print_warning_time = time.time()
    print_warning_flag = False
    print_changeway_time = time.time()
    print_changeway_flag = False
    print("init success")
    while True:
    	key = getKey()
    	if key == " ":
    		break
    try:
        while True:
            if time.time() >= time_clear:
                # print("lane:",lane)
                # print("det:", det)
                lane = 0
                det = 0
                time_clear = time.time() + 1
            if time.time() >= lane_time:
                lane_time = time.time()+ 0.02
                _, img1 = stream_front.read()
                if config.cruiser_black_flag:  # 黑白巡线
                    img1 = gather.BGR2BW(img1)
                angle = line(img1)
                lane = lane + 1
                if not red_stop_flag:
                    cart.Run_steer(angle)
            if print_flag:
                if time.time() > print_changeway_time and not print_changeway_flag:
                    print("变道")
                    print_changeway_flag = True
                elif time.time() > print_warning_time and not print_warning_flag:
                    print("施工危险区域")
                    print_warning_flag = True
                    print_flag = False
            if time.time() >= det_time:
                det_time = time.time() + 0.16
                _, img2 = stream_det.read()
                cv.imshow("img",img2)
                cv.waitKey(1)
                img2 = img2[100:, :500]
                response = infer(img2)
                front_sign_message[0],front_sign_message[1],front_sign_message[2] = gather.deal(response)
                det = det + 1
                if time.time() > time_identify:
                    if limit_num == 0 and (front_sign_message[0] == config.landmark_list["speedlimit"] or save_front_message == config.landmark_list["speedlimit"] or front_sign_message[0] == config.landmark_list["limitlifted"] or save_front_message == config.landmark_list["limitlifted"]) :  # 限速
                        limit_num = 1
                        cart.velocity = int(config.VELOCITY * 0.7)
                        cart.Kx = int(cart.velocity * 0.75)
                        print("限速")
                        time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["limitlifted"] or save_front_message == config.landmark_list["limitlifted"]:  # 解除限速
                        cart.velocity = config.VELOCITY
                        cart.Kx = int(cart.velocity * 0.875)
                        speed_limit_flag = False
                        print("解除限速")
                        time_identify = time.time() + 2.25
                        save_front_message = -1
                    elif (front_sign_message[0] == config.landmark_list["turnleft"] or save_front_message == config.landmark_list["turnleft"]) and not turn_flag:  # 左转
                        # print("左转")
                        if (front_sign_message[2][2]-front_sign_message[2][0])*(front_sign_message[2][3]-front_sign_message[2][1]) > 2800:
                            print("左转")
                            state = "left"
                            config.task_confidence["turnleft"] = 1.2
                            config.task_confidence["turnright"] = 1.2
                            turn_flag = True
                            cart.Run_steer(1.0)
                            time.sleep(1.3)
                            time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif (front_sign_message[0] == config.landmark_list["turnright"] or save_front_message == config.landmark_list["turnright"]) and not turn_flag:  # 右转
                        # print("右转")
                        if (front_sign_message[2][2]-front_sign_message[2][0])*(front_sign_message[2][3]-front_sign_message[2][1]) > 3200:
                            print("右转")
                            state = "right"
                            config.task_confidence["turnleft"] = 1.2
                            config.task_confidence["turnright"] = 1.2
                            turn_flag = True
                            cart.Run_steer(-1.0)
                            time.sleep(1.3)
                            time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["stop"] or save_front_message == config.landmark_list["stop"]:
                        # print("终点")
                        if (front_sign_message[2][3] + front_sign_message[2][1]) // 2 > 350:
                            print("终点")
                            time_stop = time.time() + 2.5
                            next_time = time.time() + 0.03
                            while time.time()<time_stop:
                                if time.time() >= next_time:
                                    _, img1 = stream_front.read()
                                    if config.cruiser_black_flag:  # 黑白巡线
                                        img1 = gather.BGR2BW(img1)
                                    angle = line(img1)
                                    cart.Run_steer(angle)
                                    next_time = time.time() + 0.03
                            cart.stop()
                            save_front_message = -1
                            break
                    elif front_sign_message[0] == config.landmark_list["sidewalk2"] or save_front_message == config.landmark_list["sidewalk2"]:
                        # print("人行道2")
                        if state == "left":
                            if (front_sign_message[2][3]+front_sign_message[2][1])//2 > 325:
                                print("人行道2")
                                cart.stop()
                                time.sleep(2)
                                cart.velocity = config.VELOCITY
                                cart.Kx = config.KX
                                time_identify = time.time() + time_proc
                        else:
                            if (front_sign_message[2][3]+front_sign_message[2][1])//2 > 330:
                                print("人行道2")
                                cart.stop()
                                time.sleep(2)
                                cart.velocity = config.VELOCITY
                                cart.Kx = config.KX
                                time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["sidewalk"] or save_front_message == config.landmark_list["sidewalk"]:
                        # print("人行道1")
                        if (front_sign_message[2][2] - front_sign_message[2][0]) * (
                                front_sign_message[2][3] - front_sign_message[2][1]) > 3500:
                            print("人行道1")
                            cart.stop()
                            time.sleep(2)
                            cart.velocity = config.VELOCITY
                            cart.Kx = config.KX
                            time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["redlight"] or save_front_message == config.landmark_list["redlight"]:
                        # print("红灯")
                        if not speed_limit_flag and redlight_num==0:
                            print("红灯")
                            redlight_num = 2
                            if state == "right":
                                time_stop = time.time() + 1.735
                            elif state == "left":
                                time_stop = time.time() + 1.7
                            next_time = time.time() + 0.03
                            while time.time()<time_stop:
                                if time.time() >= next_time:
                                    _, img1 = stream_front.read()
                                    if config.cruiser_black_flag:  # 黑白巡线
                                        img1 = gather.BGR2BW(img1)
                                    angle = line(img1)
                                    cart.Run_steer(angle)
                                    next_time = time.time() + 0.03    
                            cart.stop()
                            time.sleep(1)
                            cart.move([-15,15,-15,15])
                            time.sleep(1.25)
                            cart.stop()
                            red_stop_flag = True
                            # time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["greenlight"] or save_front_message == config.landmark_list["greenlight"]:
                        print("绿灯")
                        start_time = time.time()
                        print_changeway_time = start_time+1
                        print_warning_time = start_time+12
                        print_flag = True
                        red_stop_flag = False
                        cart.move([15,-15,15,-15])
                        time.sleep(1.25)
                        time_identify = time.time() + time_proc
                        save_front_message = -1



    except KeyboardInterrupt:
        sys.exit()
    except:
        cart.stop()
        os.system('pkill python3')
        sys.exit()
    finally:
        cart.stop()
        print("stop!")

if __name__ == '__main__':
    mp.set_start_method('spawn', force=True)
    main()
