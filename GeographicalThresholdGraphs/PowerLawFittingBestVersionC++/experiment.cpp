#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

#include <time.h>

#include "model.cpp"

#include <cassert>
#include <cmath>


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ; 
        stm << n ; 
        return stm.str() ;
    }
}

void create_file(const std::string& filename) {
  std::ofstream myfile;
  myfile.open(filename);
  myfile << "vertices" << "," << "edges" << "," << "expectedEdges" << std::endl;
  myfile.close();
}

void print_to_file(const std::string& filename, int vertices, int edges, int expectedEdges) {
  std::ofstream myfile;
  myfile.open(filename, std::ios::app);
  myfile << vertices <<","<< edges << "," << expectedEdges << std::endl;
  myfile.close();
}

int getTheoryNumberEdges(int size, int dimension, double alpha, double mode, double threshold) {
    assert(dimension == 3);
    float averageDegree = 0;
    if (threshold < mode * mode) {
        averageDegree = 0.5 - 0.5*alpha*alpha*threshold/(alpha + 1)/(alpha + 1)/mode/mode;
    } else {
        averageDegree = pow(mode, 2.*alpha)/pow(threshold, alpha)/2. * 
            (log(threshold)/(alpha + 1) - alpha*alpha/(alpha + 1)/(alpha+1) + 1);
    }
    return int(averageDegree * (size) * (size - 1) / 2.);
}

void GenerateReport(int size, int dimension, double alpha, double mode, double threshold, std::string filename) {
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);

    std::string params = "size - " + patch::to_string(size) + "; dimension - " + patch::to_string(dimension) +
                        "; alpha - " + patch::to_string(alpha) + "; mode - " + patch::to_string(mode) + "; threshold - " + 
                        patch::to_string(threshold);

    clock_t t;
    t = clock();

    Model model(size, dimension);   

    model.GenerateCoords();
    double mean = model.GenerateWeights(alpha, mode); 

    model.GenerateEdges(threshold);
  
    print_to_file(filename, model.GetVerticesNumber(), model.GetEdgesNumber(), 
        getTheoryNumberEdges(size, dimension, alpha, mode, threshold));  
}

void EdgesExperiment(int argc, char* argv[]) {
    if (false)
    { 
        std::cout << "Not right args" << std::endl;
        std::cout << "1 arg is the number of vertices" << std::endl;
        std::cout << "2 arg is the dimension of space" << std::endl;
        std::cout << "3 arg is the degree of pareto-distribution" << std::endl;
        std::cout << "4 arg is the mode of pareto-distribution" << std::endl;
        std::cout << "5 arg is the threshold for edges" << std::endl;
  
        return;
    } 

    long long size;
    int dimension;
    double alpha;
    double mode; 
    double threshold;
    bool calc_clusterCoef;   


    if (argc == 6) {
        long long size = atoi(argv[1]);
        int dimension = atoi(argv[2]);
        double alpha = atof(argv[3]);
        double mode = atof(argv[4]);  
        double threshold = atof(argv[5]);
        GenerateReport(size, dimension, alpha, mode, threshold, "temp.txt");
    } else {
        dimension = 3;

        for (alpha = 1; alpha <= 3; alpha += 1) {

            std::string filename = "alpha_" + patch::to_string(int(alpha)) + ".txt";
            create_file(filename);

            std::vector<int> sizes = {
             10,
             16,
             26,
             42,
             69,
             112,
             183,
             297,
             483,
             784,
             1274,
             2069,
             3359,
             5455,
             8858,
             15000};

            for (int size: sizes) {
                mode = 1;
                threshold = pow(size, 1. / alpha);
                GenerateReport(size, dimension, alpha, mode, threshold, filename);
            }
            std::cout << std::endl;
        }
    }
}

Model MakeModel(int size, int dimension, double alpha, double mode, double threshold, bool calcCluster) {
    Model model(size, dimension, calcCluster);   
    model.GenerateCoords();
    double mean = model.GenerateWeights(alpha, mode); 
    model.GenerateEdges(threshold);
    return model;
}

double CalculateTriangeProbability(int dimension, double alpha, double mode, double threshold, int numberExperiments = 10000) {
  double numberTriangles = 0;
  for (int attemp = 0; attemp < numberExperiments; ++attemp) {
    Model model = MakeModel(3, dimension, alpha, mode, threshold, true);
    numberTriangles += model.GetNumberTriangles();
  }
  // std::cout << "numberTriangles = " << numberTriangles << std::endl;
  return numberTriangles / numberExperiments;
}

void TrianglesExperiment() {

    long long size;
    int dimension;
    double alpha;
    double mode; 
    double threshold;

    dimension = 3;
    alpha = 1;
    mode = 1;

    std::ofstream myfile;
    myfile.open("triagles.txt");
    myfile << "size, number triagles, expected number triagles" << std::endl; 

    int NUMBER_POINTS = 10;
    int MAX_SIZE = 5000;
    int MIDDLE_SIZE = 300;
    
    for (size = 3; size < MIDDLE_SIZE; size += MIDDLE_SIZE / NUMBER_POINTS) {
      threshold = size;
      myfile << size << ", ";
      Model model = MakeModel(size, dimension, alpha, mode, threshold, true);
      double experimentTriangeProbability = CalculateTriangeProbability(dimension, alpha, mode, threshold, 100 * size * size );
      int experimentTriangesNumber = int(experimentTriangeProbability * size * (size - 1) * (size - 2) / 6. );
      myfile << model.GetNumberTriangles() << ", ";
      myfile << experimentTriangesNumber << std::endl;
    }
    for (size = MIDDLE_SIZE; size < MAX_SIZE; size += (MAX_SIZE - MIDDLE_SIZE) / NUMBER_POINTS) {
      threshold = size;
      myfile << size << ", ";
      double experimentTriangeProbability = CalculateTriangeProbability(dimension, alpha, mode, threshold, size * size);
      int experimentTriangesNumber = int(experimentTriangeProbability * size * (size - 1) * (size - 2) / 6. );
      myfile << ", ";
      myfile << experimentTriangesNumber << std::endl;
    }
    myfile.close();
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);
    TrianglesExperiment();   
    return 0;
}
 