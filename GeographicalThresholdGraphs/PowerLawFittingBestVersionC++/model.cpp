#ifndef MODEL
#define MODEL

#include <igraph.h>
#include <fstream>

#include <vector>
#include <string>

#include <climits>
#include <iostream>

#include "graph.cpp"
#include "random.cpp"

// Структура результата применения функции power_law_fit
struct PowerLawResult
{
    // Степень power-law
    double alpha;

    // Минимальное значение выборки, с которого считалось power-law
    double xmin;

    // Вероятность выборки принадлежности полученным параметрам
    double logLikehood; 

    // Статистический тест Колмогорова-Смирнова. Чем меньше - тем правдопободнее результат
    double probKS;

    // p-значение статистического теста Колмогорова-Смирнова. Если меньше 0.05 - выборка плохо подходит под результат
    double pValueKS;

    // Длина выборки
    int size;
};

// Структура результатов поиска power-Law в модели с разными выборками
struct PowerLawParams
{
    PowerLawResult resultForGoodVertices;
    PowerLawResult resultForBadVertices;
    PowerLawResult resultForAllVertices;
};

// Рассматриваемая модель
class Model
{
public:
    Model(int _size = 10000, int _dimension = 3, bool _calcCluster = false): size(_size)
                                                                           , dimension(_dimension)
                                                                           , graph(_size, _dimension, _calcCluster)
    {
    }

    // Генерация координат в модели
    void GenerateCoords()
    {
        for (int i = 0; i < size; ++i)
        {
            std::vector<double> coords = RandomGenerator::GenerateUnitNormalVector(dimension);
            for (int j = 0; j < coords.size(); j++) {
            //    std::cout << coords[j] << " ";
            }
            //std::cout << std::endl;
            graph.SetCoords(i, coords);
        }
    }

    // Генерация весов в модели
    double GenerateWeights(double alpha, double mode)
    {
        std::vector<double> weights = RandomGenerator::GenerateParetoVector(size, alpha, mode);
        graph.SetWeights(weights);

        double mean = 0.0;
        for (int i = 0; i < weights.size(); ++i)
        {
            mean += weights[i];
        }
        mean /= weights.size() + 0.0;

        return mean;
    }

    // Генерация ребер в модели
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

                // Получаем взаимодействие между вершинами
                double value = GetSimpleModelInteraction(dot, i, j);

                // Если полученное значение больше границы - доабвляем ребро
                if (value >= threshold)
                {
                    graph.AddEdge(i, j);
                }
            }
        }
    }

    // Возвращает количество вершин
    int GetVerticesNumber() const
    {
        return size;
    }

    // Возвращает количество ребер
    long long GetEdgesNumber() const
    {
        return graph.GetEdgesNumber();
    }

    // Возвращает усредненный локальный кластерный коэффициент
    double GetAverageLocalClusterCoef() const
    {
        return graph.GetAverageLocalClusterCoef();
    }

    // Возвращает среднюю длину кратчайшего пути между вершинами
    double GetAverageShortestPathLength() const
    {
        return graph.GetAverageShortestPathLength();
    }

    // Возвращает глобальный кластерный коэффициент
    double GetGlobalClusterCoef() const
    {
        return graph.GetGlobalClusterCoef();
    }

    int GetNumberTriangles() const 
    {
        return graph.GetNumberTriangles();
    }

    /* Возвращает результат power_law_fit к разным множествам вершин ("хорошие", "плохие", все)
    Записывает распределение вершин во всех 3 случаях в временные файлы для дальнейшей обработки */
    PowerLawParams PowerLawFit(double mode, int paretoDegree, double threshold)
    {
        igraph_vector_t degrees;
        igraph_vector_init(&degrees, size);

        std::vector<int> degs = graph.GetDegrees();

        // Заполняем вектор степеней вершин
        for (long long i = 0; i < size; ++i)
        {
            VECTOR(degrees)[i] = degs[i];
        }

        // Выделяем память на массивы
        igraph_vector_t degreesCutForGood;
        igraph_vector_t degreesCutForBad;
        igraph_vector_init(&degreesCutForGood, 0);
        igraph_vector_init(&degreesCutForBad, 0);
        igraph_vector_reserve(&degreesCutForGood, size);
        igraph_vector_reserve(&degreesCutForBad, size / 2);

        // Получаем условие на разделение вершин на категории
        double thresh = GetExpModelThreshold(threshold, mode);

        int counterGood = 0;
        int counterBad = 0;

        // Разделяем и подсчитываем "хорошие" и  "плохие" вершины
        for (int i = 0; i < size; ++i)
        {
            if (graph.GetWeight(i) <= thresh)
            {
                counterGood++;
                igraph_vector_push_back(&degreesCutForGood, VECTOR(degrees)[i]);
            }
            else
            {
                counterBad++;
                igraph_vector_push_back(&degreesCutForBad, VECTOR(degrees)[i]);
            }
        }

        igraph_plfit_result_t result;
        PowerLawParams powerLawParams;
        int x_min;
        int x_min_bad;

        // Подсчет и запись power-law для "хороших" вершин
        if (counterGood > 0)
        {
            igraph_power_law_fit(&degreesCutForGood, &result, -1, 0);
            FillPowerLawParams(powerLawParams, result, counterGood, 0); 
            x_min = result.xmin;
            x_min_bad = writeDegreeDistribution(degreesCutForGood, x_min, "temp_degrees/temp_good.txt", result.alpha,
                                              paretoDegree, mode, threshold, true);
        }
        else
        {
            x_min = -1;
            x_min_bad = -1;
        }
        
        
        // Подсчет и запись power-law для всех вершин
        igraph_power_law_fit(&degrees, &result, x_min, 0);
        FillPowerLawParams(powerLawParams, result, size, 2);

        writeDegreeDistribution(degrees, x_min, "temp_degrees/temp.txt", result.alpha,
                              paretoDegree, mode, threshold, true);

        // Подсчет и запись power-law для "плохих" вершин
        if (counterBad > 0)
        {
            igraph_power_law_fit(&degreesCutForBad, &result, x_min_bad, 0);
            FillPowerLawParams(powerLawParams, result, counterBad, 1);

            writeDegreeDistribution(degreesCutForBad, result.xmin, "temp_degrees/temp_bad.txt", result.alpha,
                              paretoDegree, mode, threshold, false);    
        }
        
        // Освобождаем память
        igraph_vector_destroy(&degrees);
        igraph_vector_destroy(&degreesCutForGood);
        igraph_vector_destroy(&degreesCutForBad);

        return powerLawParams;
    }

