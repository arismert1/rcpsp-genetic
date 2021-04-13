#include "rcpsp.h"
#include "generation.h"
using namespace std;

double fRand(double fMin, double fMax){ //calc random double numbers
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
    }

int activityNumber(string a){ //find int of activity from its string
    int output=0;
    for(int i=0;i<inputs;i++){
        if(a==rcpsp[i].getActivity()){
            output=i;
        }
    }
    return output;
}

int findPredecessorInd(string act, int i){ //predecessor index
        return activityNumber(rcpsp[activityNumber(act)].getPredecessors(i));
}

int latestPredecessorFinish(string act){ 
    vector<int> vec;
    vec.clear();
    for(int i=0;i<numOfPredecessors;i++){vec.push_back(findPredecessorInd(act, i));}

    int max=0;

    for(int i=0;i<numOfPredecessors;i++){
        if(rcpsp[vec[i]].finish()>max){
            max=rcpsp[vec[i]].finish();
        }
    }
    return max;
}

int timeActGoesIn(string act, vector<int> funcTimeUnits){ //time unit that activity act goes in the schedule
    int lpf=latestPredecessorFinish(act);
    int dur=rcpsp[activityNumber(act)].getDuration();
    int dem=rcpsp[activityNumber(act)].getDemand();

    for(int i=lpf;i<lpf+dur;i++){
        if(funcTimeUnits[i]+dem>resourceCap){
            lpf=i+1;
        }
    }
    return lpf;
}

bool allPredecessorsChecked(string act){
    int exitInd=0;
    bool ok;

    while(exitInd<numOfPredecessors){
        if(rcpsp[activityNumber(rcpsp[activityNumber(act)].getPredecessors(exitInd))].getaChecked()){
            ok=true;
            exitInd++;
        }
        else {
            ok=false;
            break;
        }
    }
    if(ok) return true;
    else return false;
    
}

void initRCPSP(){ //initialize the problem
    rcpspClass();
    generation();

    string in_activity; int in_duration; int in_demand; vector<string> in_predecessors; int in_start=0; bool in_aChecked=false;
    in_predecessors.clear();
    for(int k=0;k<numOfPredecessors;k++)    in_predecessors.push_back("0");

    //OPEN FILE
    fstream in_file;
    in_file.open("model.txt");
    if(!in_file.is_open()) {cout<<"Could not open file!"; exit(1);}

    int i = 0; string ch;
	getline(in_file, ch);

        while(!in_file.eof()){
            in_file>>in_activity>>in_duration>>in_demand;
            for(int k=0;k<numOfPredecessors;k++) in_file>>in_predecessors[k];
            rcpsp[i]=rcpspClass(in_activity,in_duration,in_demand,in_predecessors, in_start,in_aChecked);
            i++;
        }
    in_file.close();
    //END OPEN FILE

    rcpsp[activityNumber("S")].setaChecked(true);
}

