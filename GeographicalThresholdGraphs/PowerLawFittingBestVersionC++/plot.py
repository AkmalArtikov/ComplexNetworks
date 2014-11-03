import sys
import time
import pandas as pd
import numpy as np
import time
import random
import math
from random import shuffle
import matplotlib.pyplot as plt

def plot_one(degrees, score, title, is_line=False, is_log=False, alpha=0.0):
    plt.plot(degrees, score, linestyle="None", marker='+')

    if is_log:
        #polyfit
        #line = np.polyfit(degrees, score, 1)
        #x = [min(degrees), max(degrees)]
        #y = [line[0] * x[0] + line[1], line[0] * x[1] + line[1]]

        x = [degrees[0], degrees[len(degrees) - 1]]
        b = score[0] - alpha * x[0]
        y = [x[0] * alpha + b, x[1] * alpha + b]

        print x
        print y
        plt.plot(x, y)

    if is_line:
        x = []
        y = []

        f = open('temp_bad.txt')
        lines = f.readlines()
        lines[0] = lines[0][:-1]
        f.close()
        bounds = lines[0].split(" ")
        print bounds

        if is_log:
            x = [math.log(float(bounds[0]), 2), math.log(float(bounds[0]), 2)]

        else:
            plt.plot([min(degrees)], min(score))
            x = [bounds[0], bounds[0]]

        print x

        y = [0, max(score)]
        plt.plot(x, y)

    plt.ylim([0,max(score)])
    plt.ylabel('Frequency')
    plt.xlabel('Degrees')
    plt.title(title)
    plt.show()

def plot_figures(file_name):
    f = open(file_name)
    lines = f.readlines()
    lines[0] = lines[0][:-1]
    lines[1] = lines[1][:-2]
    lines[2] = lines[2][:-1]
    f.close()

    alpha = -float(lines[2])

    bounds = lines[0].split(" ")
    degrees = lines[1].split(" ")

    x = range(int(bounds[0]), int(bounds[1]) + 1)

    new_x = []
    for i in range(len(x)):
        new_x.append(math.log(x[i] + 0.0, 2))

    y = []
    for i in range(len(degrees)):
        y.append(int(degrees[i]))

    new_y = []
    for i in range(len(degrees)):
        new_y.append(math.log(int(degrees[i]) + 0.0000001, 2))

    #print new_x
    #print new_y

    #print len(new_x)
    #print len(new_y)

    if file_name == 'temp.txt':
        plot_one(x, y, "freq / degree", is_line=True, is_log=False)
        plot_one(new_x, new_y, "log(freq) / log(degree)", is_line=True, is_log=True, alpha=alpha)
    else:
        plot_one(x, y, "freq / degree", is_log=False)
        plot_one(new_x, new_y, "log(freq) / log(degree)", is_log=True, alpha=alpha)

good = 'temp_good.txt'
bad = 'temp_bad.txt'
full = 'temp.txt'

plot_figures(good)
plot_figures(bad)
plot_figures(full)
