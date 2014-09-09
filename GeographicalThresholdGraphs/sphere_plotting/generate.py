import igraph
import math
import numpy as np

def generate_sphere(dimension, size):
    coords = np.zeros(dimension)

    for i in range(0, size):
        radius = 0.0
        vector = np.random.normal(0, 1, dimension)

        for j in vector:
            radius += j**2

        radius = math.sqrt(radius)
        new_vector = vector / radius

        coords = np.vstack([coords, new_vector])

    return coords[1:]

def get_edges(graph, coords, threshold, weights):
    for i in range(0, len(coords)):
        for j in range(i + 1, len(coords)):
            x = coords[i]
            y = coords[j]
            dot = np.dot(x, y)

            value = dot * (weights[i] * weights[j])

            if value >= threshold:
                graph.add_edge(i, j)

    return graph

def get_weights(size, alpha, mode):

    weights = np.random.pareto(alpha, size) + mode

    return weights

def get_graph(dimension, size, threshold, alpha, mode):
    g = igraph.Graph(directed=None)
    g.add_vertices(size)

    coords = generate_sphere(dimension=dimension, size=size)

    weights = get_weights(size=size, alpha=alpha, mode=mode)

    g = get_edges(graph=g, coords=coords, threshold=threshold, weights=weights)

    return g, coords, weights
