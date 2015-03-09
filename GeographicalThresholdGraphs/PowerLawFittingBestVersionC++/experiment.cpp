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
    myfile << "vertices" << "," << "edges" << "," << "expectedEdges" << ", " << "expectedVarience" << std::endl;
    myfile.close();
}

void print_to_file(const std::string& filename, int vertices, int edges, int expectedEdges, int expectedVarience) {
    std::ofstream myfile;
    myfile.open(filename, std::ios::app);
    myfile << vertices <<","<< edges << "," << expectedEdges << "," << expectedVarience << std::endl;
    myfile.close();
}

float getAverageDegree(int dimension, double alpha, double mode, double threshold) {
    assert(dimension == 3);
    float averageDegree = 0;
    if (threshold < mode * mode) {
        averageDegree = 0.5 - 0.5*alpha*alpha*threshold/(alpha + 1)/(alpha + 1)/mode/mode;
    } else {
        averageDegree = pow(mode, 2.*alpha)/pow(threshold, alpha)/2. * 
            (log(threshold)*alpha/(alpha + 1) - alpha*alpha/(alpha + 1)/(alpha+1) + 1.);
        std::cout << "averageDegree = " << averageDegree << std::endl;
    }
  return averageDegree;
}

long double getTickProbability(int dimension, double alpha, double mode, double threshold) {
    assert(dimension == 3);
    long double tickProbability = 0;
    if (threshold < mode * mode) {
        
    } else {
      tickProbability += 1/4. * pow(mode, 2*alpha)/pow(threshold, 2*alpha)/pow(alpha+1, 2)*(pow(threshold,alpha)-pow(mode, 2*alpha));
      tickProbability += 1/4.* pow(mode, 2*alpha) / pow(threshold, alpha); 
      tickProbability -= 1/2.*pow(alpha, 2)/(alpha+1)*threshold*pow(mode, alpha-1)/(alpha+1)* pow(mode, alpha+1) / pow(threshold, alpha+1);
      tickProbability +=  1/4. *pow(alpha, 3)*pow(threshold, 2)*pow(mode, alpha-2)/pow(alpha+1, 2)/ (alpha + 2)
            * pow(mode, alpha+2) / pow(threshold, alpha+2);
    }
  return tickProbability;
}

int getTheoryNumberEdges(int size, int dimension, double alpha, double mode, double threshold) {
    assert(dimension == 3);
    double averageDegree = getAverageDegree(dimension, alpha, mode, threshold);
    std::cout << "size * averageDegree = " << size * averageDegree << std::endl;
    std::cout << std::endl;
    
    return int(averageDegree * (size) * (size - 1) / 2.);
}

int getTheoryVariance(int size, int dimension, double alpha, double mode, double threshold) {
   long double tickProbability = getTickProbability(dimension, alpha, mode, threshold);
   long double averageDegree = getAverageDegree(dimension, alpha, mode, threshold);
   return size * (size - 1) / 2 * averageDegree * (1 - averageDegree) + (tickProbability - averageDegree*averageDegree) *
      size * (size - 1) * (size - 2) / 2 ;
}

void GenerateReport(int size, int dimension, double alpha, double mode, double threshold, std::string filename) {

    std::string params = "size - " + patch::to_string(size) + "; dimension - " + patch::to_string(dimension) +
                        "; alpha - " + patch::to_string(alpha) + "; mode - " + patch::to_string(mode) + "; threshold - " + 
                        patch::to_string(threshold);

    Model model(size, dimension);   

    model.GenerateCoords();
    double mean = model.GenerateWeights(alpha, mode); 

    model.GenerateEdges(threshold);

    print_to_file(filename, model.GetVerticesNumber(), model.GetEdgesNumber(), 
         getTheoryNumberEdges(size, dimension, alpha, mode, threshold), getTheoryVariance(size, dimension, alpha, mode, threshold));  
}

double CalculateTickProbability(int dimension, double alpha, double mode, double threshold);

double getLinearGrouthThresholder(int size, double alpha, double mode) {
    double EPS = 1e-6;
    assert(fabs(mode - 1.0) < EPS);
    int numberIteration = 1000;
    double result = size * size;
    for (int i = 0; i < numberIteration; ++i) {
      result = 1. * size / 4. * (log(result)*alpha/(alpha + 1) - alpha*alpha/(alpha + 1)/(alpha+1) + 1.);
      result = pow(result, 1./alpha);
    }
    std::cout << "size = " << size << " threshold = " << result << std::endl;
    return result;    
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
        int NUMBER_EXPERIMENTS = 20;

        for (alpha = 1; alpha <= 3; alpha += 1) {

            std::string filename = "alpha_" + patch::to_string(int(alpha)) + ".txt";
            // create_file(filename);

            std::vector<int> sizes = {
             /*10,
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
             2000,
             3000,
             5000,
             6000,
             7000,
             8000,
             9000//,
             // 16000,
             //18000*/
             4000
             };

            for (int size: sizes) {
                for (int experimentID = 0; experimentID < NUMBER_EXPERIMENTS; ++experimentID) {
                  mode = 1;
                  threshold = getLinearGrouthThresholder(size, alpha, mode);
                  GenerateReport(size, dimension, alpha, mode, threshold, filename);
                }
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
  return numberTriangles / numberExperiments;
}


double CalculateTickProbability(int dimension, double alpha, double mode, double threshold) {
  int numberExperiments = 100000;
  double numberTriangles = 0;

  double numberEdges = 0;

  for (int attemp = 0; attemp < numberExperiments; ++attemp) {
    Model model = MakeModel(3, dimension, alpha, mode, threshold, true);
    numberTriangles += model.IsTick(0, 1, 2);
    numberEdges += model.IsEdge(0, 1);
  }
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

    int NUMBER_EXPERIMENTS = 20;
    
    for (size = 3; size < MIDDLE_SIZE; size += MIDDLE_SIZE / NUMBER_POINTS) {
      for (int i = 0; i < NUMBER_EXPERIMENTS; ++i) { 
        threshold = size;
        myfile << size << ", ";
        Model model = MakeModel(size, dimension, alpha, mode, threshold, true);
        double experimentTriangeProbability = CalculateTriangeProbability(dimension, alpha, mode, threshold, 100 * size * size );
        int experimentTriangesNumber = int(experimentTriangeProbability * size * (size - 1) * (size - 2) / 6. );
        myfile << model.GetNumberTriangles() << ", ";
        myfile << experimentTriangesNumber << std::endl;
      }
    }
    for (size = MIDDLE_SIZE; size < MAX_SIZE; size += (MAX_SIZE - MIDDLE_SIZE) / NUMBER_POINTS) {
      for (int i = 0; i < NUMBER_EXPERIMENTS; ++i) { 
        threshold = size;
        myfile << size << ", ";
        double experimentTriangeProbability = CalculateTriangeProbability(dimension, alpha, mode, threshold, size * size);
        int experimentTriangesNumber = int(experimentTriangeProbability * size * (size - 1) * (size - 2) / 6. );
        myfile << ", ";
        myfile << experimentTriangesNumber << std::endl;
      }
    }
    myfile.close();
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);
    
    
    EdgesExperiment(argc, argv);   
    // TrianglesExperiment();
    
    return 0;
}
 