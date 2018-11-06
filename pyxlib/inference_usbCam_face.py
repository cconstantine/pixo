#!/usr/bin/python
# -*- coding: utf-8 -*-
# pylint: disable=C0103
# pylint: disable=E1101

import sys
import time
import cv2
import numpy
import threading

from face_detector import FaceDetector
from PIL import Image, ImageDraw


def draw_boxes_on_image(image, boxes, scores):
    image_pil = Image.fromarray(image)
    draw = ImageDraw.Draw(image_pil, 'RGBA')
    height, width = image.shape[:2]

    for b, s in zip(boxes, scores):
        ymin, xmin, ymax, xmax = b
        fill = (255, 0, 0, 45)
        outline = 'red'
        draw.rectangle(
            [(xmin, ymin), (xmax, ymax)],
            fill=fill, outline=outline
        )
        draw.text((xmin, ymin), text='{:.3f}'.format(s))

    numpy.copyto(image, numpy.array(image_pil))
    return image


class DetectionThread(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.ready = False
        self.do_exit = False

        import sys
        if len(sys.argv) != 2:
            print ("usage:%s (cameraID | filename) Detect faces in the video example:%s 0"%(sys.argv[0], sys.argv[0]))
            exit(1)

        try:
            self.camID = int(sys.argv[1])
        except:
            self.camID = sys.argv[1]
        
        self.tDetector = FaceDetector('model.pb', gpu_memory_fraction=0.25, visible_device_list='0')

    def run(self):
        cap = cv2.VideoCapture(self.camID)

        while not self.do_exit:
            ret, image = cap.read()
            if ret == 0:
                break

            [h, w] = image.shape[:2]
            image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
            image = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB)
            #image = cv2.flip(image, 1)
            #image = Image.fromarray(image)
            start_time = time.time()
            boxes, scores = self.tDetector(image, score_threshold=0.3)
            self.image = draw_boxes_on_image(image, boxes, scores)
            print("Found %d faces in %fms.\n" % (len(boxes), (time.time() - start_time)*1000))
            # cv2.imshow("tensorflow based (%d, %d)" % (w, h), image)
            self.ready = True

        cap.release()

if __name__ == "__main__":
    detector = DetectionThread()
    detector.start()
    cv2.namedWindow("tensorflow face detection", cv2.WINDOW_NORMAL)
    while(detector.is_alive()):
        start_time = time.time()

        k = cv2.waitKey(1) & 0xff
        if k == ord('q') or k == 27:
            detector.do_exit = True

        if (detector.ready):
            cv2.imshow("tensorflow face detection", detector.image)

        delta_ms = (time.time() - start_time)*1000
        
        if delta_ms < 16:
            time.sleep((16-delta_ms) / 1000.0)

        delta_ms = (time.time() - start_time)*1000
        print("Window frame %fms.\n" % delta_ms)


    detector.join()

