//main
#include "PulseProcessor.h"
int main(int argc, const char * argv[]) {
    
   
    string current_directory = "/Users/tannerorndorff/Documents/cs3370/Program_4/Program_4";
    vector<string> ini_files;
    vector<string> dat_files;


    //create file that only holds the .ini files
    system("ls /Users/tannerorndorff/Documents/cs3370/Program_4/Program_4 | grep \".*\\.ini$\" > /Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/files_ini.txt");
    fstream f;
    //create string vector of all the .dat files in current directory
    system("ls /Users/tannerorndorff/Documents/cs3370/Program_4/Program_4 | grep \".*\\.dat$\" > /Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/files_dat.txt");
    f.open("/Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/files_dat.txt");
    if (f.is_open())
    {
        string input;
        //read in file and store each line into vector
        while(!f.fail() || !f.eof())
        {
            getline(f, input);
            dat_files.push_back(input);
        }
        system("rm /Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/files_dat.txt");//remove file
    }
    else
        cout << "File could not be read." << endl;
    //open correct .ini file
    int PULSE_DELTA = 0;
    double DROP_RATIO = 0;
    int BELOW_DROP_RATIO =0;
    int WIDTH =0;
    int VT =0;
    //read .ini file for these variables
    fstream fs;
    fs.open("/Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/gage2scope.ini");
    if(fs.is_open())
    {
        string buffer;
        size_t found = string::npos;
        vector<string> file_lines;
        //read whole file into vecetor string
        while (!fs.eof())
        {
            string input;
            getline(fs, input);
            file_lines.push_back(input);
        }
        for(int i = 0; i < file_lines.size(); i++)
        {
            //get and stor vt
            if(i == 0)
            {
                found = file_lines[i].find_first_not_of("vt=");
                if (found != string::npos)
                {
                    buffer = file_lines[i].substr(found,5);
                    VT = stoi(buffer);
                }
            }
            if(i == 1)
            {
                //get and stor width
                found = file_lines[i].find_first_not_of("width=");
                if (found != string::npos)
                {
                    buffer = file_lines[i].substr(found,5);
                    WIDTH = stoi(buffer);
                }
            }
            if(i ==2)
            {
                //get and stor pulse delta
                found = file_lines[i].find_first_not_of("pulse_delta=");
                if (found != string::npos)
                {
                    buffer = file_lines[i].substr(found,5);
                    PULSE_DELTA = stoi(buffer);
                }
            }
            if (i ==3)
            {
                //get and stor drop ratio
                found = file_lines[i].find_first_not_of("drop_ratio=");
                if (found != string::npos)
                {
                    buffer = file_lines[i].substr(found,5);
                    DROP_RATIO = stod(buffer);
                }
            }
            if(i==4)
            {
                //get and stor below drop ratio
                found = file_lines[i].find_first_not_of("below_drop_ratio=");
                if (found != string::npos)
                {
                    buffer = file_lines[i].substr(found,5);
                    BELOW_DROP_RATIO = stoi(buffer);
                }
            }
           
        }
        if (VT == 0 || PULSE_DELTA == 0 || DROP_RATIO == 0 || BELOW_DROP_RATIO == 0 || WIDTH == 0)
        {
            cout << "Invalid .ini file." << endl;
            return -1;
        }
        //creat pulseproc object with correct parameters for each .dat file
       
        vector<PulseProc*> tests;//vector of PulseProc objects that test the .dat files
        for (int i = 0; i < dat_files.size()-1; i++)
        {
            string filename = "/Users/tannerorndorff/Documents/cs3370/Program_4/Program_4/";
            filename += dat_files[i];
            PulseProc* objptr = new PulseProc(PULSE_DELTA, DROP_RATIO, BELOW_DROP_RATIO, WIDTH, VT, filename);
            tests.push_back(objptr);
        }
        //print the pulse location and area for each .dat file that has pulses
        for(int i = 0; i < tests.size(); i++)
        {
            tests[i]->print();
        }
    }
    else
        cout << "Could not read .ini file." << endl;
   
}