#!/usr/bin/env python3

import tflite_runtime.interpreter as tflite
import cv2
import numpy as np
import argparse


class TfLite:
    def __init__(self, model_file):
        self.interpreter = tflite.Interpreter(model_path=model_file)
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

    def set_input(self, img, dim):
        img = cv2.resize(img, dim, interpolation=cv2.INTER_AREA)
        input_data = np.expand_dims(img, axis=0).astype(np.int8)
        self.interpreter.set_tensor(self.input_details[0]['index'], input_data)

    def run(self):
        import time
        start = time.time()
        for i in range(1000):
            self.interpreter.invoke()
        end = time.time()
        print(end - start)
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


def show_output(roi, img, dim, show_level=2):
    for val in roi:
        x1, y1 = val[0] * np.array(img.shape[:2]) / np.array(dim)
        x2, y2 = val[1] * np.array(img.shape[:2]) / np.array(dim)
        if show_level & 2 == 2:
            cv2.rectangle(img, (int(x1), int(y1)), (int(x2), int(y2)), (0, 0, 255), 2)
        if show_level & 1 == 1:
            print((int(x1), int(y1)), (int(x2), int(y2)))
    if show_level & 2 == 2:
        cv2.imshow("output", img)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="tflite command tool")
    parser.add_argument('-i', '--input', help='input image file')
    parser.add_argument('-v', '--verbose', action="count", default=0, help='input image file')
    parser.add_argument('-m', '--model', default='yoloface_int8.tflite', help='tflite model file')
    args = parser.parse_args()

    tf_run = TfLite(args.model)
    dim = tf_run.input_details[0]['shape'][1:-1]
    dim = tuple(dim)
    cap = None
    while True:
        if args.input:
            timeout = 0
            img = cv2.imread(args.input)
        else:
            if not cap:
                timeout = 100
                cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
            _, img = cap.read()

        tf_run.set_input(img, dim)
        roi = tf_run.set_output(tf_run.run())
        show_output(roi, img, dim, args.verbose)
        val = cv2.waitKey(timeout)
        if val == ord('q') or args.input:
            if cap:
                cap.release()
            break

    cv2.destroyAllWindows()