void calculateFinishTime(){
    vector<string> currentSolution; int actInd;

    int sumTimeUnits=0; //poso xwro na exei to timeUnits
    for(int i=0;i<inputs;i++){
        sumTimeUnits+=rcpsp[i].getDuration();
    }

    vector<int> timeUnits; //pinakas me 8eseis oses oi xronikes monades kai value oso to total demand
    timeUnits.clear();
    for(int i=0;i<sumTimeUnits;i++) timeUnits.push_back(0);

    for(int g=0;g<numOfParents;g++){
        gen[g].setTimeFinished(0);
        for(int i=0;i<sumTimeUnits;i++) timeUnits[i]=0;

        currentSolution.clear();
        for(int i=0;i<seeds;i++) currentSolution.push_back(gen[g].getSolution(i));
        currentSolution.push_back("FINISH");

        cout<<"current sol: ";
        for(long unsigned int  i=0;i<currentSolution.size();i++) cout<<currentSolution[i]<<" ";
        cout<<endl;

        for(int k=1;k<=activityNumber("FINISH");k++){ //8ESE OLA TA ACTIVITIES NOT CHECKED
        rcpsp[k].setaChecked(false);
        }

        actInd=0;

        while(!rcpsp[activityNumber("FINISH")].isChecked()){ //MEXRI NA GINEI CHECKED TO FINISH
            if((allPredecessorsChecked(currentSolution[actInd])) && (!rcpsp[activityNumber(currentSolution[actInd])].getaChecked())){
                rcpsp[activityNumber(currentSolution[actInd])].setStart(timeActGoesIn(currentSolution[actInd],timeUnits));//ANANEWSE TO START

                int upperBound=timeActGoesIn(currentSolution[actInd],timeUnits)+rcpsp[activityNumber(currentSolution[actInd])].getDuration();

                for(int k=timeActGoesIn(currentSolution[actInd],timeUnits);k<upperBound;k++){ //ANANEWSE TIN TIMEUNITS
                    timeUnits[k]+=rcpsp[activityNumber(currentSolution[actInd])].getDemand();
                    //cout<<actInd<<" k: "<<k<<" demand:"<<timeUnits[k]<<endl;
                    if(k>gen[g].getTimeFinished()) gen[g].setTimeFinished(k); //TELIKO TIME FINISHED
                }
                rcpsp[activityNumber(currentSolution[actInd])].setaChecked(true);
                actInd=0;
            }
            else actInd++;
        }
    
        gen[g].setTimeFinished(gen[g].getTimeFinished()+1);
        cout<<"solution no. "<<g+1<<" finished in "<<gen[g].getTimeFinished()<<" time units."<<endl<<endl;
    }
}

void calculateRanks(){ //calc the rank vectors from the vectors of priority

    for(int g=0;g<numOfParents;g++){
        gen[g].clearRankVector();
    }

    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++){
            gen[g].pushRankVector(0);
        }
    }

    double tempMin=10.0;
    double lastMin=0.0;
    int rankValue;

    for(int g=0;g<numOfParents;g++){
        lastMin=0.0;
        for(int rankInd=0;rankInd<seeds;rankInd++){
            tempMin=10.0;
            for(int prioInd=0;prioInd<seeds; prioInd++){
                if((gen[g].getPriorityVector(prioInd)<tempMin) && (gen[g].getPriorityVector(prioInd)>lastMin)){
                    tempMin=gen[g].getPriorityVector(prioInd);
                    rankValue=prioInd;
                }
            }
            gen[g].setRankVector(rankValue,rankInd);
            lastMin=tempMin;
        }
    }
}

void calcPriorityVector(){

    vector<double> randArray;
    int randInd=0;
    for(int i=0;i<randArraySize;i++)
        randArray.push_back(fRand(0,1));

    for(int g=0;g<numOfParents;g++){
        gen[g].clearPriorityVector();
    }

    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++) { //VGALE EKSW TO START KAI TO FINISH
            gen[g].pushPriorityVector(randArray[randInd]);
            randInd++;
        }
    }
}

void geneticCrossing(vector<int> pairs){

    vector<double> randArray; //PINAKAS RANDOM DEKADIKON ARITHMON
    int randInd=0;
    for(int i=0;i<randArraySize;i++)
        randArray.push_back(fRand(0,1));

    int ind=0;
    vector<double> tempVec;
    tempVec.clear();

    int splitPoint;

    for(int g=0;g<numOfParents;g+=2){
        splitPoint=1.0;
        while(splitPoint<=seeds){
            if(randArray[randInd]>0.85)randArray[randInd]=0.5*randArray[randInd];
            if(randArray[randInd]<splitPoint/(seeds * 1.0)){
                ind=0;
                tempVec.clear();

                cout<<"pair "<<(g+2)/2<<": "<<pairs[g]<<" - "<<pairs[g+1]<<" Splitpoint:"<<splitPoint<<endl; 

                for(int tmp=splitPoint-1;tmp<seeds;tmp++){
                    tempVec.push_back(gen[pairs[g]].getPriorityVector(tmp));
                    gen[pairs[g]].setPriorityVector(tmp, gen[pairs[g+1]].getPriorityVector(tmp));
                    gen[pairs[g+1]].setPriorityVector(tmp, tempVec[ind]);
                    ind++;
                }
                /*cout<<"tempVec"<<endl;
                for(int k=0;k<seeds-splitPoint;k++) cout<<tempVec[i]<<" ";
                cout<<endl;*/
                randInd++;
                break;
            }
            else splitPoint++;
        }
    }  

    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++){
            cout<<gen[g].getPriorityVector(i)<<" ";
        }
        cout<<endl;
    }  
}

