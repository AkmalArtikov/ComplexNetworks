import scipy.misc 
import scipy
scipy.factorial = scipy.misc.factorial

import numpy as np
import pymf
from scipy.sparse import csr_matrix
from optparse import OptionParser

def readData(input_file_name):
	i = []
	j = []
	data = []
	with open(input_file_name) as f:
		for line in f:
			line = map(int, line.split('\t'))
			i.append(line[0])
			j.append(line[1])
			data.append(line[2])
	return csr_matrix((data, (i,j)))

def factorize(matrix):
	bnmf_mdl = pymf.BNMF(matrix.toarray(), num_bases=23)
	bnmf_mdl.factorize(niter=1000)
	return bnmf_mdl.W, bnmf_mdl.H

if __name__ == "__main__":
	parser = OptionParser()
	parser.add_option("-i", "--input", dest="input", help="Input file")

	(options, args) = parser.parse_args()
	matrix = readData(options.input)
	W, H = factorize(matrix)
	H = H.T

	count = 0
	good = 0
	with open(options.input) as f:
		for line in f:
			count += 1
			line = map(int, line.split("\t"))
			if np.sign(np.dot(W[line[0]], H[line[1]])) == line[2]:
				good += 1
	print 1. * good / count
