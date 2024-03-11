#include "PulseProcessor.h"


PulseProc::PulseProc(int pd, double dr, int bdr, int w, int vt, string fn)
{
    PULSE_DELTA = pd;
    DROP_RATIO = dr;
    BELOW_DROP_RATIO = bdr;
    WIDTH = w;
    VT = vt;
    FILENAME = fn;
    pulse_num = 0;
    readFile();
    smooth();
    detectPulse();
    aupCalc();
}
void PulseProc::readFile()//opens file with file name (returns status) reads data and stores it into the data vector
{
    //open file with file name
    fstream f;
    f.open(FILENAME, ios::in);
    if(f.is_open())
    {
        string input;
        do
        {
            //take file and store in data vector
            getline(f, input);
            if(f.fail())
                break;
            int n = stoi(input);
            data.push_back(n);
        }
        while(!f.fail());
        cout << "File has successfully opened and been read." << endl;
        //negate data so it can be used properly
        transform(data.begin(), data.end(), data.begin(), std::negate<int>());
        f.close();
    }
    else
        cout << "Error opening file.\n";
   
   
}
void PulseProc::smooth() //smooths the data so it is easier to read (stores in smooth data)
{
    //copy data to smooth data
    smooth_data.resize(data.size());//allocates space
    copy(data.begin(), data.begin()+3, smooth_data.begin());
    //transform rest of data to smooth data
    for(int i = 3; i < data.size()-4; i++)
    {
        smooth_data[i] = (data[i-3] + 2*data[i-2] + 3*data[i-1] + 3*data[i] + 3*data[i+1] + 2*data[i+2] + data[i+3]) / 15;
    }
    //copy last 3 numbers to smooth data
    copy(data.end()-4,data.end(), smooth_data.end()-4);
}
void PulseProc::detectPulse()//detects pulse locations and puts them in the pulse location vactor when found
{
    int pulse_start = 0;
    int pulse_end = 0;
    vector<int> pulse_pair;
    int i = 0;
    //You detect a pulse by looking for a rise over three consecutive points.
    while(i < smooth_data.size()-2)
    {
        //start at smoothdata at location [0]
        //If the rise (yi+2 – yi) exceeds vt, (for “voltage threshold” – supplied by an input parameter), then a pulse begins at position i.
        if((smooth_data[i+2]-smooth_data[i])>VT)
        {
            pulse_start = i;
            //After finding a pulse, move forward through the data starting at yi+2 until the samples start to decrease before looking for the next pulse.
            i += 2;
            while(smooth_data[i+1] >= smooth_data[i])//smooth_data[i+1] >= smoothdatat[i[]
            {
                i++;
            }
            //when it declines, set pulse end
            pulse_end = i;
            //store pulse pairs in locations vector
            pulse_pair.push_back(pulse_start);
            pulse_pair.push_back(pulse_end);
            pulse_locations.push_back(pulse_pair);
            //clear pulse pairs
            pulse_pair.clear();
            has_pulse = true;
            pulse_num++;
        }
        i++;
    }
    pulseVerify();//eliminates pulses that are actually only one pulse
}
void PulseProc::aupCalc()//calculates the AUP(area under pulse) and puts them in the pulse areas vector
{
    //The area is merely the sum of the values starting at the pulse start and going for width samples (another input parameter),
    for(int i = 0; i < pulse_locations.size(); i++)
    {
        if ((i+1) == pulse_locations.size() || pulse_num == 0 || (pulse_locations[i+1][0]-pulse_locations[i][0])>= WIDTH )//next peak is more than 100 away or there isnt another peak
            pulse_areas.push_back(accumulate(data.begin()+pulse_locations[i][0],data.begin()+pulse_locations[i][0]+WIDTH, 0));//acucumalte till width
        else
            pulse_areas.push_back(accumulate(data.begin()+pulse_locations[i][0], data.begin()+pulse_locations[i+1][0], 0));//accumulate till next pulse
    }
    //or until the start of the next pulse, whichever comes first. Use the original, unsmoothed data to compute the area, however. (The smooth data is just for detecting pulses.)
}

void PulseProc::pulseVerify()
{
    //After you have found where pulses begin, check to see if the start of a pulse is followed by another pulse that starts within (<=) pulse_delta positions of it.
    if(pulse_num == 0)
        return;
    int i =0;
    while (i < pulse_locations.size()-1)
    {
        if((pulse_locations[i+1][0] - pulse_locations[i][0]) <= PULSE_DELTA)
        {
            //If this occurs, find how many points between the peak of the first pulse and the start of the second pulse (non-inclusive; only look at point strictly inside this interval) are strictly less than drop_ratio times the height of the peak of the first pulse.
            int drop_ratio_count = 0;
            for (int j = pulse_locations[i][1]; j < pulse_locations[i+1][0]; j++)//iterate between the end of the first pulse and start of the second
            {
               
                if(smooth_data[j] < (smooth_data[pulse_locations[i][1]]*DROP_RATIO))//if point is less than the peak times the drop ratio
                    drop_ratio_count++;//
            }
            //If the number exceeds below_drop_ratio, omit the first pulse from further consideration (it is not a pulse of interest).
            if(drop_ratio_count > BELOW_DROP_RATIO)
            {
                pulse_locations.erase(pulse_locations.begin()+i);
                pulse_num--;
                i--;
            }
           
        }
        i++;
    }
   
   
   
}
void PulseProc::print() //prints class data to the console
{
    cout << "Total data size: " << data.size() << endl;
    cout << "Total pulses: " << pulse_num << endl;
    if (pulse_num != 0)
    {
        cout << "Pulse locations: " << endl ;
        for (int i = 0; i < pulse_locations.size(); i++)
        {
            cout << "\tLocation " << i << ": " << pulse_locations[i][0] << " - " << pulse_locations[i][1] << ", Area: " << pulse_areas[i] << endl;
        }
    }
}

