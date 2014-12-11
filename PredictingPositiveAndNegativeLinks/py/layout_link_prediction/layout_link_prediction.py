from optparse import OptionParser
from igraph import *
from random import *
from sklearn.cross_validation import train_test_split
from math import sqrt

import pickle
import numpy as np

COUNT_OF_USERS = 1000

def readDataUsers(filename):
    with open(filename) as f:
        return map(lambda x: int(x) - 1, f.readlines())

def readDataLinks(filename):
    with open(filename) as f:
        return map(lambda x: map(lambda x: int(x) - 1, x.split('\t')), f.readlines())

def getGraphAndTestData(input_users, input_links):
    users = readDataUsers(input_users)
    links = readDataLinks(input_links)

    users = users[:COUNT_OF_USERS]
    links = [x for x in links if x[0] in users and x[1] in users]

    links_train, links_test = train_test_split(links, test_size=0.1)
    g = Graph()
    g.add_vertices(users)
    g.add_edges(links_train)

    not_links = []
    while len(not_links) < len(links_test):
        a = randint(0, len(users) - 1)
        b = randint(0, len(users) - 1)
        if ([a, b] not in links):
            not_links.append([a, b])
    Xs_test = list(links_test) + not_links
    Ys_test = [1] * len(links_test) + [0] * len(not_links)
    return g, Xs_test, Ys_test


class LayoutEstimator(object):
    def __init__(self, g, alpha):
        self.g = g
        self.layout = g.layout()
        plot(g, layout=self.layout)

    def fit(self, Xs, Ys):
        pass

    def predict(self, Xs):
        result = []
        for x in Xs:
            result.append(self.isEdge(self.layout[x[0]], self.layout[x[1]]))
        return result

    def isEdge(self, x, y):
        if self.distance(x, y) < self.alpha:
            return 1
        else:
            return 0

    def distance(self, x, y):
        result = 0
        for i in xrange(len(x)):
            result += (x[i] - y[i])**2
        return sqrt(result)

    def setAlpha(self, alpha):
        self.alpha = alpha

    def getMaxAlpha(self, Xs):
        max_a = 0
        for x in Xs:
            if max_a < self.distance(self.layout[x[0]], self.layout[x[1]]):
                max_a = self.distance(self.layout[x[0]], self.layout[x[1]])
        return max_a

def getScore(Ys_predict, Ys):
    good = 0
    goodPos = 0
    goodNeg = 0
    count_pos = 0
    count_neg = 0
    for y1, y2 in zip(Ys_predict, Ys):
        if y1 == y2:
            good += 1
        if y2 == 1:
            count_pos += 1
            if y1 == y2:
                goodPos += 1
        else:
            count_neg += 1
            if y1 == y2:
                goodNeg += 1

    return 1. * good / len(Ys), 1. * goodPos / count_pos, 1. * goodNeg / count_neg

if __name__ == "__main__":
    seed(1112)

    parser = OptionParser()
    parser.add_option("-u", "--input-users", dest="input_users", help="Input file with users")
    parser.add_option("-l", "--input-links", dest="input_links", help="Input file with links")

    (options, args) = parser.parse_args()

    g, Xs_test, Ys_test = getGraphAndTestData(options.input_users, options.input_links)

    print("Graph with %d vertices and %d edges" % (g.vcount(), g.ecount()))

    estimator = LayoutEstimator(g, 1)
    max_a = estimator.getMaxAlpha(Xs_test)
    print "Max alpha:", max_a
    #for al in xrange(0, int(max_a) / 3, max(int(max_a) / 1000, 1)):
    for al in np.asarray(range(0, int(max_a) * 10)) / 10.:
        estimator.setAlpha(al)
        s, s_pos, s_neg = getScore(estimator.predict(Xs_test), Ys_test)
        print("Accuracy (alpha=%.2f): %.2f, pos: %.2f, neg: %.2f" % (al, s, s_pos, s_neg))
    