import pandas as pd
import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn import cross_validation
from random import shuffle, sample
from optparse import OptionParser

def balanceData(Xs, Ys):
    XsPos = []
    XsNeg = []
    for x, y in zip(Xs, Ys):
        if y == 1:
            XsPos.append(x)
        else:
            XsNeg.append(x)
    if len(XsPos) > len(XsNeg):
        XsPos = sample(XsPos, len(XsNeg))
    else:
        XsNeg = sample(XsNeg, len(XsPos))
    Xs = XsPos + XsNeg
    Ys = [1] * len(XsPos) + [-1] * len(XsNeg)
    return Xs, Ys

def printScore(Xs, Ys, features_range=None, em=0, tp=''):
    data = zip(Xs, Ys)
    filtered = []
    for d in data:
        if d[0][4] >= em:
            filtered.append(d)
    Xs, Ys = zip(*data)
    Xs, Ys = balanceData(Xs, Ys)
    data = zip(Xs, Ys)
    shuffle(data)
    Xs, Ys = zip(*data)
    if features_range is not None:
        Xs = zip(*zip(*Xs)[features_range[0]:features_range[1]])
    
    scores = cross_validation.cross_val_score(LogisticRegression(), Xs, Ys, scoring='roc_auc', n_jobs=-1, cv=10)
    print("Accuracy (Em=%d, features=%s): %0.2f (+/- %0.2f)" % (em, tp, scores.mean() * 100, scores.std() * 200))

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--input", dest="input", help="Input file")
    parser.add_option("-d", "--degree",
                action="store_true", dest="degree", default=False,
                help="Print score using only degree features")
    parser.add_option("-t", "--triads",
                action="store_true", dest="triads", default=False,
                help="Print score using only triads features")
    parser.add_option("-a", "--all",
                action="store_true", dest="all", default=False,
                help="Print score using all features")
    (options, args) = parser.parse_args()

    df = pd.read_csv(options.input, header=None)
    Xs = np.asarray(df[range(0,23)])
    Ys = np.asarray(df[23])
    if options.degree:
        for em in [0, 10, 25]:
            printScore(Xs, Ys, features_range=(0, 7), em=em, tp="degree")
    if options.triads:
        for em in [0, 10, 25]:
            printScore(Xs, Ys, features_range=(7, 23), em=em, tp="triads")
    if options.all:
        for em in [0, 10, 25]:
            printScore(Xs, Ys, features_range=None, em=em, tp="all")
    