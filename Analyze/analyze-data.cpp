/*
Cameron Hancock
03/10/2024
This Program takes in parameters from a .INI file and uses them to look through .dat files in a 
directory to find pulses in the given data.
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <map>

// Structure to hold pulse parameters
struct Parameters {
    int vt;
    int width;
    int pulse_delta;
    double drop_ratio;
    int below_drop_ratio;
};

// Parse INI file and read pulse parameters
Parameters parseINI(const std::string& iniFile) {
    Parameters params;
    std::ifstream file(iniFile);
    std::string line;
    std::map<std::string, std::string> paramMap;

    if (!file.is_open()) {
        std::cerr << "Can't open INI file" << std::endl;
        throw std::runtime_error("");
    }

    // Parsing INI file
    while (std::getline(file, line)) {
        // skip empty or commented lines
        if (line.empty() || line[0] == '#') continue;
        // Search for '=' indicating values
        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            std::cerr << "Invalid INI file: missing values" << std::endl;
            throw std::invalid_argument("");
        }
        std::string key = line.substr(0, delimiterPos);
        // Add key and value to a Map which will then be used to create needed values
        try {
            std::string value = (line.substr(delimiterPos + 1));
            paramMap[key] = value;
        } catch (...) {
            std::cerr << "Invalid value '" + line.substr(delimiterPos + 1) + "' for key '" + key +"'" << std::endl;
            throw std::invalid_argument("");
        }
    }

    // Check for missing parameters
    if (paramMap.size() < 5) {
        std::cerr << "Invalid INI file: missing one or more parameters" << std::endl;
        throw std::invalid_argument("");
    }
    try {
        // Assign parsed values to parameters
        params.vt = stoi(paramMap["vt"]);
        params.width = stod(paramMap["width"]);
        params.pulse_delta = stoi(paramMap["pulse_delta"]);
        params.drop_ratio = stod(paramMap["drop_ratio"]);
        params.below_drop_ratio = stoi(paramMap["below_drop_ratio"]);
    } catch (...) {
        std::cerr << "Invalid INI file: missing one or more parameters" << std::endl;
        throw std::invalid_argument("");
    }

    return params;
}

// Smooth the data
std::vector<int> smoothData(const std::vector<int>& data) {
    // Insert the first 3 data points
    std::vector<int> smoothedData(data.begin(), data.begin() + 3);
    // Smooth data and insert to smoothedData[]
    for (size_t i = 3; i < data.size() - 3; ++i) {
        int smoothValue = (data[i - 3] + 2 * data[i - 2] + 3 * data[i - 1] + 3 * data[i] + 3 * data[i + 1] + 2 * data[i + 2] + data[i + 3]) / 15;
        smoothedData.push_back(smoothValue);
    }
    // Insert the last 3 data points
    smoothedData.insert(smoothedData.end(), data.end() - 3, data.end());
    return smoothedData;
}

// Find pulses and compute areas
void findPulses(const std::string& filename, const std::vector<int>& data, const Parameters& params) {
    std::vector<int> smoothedData = smoothData(data);
    bool pulseDetected = false;
    std::cout << filename << ":" << std::endl;

    // Search for pulses
    for (size_t i = 0; i < smoothedData.size() - 2; ++i) {
        int rise = smoothedData[i + 2] - smoothedData[i];
        if (rise > params.vt) {
            pulseDetected = true;
            int pulseStart = i;
            int peak = i+2;
            while (smoothedData[peak] <= smoothedData[peak+1]) {
                peak++;
            }

            // Check for piggyback pulses
            bool potentialPiggyback = false;
            int possiblePeak;
            for (size_t j = peak + 1; j <= peak + params.pulse_delta && j < smoothedData.size() - 2; ++j) {
                int rise2 = smoothedData[j + 2] - smoothedData[j];
                if (rise2 > params.vt) {
                    potentialPiggyback = true;
                    possiblePeak = j;
                    break;
                }
            }
            // Test potential piggyback pulses
            if (potentialPiggyback) {
                int peakValue = data[peak];
                int countBelowDropRatio = 0;
                for (size_t k = peak + 1; k < possiblePeak-1; k++) {
                    if (data[k] < params.drop_ratio * peakValue) {
                        countBelowDropRatio++;
                    }
                }
                if (countBelowDropRatio > params.below_drop_ratio) {
                    pulseDetected = false;
                    std::cout << "Found Piggyback at " << i << std::endl;
                }
            }

            // Calculate and display area of pulse
            if (pulseDetected) {
                size_t nextPeak = data.size();
                // Find potential next peak
                for (size_t j=peak; j<smoothedData.size()-2; ++j) {
                    if (smoothedData[j+2]-smoothedData[j] > params.vt) {
                        nextPeak = j;
                        break;
                    }
                }
                // Calculate area and display pulse
                int area = 0;
                size_t endPos = std::min(nextPeak, std::min(i + params.width, data.size()));
                for (size_t k = pulseStart; k < endPos; ++k) {
                    area += data[k];
                }
                std::cout << pulseStart << " (" << area << ")" << std::endl;
            }
            // Update iterator
            i = peak;
        }
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Test for ".INI" File
    if (argc != 2) {
        std::cerr << "No \".INI\" file provided" << std::endl;
        return 1;
    }

    std::string iniFile = argv[1];
    Parameters params;
    // Pull data from ".INI" file to usable parameters
    try {
        params = parseINI(iniFile);
    } catch (...) {
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
                    // Negate values while inputting them
                    data.push_back(-value);
                }
                findPulses(filename, data, params);
            } else {
                std::cerr << "Can't open file: " << filename << std::endl <<std::endl;
            }
        }
    }

    return 0;
}
