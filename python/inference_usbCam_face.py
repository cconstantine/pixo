#!/usr/bin/python
# -*- coding: utf-8 -*-
# pylint: disable=C0103
# pylint: disable=E1101

import sys
import time
import cv2
import numpy

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


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print ("usage:%s (cameraID | filename) Detect faces\
 in the video example:%s 0"%(sys.argv[0], sys.argv[0]))
        exit(1)

    try:
    	camID = int(sys.argv[1])
    except:
    	camID = sys.argv[1]
    
    tDetector = FaceDetector('model.pb', gpu_memory_fraction=0.25, visible_device_list='0')

    cap = cv2.VideoCapture(camID)
    windowNotSet = True
    while True:
        ret, image = cap.read()
        if ret == 0:
            break

        [h, w] = image.shape[:2]
        image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
	image = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB)
        #image = cv2.flip(image, 1)
        #image = Image.fromarray(image)
        start_time = time.time()
        boxes, scores = tDetector(image, score_threshold=0.3)
        image = draw_boxes_on_image(image, boxes, scores)
        print("Found %d faces in %fms.\n" % (len(boxes), (time.time() - start_time)*1000))
        if windowNotSet is True:
            cv2.namedWindow("tensorflow based (%d, %d)" % (w, h), cv2.WINDOW_NORMAL)
            windowNotSet = False
        
        cv2.imshow("tensorflow based (%d, %d)" % (w, h), image)

        k = cv2.waitKey(1) & 0xff
        if k == ord('q') or k == 27:
            break

    cap.release()
