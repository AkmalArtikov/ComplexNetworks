#ifndef GRAPH
#define GRAPH
#include <iostream>

#include <vector>

// Неориентированный граф
class Graph
{
public:
    Graph(int size_ = 10000, int dimension_ = 3): size(size_)
                                                , dimension(dimension_)
                                                , degrees(size_, 0)
                                                , weights(size_)
                                                , coords(size_, std::vector<double>(dimension_))
                                                , edgesNumber(0)
                                                , adjMatrix(size_, std::vector<bool>(size_, false))
    {};

    // Отмечает появление ребра. Обновляет степени вершин
    void AddEdge(int i, int j)
    {
        degrees[i]++;
        degrees[j]++;
        adjMatrix[i][j] = true;
        adjMatrix[j][i] = true;
        edgesNumber++;
    }

    // Устанавливает координаты вершин
    void SetCoords(int vertex, const std::vector<double>& newCoords)
    {
        for (int i = 0; i < dimension; ++i)
        {
            SetCoord(vertex, i, newCoords[i]);
        }
    }

    // Устанавливает веса вершин
    void SetWeights(const std::vector<double>& newWeights)
    {
        for (int i = 0; i < size; ++i)
        {
            SetWeight(i, newWeights[i]);
        }
    }

    // Возвращает массив степеней вершин
    const std::vector<int>& GetDegrees() const
    {
        return degrees;
    }

    // Возвращает вес вершины
    double GetWeight(int vertex) const
    {
        return weights[vertex];
    }

    // Возвращает указанную координату вершины 
    double GetCoords(int vertex, int coord) const
    {
        return coords[vertex][coord];
    }

    // Возвращает количество ребер
    long long GetEdgesNumber() const
    {
        return edgesNumber;
    }

    /* Возвращает усредненный локальный кластерный коэффициент. 
    Считаются кластерные коэфициенты каждой вершины, которые затем усредняются */
    double GetAverageLocalClusterCoef() const
    {
        double result = 0.0;

        for (int i = 0; i < size; ++i)
        {
            result += GetLocalClusterCoef(i);
        }

        return result / size;
    }

    // Возвращает глобальный кластерный коэффициент
    double GetGlobalClusterCoef() const
    {
        double closedTriplets = 0;
        double connectedTriplets = 0;

        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                for (int k = 0; k < size; ++k)
                {
                    if (i != k && i != k && j != k)
                    {
                        int edgesInTriplet = 0;

                        if (adjMatrix[i][j])
                        {
                            edgesInTriplet++;
                        }

                        if (adjMatrix[i][k])
                        {
                            edgesInTriplet++;
                        }

                        if (adjMatrix[j][k])
                        {
                            edgesInTriplet++;
                        }

                        if (edgesInTriplet == 3)
                        {
                            closedTriplets++;
                            connectedTriplets++;
                        }
                        else
                        {
                            if (edgesInTriplet == 2)
                            {
                                connectedTriplets++;
                            }
                        }
                    }
                }
            }
        }

        if (connectedTriplets == 0.0)
        {
            return 0;
        }

        return closedTriplets / connectedTriplets;
    }

private:
    // Устанавливает координату вершины
    void SetCoord(int vertex, int coord, double value)
    {
        coords[vertex][coord] = value;
    }

    // Устанавливает вес вершины
    void SetWeight(int vertex, double weight)
    {
        weights[vertex] = weight;
    }

    // Возвращает локальный кластерный коэффициент вершины
    double GetLocalClusterCoef(int ind) const
    {
        std::vector<int> neighbours;
        int degree = degrees[ind];

        if (degree == 0 || degree == 1)
        {
            return 0.0;
        }

        for (int i = 0; i < size; ++i)
        {
            if (adjMatrix[ind][i])
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

                if (j != i && first != ind && first < second && adjMatrix[first][second])
                {
                    links++;
                }
            }
        }

        double result = (2 * links) / (degree * (degree - 1));

        return result;
    }

    // Количество вершин
    int size;

    // Размерность пространства (количество координат вершины)
    int dimension;

    // Количество ребер
    long long edgesNumber;
 
    // Степени вершин 
    std::vector<int> degrees;
    
    // Массив векторов координат вершин
    std::vector<std::vector<double> > coords;

    // Веса вершин
    std::vector<double> weights;

    // Матрица смежности. vector<bool> работает оптимизированно в c++
    std::vector<std::vector<bool> > adjMatrix;
};

#endif 
