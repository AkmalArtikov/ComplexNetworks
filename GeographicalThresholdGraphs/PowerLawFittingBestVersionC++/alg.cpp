#include <igraph.h>
#include <iostream>
#include <math.h>
#include "newran02/newran.h"
#include <time.h>
#include <vector>
#include <fstream>
#include <climits>
#include <string>

class Graph
{
public:
    Graph(long long size_ = 10000, int dimension_ = 3): size(size_)
                                            , dimension(dimension_)
                                            , degrees(size_, 0)
                                            , weights(size_)
                                            , coords(size_, std::vector<double>(dimension_))
                                            , edges_number(0)
                                            , adj_matrix(size_, std::vector<bool>(size_, false))
    {};

    void AddEdge(long long i, long long j)
    {
        degrees[i]++;
        degrees[j]++;
        adj_matrix[i][j] = true;
        adj_matrix[j][i] = true;
        edges_number++;
    }

    void SetCoord(long long i, long long j, double coord)
    {
        coords[i][j] = coord;
    }

    void SetWeight(long long i, double weight)
    {
        weights[i] = weight;
    }

    const std::vector<int>& GetDegrees() const
    {
        return degrees;
    }

    double GetWeight(long long i) const
    {
        return weights[i];
    }

    double GetCoords(long long i, long long j) const
    {
        return coords[i][j];
    }

    long long GetEdgesNumber() const
    {
        return edges_number;
    }

    double GetClusterCoef() const
    {
        double result = 0.0;

        for (int i = 0; i < size; ++i)
        {
            result += GetClusterCoef(i);
        }

        return result / size;
    }

private:
    double GetClusterCoef(int ind) const
    {
        std::vector<int> neighbours;

        for (int i = 0; i < size; ++i)
        {
            if (adj_matrix[ind][i])
            {
                neighbours.push_back(i);
            }
        }

        int links = 0;

        for (int i = 0; i < neighbours.size(); ++i)
        {
            for (int j = 0; j < neighbours.size(); ++j)
            {
                int first = neighbours[i];
                int second = neighbours[j];

                if (j != i && first != ind && first < second && adj_matrix[first][second])
                {
                    links++;
                }
            }
        }

        int degree = degrees[ind];
        double result = (2 * links) / (degree * (degree - 1));

        return result;
    }

    long long size;
    int dimension;
    long long edges_number;
    std::vector<int> degrees;
    std::vector<std::vector<double> > coords;
    std::vector<double> weights;
    std::vector<std::vector<bool> > adj_matrix;
};


class Model
{
public:
    Model(long long size_ = 10000, int dimension_ = 3): size(size_)
                                                , dimension(dimension_)
                                                , graph(size_, dimension_)
    {
    }

    void GenerateCoords()
    {
        Normal Z;

        for (long long i = 0; i < size; ++i)
        {
            double radius = 0.0;

            std::vector<double> coords;

            for (int j = 0; j < dimension; ++j)
            {
                double value = Z.Next();
                coords.push_back(value);
                radius += ((double) value * (double) value);
            }

            radius = sqrt(radius);

            for (int j = 0; j < dimension; ++j)
            {
                graph.SetCoord(i, j, coords[j] / radius);
            }
        }
    }

    void SetWeights(double alpha, double mode)
    {
        Real shape = alpha;
        Pareto P(shape);

        for (long long i = 0; i < size; ++i)
        {
            igraph_real_t value = P.Next();
            value += mode - 1.0;
            graph.SetWeight(i, value);
        }
    }

    void GenerateEdges(double threshold)
    {
        for (long long i = 0; i < size; ++i)
        {
            for (long long j = i + 1; j < size; ++j)
            {
                double dot = 0.0;

                for (int k = 0; k < dimension; ++k)
                {
                    dot += graph.GetCoords(i, k) * graph.GetCoords(j, k);
                }

                // w*w'*(x,x')
                double value = dot * (graph.GetWeight(i) * graph.GetWeight(j));

                // w*w'*e^(x,x')
                //double value = exp(dot) * (graph.GetWeight(i) * graph.GetWeight(j));    

                if (value >= threshold)
                {
                    graph.AddEdge(i, j);
                }
            }
        }
    }

