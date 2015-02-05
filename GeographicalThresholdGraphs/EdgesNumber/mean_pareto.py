import igraph
import math
import numpy as np
import time

def get_weights(size, alpha, mode):

    weights = np.random.pareto(alpha, size) + mode

    mean = 0.0

    for weight in weights:
        mean += weight

    mean /= len(weights)

    return weights, mean

weights, mean = get_weights(1000000, 4, 0.0001)
print mean