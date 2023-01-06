#!/usr/bin/env python3

import tflite_runtime.interpreter as tflite
import cv2
import numpy as np
import socket
import threading
from cpuinfo import get_cpu_info
import time
payload_type = {'names': ('cmd', 'end', 'f_size', 't_size'), 'formats': ('u1', 'u1', 'u2', 'u4')}


class TfLite:
    def __init__(self, model_content):
        self.interpreter = tflite.Interpreter(model_content=model_content)
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()
        self.dim = (56, 56)
        self.shape = (1280, 720, 3)

    def set_input(self, img):
        self.dim = tuple(self.input_details[0]['shape'][1:-1])
        img = cv2.resize(img, self.dim, interpolation=cv2.INTER_AREA)
        input_data = np.expand_dims(img, axis=0).astype(np.int8)
        self.interpreter.set_tensor(self.input_details[0]['index'], input_data)

    def run(self):
        self.interpreter.invoke()
        output_data = self.interpreter.get_tensor(self.output_details[0]['index'])
        output = output_data[0].reshape((7, 7, 3, 6))
        return output

    def set_output(self, output):
        def sigmoid(x):
            s = 1 / (1 + np.exp(-x))
            return s

        anchors = ((9, 14), (12, 17), (18, 26))
        ROI = []
        found = np.argwhere(output[..., 4] >= -9)
        for items in found:
            res = output[tuple(items)]
            data = (res[:4] + 15) * 0.14218327403068542

            x = (sigmoid(data[0]) + items[1]) * 8
            y = (sigmoid(data[1]) + items[0]) * 8
            w = np.exp(data[2]) * anchors[items[2]][0]
            h = np.exp(data[3]) * anchors[items[2]][1]

            x1, x2 = x - w / 2, x + w / 2
            y1, y2 = y - h / 2, y + h / 2

            ROI.append([[x1, y1], [x2, y2]])
        return ROI


class tfThread(threading.Thread):
    SEND_MODEL_CMD = 1
    SEND_INPUT_CMD = 2
    SEND_RUN_CMDS = 3
    GET_RESULT_CMDS = 4
    GET_CPUINFO_CMDS = 5
    SEND_JPG_CMD = 6

    def __init__(self, threadID, name, conn):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.conn = conn
        self.tf = None
        self.input = None
        self.roi = []
        self.time = 0
        self.tf_data = [
                {'data': bytes(0), 'status': 0},  # output
                {'data': bytes(0), 'status': 0},  # model
                {'data': bytes(0), 'status': 0},  # input
        ]

    def handler_cmds(self, cmds, conn):
        header = np.frombuffer(cmds[:8], dtype=payload_type)
        header = header.view(np.recarray)[0]
        if header.cmd in (self.SEND_MODEL_CMD, self.SEND_INPUT_CMD, self.SEND_JPG_CMD):
            if header.cmd == self.SEND_JPG_CMD:
                tf_data = self.tf_data[2]
            else:
                tf_data = self.tf_data[header.cmd]
            if tf_data['status'] > 0:
                tf_data['status'] = 0
                tf_data['data'] = bytes(0)
            if header.end == 1:
                tf_data['status'] = 1
            if header.end == 1 and header.cmd == self.SEND_INPUT_CMD:
                tf_data['data'] += cmds[8:8+header.f_size-4]
                shape_data = cmds[8+header.f_size-4: 8+header.f_size]
                self.shape = (int.from_bytes(shape_data[:2], 'little'), int.from_bytes(shape_data[-2:], 'little'), 3)
                self.input = np.frombuffer(tf_data['data'], np.uint8).reshape(self.shape)
            elif header.end == 1 and header.cmd == self.SEND_JPG_CMD:
                tf_data['data'] += cmds[8:8+header.f_size]
                self.input = cv2.imdecode(np.frombuffer(tf_data['data'], np.uint8), cv2.IMREAD_COLOR)
                self.shape = self.input.shape
            else:
                tf_data['data'] += cmds[8:8+header.f_size]
        elif header.cmd == self.SEND_RUN_CMDS:
            if not self.tf or self.tf_data[self.SEND_MODEL_CMD]['status'] != 2:
                self.tf = TfLite(self.tf_data[self.SEND_MODEL_CMD]['data'])
                self.tf_data[self.SEND_MODEL_CMD]['status'] = 2
            self.tf.set_input(self.input)
            start = time.time()
            output = self.tf.run()
            end = time.time()
            self.time = int(1000 * (end - start))
            print(1000 * (end - start))
            self.roi = self.tf.set_output(output)
        elif header.cmd == self.GET_RESULT_CMDS:
            send_data = bytes(0)
            header = np.zeros(1, dtype=payload_type)
            header = header.view(np.recarray)[0]
            header.cmd = self.GET_RESULT_CMDS
            for val in self.roi:
                # x1, y1 = val[0] * np.array(self.shape[:-1]) / np.array(self.tf.dim)
                # x2, y2 = val[1] * np.array(self.shape[:-1]) / np.array(self.tf.dim)
                # data = np.array([x1, y1, x2, y2])
                # data[::2] = np.clip(data[::2], 0, self.shape[0])
                # data[1::2] = np.clip(data[1::2], 0, self.shape[1])
                x1, y1 = val[0]
                x2, y2 = val[1]
                data = np.array([x1, y1, x2, y2])
                data = np.clip(data, 0, 56)
                data = data.astype('u2')
                roidata = "roi:["
                roidata += '{},'.format(data[0])
                roidata += '{},'.format(data[1])
                roidata += '{},'.format(data[2])
                roidata += '{},'.format(data[3])
                roidata += '{}'.format(self.time)
                roidata += ']'
                #send_data += data.tobytes()
                send_data += roidata.encode()

            header.t_size = len(send_data)
            header.f_size = self.time
            conn.send(header.tobytes() + send_data + bytes(1024 - header.itemsize - len(send_data)))
        elif header.cmd == self.GET_CPUINFO_CMDS:
            header = np.zeros(1, dtype=payload_type)
            header = header.view(np.recarray)[0]
            header.cmd = self.GET_CPUINFO_CMDS
            cpuinfo = get_cpu_info()['brand_raw']
            cpuinfo += '\n'
            cpuinfo += str(get_cpu_info()['count'])
            send_cpu = 'cpui[';
            send_cpu += cpuinfo;
            send_cpu += ']'
            data = send_cpu.encode()
            header.f_size = len(data)
            conn.send(header.tobytes() + data + bytes(1024 - header.itemsize - len(data)))

    def run(self):
        print(f"Connect from: {self.name}")
        data = bytes(0)
        while True:
            data += conn.recv(1024)
            if len(data) == 0:
                break
            if len(data) >= 1024:
                self.handler_cmds(data[:1024], conn)
                data = data[1024:]
        print(f"{self.name} disconnected.")


if __name__ == '__main__':
    s = socket.socket()
    s.bind(('0.0.0.0', 1001))
    s.listen(1)
    print('port:1001')
    try:
        while True:
            conn, addr = s.accept()
            tf = tfThread(1, f'{addr[0]}:{addr[1]}', conn)
            tf.start()
    except KeyboardInterrupt:
        s.close()
