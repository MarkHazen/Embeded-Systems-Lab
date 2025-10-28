#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  camera_pi.py
#  
#  
#  
import time
import io
import threading
import picamera
import base64
import cv2
import zmq
import socket
from threading import Thread

class Camera(object):
    thread = None  # background thread that reads frames from camera
    frame = None  # current frame is stored here by background thread
    last_access = 0  # time of last client access to the camera

    def initialize(self):
        if Camera.thread is None:
            # start background frame thread
            Camera.thread = threading.Thread(target=self._thread)
            Camera.thread.start()

            # wait until frames start to be available
            while self.frame is None:
                time.sleep(0)

    def get_frame(self):
        Camera.last_access = time.time()
        self.initialize()
        return self.frame

    @classmethod
    def _thread(cls):
        with picamera.PiCamera() as camera:
            # Camera setup: camera.resolution = (X,X)

            camera.hflip = True
            camera.vflip = True

            stream = io.BytesIO()
            for foo in camera.capture_continuous(stream, 'jpeg',
                                                 use_video_port=True):
                # store frame
                stream.seek(0)
                cls.frame = stream.read()

                # reset stream for next frame
                stream.seek(0)
                stream.truncate()

                # if there hasn't been any clients asking for frames in
                # the last 10 seconds stop the thread
                ######################################
                if time.time() - cls.last_access > 10:
                    break
        cls.thread = None

camera = Camera()
# Setup the UDP socket

def get_f():
    global camera,connection
    image = camera.get_frame()
    print(len(image))
    try:
        # Send image to the server
        pass
    except:
        print("something happened")

def read_send_image():
    while(1):
        get_f()
        time.sleep(0.01)
       
if __name__ == '__main__': 
   read_send_image()