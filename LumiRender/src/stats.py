# -*- coding:utf-8 -*-

import os

count = 0

com = 0

inCom = False

num_file = 0

for root,dirs,files in os.walk(os.path.join(os.getcwd(), "LumiRender/src")):
    for file in files:
        fn = os.path.join(root,file)
        if "ext" in fn:
            continue
        if "gui" in fn:
            continue
        if "jitify" in fn:
            continue
        if "sdk_pt" in fn:
            continue
        try:
            f = open(fn, "r")
            count += len(f.readlines())
        except :
            print(fn)
        num_file += 1


print(count, num_file)
        