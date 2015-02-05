#ifndef RANDOM
#define RANDOM

#include "newran02/newran.h"
#include <vector>
#include <math.h>

// Фабрика случайных чисел
class RandomGenerator
{
public:

	// Возвращает равномерно выбранный вектор единичной длины
	static std::vector<double> GenerateUnitNormalVector(int dimension)
	{
		Normal z;
		double radius = 0.0;

        std::vector<double> coords;

        for (int j = 0; j < dimension; ++j)
        {
            double value = z.Next();
            coords.push_back(value);
            radius += ((double) value * (double) value);
        }

        radius = sqrt(radius);

        for (int j = 0; j < dimension; ++j)
        {
        	coords[j] /= radius; 
        }

        return coords;
	}

	// Возвращает вектор величин из Парето распределения с указанными параметрами
	static std::vector<double> GenerateParetoVector(int size, double alpha, double mode)
	{
		Real shape = alpha;
        Pareto p(shape);

        std::vector<double> values(size);

        for (int i = 0; i < size; ++i)
        {
        	values[i] = GeneratePareto(p, mode);
        }

        return values;
	}

private:
	// Возвращает величину из Парето распределения с указанным начальным значением
	static double GeneratePareto(Pareto p, double mode)
	{
        return p.Next() + mode - 1.0;
	}
};

#endif
