#include <string>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<int> readFileToVector(const std::string& filename) {
    std::ifstream source;
    source.open(filename);
    std::vector<int> points;
    std::string line;
    while (std::getline(source, line)) {
        points.push_back(stoi(line));
    }
    return points;
}

void displayVector(const std::vector<int&> points) {
    for (int i=0; i < points.size(); i++) {
        std::cout << points[i] << std::endl;
    }
}

int main(int argc, char **argv) {
    std::vector<int> data[argc-1];
    for (int k=1; k<argc; k++) {
        std::string dataFile(argv[k]);
        std::vector<int> dataPoints = readFileToVector(dataFile);
        data[k-1] = dataPoints;
    }

    return 0;
}