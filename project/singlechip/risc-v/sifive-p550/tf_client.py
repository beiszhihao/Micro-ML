import socket
import numpy as np
import argparse
import cv2
payload_type = {'names': ('cmd', 'end', 'f_size', 't_size'), 'formats': ('u1', 'u1', 'u2', 'u4')}
SEND_MODEL_CMD = 1
SEND_INPUT_CMD = 2
SEND_RUN_CMDS = 3
GET_RESULT_CMDS = 4
GET_CPUINFO_CMDS = 5
SEND_JPG_CMD = 6
BUF_SIZE = 1024


def send_data(sk, data_content, _type):
    header = np.zeros(1, dtype=payload_type)
    header = header.view(np.recarray)[0]
    header.t_size = len(data_content)
    header.cmd = _type
    data_size = BUF_SIZE - header.itemsize
    total = 0
    while True:
        remain = len(data_content) - total
        header.f_size = min(remain, data_size)
        data = data_content[total: total + header.f_size]
        if remain <= data_size:
            data += bytes(data_size-remain)
            header.end = 1
        total += header.f_size
        sk.send(header.tobytes() + data)
        if remain <= data_size:
            break


def send_model_file(sk, file_name):
    data_content = open(file_name, 'rb').read()
    send_data(sk, data_content, SEND_MODEL_CMD)


def send_input_file(sk, file_name):
    data_content = open(file_name, 'rb').read()
    send_data(sk, data_content, SEND_INPUT_CMD)


def send_jpg_file(sk, file_name):
    data_content = open(file_name, 'rb').read()
    send_data(sk, data_content, SEND_JPG_CMD)


def send_input_data(sk, img):
    h, w = img.shape[:-1]
    data = img.tobytes() + h.to_bytes(2, 'little') + w.to_bytes(2, 'little')
    send_data(sk, data, SEND_INPUT_CMD)


def tf_run(sk):
    header = np.zeros(1, dtype=payload_type)
    header = header.view(np.recarray)[0]
    header.cmd = SEND_RUN_CMDS
    sk.send(header.tobytes() + bytes(BUF_SIZE-header.itemsize))


def tf_get_res(sk):
    header = np.zeros(1, dtype=payload_type)
    header = header.view(np.recarray)[0]
    header.cmd = GET_RESULT_CMDS
    sk.send(header.tobytes() + bytes(BUF_SIZE-header.itemsize))
    rec_data = bytes(0)
    while True:
        rec_data += sk.recv(1024)
        if len(rec_data) >= 1024:
            break
    header = np.frombuffer(rec_data[:8], dtype=payload_type)
    header = header.view(np.recarray)[0]
    ROI = []
    time = header.f_size
    if header.cmd == GET_RESULT_CMDS:
        counts = header.t_size // header.itemsize
        for i in range(counts):
            roi = np.frombuffer(rec_data[8*(i+1): 8*(i+2)], dtype=np.uint16)
            ROI.append(roi)
    return ROI, time


def get_target_cpuinfo(sk):
    header = np.zeros(1, dtype=payload_type)
    header = header.view(np.recarray)[0]
    header.cmd = GET_CPUINFO_CMDS
    sk.send(header.tobytes() + bytes(BUF_SIZE-header.itemsize))
    rec_data = bytes(0)
    while True:
        rec_data += sk.recv(1024)
        if len(rec_data) >= 1024:
            break
    header = np.frombuffer(rec_data[:8], dtype=payload_type)
    header = header.view(np.recarray)[0]
    cpuinfo = ''
    if header.cmd == GET_CPUINFO_CMDS:
        cpuinfo = rec_data[8: 8 + header.f_size]
    return cpuinfo.decode('utf-8')


if __name__ == '__main__':
    s = socket.socket()
    s.connect(('10.239.133.42', 12345))

    print(get_target_cpuinfo(s))
    parser = argparse.ArgumentParser(description="tflite command tool")
    parser.add_argument('-i', '--input', help='input image file')
    parser.add_argument('-v', '--verbose', action="count", default=0, help='input image file')
    parser.add_argument('-m', '--model', default='yoloface_int8.tflite', help='tflite model file')
    args = parser.parse_args()

    send_model_file(s, args.model)

    def handle_result(img, timeout=0, format='narray'):
        if format == 'narray':
            send_input_data(s, img)
        else:
            send_jpg_file(s, img)
        tf_run(s)
        roi, r_time = tf_get_res(s)
        print(f"ftflite run times: {r_time}ms")
        if args.verbose:
            for val in roi:
                cv2.rectangle(img, (val[0], val[1]), (val[2], val[3]), (0, 0, 255), 2)
            cv2.imshow("main", img)
            return cv2.waitKey(timeout)
        else:
            for val in roi:
                print(val)
        return 0
    if args.input:
        handle_result(args.input, 0, 'jpg')
    else:
        cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
        while True:
            ret, img = cap.read()
            if handle_result(img, 100) == ord('q'):
                break

    s.close()
    cv2.destroyAllWindows()