    void PrintPowerLawFit(double mode, int pareto_degree, double threshold, std::ofstream& report, std::ofstream& report_cur)
    {
        igraph_vector_t degrees;
        igraph_vector_init(&degrees, size);

        std::vector<int> deg = graph.GetDegrees();

        //degrees contains degrees of each vertex
        for (long long i = 0; i < size; ++i)
        {
            VECTOR(degrees)[i] = deg[i];
        }

        igraph_plfit_result_t result;

        igraph_vector_t degrees_cut;
        igraph_vector_t degrees_cut_bad;
        igraph_vector_init(&degrees_cut, 0);
        igraph_vector_init(&degrees_cut_bad, 0);
        igraph_vector_reserve(&degrees_cut, size);
        igraph_vector_reserve(&degrees_cut_bad, size / 2);

        // w*w'*(x,x')
        double thresh = threshold / mode;

        // w*w'*e^(x,x')
        //double thresh = threshold / (exp(1) * mode);        
        
        long long counter = 0;
        long long counter_bad = 0;

        for (long long i = 0; i < size; ++i)
        {
            if (graph.GetWeight(i) <= thresh)
            {
                counter++;
                igraph_vector_push_back(&degrees_cut, VECTOR(degrees)[i]);
            }
            else
            {
                counter_bad++;
                igraph_vector_push_back(&degrees_cut_bad, VECTOR(degrees)[i]);
            }
        }

        report << std::endl;
        report_cur << std::endl;
        std::cout << std::endl;

        igraph_power_law_fit(&degrees_cut, &result, -1, 0);
        
        report << "NOT FULL POWER LAW GOOD VERTICES with " << counter  << " vertices " << std::endl;
        report_cur << "NOT FULL POWER LAW GOOD VERTICES with " << counter  << " vertices " << std::endl;
        std::cout << "NOT FULL POWER LAW GOOD VERTICE with " << counter  << " vertices " << std::endl;

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        report_cur << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << std::endl;

        int x_min = result.xmin;
        int x_min_bad = write_degrees_distrib(degrees_cut, x_min, "temp_degrees/temp_good.txt", result.alpha,
                                              pareto_degree, mode, threshold, false);

        report << "FULL POWER LAW with " << size  << " vertices " << std::endl;
        report_cur << "FULL POWER LAW with " << size  << " vertices " << std::endl;
        std::cout << "FULL POWER LAW with " << size  << " vertices " << std::endl;

        igraph_power_law_fit(&degrees, &result, x_min, 0);

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        report_cur << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << std::endl;

        write_degrees_distrib(degrees, x_min, "temp_degrees/temp.txt", result.alpha,
                              pareto_degree, mode, threshold, true);

        igraph_power_law_fit(&degrees_cut_bad, &result, x_min_bad, 0);
        report << "NOT FULL POWER LAW BAD VERTICES with " << counter_bad  << " vertices " << std::endl;
        report_cur << "NOT FULL POWER LAW BAD VERTICES with " << counter_bad  << " vertices " << std::endl;
        std::cout << "NOT FULL POWER LAW BAD VERTICES with " << counter_bad  << " vertices " << std::endl;

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        report_cur << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;

        write_degrees_distrib(degrees_cut_bad, result.xmin, "temp_degrees/temp_bad.txt", result.alpha,
                              pareto_degree, mode, threshold, false);

        igraph_vector_destroy(&degrees);
        igraph_vector_destroy(&degrees_cut);
        igraph_vector_destroy(&degrees_cut_bad);
    }

    long long GetVerticesNumber() const
    {
        return size;
    }

    long long GetEdgesNumber() const
    {
        return graph.GetEdgesNumber();
    }

    double GetClusterCoef() const
    {
        return graph.GetClusterCoef();
    }

private:
    int write_degrees_distrib(const igraph_vector_t &degrees, int min, std::string file_name, double alpha, 
                              double pareto_degree, double mode, double threshold, bool commute)
    {
        std::ofstream report (file_name.c_str(), std::fstream::in);
        int ind_folder = file_name.find("/", 0);
        std::string first = file_name.substr(0, ind_folder + 1);
        std::string second = "commute_" + file_name.substr(ind_folder + 1, file_name.length() - ind_folder);
        std::string commute_file_name = first + second;
        std::ofstream report_commute (commute_file_name.c_str(), std::fstream::in);

        int max = 0;

        long long deg_size = igraph_vector_size(&degrees);

        //max degree from samples
        for (long long i = 0; i < deg_size; ++i)
        {

            if (VECTOR(degrees)[i] > max)
            {
                max = VECTOR(degrees)[i];
            }
        }

        report << min << " " << max << std::endl;

        long long size_degree = max - min;

        //create array for degree frequences
        std::vector<long long> deg_prob(size_degree + 1, 0);

        //create array for commute_func
        std::vector<double> commute_degrees(deg_size + 1, 0.0);

        if (commute)
        {
            for (int i = 0; i < deg_size; ++i)
            {
                long long degree = VECTOR(degrees)[i];

                for (int i = 0; i <= degree; i++)
                {
                    commute_degrees[i]++;
                }
            }
        }

        for (int i = 0; i < size_degree; ++i)
        {
            if (VECTOR(degrees)[i] - min >= 0)
            {
                deg_prob[VECTOR(degrees)[i] - min]++;    
            }
            
        }

        if (commute)
        {
            for (long long i = 0; i < deg_size; ++i)
            {
                commute_degrees[i] /= deg_size;
                report_commute << commute_degrees[i] << " ";
            }
        }

        for (long long i = 0; i <  deg_prob.size(); ++i)
        {
            report << deg_prob[i] << " ";
        }

        report << std::endl;

        report << alpha << " " << size << " " << dimension << " " << pareto_degree << " "
               << mode << " " << threshold;

        report << std::endl;

        report.close();
        report_commute.close();

        return max;
    }

