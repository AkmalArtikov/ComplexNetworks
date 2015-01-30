#include <igraph.h>
#include <iostream>
#include <fstream>

#include <string>

#include <time.h>

#include "graph.cpp"
#include "random.cpp"
#include "model.cpp"
#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ; 
        stm << n ;
        return stm.str() ;
    }
}

void WriteMessage(std::string message, std::ofstream& report, std::ofstream& reportCur)
{
    report << message << std::endl;
    reportCur << message << std::endl;
    std::cout << message << std::endl;
}

void WritePowerLawResults(PowerLawParams powerLawParams, std::ofstream& report, std::ofstream& reportCur)
{
    PowerLawResult resultForGoodVertices = powerLawParams.resultForGoodVertices;
    PowerLawResult resultForBadVertices = powerLawParams.resultForBadVertices;
    PowerLawResult resultForAllVertices = powerLawParams.resultForAllVertices;

    WriteMessage(" ", report, reportCur);
    std::string goodVerticesPowerLaw = "Power-Law with good vertices. Has " + 
                                    patch::to_string(resultForGoodVertices.size) + " vertices ";
    WriteMessage(goodVerticesPowerLaw, report, reportCur);
    goodVerticesPowerLaw = patch::to_string(resultForGoodVertices.alpha) + " " + 
                           patch::to_string(resultForGoodVertices.xmin) + " " +
                           patch::to_string(resultForGoodVertices.logLikehood) + " " + 
                           patch::to_string(resultForGoodVertices.probKS) + " " + 
                           patch::to_string(resultForGoodVertices.pValueKS) + " ";
    WriteMessage(goodVerticesPowerLaw, report, reportCur);
    
    WriteMessage(" ", report, reportCur);
    std::string badVerticesPowerLaw = "Power-Law with bad vertices. Has " + 
                                    patch::to_string(resultForBadVertices.size) + " vertices ";
    WriteMessage(badVerticesPowerLaw, report, reportCur);
    badVerticesPowerLaw = patch::to_string(resultForBadVertices.alpha) + " " + 
                          patch::to_string(resultForBadVertices.xmin) + " " +
                          patch::to_string(resultForBadVertices.logLikehood) + " " + 
                          patch::to_string(resultForBadVertices.probKS) + " " + 
                          patch::to_string(resultForBadVertices.pValueKS) + " ";
    WriteMessage(badVerticesPowerLaw, report, reportCur);

    WriteMessage(" ", report, reportCur);
    std::string allVerticesPowerLaw = "Power-Law with all vertices. Has " + 
                                    patch::to_string(resultForAllVertices.size) + " vertices ";
    WriteMessage(allVerticesPowerLaw, report, reportCur);
    allVerticesPowerLaw = patch::to_string(resultForAllVertices.alpha) + " " + 
                          patch::to_string(resultForAllVertices.xmin) + " " +
                          patch::to_string(resultForAllVertices.logLikehood) + " " + 
                          patch::to_string(resultForAllVertices.probKS) + " " + 
                          patch::to_string(resultForAllVertices.pValueKS) + " ";
    WriteMessage(allVerticesPowerLaw, report, reportCur);
}


void GenerateReport(int size, int dimension, double alpha, double mode, double threshold, bool calc_clusterCoef)
{
    std::ofstream report ("report.txt", std::fstream::out | std::fstream::app);
    std::ofstream reportCur ("results_graphs/report.txt", std::fstream::out | std::fstream::trunc);
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);

    std::string params = "size - " + patch::to_string(size) + "; dimension - " + patch::to_string(dimension) +
                        "; alpha - " + patch::to_string(alpha) + "; mode - " + patch::to_string(mode) + "; threshold - " + 
                        patch::to_string(threshold);
    WriteMessage(params, report, reportCur);

    clock_t t;
    t = clock();

    Model model(size, dimension);

    std::cout << "Generating sphere" << std::endl;
    model.GenerateCoords();

    std::string genWeights = "Generating weights :  Pareto";
    WriteMessage(genWeights, report, reportCur);
    model.GenerateWeights(alpha, mode); 

    std::string genEdges = "Generating edges :  w*w'*(x,x')";
    WriteMessage(genEdges, report, reportCur);
    model.GenerateEdges(threshold);
  
    std::string sizeOfModel = "Graph has " + patch::to_string(model.GetVerticesNumber()) + 
                            " vertices and " + patch::to_string(model.GetEdgesNumber()) + " edges";
    WriteMessage(sizeOfModel, report, reportCur);
 
    std::cout << "Power-Law fitting" << std::endl;
    PowerLawParams powerLawParams = model.PowerLawFit(mode, alpha, threshold);
    WritePowerLawResults(powerLawParams, report, reportCur);

    t = clock() - t;
    float cur_time = ((float)t)/CLOCKS_PER_SEC;

    if (calc_clusterCoef)
    {
        WriteMessage(" ", report, reportCur);
        double clusterCoef = model.GetAverageLocalClusterCoef();
        std::string clusterString = "Graph has Average Local Cluster Coefficient: " + patch::to_string(clusterCoef);
        WriteMessage(clusterString, report, reportCur);

 
        WriteMessage(" ", report, reportCur);
        clusterCoef = model.GetGlobalClusterCoef();
        clusterString = "Graph has Global Cluster Coefficient: " + patch::to_string(clusterCoef);
        WriteMessage(clusterString, report, reportCur);
    }
    
    WriteMessage(" ", report, reportCur);
    std::string modelTime = "It took " + patch::to_string(cur_time) + " seconds";
    WriteMessage(modelTime, report, reportCur);

    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    report << "Current local time and date: "<< asctime(timeinfo) << std::endl;
    reportCur << "Current local time and date: "<< asctime(timeinfo) << std::endl;
    std::cout << "Current local time and date: "<< asctime(timeinfo) << std::endl;

    report << " ----------------------------------------" << std::endl;
    report << std::endl;
    reportCur << std::endl; 

    reportCur.close();
    report.close(); 
}

int main(int argc, char* argv[])
{
    if (argc != 7)
    { 
        std::cout << "Not right args" << std::endl;
        std::cout << "1 arg is the number of vertices" << std::endl;
        std::cout << "2 arg is the dimension of space" << std::endl;
        std::cout << "3 arg is the degree of pareto-distribution" << std::endl;
        std::cout << "4 arg is the mode of pareto-distribution" << std::endl;
        std::cout << "5 arg is the threshold for edges" << std::endl;
        std::cout << "6 arg is 0 if you don't want to compute claster_coefficient" << std::endl;
  
        return -1;
    } 

    long long size = atoi(argv[1]);
    int dimension = atoi(argv[2]);
    double alpha = atof(argv[3]);
    double mode = atof(argv[4]);
    double threshold = atof(argv[5]);
    bool calc_clusterCoef = atoi(argv[6]);   

    GenerateReport(size, dimension, alpha, mode, threshold, calc_clusterCoef);

    return 0;
}

//больший размер?
//exp - странные результаты/ снизить threshold?
//перебрать другие случаи доказанные
//изменение параметроов (размер, размерность, граница, степень распределения) и отслеживание другие (клас. коэф, power-law)
//диаметр графа?

//комулитивную функцию норм посчитать!!! допроверить то, что доказано. Заплить все на сервак (Дима) и считать там. 

//exp / (w*w')???? Power-law наоборот????
