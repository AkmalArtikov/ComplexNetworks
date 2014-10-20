#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iterator>

template<class OptFunction>
class NumericalDerivative {
private:
	OptFunction funct;
	const double epsilon;
public:
	NumericalDerivative(const OptFunction &f) : funct(f), epsilon(10.0e-5) {}

	std::vector<double> operator()(const std::vector<double> &point, const std::vector<size_t> &variables) {
		std::vector<double> result(funct.getDimension(), 0);
		for (std::vector<size_t>::const_iterator i = variables.begin(), end = variables.end(); i != end; ++i) {
			std::vector<double> firstPoint(point);
			firstPoint[*i] += epsilon;
			std::vector<double> secondPoint(point);
			secondPoint[*i] -= epsilon;
			result[*i] = (funct(firstPoint) - funct(secondPoint)) / (2 * epsilon);
		}
		return result;
	}
};


template<class OptFunction, class OptFunctionDerivative>
class SGDOptimizer {
private:
	OptFunction funct;
	OptFunctionDerivative deriv;
	std::vector<std::vector<double> > parameters;
	std::vector<double> currentPoint;
	double alpha;
	size_t objectsCount;
	bool fractStep;
public:
	explicit SGDOptimizer(const OptFunction &f, const OptFunctionDerivative &d, const std::vector<std::vector<double> > &params, double alp, size_t oCount, bool fractStp = false) : 
				funct(f), deriv(d), parameters(params), alpha(alp), objectsCount(oCount), fractStep(fractStp) {
		for (std::vector<std::vector<double> >::const_iterator i = params.begin(), end = params.end(); i != end; ++i) {
			std::copy(i->begin(), i->end(), std::inserter(currentPoint, currentPoint.end()));
		}
		srand(time(NULL));
	}
	
	void optimize(size_t steps) {
		std::vector<size_t> indices;
		for (size_t i = 0; i < objectsCount; ++i) {
			indices.push_back(i);
		}
		for (size_t s = 0; s < steps; ++s) {
			std::vector<size_t> shuffled(indices);
			std::random_shuffle(shuffled.begin(), shuffled.end());
			for (size_t it = 0; it < shuffled.size(); ++it) {
				size_t index = shuffled[it];
				std::vector<double> grad = deriv(currentPoint, index);
				for (size_t i = 0; i < grad.size(); ++i) {
					if (fractStep) {
						currentPoint[i] -= alpha * grad[i] / (s + 1);
					} else {
						currentPoint[i] -= alpha * grad[i];
					}
				}
				if (it % 1000 == 0) {
					std::cout << "Epoch: " << s << ". Object: " << it << ". Current value: " << funct(currentPoint) << std::endl;
				}
			}
		}
	}

	const std::vector<std::vector<double> >& getParameters() {
		size_t currentIndex = 0;
		for (size_t i = 0; i < parameters.size(); ++i) {
			for (size_t j = 0; j < parameters[i].size(); ++j) {
				parameters[i][j] = currentPoint[currentIndex];
				++currentIndex;
			}
		}
		return parameters;
	}
};
