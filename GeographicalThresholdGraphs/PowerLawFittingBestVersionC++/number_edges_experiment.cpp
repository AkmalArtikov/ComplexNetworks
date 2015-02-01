#include <igraph.h>
#include <iostream>
#include <fstream>

#include <string>

#include <time.h>

#include "graph.cpp"
#include "random.cpp"
#include "model.cpp"
#include <sstream>

long long getNumberEdges(int size, int dimension, double alpha, double mode, double threshold)
{
    Model model(size, dimension);
    model.GenerateCoords();
    model.GenerateWeights(alpha, mode); 
    model.GenerateEdges(threshold);
  
    return model.GetEdgesNumber();
}

int main(int argc, char* argv[])
{
    long long size = 10000; 
    int dimension = 3; 
    double alpha = 2; 
    double mode = 23; 
    double threshold = 2;

    long long numberEdges = getNumberEdges(size, dimension, alpha, mode, threshold);

    std::cout << numberEdges;
    return 0;
}