    Graph graph;
    int size;
    int dimension;
};

void generateReport(int size, int dimension, double alpha, double mode, double threshold)
{
    std::ofstream report ("report.txt", std::fstream::out | std::fstream::app);
    std::ofstream report_cur ("results_graphs/report.txt", std::fstream::out | std::fstream::trunc);
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);

    report << "size - " << size << "; dimension - " << dimension << "; alpha - " << alpha <<
            "; mode - " << mode << "; threshold - " << threshold << std::endl;
    report_cur << "size - " << size << "; dimension - " << dimension << "; alpha - " << alpha <<
            "; mode - " << mode << "; threshold - " << threshold << std::endl;
    std::cout << "size - " << size << "; dimension - " << dimension << "; alpha - " << alpha <<
            "; mode - " << mode << "; threshold - " << threshold << std::endl;

    clock_t t;
    t = clock();

    Model graph(size, dimension);

    std::cout << "Generating sphere" << std::endl;

    graph.GenerateCoords();

    std::string gen_weights = "Generating weights :  Pareto";
    std::string gen_edges = "Generating edges :  ww'*(x,x')";

    report << gen_weights << std::endl;
    report_cur << gen_weights << std::endl;
    std::cout << gen_weights << std::endl;    

    graph.SetWeights(alpha, mode);

    report << gen_edges << std::endl;
    report_cur << gen_edges << std::endl;
    std::cout << gen_edges << std::endl;

    graph.GenerateEdges(threshold);

    report << "Graph has " << graph.GetVerticesNumber() << " vertices and "
            << graph.GetEdgesNumber() << " edges" << std::endl;
    report_cur << "Graph has " << graph.GetVerticesNumber() << " vertices and "
            << graph.GetEdgesNumber() << " edges" << std::endl;
    std::cout << "Graph has " << graph.GetVerticesNumber() << " vertices and "
            << graph.GetEdgesNumber() << " edges" << std::endl;

    graph.PrintPowerLawFit(mode, alpha, threshold, report, report_cur);

    t = clock() - t;
    float cur_time = ((float)t)/CLOCKS_PER_SEC;

    report << std::endl;
    report_cur << std::endl;
    std::cout << std::endl;

    double cluster_coef = graph.GetClusterCoef();

    report << "Graph has Cluster Coefficient: " << cluster_coef << std::endl;
    report_cur << "Graph has Cluster Coefficient: " << cluster_coef << std::endl;
    std::cout << "Graph has Cluster Coefficient: " << cluster_coef << std::endl;

    report << std::endl;
    report_cur << std::endl;
    std::cout << std::endl;

    report << "It took " << cur_time << " seconds" << std::endl;
    report_cur << "It took " << cur_time << " seconds" << std::endl;
    std::cout << "It took " << cur_time << " seconds" << std::endl;

    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    report << "Current local time and date: " << asctime(timeinfo) <<std::endl;
    report_cur << "Current local time and date: " << asctime(timeinfo) <<std::endl;
    std::cout << "Current local time and date: " << asctime(timeinfo) <<std::endl;

    report << " ----------------------------------------" << std::endl;
    report << std::endl;
    report_cur << std::endl;

    report_cur.close();
    report.close();
}

int main()
{
    long long size = 300000;
    int dimension = 3;
    double alpha = 6;
    double mode = 0.4;
    double threshold = 0.6;

    generateReport(size, dimension, alpha, mode, threshold);

    return 0;
}

//exp - странные результаты/ снизить threshold?
//перебрать другие случаи доказанные
//изменение параметроов (размер, размерность, граница, степень распределения) и отслеживание другие (клас. коэф, power-law)
//диаметр графа?
