import sys
import time
import pandas as pd
import numpy as np
import time
import random
import math
from random import shuffle
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

def plot_edges(thetas, edges, title):
    plt.figure(figsize=(12, 10))
    plt.plot(thetas, edges, label="Edges in network")
    plt.ylabel('Edges')
    plt.xlabel('Theta - threshold parameter')
    plt.title(title)
    plt.legend(loc='upper left')
    plt.savefig('threshold_experiment_edges.png')
    plt.show()

def plot_alphas(thetas, alpha_good, alpha_full, alpha_bad, title):
    plt.figure(figsize=(12, 10))
    plt.plot(thetas, alpha_good, label="Good vertices")
    plt.plot(thetas, alpha_full, label='Full vertices')
    plt.plot(thetas, alpha_bad, label='Bad vertices')
    plt.ylabel('Power-law degree')
    plt.xlabel('Theta - threshold parameter')
    plt.title(title)
    plt.legend(loc='upper left')
    plt.savefig('threshold_experiment_alpha.png')
    plt.show()

theta = [0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
alpha_good = [1.49672, 2.23772, 1.94461, 1.88009, 1.87968, 1.92901, 1.94001, 1.91531]
alpha_full = [2, 1.92758, 1.82748, 1.81236, 1.83122, 1.88457, 1.90416, 1.89205]
alpha_bad = [3.11964, 3.342, 3.488, 3.62621, 3.73184, 3.7466, 3.76378, 3.91871]
edges = [7571405660, 3361120625, 1623262649, 883555600, 505224132, 305699267, 199794275, 131199332]

plot_edges(theta, edges, "Number of edges dependency on threshold parameter")
plot_alphas(theta, alpha_good, alpha_full, alpha_bad, "Power-law dependency on threshold parameter")