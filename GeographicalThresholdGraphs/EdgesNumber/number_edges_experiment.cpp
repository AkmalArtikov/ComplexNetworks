#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

#include <time.h>

#include "model.cpp"

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
    double alpha = 4; 
    double mode = 0.4; 
    double threshold = 0.6;

    long long numberEdges = getNumberEdges(size, dimension, alpha, mode, threshold);

    std::cout << numberEdges;
    return 0;
}
