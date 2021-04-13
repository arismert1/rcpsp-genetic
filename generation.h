#ifndef GENERATION_H
#define GENERATION_H

#include "rcpsp.h"

using namespace std;

class generation
{
private:
    vector<double> priorityVector;
    vector<string> solution;
    vector<int> rankVector;
    int timeFinished;
public:
    generation(){priorityVector.clear(); solution.clear(); rankVector.clear(); timeFinished=0;}
    generation(vector<double> in_priorityVector, vector<string> in_solution, vector<int> in_rakVector, int in_timeFinished){
        for(int i=0;i<seeds;i++){
            priorityVector.push_back(0.0);
            solution.push_back("0");
            rankVector.push_back(0);
        }
        for(int i=0;i<seeds;i++){
            priorityVector[i]=in_priorityVector[i];
            solution[i]=in_solution[i];
            rankVector[i]=in_rakVector[i];
        }
        timeFinished=in_timeFinished;
    }

    double getPriorityVector(int i){return priorityVector[i];}
    string getSolution(int i){return solution[i];}
    int getRankVector(int i){return rankVector[i];}
    int getTimeFinished(){return timeFinished;}

    void pushRankVector(int value){rankVector.push_back(value);}
    void pushPriorityVector(double value){priorityVector.push_back(value);}
    void pushSolution(string value){solution.push_back(value);}

    void setPriorityVector(int position, double value){priorityVector[position]=value;}
    void setSolution(int position, string value){solution[position]=value;}
    void setRankVector(int position, int value){rankVector[position]=value;}
    void setTimeFinished(int value){timeFinished=value;}

    void clearPriorityVector(){priorityVector.clear();}
    void clearSolution(){solution.clear();}
    void clearRankVector(){rankVector.clear();}
}gen[numOfParents+1];

#endif