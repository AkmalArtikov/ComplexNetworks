#include <iostream>
#include <set>
//#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <memory>

template<class T>
class Vertex {
private:
    T name;
    mutable std::set<std::pair<Vertex<T>*, int> > signedEdges;
public:
    Vertex(const T &name) {
        this->name = name;
    }
    
    const T& getName() const {
        return name;
    }
    
    void setName(const T &name) {
        this->name = name;
    }
    
    void addEdge(Vertex<T> *v, int sign) const {
        signedEdges.insert(std::make_pair(v, sign));
    }
    
    bool operator==(const Vertex<T> &v) {
        return (this->name == v.name);
    }
    
    bool operator<(const Vertex<T> &v) const {
        return (this->name < v.name);
    }
};

template<class T>
class VertexNode {
public:
    
};

template<class T>
struct VertexHasher {
    size_t operator()(const Vertex<T> &v) const {
        v.getName();
        //std::hash<std::string> hashFn;
        return 1;//hashFn(v.getName());
    }
};

template<class T>
struct VertexPtrComparator {
    size_t operator()(const std::auto_ptr<Vertex<T> > &v1, const std::auto_ptr<Vertex<T> > &v2) {
        return (v1.get()->getName() < v2.get()->getName());
    }
};

template<class T>
class Graph {
private:
    std::set<std::auto_ptr<Vertex<T> > , VertexPtrComparator<T> > vertices;
public:
    void addVertex(const T &name) {
        if (vertices.find(std::auto_ptr<Vertex<T> >(new Vertex<T>(name))) == vertices.end()) {
            std::auto_ptr<Vertex<T> > tmp = std::auto_ptr<Vertex<T> >(new Vertex<T>(name));
            vertices.insert(tmp);
        }
    }
    
    void addEdge(const T &fromName, const T &toName, int sign) {
        this->addVertex(fromName);
        this->addVertex(toName);
        vertices.find(std::auto_ptr<Vertex<T> >(new Vertex<T>(fromName)))->get()->addEdge(vertices.find(std::auto_ptr<Vertex<T> >(new Vertex<T>(toName)))->get(), sign);
    }
};

int main() {
    std::ifstream infile("/Users/vokyleb/Programming/science/data/raw/soc-sign-epinions.txt");
    std::string line;
    Graph<int> g;
    while (std::getline(infile, line)) {
        if (line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        int from, to, sign;
        iss >> from >> to >> sign;
        g.addEdge(from, to, sign);
    }
    return 0;
}