#ifndef RCPSP_H
#define RCPSP_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <istream>
#include <locale>
#include <algorithm>
#include <vector>
#include <cmath>
#include <time.h>

#define resourceCap 5 //limit of resources per time unit
#define numOfParents 4 //number of parents to use in the genetic algorithm later
#define inputs 10 //number of input file lines -1
#define seeds inputs-2 //number of inputs without START and FINISH
#define numOfPredecessors 3 //number of max predecessors of each activity
#define randArraySize 200

using namespace std;

class rcpspClass
{
private:
    string activity;
    int duration;
    int demand;
    vector<string> predecessors;
    int start;
    bool aChecked; //true if activity IS checked
public:
    rcpspClass(){
        activity="0";
        duration=0; 
        demand=0; 
        predecessors.clear();
        start=0;
        aChecked=false;
    }
    rcpspClass(string in_activity, int in_duration, int in_demand, vector<string> in_predecessors, int in_start, bool in_aChecked){
        activity=in_activity; 
        duration=in_duration; 
        demand=in_demand;
        start=in_start;
        aChecked=in_aChecked;
        for(int i=0; i<numOfPredecessors;i++){
            predecessors.push_back("0");
        }
        for(int i=0; i<numOfPredecessors;i++){
            predecessors[i]=in_predecessors[i];
        }
    }

    string getActivity(){return activity;}
    int getDuration(){return duration;}
    int getDemand(){return demand;}
    int getStart(){return start;}
    bool getaChecked(){return aChecked;}
    string getPredecessors(int i){return predecessors[i];}

    bool isChecked(){
        if(aChecked) return true;
        else return false;
    }

    int finish(){return start+duration;}

    void setStart(int value){start=value;}
    void setaChecked(bool value){aChecked=value;}

}rcpsp[inputs+1];

#endif