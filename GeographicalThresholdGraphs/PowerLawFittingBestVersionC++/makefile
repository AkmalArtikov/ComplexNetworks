all: test
test: experiment.cpp newran.o myexcept.o extreal.o
	g++ newran.o myexcept.o extreal.o model.cpp graph.cpp random.cpp -o test experiment.cpp -I/usr/local/include/igraph -L/usr/local/lib -ligraph

newran.o: newran02/newran.cpp
	g++ -g -w -c newran02/newran.cpp

myexcept.o: newran02/myexcept.cpp
	g++ -g -w -c newran02/myexcept.cpp

extreal.o: newran02/extreal.cpp
	g++ -g -w -c newran02/extreal.cpp

clean:
	rm -rf *o test