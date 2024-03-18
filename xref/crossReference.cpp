/*
Cameron Hancock
03/17/2024
This program reads a file and counts how many times each word is used. It then prints out the 
words in alphabetical order and what lines they are found as well as how many times they are found
on said line.
*/
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

struct wordInfo {
    std::string word;
    int count;
    std::vector<int> positions;
};

// print word info
void printWordInfo(const wordInfo& info) {
    std::cout << info.word << "             : ";
    int lastLine = 0;
    int currentLine = 0;
    int numOnLine = 1;
    int printedOnLine = 0;
    for (int line : info.positions) {
        if (printedOnLine > 0 && printedOnLine < 9) {
            std::cout << ", ";
        }
        currentLine = line;
        if (currentLine == lastLine) {
            numOnLine++;
        } else if (lastLine != 0) {
            if (printedOnLine > 8) {
                std::cout << std::endl << "                 : ";
                printedOnLine = 0;
            }
            std::cout << lastLine << ":" << numOnLine;
            printedOnLine++;
            numOnLine = 1;
        }
        lastLine = currentLine;
    }
    if (printedOnLine > 8) {
        std::cout << std::endl << "                 : ";
        printedOnLine = 0;
    } else if (printedOnLine > 0) {std::cout << ", ";}
    std::cout << currentLine << ":" << numOnLine << std::endl;
}

int main(int argc, char **argv) {
    // Check file input
    if (argc < 2) {
        std::cerr << "No input file provided" << std::endl;
        return 1;
    }
    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }
    std::unordered_map<std::string, wordInfo> hashMap;
    std::string line;
    int lineNum = 1;

    // Read file and take a count of each word;
    while (std::getline(inputFile, line)) {
        std::istringstream iss (line);
        std::string word;
        while (iss >> word) {
            // Check if word contains numbers
            bool digitCheck = false;
            for (int i = 0; i < std::size(word); i++) {
                if (std::isdigit(word[i])){
                    digitCheck = true;
                }
            }
            if (digitCheck) {break;}
            // Remove non letter characters except - and '
            word.erase(std::remove_if(word.begin(), word.end(), [](char c) {return !(isalpha(c) || c=='-' || c=='\'');}), word.end());
            if (word.size() == 0) {break;}
            // Hash word and increase count and line position of hash
            wordInfo& info = hashMap[word];
            info.word = word;
            info.count++;
            info.positions.push_back(lineNum);
        }
        lineNum++;
    }
    // Sort words into a vector
    std::vector<wordInfo> sorted;
    for (const auto& pair : hashMap) {
        sorted.push_back(pair.second);
    }
    std::sort(sorted.begin(), sorted.end(), [](const wordInfo& a, const wordInfo& b) {
        std::string first = a.word;
        std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) {return std::tolower(c);});
        std::string second = b.word;
        std::transform(second.begin(), second.end(), second.begin(), [](unsigned char c) {return std::tolower(c);});
        return first < second;
    });
    // Print sorted word info
    for (const wordInfo& info : sorted) {
        printWordInfo(info);
    }

    return 0;
}
