#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <filesystem>
#include <map>

// Structure to hold pulse parameters
struct PulseParameters {
    int vt;
    int width;
    int pulse_delta;
    double drop_ratio;
    int below_drop_ratio;
};

// Function to parse INI file and read pulse parameters
PulseParameters parseINI(const std::string& iniFile) {
    PulseParameters params;
    std::ifstream file(iniFile);
    std::string line;
    std::map<std::string, int> paramMap;

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open INI file");
    }

    // Parsing INI file
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Ignore comments and empty lines

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            throw std::invalid_argument("Invalid INI file: missing '='");
        }

        std::string key = line.substr(0, delimiterPos);
        // int value = std::stoi(line.substr(delimiterPos + 1));

        // paramMap[key] = value;
        try {
            int value = std::stoi(line.substr(delimiterPos + 1));
            paramMap[key] = value;
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid value '" + line.substr(delimiterPos + 1) + "' for key '" + key +"'");
        }
    }

    // Check for missing or duplicate keys
    if (paramMap.size() != 5) {
        throw std::invalid_argument("Invalid INI file: missing one or more keys");
    }

    // Assign parsed values to parameters
    params.vt = paramMap["vt"];
    params.width = paramMap["width"];
    params.pulse_delta = paramMap["pulse_delta"];
    params.drop_ratio = static_cast<double>(paramMap["drop_ratio"]) / 100.0;
    params.below_drop_ratio = paramMap["below_drop_ratio"];

    return params;
}

// Function to smooth the data as described
std::vector<int> smoothData(const std::vector<int>& data) {
    std::vector<int> smoothedData(data.begin(), data.begin() + 3);

    for (size_t i = 3; i < data.size() - 3; ++i) {
        int smoothValue = (data[i - 3] + 2 * data[i - 2] + 3 * data[i - 1] +
                           3 * data[i] + 3 * data[i + 1] + 2 * data[i + 2] + data[i + 3]) / 15;
        smoothedData.push_back(smoothValue);
    }

    smoothedData.insert(smoothedData.end(), data.end() - 3, data.end());
    return smoothedData;
}

// Function to find pulses and compute areas
void findPulsesAndComputeAreas(const std::string& filename, const std::vector<int>& data, const PulseParameters& params) {
    std::vector<int> smoothedData = smoothData(data);
    bool pulseDetected = false;
    std::cout << filename << ":" << std::endl;

    for (size_t i = 0; i < smoothedData.size() - 2; ++i) {
        int rise = smoothedData[i + 2] - smoothedData[i];
        if (rise > params.vt) {
            pulseDetected = true;
            int pulseStart = i;

            // Check for piggyback pulses
            bool isPiggyback = false;
            for (size_t j = i + 1; j <= i + params.pulse_delta && j < smoothedData.size() - 2; ++j) {
                int rise2 = smoothedData[j + 2] - smoothedData[j];
                if (rise2 > params.vt) {
                    isPiggyback = true;
                    break;
                }
            }

            if (isPiggyback) {
                int peakValue = data[pulseStart];
                int countBelowDropRatio = 0;
                for (size_t k = pulseStart + 1; k < smoothedData.size() - 2 && k <= i + params.pulse_delta; ++k) {
                    if (data[k] < params.drop_ratio * peakValue) {
                        countBelowDropRatio++;
                    }
                }
                if (countBelowDropRatio > params.below_drop_ratio) {
                    pulseDetected = false;
                }
            }

            if (pulseDetected) {
                int area = 0;
                size_t endPos = std::min(i + params.width, data.size());
                for (size_t k = pulseStart; k < endPos; ++k) {
                    area += data[k];
                }
                
                std::cout << pulseStart << " (" << area << ")" << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ini_file>" << std::endl;
        return 1;
    }

    std::string iniFile = argv[1];
    PulseParameters params;

    try {
        params = parseINI(iniFile);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Iterate through .dat files in current directory
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        std::string filename = entry.path().filename().string();
        if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dat") {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::vector<int> data;
                int value;
                while (file >> value) {
                    data.push_back(-value); // Negate values
                }
                findPulsesAndComputeAreas(filename, data, params);
            } else {
                std::cerr << "Failed to open file: " << filename << std::endl;
            }
        }
    }

    return 0;
}