void geneticMutation(){
    int mutatedChild=0;

    for(int g=0;g<numOfParents;g++){
        if(gen[g].getTimeFinished()>mutatedChild)
            mutatedChild=g;
    }

    int mutatedActivity=fRand(0,seeds);

    cout<<"MUTATED CHILD: "<<mutatedChild<<" ";
    cout<<"MUTATED ACTIVITY: "<<mutatedActivity<<endl;

    gen[mutatedChild].setPriorityVector(mutatedActivity,gen[mutatedChild].getPriorityVector(mutatedActivity)*1.5);

    cout<<"Mutated Child no. "<<mutatedChild<<" activity "<<mutatedActivity<<"'s vector becomes "<<gen[mutatedChild].getPriorityVector(mutatedActivity)<<endl;
}

//----------------------------------PRINT FUNCTIONS-----------------------------------------------------
void printRankVector(){

    cout<<"Ranks: "<<endl;
    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++){
            cout<<gen[g].getRankVector(i)<<" ";
        }
        cout<<endl;
    }
}

void printActivityVector(){
    for(int g=0;g<numOfParents;g++){
        gen[g].clearSolution();
    }
    cout<<"Activities: "<<endl;

    vector<string> midActivities;
    midActivities.clear();
    for(int i=1;i<inputs-1;i++) midActivities.push_back(rcpsp[i].getActivity());

    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++){
            gen[g].pushSolution(midActivities[gen[g].getRankVector(i)]);
            //tempSolutionVector.push_back(rcpsp[rankVector[i]].getActivity());
            cout<<gen[g].getSolution(i)<<" ";
        }
        cout<<endl;
    }
}

void printPriorityVector(){
    cout<<"Vector of Priority: "<<endl;
    for(int g=0;g<numOfParents;g++){
        for(int i=0;i<seeds;i++) {
            cout<<gen[g].getPriorityVector(i)<<" ";
        }
        cout<<endl;
    }
}

void printModel(){
    for(int i=0;i<inputs;i++){
        cout<<rcpsp[i].getActivity()<<" "<<rcpsp[i].getDuration()<<" "<<rcpsp[i].getDemand()<<" ";
        for(int k=0;k<numOfPredecessors;k++) cout<<rcpsp[i].getPredecessors(k)<<" ";
        cout<<rcpsp[i].getaChecked()<<endl;
    }
}

int main()
{
    //INIT
    initRCPSP();

    printModel();
    //END INIT

    //CONSTRUCTIVE
    calcPriorityVector();

    printPriorityVector();

    calculateRanks();

    printRankVector();

    printActivityVector();

    calculateFinishTime();
    //END CONSTRUCTIVE

    //GENETIC
    for(int loops=0;loops<3;loops++){ //number of loops 

        vector<int> pairs;
        pairs.clear();
        for(int i=0;i<numOfParents;i++) {pairs.push_back(i);}

        random_shuffle(pairs.begin(), pairs.end());

        geneticCrossing(pairs);

        geneticMutation();

        printPriorityVector();

        calculateRanks();

        printRankVector();

        printActivityVector();

        calculateFinishTime();
    }
    //END GENETIC

    return 0;
}