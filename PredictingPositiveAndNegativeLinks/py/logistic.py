import pandas as pd
import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn import cross_validation

if __name__ == "__main__":
	df = pd.read_csv("epinion-features.csv", header=None)
	Xs = np.asarray(df[range(0,7)])
	Ys = np.asarray(df[7])

	X_train, X_test, y_train, y_test = cross_validation.train_test_split(Xs, Ys, test_size=0.4, random_state=0)
	
	model = LogisticRegression()
	model.fit(X_train, y_train)
	print model.score(X_test, y_test)