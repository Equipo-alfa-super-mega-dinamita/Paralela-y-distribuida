#!/usr/bin/env python
# coding: utf-8

# In[2]:

import pandas as pd
import subprocess
import os
import struct

#!gcc blur-effect.c -o blur -lm -pthread
os.system('mpicc blur-effect.c -o blur -lm')



image_names = ['1080']
images = []
dfs = {}
iterations = 10



for image in image_names:
    print("Image name: ", image)
    times = pd.DataFrame()
    for threads in range(4):
        for kernel in range(7):        
            t = []
            print("\tThreads: ", 2**threads, "; Kernel size: ",3 + 2*kernel)
            for _ in range(iterations):

                #cmd_params = ["/usr/bin/time", "-f", "%e", "./blur", "{}.jpg".format(image), "{}_blur.jpg".format(image), str(3 + 2*kernel), str(2**threads)]
                cmd_params = ["/usr/bin/time", "-f", "%e", "mpirun","-q","--hostfile","mpi_hosts","-np",str(2**threads),"./blur", "{}.jpg".format(image), "{}_blur.jpg".format(image), str(3 + 2*kernel)]
                #t_out = !/usr/bin/time -f "%e" ./blur {image}.jpg {image}_blur.jpg {3 + 2*kernel} {2**threads}
                out = subprocess.Popen(cmd_params, 
                    stdout=subprocess.PIPE, 
                    stderr=subprocess.STDOUT)
                stdout,stderr = out.communicate()                
                t.append(float(stdout))            
            times = times.append(pd.Series(t, dtype = "float64"), ignore_index = True)
    print(times)
    dfs[image] = times
    dfs[image].to_csv( image + "_time.csv" )