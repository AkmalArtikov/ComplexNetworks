#include <iostream>
#include <stdexcept>
#include <cstdlib> 
#include <ctime> 

#include "sgd/sgd.hpp"
#include "ezOptionParser/ezOptionParser.hpp"

#define UNUSED(x) (void)(x)

using namespace ez;

class MFCostFunction {
private:
    std::vector<std::vector<int> > objects;
    size_t count;
    size_t maxIndex;
public:
    MFCostFunction(const std::vector<std::vector<int> > &obj, size_t c, size_t m) : objects(obj), count(c), maxIndex(m) {}

    double operator()(const std::vector<double> &point) {
        double result = 0;
        for (size_t i = 0; i < objects.size(); ++i) {
            double tmp = 0;
            for (size_t j = 0; j < count; ++j) {
                tmp += point[objects[i][0] * count + j] * point[(maxIndex + 1) * count + objects[i][1] * count + j];
            }
            tmp = (objects[i][2] - tmp) * (objects[i][2] - tmp);
            result += tmp;
        }
        return result;
    }

    size_t getDimension() {
        return maxIndex * count * 2;
    }
};

class MFCostFunctionD {
private:
    std::vector<std::vector<int> > objects;
    size_t count;
    size_t maxIndex;
public:
    MFCostFunctionD(const std::vector<std::vector<int> > &obj, size_t c, size_t m) : objects(obj), count(c), maxIndex(m) {}

    std::vector<double> operator()(const std::vector<double> &point, size_t objIndex) {
        std::vector<double> result((maxIndex + 1) * 2 * count, 0);
        double tmp = 0;
        for (size_t j = 0; j < count; ++j) {
            tmp += point[objects[objIndex][0] * count + j] * point[(maxIndex + 1) * count + objects[objIndex][1] * count + j];
        }
        for (size_t n = 0; n < count; ++n) {
            double tmp1 = -2 * ((double) objects[objIndex][2] - tmp) * point[(maxIndex + 1) * count + objects[objIndex][1] * count + n];
            double tmp2 = -2 * (objects[objIndex][2] - tmp) * point[objects[objIndex][0] * count + n];
            result[objects[objIndex][0] * count + n] = tmp1;
            result[(maxIndex + 1) * count + objects[objIndex][1] * count + n] = tmp2;
        }
        return result;
    }
};

void runSGDAndMakeFile(const std::string &inFile, const std::string &outFile, int count, int epochs) {
    if (count < 1 || epochs < 1) {
        throw std::runtime_error("Incorrect count of features");
    }
    std::vector<std::vector<int>> edges;
    int maxIndex = 0;
    std::ifstream in(inFile);
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        int from, to, sign;
        iss >> from >> to >> sign;
        std::vector<int> tmp;
        tmp.push_back(from);
        tmp.push_back(to);
        tmp.push_back(sign);
        if (maxIndex < from) {
            maxIndex = from;
        }
        if (maxIndex < to) {
            maxIndex = to;
        }
        edges.push_back(tmp);
    }
    std::vector<std::vector<double> > params;
    for (int i = 0; i < 2 * maxIndex; ++i) {
        std::vector<double> tmp;
        for (int j = 0; j < count; ++j) {
            tmp.push_back(1 - (float) (rand() % 500) / 1000);
        }
        params.push_back(tmp);
    }
    
    /*
    edges = std::vector<std::vector<int> >(1, std::vector<int>(3, 0));
    edges[0][0] = 0;
    edges[0][1] = 1;
    edges[0][2] = 1;
    maxIndex = 1;
    count = 2;
    params = std::vector<std::vector<double> >();
    for (int i = 0; i < 2 * (maxIndex + 1); ++i) {
        std::vector<double> tmp;
        for (int j = 0; j < count; ++j) {
            tmp.push_back((float) (rand() % 500) / 1000);
        }
        params.push_back(tmp);
    }
    for (size_t i = 0; i < params.size(); ++i) {
        for (size_t j = 0; j < params[0].size(); ++j) {
            std::cout << params[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */

    MFCostFunction cf(edges, count, maxIndex);
    MFCostFunctionD cfd(edges, count, maxIndex);
    SGDOptimizer<MFCostFunction, MFCostFunctionD > opt(cf, cfd, params, 0.5, edges.size(), true);
    opt.optimize(epochs);

    std::vector<std::vector<double> > optParams = opt.getParameters();
    std::ofstream out(outFile);
    out << "# P-vectors" << std::endl;
    for (size_t i = 0; i < params.size() / 2; ++i) {
        for (size_t j = 0; j < params[i].size(); ++j) {
            out << params[i][j] << " ";
        }
        out << std::endl;
    }
    out << "# Q-vectors" << std::endl;
    for (size_t i = params.size() / 2; i < params.size(); ++i) {
        for (size_t j = 0; j < params[i].size(); ++j) {
            out << params[i][j] << " ";
        }
        out << std::endl;
    }
}

void printUsage(ezOptionParser &opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

void prepareOptions(ezOptionParser &opt) {
    opt.overview = "Matrix factorization via SGD";
    opt.syntax = "mf_sgd [OPTIONS]";
    opt.example = "mf_sgd -i <path_to_input_file> -o <path_to_output_file>\n";
    
    opt.add(
        "",
        0,
        0,
        0,
        "Display usage instructions.",
        "-h",
        "-help",
        "--help"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to input file.",
        "-i",
        "--input"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to output file.",
        "-o",
        "--output"
    );

    opt.add(
        "",
        0,
        1,
        0,
        "Count of features",
        "-f",
        "--features"
    );

    opt.add(
        "",
        0,
        1,
        0,
        "Count of epochs",
        "-e",
        "--epochs"
    );
}

int main(int argc, const char* argv[]) {
    ezOptionParser opt;
    prepareOptions(opt);
    
    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        printUsage(opt);
        return 1;
    }

    if (opt.isSet("-i")) {
        if (!opt.isSet("-o") || !opt.isSet("-f") || !opt.isSet("-e")) {
            std::cout << "usage: mf_sgd -i <in> -o <out> -f <count>";
            return 1;
        }
        std::string inputFile;
        opt.get("-i")->getString(inputFile);
        std::string outputFile;
        opt.get("-o")->getString(outputFile);
        int count;
        opt.get("-f")->getInt(count);
        int epochs;
        opt.get("-e")->getInt(epochs);
        runSGDAndMakeFile(inputFile, outputFile, count, epochs);
    }

    return 0;
}