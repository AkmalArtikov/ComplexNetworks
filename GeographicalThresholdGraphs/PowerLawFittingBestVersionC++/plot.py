import sys
import time
import pandas as pd
import numpy as np
import time
import random
import math
from random import shuffle
import matplotlib.pyplot as plt

def plot_one(degrees, score, title, size, dimension, alpha_pareto, mode_pareto,
                 threshold, is_line=False, is_log=False, alpha=0.0, file_name="temp_degrees/temp.txt"):
    plt.figure(figsize=(12, 10))
    label = ""
    if is_log:
        label = "log(degree_freq)"
    else:
        label = "degree_freq"

    plt.plot(degrees, score, linestyle="None", marker='+', label=label)

    if is_log:
        x = [degrees[0], degrees[len(degrees) - 1]]
        b = score[0] - alpha * x[0]
        y = [x[0] * alpha + b, x[1] * alpha + b]

        label = "power-law approximation"
        plt.plot(x, y, label=label)

    if is_line:
        x = []
        y = []

        f = open('temp_degrees/temp_bad.txt')
        lines = f.readlines()
        lines[0] = lines[0][:-1]
        f.close()
        bounds = lines[0].split(" ")

        if is_log:
            x = [math.log(float(bounds[0]), 2), math.log(float(bounds[0]), 2)]

        else:
            plt.plot([min(degrees)], min(score))
            x = [bounds[0], bounds[0]]

        y = [0, max(score)]
        label = "good and bad vertices threshold"
        plt.plot(x, y, label=label)

    title = "size {0}, dimension {1}, alpha_pareto {2}, mode_pareto {3}, threshold {4}. {5}".format(size, dimension,
                                                                                                    alpha_pareto,
                                                                                                    mode_pareto,
                                                                                                    threshold,
                                                                                                    title)

    plt.ylim([0,max(score)])
    plt.ylabel('Frequency')
    plt.xlabel('Degrees')
    plt.title(title)

    name = "results_graphs/"
    if file_name == "temp_degrees/temp.txt":
        name += "full"

    if file_name == "temp_degrees/temp_good.txt":
        name += "good"

    if file_name == "temp_degrees/temp_bad.txt":
        name += "bad"

    if is_log:
        name += "_log"
    else:
        name += "_norm"
    plt.legend(loc='upper right')
    plt.savefig(name + '.png')
    #plt.show()

def plot_figures(file_name):
    f = open(file_name)
    lines = f.readlines()
    lines[0] = lines[0][:-1]
    lines[1] = lines[1][:-2]
    lines[2] = lines[2][:-1]
    f.close()

    bounds = lines[0].split(" ")
    degrees = lines[1].split(" ")
    params = lines[2].split(" ")

    alpha = -float(params[0])
    size = int(params[1])
    dimension = int(params[2])
    alpha_pareto = int(params[3])
    mode_pareto = float(params[4])
    threshold = float(params[5])


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

    if file_name == 'temp_degrees/temp.txt':
        plot_one(x, y, "freq / degree", size, dimension, alpha_pareto, mode_pareto,
                 threshold, is_line=True, is_log=False, file_name=file_name)
        plot_one(new_x, new_y, "log(freq) / log(degree)", size, dimension, alpha_pareto, mode_pareto,
                 threshold, is_line=True, is_log=True, alpha=alpha, file_name=file_name)
    else:
        plot_one(x, y, "freq / degree", size, dimension, alpha_pareto, mode_pareto,
                 threshold, is_log=False, file_name=file_name)
        plot_one(new_x, new_y, "log(freq) / log(degree)", size, dimension, alpha_pareto, mode_pareto,
                 threshold, is_log=True, alpha=alpha, file_name=file_name)

def plot_figure_commute(file_name):
    f = open(file_name)
    lines = f.readlines()
    lines[0] = lines[0][:-1]
    f.close()

    probs = lines[0].split(" ")
    x = xrange(len(probs))

    ind = 0
    for i in xrange(len(probs)):
        if float(probs[i]) < 1.0:
            ind = i
            break

    x = range(ind, len(probs))
    probs = probs[ind:]

    plt.figure(figsize=(12, 10))
    plt.plot(x, probs, label="D(degree > x)")
    plt.ylabel('Percentage of vertices which degree> x')
    plt.xlabel('Degrees')
    plt.title("Commute function for degrees")
    plt.legend(loc='upper right')

    name = "results_graphs/"
    if file_name == "temp_degrees/commute_temp.txt":
        name += "ful_commute"

    plt.savefig(name + '.png')

    new_x = [math.log(xx + 0.000000001, 2) for xx in x]
    new_probs = [math.log(float(xx) * len(x) + 0.000000001, 2) for xx in probs]

    plt.figure(figsize=(12, 10))
    plt.plot(new_x, new_probs, label="log(D(degree > x))")
    plt.legend(loc='upper right')
    plt.title("Commute function for degrees in log/log coords")
    plt.ylabel('log Number of vertices which degree> x')
    plt.xlabel('log Degrees')

    plt.savefig(name + '_log' + '.png')



good = 'temp_degrees/temp_good.txt'
bad = 'temp_degrees/temp_bad.txt'
full = 'temp_degrees/temp.txt'
commute_full = 'temp_degrees/commute_temp.txt'

plot_figures(good)
plot_figures(bad)
plot_figures(full)
plot_figure_commute(commute_full)