private:
    // Возвращает взаимодействие между вершинами в простой модели
    double GetSimpleModelInteraction(double dot, int i, int j)
    {
        return dot * (graph.GetWeight(i) * graph.GetWeight(j));
    }

    // Возвращает границу разделения на 2 категории вершин в простой модели
    double GetSimpleModelThreshold(double threshold, double mode)
    {
        return threshold / mode;
    }

    // Возвращает взаимодействие между вершинами в экспоненциальной модели
    double GetExpModelInteraction(double dot, int i, int j)
    {
        return exp(dot) * (graph.GetWeight(i) * graph.GetWeight(j));    
    }

    // Возвращает границу разделения на 2 категории вершин в экспоненциальной модели
    double GetExpModelThreshold(double threshold, double mode)
    {
        return threshold / (exp(1) * mode);
    }

    // Возвращает взаимодействие между вершинами в обратно-экспоненциальной модели
    double GetExpReverseModelInteraction(double dot, int i, int j)
    {
        return exp(dot) / (graph.GetWeight(i) * graph.GetWeight(j));    
    }

    // Возвращает границу разделения на 2 категории вершин в обратно-экспоненциальной модели
    double GetExpReverseModelThreshold(double threshold, double mode)
    {
        return (1.0) / (threshold * exp(1) * mode);  
    }

    // Заполнить стуктуру с общим результатом данными, полученными в ходе конкретного эксперимента
    void FillPowerLawParams(PowerLawParams& powerLawParams, igraph_plfit_result_t result, 
                            int size, int type)
    {
        PowerLawResult powerLawResult;

        powerLawResult.alpha = result.alpha;
        powerLawResult.xmin = result.xmin;
        powerLawResult.logLikehood = result.L;
        powerLawResult.probKS = result.D; 
        powerLawResult.pValueKS = result.p;
        powerLawResult.size = size;

        if (type == 0)
        {
            powerLawParams.resultForGoodVertices = powerLawResult;
        }
        else
        {
            if (type == 1)
            {
                powerLawParams.resultForBadVertices = powerLawResult;
            }
            else
            {
                powerLawParams.resultForAllVertices = powerLawResult;
            }
        }
    }

    // Записать степени вершин в файл для дальнейшей обработки
    int writeDegreeDistribution(const igraph_vector_t &degrees, int min, std::string file_name, double alpha, 
                                double paretoDegree, double mode, double threshold, bool calcCommulate)
    {
        std::ofstream report (file_name.c_str(), std::fstream::in);
        int indFolder = file_name.find("/", 0);
        std::string first = file_name.substr(0, indFolder + 1);
        std::string second = "cumulative_" + file_name.substr(indFolder + 1, file_name.length() - indFolder);
        std::string calcCommulateFileName = first + second;
        std::ofstream reportCalcCommulate (calcCommulateFileName.c_str(), std::fstream::in);

        int max = 0;

        int degSize = igraph_vector_size(&degrees);

        //max degree from samples
        for (int i = 0; i < degSize; ++i)
        {

            if (VECTOR(degrees)[i] > max)
            {
                max = VECTOR(degrees)[i];
            }
        }

        report << min << " " << max << std::endl;

        int sizeDegree = max - min;

        //create array for degree frequences
        std::vector<int> degProb(sizeDegree + 1, 0);

  /*      std::vector<int> cutDegrees;

        for (int i = 0; i < degSize; ++i)
        {
            if (VECTOR(degrees)[i] >= min)
            {
                cutDegrees.push_back(VECTOR(degrees)[i]);
            }
        }*/

        //create array for calcCommulate_func
        std::vector<double> calcCommulateDegrees(max, 0.0);

        if (calcCommulate)
        {
            for (int i = 0; i < degSize; ++i)
            {
                int degree = VECTOR(degrees)[i];

                for (int j = 0; j <= degree; ++j)
                {
                    calcCommulateDegrees[j]++;
                }
            }
        }

        for (int i = 0; i < sizeDegree; ++i)
        {
            if (VECTOR(degrees)[i] - min >= 0)
            {
                degProb[VECTOR(degrees)[i] - min]++;    
            }
        }

        if (calcCommulate)
        {
            for (int i = 0; i < calcCommulateDegrees.size(); ++i)
            {
                calcCommulateDegrees[i] /= degSize;
                reportCalcCommulate << calcCommulateDegrees[i] << " ";
            }
        }

        for (int i = 0; i <  degProb.size(); ++i)
        {
            report << degProb[i] << " ";
        }

        report << std::endl;

        report << alpha << " " << size << " " << dimension << " " << paretoDegree << " "
               << mode << " " << threshold;

        report << std::endl;

        report.close();
        reportCalcCommulate.close();

        return max;
    }

    // Неориентированный граф
    Graph graph;

    // Количество вершин
    int size;

    // Размерность пространства
    int dimension;
};

#endif