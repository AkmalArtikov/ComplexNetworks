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
    Graph(int size_ = 10000, int dimension_ = 3): size(size_)
                                            , dimension(dimension_)
                                            , degrees(size_, 0)
                                            , weights(size_)
                                            , coords(size_, std::vector<double>(dimension_))
                                            , edges_number(0)
    {};

    void AddEdge(int i, int j)
    {
        degrees[i]++;
        degrees[j]++;
        edges_number++;
    }

    void SetCoord(int i, int j, double coord)
    {
        coords[i][j] = coord;
    }

    void SetWeight(int i, double weight)
    {
        weights[i] = weight;
    }

    const std::vector<int>& GetDegrees() const
    {
        return degrees;
    }

    double GetWeight(int i) const
    {
        return weights[i];
    }

    double GetCoords(int i, int j) const
    {
        return coords[i][j];
    }

    int GetEdgesNumber() const
    {
        return edges_number;
    }

private:
    int size;
    int dimension;
    int edges_number;
    std::vector<int> degrees;
    std::vector<std::vector<double> > coords;
    std::vector<double> weights;
};


class Model
{
public:
    Model(int size_ = 10000, int dimension_ = 3): size(size_)
                                                , dimension(dimension_)
                                                , graph(size_, dimension_)
    {
    }

    void GenerateCoords()
    {
        Normal Z;

        for (int i = 0; i < size; ++i)
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

        for (int i = 0; i < size; ++i)
        {
            igraph_real_t value = P.Next();
            value += mode - 1.0;
            graph.SetWeight(i, value);
        }
    }

    void GenerateEdges(double threshold)
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = i + 1; j < size; ++j)
            {
                double dot = 0.0;

                for (int k = 0; k < dimension; ++k)
                {
                    dot += graph.GetCoords(i, k) * graph.GetCoords(j, k);
                }

                double value = dot * (graph.GetWeight(i) * graph.GetWeight(j));

                if (value >= threshold)
                {
                    graph.AddEdge(i, j);
                }
            }
        }
    }

    void PrintPowerLawFit(double mode, double threshold, std::ofstream& report)
    {
        igraph_vector_t degrees;
        igraph_vector_init(&degrees, size);

        std::vector<int> deg = graph.GetDegrees();

        for (int i = 0; i < size; ++i)
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

        double thresh = threshold / mode;
        int counter = 0;
        int counter_bad = 0;

        for (int i = 0; i < size; ++i)
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
        std::cout << std::endl;

        igraph_power_law_fit(&degrees_cut, &result, -1, 0);
        report << "NOT FULL POWER LAW GOOD VERTICES with " << counter  << " vertices " << std::endl;
        std::cout << "NOT FULL POWER LAW GOOD VERTICE with " << counter  << " vertices " << std::endl;

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << std::endl;

        int x_min = result.xmin;
        int x_min_bad = write_degrees_distrib(degrees_cut, x_min, "temp_good.txt", result.alpha);

        report << "FULL POWER LAW with " << size  << " vertices " << std::endl;
        std::cout << "FULL POWER LAW with " << size  << " vertices " << std::endl;

        igraph_power_law_fit(&degrees, &result, x_min, 0);

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << std::endl;

        write_degrees_distrib(degrees, x_min, "temp.txt", result.alpha);

        igraph_power_law_fit(&degrees_cut_bad, &result, x_min_bad, 0);
        report << "NOT FULL POWER LAW BAD VERTICES with " << counter_bad  << " vertices " << std::endl;
        std::cout << "NOT FULL POWER LAW BAD VERTICES with " << counter_bad  << " vertices " << std::endl;

        report << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;
        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;

        write_degrees_distrib(degrees_cut_bad, result.xmin, "temp_bad.txt", result.alpha);

        igraph_vector_destroy(&degrees);
        igraph_vector_destroy(&degrees_cut);
        igraph_vector_destroy(&degrees_cut_bad);
    }

    int GetVerticesNumber() const
    {
        return size;
    }

    int GetEdgesNumber() const
    {
        return graph.GetEdgesNumber();
    }

private:
    int write_degrees_distrib(const igraph_vector_t &degrees, int min, std::string file_name, double alpha)
    {
        std::ofstream report (file_name.c_str(), std::fstream::in);

        int max = 0;

        int deg_size = igraph_vector_size(&degrees);

        for (int i = 0; i < deg_size; ++i)
        {
            if (VECTOR(degrees)[i] > max)
            {
                max = VECTOR(degrees)[i];
            }
        }

        report << min << " " << max << std::endl;

        int size = max - min;

        std::vector<int> deg_prob(size + 1, 0);

        for (int i = 0; i < deg_size; ++i)
        {
            if (VECTOR(degrees)[i] - min >= 0)
            {
                deg_prob[VECTOR(degrees)[i] - min]++;    
            }
        }

        for (int i = 0; i < deg_prob.size(); ++i)
        {
            report << deg_prob[i] << " ";
        }

        report << std::endl;

        report << alpha;

        report << std::endl;

        report.close();

        return max;
    }

    Graph graph;
    int size;
    int dimension;
};

void generateReport(int size, int dimension, double alpha, double mode, double threshold)
{
    std::ofstream report ("report.txt", std::fstream::out | std::fstream::app);
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);

    report << "size - " << size << "; dimension - " << dimension << "; alpha - " << alpha <<
            "; mode - " << mode << "; threshold - " << threshold << std::endl;
    std::cout << "size - " << size << "; dimension - " << dimension << "; alpha - " << alpha <<
            "; mode - " << mode << "; threshold - " << threshold << std::endl;

    clock_t t;
    t = clock();

    Model graph(size, dimension);

    std::cout << "Generating sphere" << std::endl;

    graph.GenerateCoords();

    std::cout << "Generating weights" << std::endl;

    graph.SetWeights(alpha, mode);

    std::cout << "Generating edges" << std::endl;

    graph.GenerateEdges(threshold);

    report << "Graph has " << graph.GetVerticesNumber() << " vertices and "
            << graph.GetEdgesNumber() << " edges" << std::endl;
    std::cout << "Graph has " << graph.GetVerticesNumber() << " vertices and "
            << graph.GetEdgesNumber() << " edges" << std::endl;

    graph.PrintPowerLawFit(mode, threshold, report);

    t = clock() - t;
    float time = ((float)t)/CLOCKS_PER_SEC;

    report << std::endl;
    std::cout << std::endl;

    report << "It took " << time << " seconds" << std::endl;
    std::cout << "It took " << time << " seconds" << std::endl;

    report << " ----------------------------------------" << std::endl;
    report << std::endl;

    report.close();
}

int main()
{
    int size = 300000;
    int dimension = 3;
    double alpha = 8.0;
    double mode = 0.5;
    double threshold = 0.75;

    generateReport(size, dimension, alpha, mode, threshold);

    return 0;
}

