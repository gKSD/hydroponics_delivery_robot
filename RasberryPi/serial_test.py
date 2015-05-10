#!/usr/bin/env python

import serial
import string

rot13 = string.maketrans( 
    "ABCDEFGHIJKLMabcdefghijklmNOPQRSTUVWXYZnopqrstuvwxyz", 
    "NOPQRSTUVWXYZnopqrstuvwxyzABCDEFGHIJKLMabcdefghijklm")

test=serial.Serial("/dev/ttyAMA0",9600)
test.open()

try:
    test.write('a')
    test.write('a')
    test.write('a')
#    while True:
#                line = test.readline()
#                test.write(line)
                
except KeyboardInterrupt:
    pass # do cleanup here

test.close()
