#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 31 10:07:10 2022

@author: manea
"""
from starapi import starapi
path = "/usr/local/STAR-Dundee/STAR-System/lib/x86-64"

star = starapi(path)
print(star.list_devices())
star.open_channel(1)
star.send_msg(byte_msg= b'\xbe\xef\x12\x34', channel = 1)
star.open_channel(2)
print(star.rec_msg(channel = 2))
star.send_msg(byte_msg= b'\x11\xb0\x01m\x00\x00\x00\x00\xff\x80\x00\x00\x00!\x08\x08\xa0\x00\x00\x08\xb6\x10\x90\xc0\x03\x00\x01\xbe\xef\x40', channel = 1)
print(star.rec_msg(channel = 1))

#alwas make sure to close the channel after usage 
star.close_channel(1)
star.close_channel(2)