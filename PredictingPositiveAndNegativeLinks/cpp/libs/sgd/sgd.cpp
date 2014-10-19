#include <sgd.h>

std::vector<double> SGDOptimizer::operator()(const std::vector<double> &point, const std::vector<size_t> &variables) {
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

	
void SGDOptimizer::optimize(size_t steps) {
	for (size_t s = 0; s < steps; ++s) {
		size_t index = rand() % parameters.size();
		size_t firstNumber = 0;
		for (size_t i = 0; i < index; ++i) {
			firstNumber += parameters[i].size();
		}
		std::vector<size_t> variables;
		for (size_t i = 0; i < parameters[index].size(); ++i) {
			variables.push_back(firstNumber + i);
		}
		std::vector<double> grad = deriv(currentPoint, variables);
		for (size_t i = 0; i < grad.size(); ++i) {
			if (fractStep) {
				currentPoint[i] -= alpha * grad[i] / (s + 1);
			} else {
				currentPoint[i] -= alpha * grad[i];
			}
		}
		for (size_t i = firstNumber; i < firstNumber + parameters[index].size(); ++i) {
			if (fractStep) {
				parameters[index][i - firstNumber] -= alpha * grad[i] / (s + 1);
			} else {
				parameters[index][i - firstNumber] -= alpha * grad[i];
			}
			std::cout << "Current value: " << funct(currentPoint) << std::endl;
		}
	}
}

const std::vector<std::vector<double> >& SGDOptimizer::getParameters() {
	return parameters;
}
