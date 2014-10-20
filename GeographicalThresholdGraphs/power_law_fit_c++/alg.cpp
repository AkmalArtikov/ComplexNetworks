#include <igraph.h>
#include <iostream>
#include <math.h>
#include "newran02/newran.h"
#include <time.h>

class Network
{
public:
    Network(int size_ = 10000, int dimension_ = 3): size(size_), dimension(dimension_)
    {
        igraph_empty(&graph, size, IGRAPH_UNDIRECTED);
        igraph_matrix_init(&coords, size, dimension);
        igraph_vector_init(&weights, size);
/*
        igraph_vector_t edges;
        igraph_vector_init(&edges, 2);
        igraph_vector_push_back(&edges, 0);
        igraph_vector_push_back(&edges, 1);
        igraph_add_edges(&graph, &edges, 0);

        igraph_vector_destroy(&edges);*/
    }

    ~Network()
    {
        igraph_destroy(&graph);
        igraph_matrix_destroy(&coords);
        igraph_vector_destroy(&weights);
    }

    void GenerateCoords()
    {
        Normal Z;

        for (int i = 0; i < size; ++i)
        {
            double radius = 0.0;

            for (int j = 0; j < dimension; ++j)
            {
                igraph_real_t value = Z.Next();
                igraph_matrix_set(&coords, i, j, value);
                radius += ((double) value * (double) value);
            }

            radius = sqrt(radius);

            for (int j = 0; j < dimension; ++j)
            {
                igraph_matrix_set(&coords, i, j, igraph_matrix_e(&coords, i, j) / radius);
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
            igraph_vector_set(&weights, i, value);

          //  std::cout << value << " ";

        }
    }

    void GenerateEdges(double threshold)
    {
        igraph_vector_t edges;
        igraph_vector_init(&edges, 0);
        igraph_vector_reserve(&edges, 10000000);

        int counter = 0;

        for (int i = 0; i < size; ++i)
        {
            for (int j = i + 1; j < size; ++j)
            {
                double dot = 0.0;

                for (int k = 0; k < dimension; ++k)
                {
                    dot += igraph_matrix_e(&coords, i, k) * igraph_matrix_e(&coords, j, k);
                }

                double value = dot * (igraph_vector_e(&weights, i) * igraph_vector_e(&weights, j));

                if (value >= threshold)
                {
                    igraph_vector_push_back(&edges, i);
                    igraph_vector_push_back(&edges, j);
                    counter += 2;

//                    std::cout << igraph_vector_e(&weights, i) << " " << igraph_vector_e(&weights, j) << " " << dot << std::endl;

                    if (counter > 9990000)
                    {
                        igraph_add_edges(&graph, &edges, 0);
                        igraph_vector_destroy(&edges);
                        igraph_vector_init(&edges, 0);
                        igraph_vector_reserve(&edges, 10000000);
                        counter = 0;
                    }
                }
            }
        }

        igraph_add_edges(&graph, &edges, 0);

        igraph_vector_destroy(&edges);
    }

    void PowerLawFit(double mode, double threshold)
    {
        igraph_vs_t vertices;
        igraph_vector_t degrees;
        igraph_vector_init(&degrees, 0);
        igraph_vs_all(&vertices);

        igraph_degree(&graph, &degrees, vertices, IGRAPH_IN, IGRAPH_NO_LOOPS);

        igraph_plfit_result_t result;

        igraph_vector_t degrees_cut;
        igraph_vector_init(&degrees_cut, 0);
        igraph_vector_reserve(&degrees_cut, size);

        double thresh = threshold / mode;
        int counter = 0;

        for (int i = 0; i < size; ++i)
        {
            if (VECTOR(weights)[i] <= thresh)
            {
                counter++;
                igraph_vector_push_back(&degrees_cut, VECTOR(degrees)[i]);
            }
        }

        std::cout << counter << std::endl;

        /*for (int i = 0; i < size; ++i)
        {
            std::cout << VECTOR(degrees)[i] << " ";
        }*/
        std::cout << "FULL POWER LAW" << std::endl;

        igraph_power_law_fit(&degrees, &result, -1, 0);

        std::cout << "FULL POWER LAW" << std::endl;

        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;

        /*for (int i = 0; i < size; ++i)
        {
            std::cout << VECTOR(degrees_cut)[i] << " ";
        }*/
        std::cout << "NOT FULL POWER LAW" << std::endl;

        igraph_power_law_fit(&degrees_cut, &result, -1, 0);
        std::cout << "NOT FULL POWER LAW" << std::endl;

        std::cout << result.alpha << " " << result.xmin << " " << result.D << " " << result.p << std::endl;

        igraph_vector_destroy(&degrees);
        igraph_vector_destroy(&degrees_cut);
    }

    void PrintCoords(int row) const
    {
        for (int j = 0; j < dimension; ++j)
        {
            std::cout << igraph_matrix_e(&coords, row, j) << " ";
        }
        std::cout << std::endl;
    }

    int GetVerticesNumber() const
    {
        return (int) igraph_vcount(&graph);
    }

    int GetEdgesNumber() const
    {
        return (int) igraph_ecount(&graph);
    }

private:
    igraph_t graph;
    igraph_integer_t size;
    igraph_integer_t dimension;
    igraph_matrix_t coords;
    igraph_vector_t weights;
};

int main()
{
    srand(time(NULL));
    Random::Set(((float) rand()) / (float) RAND_MAX);

    int size = 40000;
    int dimension = 3;
    double alpha = 4.0;
    double mode = 0.5;
    double threshold = 0.75;

    Network graph(size, dimension);

    std::cout << "Generating sphere" << std::endl;

    graph.GenerateCoords();

    std::cout << "Generating weights" << std::endl;

    graph.SetWeights(alpha, mode);

    std::cout << "Generating edges" << std::endl;

    graph.GenerateEdges(threshold);

    std::cout << graph.GetVerticesNumber() << " vertices and " << graph.GetEdgesNumber() << " edges" << std::endl;

    graph.PowerLawFit(mode, threshold);

    return 0;
}


//TODO: report, check ideas (change alpha, threshold, mode)