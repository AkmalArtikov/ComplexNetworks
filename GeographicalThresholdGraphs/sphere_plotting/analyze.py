import string

def get_degree_distribution(graph):
    hist = graph.degree_distribution().to_string()
    lines = string.split(hist, '\n')

    first = lines[1]
    begin = first.find("[") + 1
    end = first.find(",")
    value = int(first[begin:end])

    degree_distribution = []
    for i in range(0, value):
        degree_distribution.append(0)

    for i in range(1, len(lines)):
        begin = lines[i].find("(") + 1
        end = len(lines[i]) - 1

        if begin == 0 or end == 0:
            continue

        value = int(lines[i][begin:end])
        degree_distribution.append(value)

    return degree_distribution

def get_cut_degree_distribution(graph, threshold, mode, size, weights):
    degree_distribution = []

    for i in range(0, size):
        degree_distribution.append(0)

    for vs in graph.vs:
        if weights[i] <= (threshold + 0.0) / (mode + 0.0):
            degree_distribution[vs.degree()] += 1

    return degree_distribution