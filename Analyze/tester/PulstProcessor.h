#ifndef PulseProcessor_h
#define PulseProcessor_h

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>

using namespace std;
class PulseProc
{
    vector<vector<int>> pulse_locations;//vector of vectors of ints
    vector<int> pulse_areas;
    vector<int> data;
    vector<int> smooth_data;
    int PULSE_DELTA;
    double DROP_RATIO;
    int BELOW_DROP_RATIO;
    int WIDTH;
    int VT;
    int pulse_num;
    string FILENAME;
    bool has_pulse;
    void pulseVerify();//checks to see if two pulses arent acually just one
public:
    PulseProc(int pd, double dr, int bdr, int w, int vt, string fn);
    void readFile();//opens file with file name (returns status) reads data and stores it into the data vector
    void smooth(); //smooths the data so it is easier to read (stores in smooth data)
    void detectPulse();//detects pulse locations and puts them in the pulse location vactor when found
    void aupCalc();//calculates the AUP(area under pulse) and puts them in the pulse areas vector
    void print(); //prints class data to the console
};



#endif /* PulseProcessor_h */