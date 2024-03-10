#include <string>
#include <fstream>
#include <iostream>
#include <vector>

// Input data pioints from file to vector
std::vector<int> readFileToVector(const std::string& filename) {
    std::ifstream source;
    source.open(filename);
    std::vector<int> points;
    std::string line;
    while (std::getline(source, line)) {
        points.push_back(stoi(line)*(-1));
    }
    return points;
}
void displayVector(const std::vector<int> points) {
    for (int i=0; i < points.size(); i++) {
        std::cout << points[i] << std::endl;
    }
}

// Smooth data points by taking a relative average
std::vector<int> smoothData(const std::vector<int> points) {
    std::vector<int> smoothedPoints;
    // Grab the first three points of the vector
    for (unsigned int i=0; i<3; i++) {
        smoothedPoints.push_back(points[i]);
    }
    // Take the weighted average of the subsequent points
    for (unsigned int i=3; i<points.size()-3; i++) {
        int newPoint = (points[i-3]+2*points[i-2]+3*points[i-1]+3*points[i]+3*points[i+1]+2*points[i+2]+points[i+3])/15;
        smoothedPoints.push_back(newPoint);
    }
    // Grab the last three points of the vector
    for (unsigned int i=points.size()-4; i<points.size(); i++) {
        smoothedPoints.push_back(points[i]);
    }
    return smoothedPoints;
}

int main(int argc, char **argv) {
    std::vector<int> data[argc-1];
    for (int k=1; k<argc; k++) {
        std::string dataFile(argv[k]);
        std::vector<int> dataPoints = readFileToVector(dataFile);
        data[k-1] = dataPoints;
    }
    // Write new data points to file
    std::ofstream tempFile;
    tempFile.open("graphingPoints.dat");
    // Test that vectors are filled propperly
    for (unsigned int i=0; i < argc-1; i++) {
        for (unsigned int j=0; j < data[i].size(); j++) {
            std::cout << data[i][j] << std::endl;
        }
        std::cout << std::endl;
    }
    // Test print smoothed vector
    for (unsigned int i=0; i < argc-1; i++) {
        std::vector smooth = smoothData(data[i]);
        for (unsigned int i=0; i < smooth.size(); i++) {
            tempFile << smooth[i] << std::endl;
        } 
    }

    return 0;
}