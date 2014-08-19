from igraph import *
import pickle
import numpy as np


def namesOfVertices(g):
	return map(lambda x: x['name'], g.vs)

def wikiDataToGraph(path_to_file):
	g = Graph()
	lines_count = 0
	names = set()
	with open(path_to_file) as wiki_file:
		current_user = None
		for line in wiki_file:
			if lines_count % 1000 == 0:
				print 'Wiki: ' + str(lines_count) + " are processed."
			lines_count += 1
			if line[0] == 'U':
				current_user = line.split('\t')[1]
				if current_user not in names:
					g.add_vertex(current_user)
					names.add(current_user)
			elif line[0] == 'V':
				v_user = line.split('\t')[2]
				v_rating = int(line.split('\t')[1])
				if v_rating == 0:
					continue
				else:
					if v_user not in names:
						g.add_vertex(v_user)
						names.add(v_user)
					g.add_edge(v_user, current_user, sign=v_rating)
	return g

def epinionsOrSlashdotDataToGraph(path_to_file):
	g = Graph()
	lines_count = 0
	names = set()
	with open(path_to_file) as epinions_file:
		for line in epinions_file:
			if lines_count % 1000 == 0:
				print 'Epinions: ' + str(lines_count) + " are processed."
			lines_count += 1
			if line.startswith('#'):
				continue
			splitted = line.split('\t')
			from_user = splitted[0]
			to_user = splitted[1]
			sign = splitted[2]
			if from_user not in names:
				g.add_vertex(from_user)
				names.add(from_user)
			if to_user not in names:
				g.add_vertex(to_user)
				names.add(to_user)
			g.add_edge(from_user, to_user, sign=sign)
	return g

def graphToFeatures(g):
	Xs = []
	Ys = []
	count = 0
	for e in g.es[:2000]:
		if count % 1000 == 0:
			print str(count) + " edges are processed."
		count += 1
		u, v = e.tuple
		tmp = []
		tmp.append(len(filter(lambda x: x['sign'] == 1, g.es.select(_target=v))))
		tmp.append(len(filter(lambda x: x['sign'] == -1, g.es.select(_target=v))))
		tmp.append(len(filter(lambda x: x['sign'] == 1, g.es.select(_source=u))))
		tmp.append(len(filter(lambda x: x['sign'] == -1, g.es.select(_source=u))))

		tmp.append(len(set(map(lambda x: x.index, g.vs[u].neighbors())) \
			.intersection(set(map(lambda x: x.index, g.vs[v].neighbors())))))
		tmp.append(g.vs[u].outdegree())
		tmp.append(g.vs[v].indegree())
		tmp = np.asarray(tmp)
		Xs.append(tmp)
		Ys.append(e['sign'])
	return np.asarray(Xs), np.asarray(Ys)


if __name__ == '__main__':
	#g = wikiDataToGraph('wikiElec.ElecBs3.txt')
	#print "Data loaded!"
	#with open('Graph.model', 'w') as f:
	#	pickle.dump(g, f)
	#g = None
	#with open('Graph.model') as model_file:
	#	g = pickle.load(model_file)
	#Xs, Ys = graphToFeatures(g)
	#with open("Xs.data", 'w') as Xs_file, open("Ys.data", 'w') as Ys_file:
	#	pickle.dump(Xs, Xs_file)
	#	pickle.dump(Ys, Ys_file)
	g = epinionsOrSlashdotDataToGraph('data/raw/soc-sign-epinions.txt')
	with open('EpinionsGraph.pickle') as f:
		pickle.dump(g, f)
	print len(g.vs)
	print len(g.es)
