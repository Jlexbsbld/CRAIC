import multiprocessing as mp
import gather
import time
import os
import cv2 as cv
import device
import config
import cruiser_infer
import sys, select, termios, tty

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
    front_sign_message = [0, 0, [0, 0, 0, 0]]

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
    # print("spend_time", time.time() - spend)
    # cart.velocity = 40
    # cart.Kx = 40*0.875
    start_time = time.time()
    print_flag = False
    print_warning_time = time.time()
    print_warning_flag = False
    print_changeway_time = time.time()
    print_changeway_flag = False
    kx_recover_time = time.time()
    kx_recover_flag = True
    turn_rl_flag=False
    print("初始化完成")
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
                lane_time = time.time() + 0.02
                _, img1 = stream_front.read()
                if config.cruiser_black_flag:  # 黑白巡线
                    img1 = gather.BGR2BW(img1)
                angle = line(img1)
                lane = lane + 1
                if not red_stop_flag:
                    if turn_rl_flag:
                        cart.Run_steer(angle)
                    else:
                        cart.Run_steer(0)
            if print_flag:
                if time.time() > print_changeway_time and not print_changeway_flag:
                    print("识别到变道")
                    print_changeway_flag = True
                elif time.time() > print_warning_time and not print_warning_flag:
                    print("识别到危险")
                    print_warning_flag = True
                    print_flag = False
            if time.time() > kx_recover_time and not kx_recover_flag:
                cart.kx = config.kx_low
                cart.velocity = config.velocity - 2
                # print("复原")
                kx_recover_flag = True
            if time.time() >= det_time:
                det_time = time.time() + 0.16
                _, img2 = stream_det.read()
                cv.imshow("img", img2)
                cv.waitKey(1)
                img2 = img2[100:, :500]
                response = infer(img2)
                front_sign_message[0], front_sign_message[1], front_sign_message[2] = gather.deal(response)
                det = det + 1
                if time.time() > time_identify:
                    if limit_num == 0 and (
                            front_sign_message[0] == config.landmark_list["speedlimit"] or save_front_message ==
                            config.landmark_list["speedlimit"] or front_sign_message[0] == config.landmark_list[
                                "limitlifted"] or save_front_message == config.landmark_list["limitlifted"]):  # 限速
                        limit_num = 1
                        cart.velocity = config.velocity_slow
                        cart.kx = config.kx_slow
                        print("识别到限速")
                        time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["limitlifted"] or save_front_message == \
                            config.landmark_list["limitlifted"]:  # 解除限速
                        cart.velocity = config.velocity
                        cart.kx = config.kx
                        speed_limit_flag = False
                        print("识别到解除限速")
                        time_identify = time.time() + 2.25
                        save_front_message = -1
                    elif (front_sign_message[0] == config.landmark_list["turnleft"] or save_front_message ==
                          config.landmark_list["turnleft"]) and not turn_flag:  # 左转
                        # print("左转")
                        if (front_sign_message[2][2] - front_sign_message[2][0]) * (
                                front_sign_message[2][3] - front_sign_message[2][1]) > 2800:
                            print("识别到左转")
                            state = "left"
                            turn_flag = True
                            cart.Run_steer(0.8)
                            time.sleep(1.15)
                            time_identify = time.time() + time_proc
                            turn_rl_flag = True
                            config.task_confidence["turnleft"] = 1.2
                            config.task_confidence["turnright"] = 1.2
                        save_front_message = -1
                    elif (front_sign_message[0] == config.landmark_list["turnright"] or save_front_message ==
                          config.landmark_list["turnright"]) and not turn_flag:  # 右转
                        # print("右转")
                        if (front_sign_message[2][2] - front_sign_message[2][0]) * (
                                front_sign_message[2][3] - front_sign_message[2][1]) > 2800: # 3000
                            print("识别到右转")
                            state = "right"
                            turn_flag = True
                            cart.Run_steer(-0.8)
                            time.sleep(1.4)
                            time_identify = time.time() + time_proc
                            turn_rl_flag = True
                            config.task_confidence["turnleft"] = 1.2
                            config.task_confidence["turnright"] = 1.2
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["stop"] or save_front_message == \
                            config.landmark_list["stop"]:
                        # print("终点")
                        if (front_sign_message[2][3] + front_sign_message[2][1]) // 2 > 350:
                            print("识别到终点")
                            time_stop = time.time() + 2.5
                            next_time = time.time() + 0.03
                            while time.time() < time_stop:
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
                    elif front_sign_message[0] == config.landmark_list["sidewalk2"] or save_front_message == \
                            config.landmark_list["sidewalk2"]:
                        # print("人行道2")
                        if state == "left":
                            if (front_sign_message[2][3] + front_sign_message[2][1]) // 2 > 320:
                                print("识别到人行道2")
                                cart.stop()
                                time.sleep(3)
                                cart.velocity = config.velocity
                                cart.kx = config.kx
                                time_identify = time.time() + time_proc
                        else:
                            if (front_sign_message[2][3] + front_sign_message[2][1]) // 2 > 330:
                                print("识别到人行道2")
                                cart.stop()
                                time.sleep()
                                cart.velocity = config.velocity
                                cart.kx = config.kx
                                time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["sidewalk"] or save_front_message == \
                            config.landmark_list["sidewalk"]:
                        # print("人行道1")
                        if (front_sign_message[2][2] - front_sign_message[2][0]) * (
                                front_sign_message[2][3] - front_sign_message[2][1]) > 3500:
                            print("识别到人行道1")
                            cart.stop()
                            time.sleep(2)
                            cart.velocity = config.velocity
                            cart.kx = config.kx
                            time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["redlight"] or save_front_message == \
                            config.landmark_list["redlight"]:
                        # print("红灯")
                        if not speed_limit_flag and redlight_num == 0:
                            print("识别到红灯")
                            redlight_num = 2
                            if state == "right":
                                time_stop = time.time() + 1.1 # 1.42
                            elif state == "left":
                                time_stop = time.time() + 1.35
                            next_time = time.time() + 0.03
                            while time.time() < time_stop:
                                if time.time() >= next_time:
                                    _, img1 = stream_front.read()
                                    if config.cruiser_black_flag:  # 黑白巡线
                                        img1 = gather.BGR2BW(img1)
                                    angle = line(img1)
                                    cart.Run_steer(angle)
                                    next_time = time.time() + 0.03
                            cart.stop()
                            time.sleep(1)
                            cart.move([-10, -10, 10, 10])
                            time.sleep(1.2)
                            cart.stop()
                            red_stop_flag = True
                            # time_identify = time.time() + time_proc
                        save_front_message = -1
                    elif front_sign_message[0] == config.landmark_list["greenlight"] or save_front_message == \
                            config.landmark_list["greenlight"]:
                        print("识别到绿灯")
                        start_time = time.time()
                        print_changeway_time = start_time + 1
                        print_warning_time = start_time + 12
                        kx_recover_time = start_time + 5
                        print_flag = True
                        red_stop_flag = False
                        cart.move([10, 10, -10, -10])
                        time.sleep(1.2)
                        time_identify = time.time() + time_proc
                        cart.velocity = config.velocity_slow
                        kx_recover_flag = False
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
