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
	NumericalDerivative() : epsilon(10.0e-5) {}

	std::vector<double> operator()(const std::vector<double> &point, const std::vector<size_t> &variables);
};


template<class OptFunction, class OptFunctionDerivative = NumericalDerivative<OptFunction> >
class SGDOptimizer {
private:
	std::vector<std::vector<double> > parameters;
	std::vector<double> currentPoint;
	OptFunction funct;
	OptFunctionDerivative deriv;
	double alpha;
	bool fractStep;
public:
	SGDOptimizer(const std::vector<std::vector<double> > &params, double alp, bool fractStp = false) : 
				parameters(params), alpha(alp), fractStep(fractStp) {
		for (std::vector<std::vector<double> >::const_iterator i = params.begin(), end = params.end(); i != end; ++i) {
			std::copy(i->begin(), i->end(), std::inserter(currentPoint, currentPoint.end()));
		}
		srand(time(NULL));
	}
	
	void optimize(size_t steps);

	const std::vector<std::vector<double> >& getParameters();
};
