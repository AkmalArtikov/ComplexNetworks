#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "ezOptionParser/ezOptionParser.hpp"
#include "ngraph/ngraph.hpp"

#define UNUSED(x) (void)(x)

using namespace NGraph;
using namespace ez;

template<class T>
std::vector<std::pair<std::vector<double>, int> > generateFeatures(const tGraph<T> &g, const tGraph<T> &gPos, const tGraph<T> &gNeg, const std::vector<std::pair<std::pair<T, T>, int> > &edges) {
    std::vector<std::pair<std::vector<double>, int> > result;
    int count = 0;
    int fullSize = edges.size();
    int step = fullSize / 100;
    for (std::vector<std::pair<std::pair<unsigned int, unsigned int>, int> >::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        if (count % step == 0) {
            std::cout << (count * 100) / fullSize << "% are processed" << std::endl;
        }
        ++count;
        std::vector<double> tmp;
        std::vector<unsigned int> inter;
        
        int u = i->first.first;
        int v = i->first.second;
        
        // degree features
        
        tmp.push_back(gPos.in_degree(u));
        tmp.push_back(gNeg.in_degree(v));
        tmp.push_back(gPos.out_degree(u));
        tmp.push_back(gNeg.out_degree(v));
        
        Graph::vertex_set uN = g.out_neighbors(u);
        Graph::vertex_set uIn = g.in_neighbors(u);
        Graph::vertex_set vN = g.out_neighbors(v);
        Graph::vertex_set vIn = g.in_neighbors(v);
        
        uN.insert(uIn.begin(), uIn.end());
        vN.insert(vIn.begin(), vIn.end());
        std::set_intersection(uN.begin(), uN.end(), vN.begin(), vN.end(), std::inserter(inter, inter.begin()));
        tmp.push_back(inter.size());
        tmp.push_back(g.out_degree(u));
        tmp.push_back(g.in_degree(v));
        
        // triad features
        
        std::vector<double> triad(16, 0);
        for (auto w : inter) {
            if (g.includes_edge(u, w)) {
                if (g.includes_edge(v, w)) {
                    if (gPos.includes_edge(u, w)) {
                        if (gPos.includes_edge(v, w)) {
                            ++triad[0];
                        } else if (gNeg.includes_edge(v, w)) {
                            ++triad[1];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else if (gNeg.includes_edge(u, w)) {
                        if (gPos.includes_edge(v, w)) {
                            ++triad[2];
                        } else if (gNeg.includes_edge(v, w)) {
                            ++triad[3];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else {
                        std::cout << "Error: incorrect graph." << std::endl;
                    }
                } else if (g.includes_edge(w, v)) {
                    if (gPos.includes_edge(u, w)) {
                        if (gPos.includes_edge(w, v)) {
                            ++triad[4];
                        } else if (gNeg.includes_edge(w, v)) {
                            ++triad[5];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else if (gNeg.includes_edge(u, w)) {
                        if (gPos.includes_edge(w, v)) {
                            ++triad[6];
                        } else if (gNeg.includes_edge(w, v)) {
                            ++triad[7];
                        }
                    } else {
                        std::cout << "Error: incorrect graph." << std::endl;
                    }
                }
            } else if (g.includes_edge(w, u)) {
                if (g.includes_edge(v, w)) {
                    if (gPos.includes_edge(w, u)) {
                        if (gPos.includes_edge(v, w)) {
                            ++triad[8];
                        } else if (gNeg.includes_edge(v, w)) {
                            ++triad[9];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else if (gNeg.includes_edge(w, u)) {
                        if (gPos.includes_edge(v, w)) {
                            ++triad[10];
                        } else if (gNeg.includes_edge(v, w)) {
                            ++triad[11];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else {
                        std::cout << "Error: incorrect graph." << std::endl;
                    }
                } else if (g.includes_edge(w, v)) {
                    if (gPos.includes_edge(w, u)) {
                        if (gPos.includes_edge(w, v)) {
                            ++triad[12];
                        } else if (gNeg.includes_edge(w, v)) {
                            ++triad[13];
                        } else {
                            std::cout << "Error: incorrect graph." << std::endl;
                        }
                    } else if (gNeg.includes_edge(w, u)) {
                        if (gPos.includes_edge(w, v)) {
                            ++triad[14];
                        } else if (gNeg.includes_edge(w, v)) {
                            ++triad[15];
                        }
                    } else {
                        std::cout << "Error: incorrect graph." << std::endl;
                    }
                }
            } else {
                std::cout << "Error: incorrect graph." << std::endl;
            }
        }
        tmp.insert(tmp.end(), triad.begin(), triad.end());
        
        result.push_back(std::make_pair(tmp, i->second));
    }
    std::cout << "100% are processed" << std::endl;
    return result;
}

void featuresToCSV(const std::string &filename, const std::vector<std::pair<std::vector<double>, int> > &data) {
    std::ofstream out(filename);
    for (std::vector<std::pair<std::vector<double>, int> >::const_iterator i = data.begin(); i != data.end(); ++i) {
        for (std::vector<double>::const_iterator j = i->first.begin(); j != i->first.end(); ++j) {
            out << *j << ",";
        }
        out << i->second << std::endl;
    }
}

template<class T>
void edgesToCSV(const std::string &filename, const std::vector<std::pair<std::pair<T, T>, int> > &edges) {
    std::ofstream out(filename);
    for (typename std::vector<std::pair<std::pair<T, T>, int> >::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        out << i->first.first << "\t" << i->first.second << "\t" << i->second << std::endl;
    }
}

void readEpinionsOrSlashdotFileAndMakeCSV(const std::string &inFilename, const std::string &outFilename, bool forLibFM = false) {
    std::ifstream infile(inFilename);
    std::string line;
    Graph gPos;
    Graph gNeg;
    Graph g;
    std::vector<std::pair<std::pair<unsigned int, unsigned int>, int> > edges;
    std::cout << "Read " << inFilename << "." << std::endl;
    while (std::getline(infile, line)) {
        if (line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        int from, to, sign;
        iss >> from >> to >> sign;
        if (!g.includes_edge(from, to)) {
            edges.push_back(std::make_pair(std::make_pair(from, to), sign));
        }
        g.insert_edge(from, to);
        gPos.insert_vertex(from);
        gPos.insert_vertex(to);
        gNeg.insert_vertex(from);
        gNeg.insert_vertex(to);
        if (sign == 1) {
            gPos.insert_edge(from, to);
        } else {
            gNeg.insert_edge(from, to);
        }
    }
    std::cout << "Graph is readed." << std::endl;
    if (forLibFM) {
        edgesToCSV(outFilename, edges);
    } else {
        std::vector<std::pair<std::vector<double>, int> > features = generateFeatures(g, gPos, gNeg, edges);
        featuresToCSV(outFilename, features);
    }
}

void readWikiFileAndMakeCSV(const std::string &inFilename, const std::string &outFilename, bool forLibFM = false) {
    std::ifstream infile(inFilename);
    std::string line;
    Graph gPos;
    Graph gNeg;
    Graph g;
    std::vector<std::pair<std::pair<unsigned int, unsigned int>, int> > edges;
    std::cout << "Read " << inFilename << "." << std::endl;
    int u = -1;
    while (std::getline(infile, line)) {
        if (line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        char recordType;
        iss >> recordType;
        if (recordType == 'U') {
            iss >> u;
        } else if (recordType == 'V') {
            int sign;
            iss >> sign;
            if (sign != 0) {
                int v;
                iss >> v;
                if (!g.includes_edge(v, u)) {
                    edges.push_back(std::make_pair(std::make_pair(v, u), sign));
                }
                g.insert_edge(v, u);
                gPos.insert_vertex(v);
                gPos.insert_vertex(u);
                gNeg.insert_vertex(v);
                gNeg.insert_vertex(u);
                if (sign == 1) {
                    gPos.insert_edge(v, u);
                } else {
                    gNeg.insert_edge(v, u);
                }

            }
        }
    }
    std::cout << "Graph is readed." << std::endl;
    if (forLibFM) {
        edgesToCSV(outFilename, edges);
    } else {
        std::vector<std::pair<std::vector<double>, int> > features = generateFeatures(g, gPos, gNeg, edges);
        featuresToCSV(outFilename, features);
    }
}

void printUsage(ezOptionParser &opt) {
	std::string usage;
	opt.getUsage(usage);
	std::cout << usage;
};

void prepareOptions(ezOptionParser &opt) {
    opt.overview = "Prepare data";
	opt.syntax = "prepare [OPTIONS]";
	opt.example = "prepare -wi <path_to_wiki_input_file> -wo <path_to_wiki_out_file>\n";
    
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
        0,
        0,
        "Only print edges",
        "-e",
        "--edges"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to epinions input file.",
        "-ei",
        "--epinions-input"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to epinions output file.",
        "-eo",
        "--epinions-output"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to slashdot input file.",
        "-si",
        "--slashdot-input"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to slashdot output file.",
        "-so",
        "--slashdot-output"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to wiki input file.",
        "-wi",
        "--wiki-input"
    );
    
    opt.add(
        "",
        0,
        1,
        0,
        "Path to wiki output file.",
        "-wo",
        "--wiki-output"
    );
}

int main(int argc, const char *argv[]) {
    ezOptionParser opt;
    prepareOptions(opt);
    
    opt.parse(argc, argv);
    
    if (opt.isSet("-h")) {
		printUsage(opt);
        return 1;
	}
    
    if (opt.isSet("-ei")) {
        if (!opt.isSet("-eo")) {
            std::cout << "usage: prepare -ei <in> -eo <out>";
            return 1;
        }
        std::string inputFile;
        opt.get("-ei")->getString(inputFile);
        std::string outputFile;
        opt.get("-eo")->getString(outputFile);
        readEpinionsOrSlashdotFileAndMakeCSV(inputFile, outputFile, opt.isSet("-e"));
    }
    
    if (opt.isSet("-si")) {
        if (!opt.isSet("-so")) {
            std::cout << "usage: prepare -si <in> -so <out>";
            return 1;
        }
        std::string inputFile;
        opt.get("-si")->getString(inputFile);
        std::string outputFile;
        opt.get("-so")->getString(outputFile);
        readEpinionsOrSlashdotFileAndMakeCSV(inputFile, outputFile, opt.isSet("-e"));
    }
    
    if (opt.isSet("-wi")) {
        if (!opt.isSet("-wo")) {
            std::cout << "usage: prepare -wi <in> -wo <out>";
            return 1;
        }
        std::string inputFile;
        opt.get("-wi")->getString(inputFile);
        std::string outputFile;
        opt.get("-wo")->getString(outputFile);
        readWikiFileAndMakeCSV(inputFile, outputFile, opt.isSet("-e"));
    }
    
    return 0;
}