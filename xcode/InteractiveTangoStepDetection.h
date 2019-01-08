//
//  InteractiveTangoStepDetection.h
//  InteractiveTangoReadFromAndroid
//
//  Created by courtney on 10/15/14.

#ifndef InteractiveTangoReadFromAndroid_InteractiveTangoStepDetection_h
#define InteractiveTangoReadFromAndroid_InteractiveTangoStepDetection_h



#include <algorithm>

namespace InteractiveTango {
    
    
    // Define window sizes, indexes and bounds
    // A: Small window ( 0.1 sec )
    // B: Medium window ( 1 sec )
    // C: Large window ( 60 sec )
#define windowSizeA SR/10.0
#define windowSizeB 50
#define windowSizeC 256 //SR*60.0 TEST
#define windowSizePrevSwing windowSizeA*2
#define waitBetweenPeaks SR/3 //for testing
    
#define PEAK_THRESH 3.0
    
class Stationary : public SignalAnalysis
{
public:
    Stationary( int id1, std::string portz1, SignalAnalysis *s  ) : SignalAnalysis( s, windowSizeC )
    {
        stationary = false;
        ID1 = id1;
        port = portz1;
    };
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;
        
//disabling OSC messages that we are not using 4-8-2015
        
//        ci::osc::Message msg;
//        
//        msg.setAddress(SHIMMER_STATIONARY);
//        
//        msg.addIntArg(ID1);
//        msg.addIntArg(atoi(port.c_str()));
//        
//        msg.addIntArg( stationary ); // true if previous A block  was ’ peak ’ ( false = non peak )
        
//        msgs.push_back(msg);
        
        return msgs;
    }
    
    void update(float seconds) //try a low, then high pass filter
    {
        SignalAnalysis::update(seconds);
        if( data1.size()<windowSizeA ) return ; // don't bother until there is enough
        
        //find magnitude
        float mag;
        float statSum = 0;
        float G_FORCE2 = 9.8066 * 2.5;
        for( int i=(data1.size() - 3); i<data1.size(); i++ )
        {
            float x = data1[i]->getData(2)/G_FORCE2 ;
            float y = data1[i]->getData(3)/G_FORCE2 ;
            //float z = data1[i]->getData(4)/G_FORCE2 ;
            
            mag = sqrt( x*x + y*y );//+ z*z );
//            std::cout << "   x  " << x << "   y " << y << "    mag " << mag << std::endl;
            statSum += (  mag <= 0.1 );
        }
        
        stationary = (statSum / 3) > 0.3;
    }
    
private:
    bool stationary;
    int ID1;
    std::string port;
};
    
class StepDetection: public SignalAnalysis
{
public:
    StepDetection( int id1, std::string portz1, SignalAnalysis *s );
    ~StepDetection() ;
    
    //int timestamp;
    
//    // std. dev. of 1 and 0.1 sec windows (B and A windows resp . )
//    double stdDevB[numberOfBwindows];
//    double stdDevA[numberOfAwindows*numberOfBwindows];
    
    // ;total as std. dev.  ( last samples up to 1 minute )
//    double stdDevTotX ;
//    double stdDevTotY ;
//    double stdDevTotZ ;
    
    bool xMove ; // true if previous B block was ’moving’ phase ( false = stop )
    bool xSwing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool xPeak ; // true if previous A block  was ’ peak ’ ( false = non peak )
    
    bool yMove ; // true if previous B block was ’moving’ phase ( false = stop )
    bool ySwing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool yPeak ; // true if previous A block  was ’ peak ’ ( false = non peak )

    bool xdMove ; // true if previous B block was ’moving’ phase ( false = stop )
    bool xdSwing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool xdPeak ; // true if previous A block  was ’ peak ’ ( false = non peak )
    
    bool ydMove ; // true if previous B block was ’moving’ phase ( false = stop )
    bool ydSwing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool ydPeak ; // true if previous A block  was ’ peak ’ ( false = non peak )
    
    bool xd2Move ; // true if previous B block was ’moving’ phase ( false = stop )
    bool xd2Swing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool xd2Peak ; // true if previous A block  was ’ peak ’ ( false = non peak )
    
    bool yd2Move ; // true if previous B block was ’moving’ phase ( false = stop )
    bool yd2Swing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool yd2Peak ; // true if previous A block  was ’ peak ’ ( false = non peak )
    
    bool zMove ; // true if previous B block was ’moving’ phase ( false = stop )
    bool zSwing ; // true if previous A block was was ’ swing ’ phase ( false = stance )
    bool zPeak ; // true if previous A block  was ’ peak ’ ( false = non peak )

    std::vector<int> previousSwingPhase;
//    double previousZSwingPhase[windowSizeA];

    
    bool isXMoving(){ return xMove; };
    bool isXSwing(){ return xSwing; };
    bool isXPeak(){ return xPeak; };
    
    bool isYMoving(){ return yMove; };
    bool isYSwing(){ return ySwing; };
    bool isYPeak(){ return yPeak; };
    
    bool combinedPeak;
    int sinceLastPeak;
    bool lastFramePeak;
    
    bool isZMoving(){ return zMove; };
    bool isZSwing(){ return zSwing; };
    bool isZPeak(){ return zPeak; };
    

//    static const int  numberOfAwindows = windowSizeB / windowSizeA ;
//    static const int  numberOfBwindows = windowSizeC / windowSizeB ;
    
    // Data output to file
    std::ofstream fout ;
    std::string FileName ;
    
    int ID1;
	std::string port1;
    
    virtual std::vector<ci::osc::Message> getOSC();
    virtual void update(float seconds);
    

protected:
    //bool execute( bool &move, bool &swing, bool &peak, int bufferS, double inputValues[]);
    double findStdDev( double *buffer, int startIndex, int endIndex );
    double findStdDev( std::vector<double>buffer, int startIndex, int endIndex );

    bool execute( bool &move, bool &swing, bool &peak,int bufferS, double inputValues[], bool isY);
    bool execute(std::vector<double> input);
    bool findSwing(std::vector<double> input);


    void derivative(double input[], double *output, int sz);
    void derivative(double input[], std::vector<double> &output, int sz);

    void findMag(double inputX[], double inputY[], double *output, int sz);
    std::vector<double> findMag(std::vector<double> inputX, std::vector<double> inputY);
    void findpeaks(double input[], std::vector<double> &peaks, std::vector<int> &indices, int sz );
};
    
void StepDetection::findpeaks(double input[], std::vector<double> &peaks, std::vector<int> &indices, int sz )
{
/* from matlab
    y = y(:)';
    
    switch length(y)
case 0
    ind = [];
case 1
    ind = 1;
otherwise
    dy = diff(y);
    not_plateau_ind = find(dy~=0);
    ind = find( ([dy(not_plateau_ind) 0]<0) & ([0 dy(not_plateau_ind)]>0) );
    ind = not_plateau_ind(ind);
    if y(1)>y(2)
        ind = [1 ind];
    end
    if y(end-1)<y(end)
        ind = [ind length(y)];
    end
    end
    
    if nargout > 1
        peaks = y(ind);
    end
*/
    std::vector<double> dy;
    derivative(input, dy, sz);
    
    //find non-zero
    std::vector<int> not_plateau_ind;
    for(int i=0; i<sz; i++)
    {
        if( dy[i]!=0 ) not_plateau_ind.push_back(i);
    }
    
    if(not_plateau_ind.empty()) return; 
    for( int i=1; i<not_plateau_ind.size()-1; i++ )
    {
        if( ( dy[not_plateau_ind[i+1]] < 0 )  &  ( dy[not_plateau_ind[i]] > 0 )  )
        {
            indices.push_back( not_plateau_ind[i]  );
        }
    }
    
//later, deal with
//    if( input[0]<input[1] )
//        indices.insert(indices.begin(), 0);
//    
//    if( input[sz-2]<input[sz-1] )
//        indices.push_back(sz-1);
    
    for( int i=0; i<indices.size(); i++)
        peaks.push_back(input[indices[i]]);
}

StepDetection::StepDetection( int id1, std::string portz1, SignalAnalysis *s  ) : SignalAnalysis( s, windowSizeC )
{

    xMove = false;
    xSwing = false;
    xPeak = false;
    
    yMove = false;
    ySwing = false;
    yPeak = false;

    xdMove = false;
    xdSwing = false;
    xdPeak = false;
    
    yd2Move = false;
    yd2Swing = false;
    yd2Peak = false;
    
    xd2Move = false;
    xd2Swing = false;
    xd2Peak = false;
    
    ydMove = false;
    ydSwing = false;
    ydPeak = false;
    
    zMove = false;
    zSwing = false;
    zPeak = false;
    
    combinedPeak = false;
    lastFramePeak = false;
    
    ID1 = id1;
    port1 = portz1;
    
    sinceLastPeak = waitBetweenPeaks + 1;
    
}
    
StepDetection::~StepDetection(){}
    
//void StepDetection::derivative(double input[], double *output, int sz)
//{
//    assert(sz > 2); //absolute min.
//    
//    output[0] = 0;
//    for(int i=1; i<sz; i++)
//    {
//        output[i] = input[i-1] - input[i];
//    }
//
//}
    

void StepDetection::findMag(double inputX[], double inputY[], double *output, int sz)
{
    for( int i=0; i<sz; i++ )
    {
        output[i] = sqrt( inputX[i]*inputX[i] + inputY[i]*inputY[i] );
    }
}
    
std::vector<double> StepDetection::findMag(std::vector<double> inputX, std::vector<double> inputY)
{
    std::vector<double> output;
    for( int i=0; i<inputX.size(); i++ )
    {
        output.push_back( sqrt( inputX[i]*inputX[i] + inputY[i]*inputY[i] ) );
    }
    return output;
}

    
void StepDetection::derivative(double *input, double *output, int sz)
{
    assert(sz > 2); //absolute min.
        
    output[0] = 0;
    for(int i=1; i<sz; i++){
        output[i] = ( input[i] - input[i-1] );
    }
    
}
    
void StepDetection::derivative(double input[], std::vector<double> &output, int sz)
{
    assert(sz > 2); //absolute min.
    
    output.push_back(0);
    for(int i=1; i<sz; i++)
    {
        output.push_back( ( input[i] - input[i-1] ) );
    }
}


    
double StepDetection::findStdDev( double *buffer, int startIndex, int endIndex )
{
    double sum = 0 ;
    double dev = 0 ;
    double stddev;
    
    int N = endIndex - startIndex;
    for (int i = startIndex; i < endIndex; i ++)
    {
        sum += buffer[i] ;
    }
    for ( int i = startIndex; i < endIndex ; i ++)
    {
        dev += pow( buffer [i]- (sum/N), 2.0 ) ;
    }
    stddev = sqrt( dev / N ) ;
    
    return stddev; 
}
    
double StepDetection::findStdDev( std::vector<double>buffer, int startIndex, int endIndex )
{
    double sum = 0 ;
    double dev = 0 ;
    double stddev;
        
    int N = endIndex - startIndex;
    for (int i = startIndex; i < endIndex; i ++)
    {
        sum += buffer[i] ;
    }
    for ( int i = startIndex; i < endIndex ; i ++)
    {
        dev += pow( buffer [i]- (sum/N), 2.0 ) ;
    }
    stddev = sqrt( dev / N ) ;
        
    return stddev;
}
    
void StepDetection::update(float seconds)
{
    SignalAnalysis::update(seconds);
    if( data1.size() < windowSizeA ) return ;
    
    //fill double with buffer values from x
    double inputValuesX[data1.size()] ;
    double inputValuesY[data1.size()] ;
//    double inputValuesZ[data1.size()] ;
    
    for(int i=0; i < data1.size(); i++)
    {
        inputValuesX[i] = data1[i]->getData(2); //get x accel
    }
    double *diff = new double[data1.size()];
    double *diffBefore = new double[data1.size()];
    derivative(inputValuesX, diffBefore, data1.size());
    derivative(diffBefore, diff, data1.size());
    
    execute(xdMove, xSwing, xPeak, data1.size(), inputValuesX, false );
    execute(xdMove, xdSwing, xdPeak, data1.size(), diffBefore, false );
    execute(xdMove, xd2Swing, xd2Peak, data1.size(), diff, false );
    
    //fill double with buffer values from y
    for(int i=0; i < data1.size(); i++)
    {
        inputValuesY[i] = data1[i]->getData(3); //get y accel
    }
    derivative(inputValuesY, diffBefore, data1.size());
    derivative(diffBefore, diff, data1.size());
    execute(yMove, ySwing, yPeak, data1.size(), inputValuesY, true );
    execute(ydMove, ydSwing, ydPeak, data1.size(), diffBefore, true );
    execute(yd2Move, yd2Swing, yd2Peak, data1.size(), diff, true );
    
    /********NEW*********/
    
    int window = getNewSampleCount()+2;
    
    double mag[int(data1.size())], magDiff[data1.size()], magDiff2[data1.size()];
    findMag(inputValuesX, inputValuesY, mag, data1.size());
    derivative(mag, magDiff, data1.size());
    derivative(magDiff, magDiff2, data1.size());
    
    bool midSwing = false;
    int index = data1.size()-window;
    while (index < data1.size() && !midSwing)
    {
        if(  ( magDiff[index] == 0 ) ||  index > 0  )
        {
            if(  ( magDiff[index] == 0 ) || ( magDiff[index] > 0  && magDiff[index-1] <= 0 ) ||  ( magDiff[index] < 0  && magDiff[index-1] >= 0 ) )
            {
//                zeroCrossingCount++;
                //                    std::cout << "zeroCrossingCount  " << zeroCrossingCount << "   diff2:  " << diff2[index] << std::endl;
                midSwing = magDiff2[index] < 0 ;//|| magDiff2[index] == 0;
                //midSwing = true;
            }
        }
        index++;
    }
    
    midSwing =  (midSwing) && ( (sinceLastPeak > waitBetweenPeaks ) );
    if( midSwing || lastFramePeak ) sinceLastPeak = 0;
    else sinceLastPeak += getNewSampleCount(); //get about 2-3 samples each cycle?
    lastFramePeak = midSwing;
    
      combinedPeak = midSwing; //OK FOR NOW -- TODO: seperate this into STDDEV signal & step detection
}
    
bool StepDetection::execute(std::vector<double> input)
{
    double stdDevTot = 0;
    double stdDevA ;
    double sumOrigSigA = 0;
    bool peak = false;
//    bool swing = false;
    bool move = false;
    
    int bufferSize = input.size();
    int bufferSizeB = (windowSizeB < bufferSize) ? windowSizeB : bufferSize;
    
    stdDevTot = findStdDev(input, 0, bufferSize); //std dev of all window
    double stdDevB =  findStdDev(input, bufferSize-bufferSizeB, bufferSize); //std dev of 1 sec.
    
    // Define 1 sec block as move / stop
    if ( stdDevB > 0.2*stdDevTot ){
        move = true;
    }
    else{
        move = false ;
    }
    
    // Calculate STDDEV for 0.1 sec window (A) that has been filled
    if( move )
    {
        int bufferSizeA =  (windowSizeA*2 < bufferSize) ? windowSizeA*2 : bufferSize;
        stdDevA = findStdDev(input, bufferSize-bufferSizeA, bufferSize);
        
        for(int i=bufferSize-bufferSizeA; i<bufferSize; i++)
        {
            sumOrigSigA += input[i];
        }
        // Define 0.1 sec window (A) as stance / swing
////        if ( stdDevA > 0.2 * stdDevTot ){ //was 2.0 factor... checking...
////            swing = true;
//        }
//        else{
//            swing = false;
//        }
    }
//    else{
//        swing = false;
//    }
    
    if( move )
    {
        // Define 0.1 sec window (A) as peak / nonpeak
        if ( stdDevA > stdDevTot ){ //&& ( sumOrigSigA < 0 )){//|| !isY ) ){
            peak = true;
        }
        else{
            peak = false;
        }
    }
    else{
        peak = false ;
    }
    
    return peak;

}

    
bool StepDetection::findSwing(std::vector<double> input)
{
    double stdDevTot = 0;
    double stdDevA ;
    double sumOrigSigA = 0;
    bool swing = false;
    bool move = false;
        
    int bufferSize = input.size();
    int bufferSizeB = (windowSizeB < bufferSize) ? windowSizeB : bufferSize;
        
    stdDevTot = findStdDev(input, 0, bufferSize); //std dev of all window
    double stdDevB =  findStdDev(input, bufferSize-bufferSizeB, bufferSize); //std dev of 1 sec.
        
        // Define 1 sec block as move / stop
    if ( stdDevB > 0.2*stdDevTot ){
        move = true;
    }
    else{
        move = false ;
    }
    
    // Calculate STDDEV for 0.1 sec window (A) that has been filled
    if( move )
    {
        int bufferSizeA =  (windowSizeA < bufferSize) ? windowSizeA : bufferSize;
        stdDevA = findStdDev(input, bufferSize-bufferSizeA, bufferSize);
        
        for(int i=bufferSize-bufferSizeA; i<bufferSize; i++)
        {
            sumOrigSigA += input[i];
        }
        // Define 0.1 sec window (A) as stance / swing
        if ( stdDevA > 0.2*stdDevTot ){ //was 2.0 factor... checking...
            swing = true;
        }
        else{
            swing = false;
        }
    }
    else{
        swing = false;
    }
    return swing;
};

    bool StepDetection::execute( bool &move, bool &swing, bool &peak,int bufferS, double inputValues[], bool isY )
    {
        double stdDevTot = 0;
        double stdDevA ;
        double sumOrigSigA = 0;
        
        int bufferSize = bufferS;
        int bufferSizeB = (windowSizeB < bufferSize) ? windowSizeB : bufferSize;
        
        //find derivative of the signal
        double *diff;// = new double[bufferSize];
        double *diffBefore = new double[bufferSize];
        
        diff = inputValues;
        
        //derivative(inputValues, diff, bufferSize);

//        derivative(inputValues, diffBefore, bufferSize);
//        derivative(diffBefore, diff, bufferSize);
        
//        int newS = std::min(getNewSampleCount(), bufferSize);
//        for(int i= bufferSize - newS; i<bufferSize; i++)
//        {
//            std::cout << data1[i]->getData(0) << "," << inputValues[i] << "," << diffBefore[i] <<"," << diff[i] << "\n";
//        }
        
        stdDevTot = findStdDev(diff, 0, bufferSize); //std dev of all window
        double stdDevB =  findStdDev(diff, bufferSize-bufferSizeB, bufferSize); //std dev of 1 sec.
        
        // Define 1 sec block as move / stop
        if ( stdDevB > 0.2*stdDevTot ){
            move = true;
        }
        else{
            move = false ;
        }
        
        // Calculate STDDEV for 0.1 sec window (A) that has been filled
        if( move )
        {
            int bufferSizeA =  (windowSizeA < bufferSize) ? windowSizeA : bufferSize;
            stdDevA = findStdDev(diff, bufferSize-bufferSizeA, bufferSize);
            
            for(int i=bufferSize-bufferSizeA; i<bufferSize; i++)
            {
                sumOrigSigA += inputValues[i];
            }
            // Define 0.1 sec window (A) as stance / swing
            if ( stdDevA > 0.2 * stdDevTot ){ //was 2.0 factor... checking...
                swing = true;
            }
            else{
                swing = false;
            }
        }
        else{
            swing = false;
        }
        
        if( move )
        {
            // Define 0.1 sec window (A) as peak / nonpeak
            if ( stdDevA > stdDevTot ){ //&& ( sumOrigSigA < 0 )){//|| !isY ) ){
                peak = true;
            }
            else{
                peak = false;
            }
        }
        else{
            peak = false ;
        }
        
        //TODO: peak removal -- consider lower peaks when x & y trajectory the same
        
        float prevPeak = 0;
        for( int i=0; i<previousSwingPhase.size(); i++ )
        {
            prevPeak += previousSwingPhase[i];
        }
//        prevPeak /= previousSwingPhase.size();
//        peak = peak && ( prevPeak >=0.7 );  //FOR TESTING!!!
        
        previousSwingPhase.push_back(swing);
        if( previousSwingPhase.size() > windowSizePrevSwing )
        {
            previousSwingPhase.erase( previousSwingPhase.begin() );
        }
        
//        std::cout << "totStdDev    " << stdDevTot << " stddevA   " << stdDevA << "   stdDevB  " << stdDevB << std::endl;
        
        //delete [] diff;
        delete [] diffBefore;
        return true;
    };
    

    
    
std::vector<ci::osc::Message> StepDetection::getOSC()
{

    std::vector<ci::osc::Message> msgs;
        
    ci::osc::Message msg;
    
    //disabling OSC messages that we are not using 4-8-2015
    
//    msg.setAddress(SHIMMER_STEP_DETECTION2);
//    
//    msg.addIntArg(ID1);
//    msg.addIntArg(atoi(port1.c_str()));
//    
//    msg.addIntArg( xMove );
//    msg.addIntArg( xSwing ); // true if previous A block was was ’ swing ’ phase ( false = stance )
//    msg.addIntArg( xPeak ); // true if previous A block  was ’ peak ’ ( false = non peak )
//    
//    msg.addIntArg( yMove ); // true if previous B block was ’moving’ phase ( false = stop )
//    msg.addIntArg( ySwing ); // true if previous A block was was ’ swing ’ phase ( false = stance )
//    msg.addIntArg( yPeak ); // true if previous A block  was ’ peak ’ ( false = non peak )
//    
//    msg.addIntArg( zMove ); // true if previous B block was ’moving’ phase ( false = stop )
//    msg.addIntArg( zSwing ); // true if previous A block was was ’ swing ’ phase ( false = stance )
//    msg.addIntArg( zPeak ); // true if previous A block  was ’ peak ’ ( false = non peak )
//
//    msg.addIntArg( combinedPeak ); // true if previous A block  was ’ peak ’ ( false = non peak )
    
    return msgs;
}
    
// Actions
    
class PeakTimer
{
protected:
    double waitTime;
    double peakTime;
    double curTime;
public:
    PeakTimer()
    {
        waitTime = 0;
        peakTime = 0;
        curTime = 0;
    };
    
    PeakTimer(double w)
    {
        waitTime = w;
    };
    
    void setWaitTime(double wt)
    {
        waitTime = wt;
    };
    
    void peakHappened()
    {
        peakTime = curTime;
    };
    
    void update(double seconds)
    {
        curTime = seconds;
    };
    
    bool readyForNextPeak()
    {
        return ( (peakTime + waitTime) < curTime );
    };
};
    
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
    
class SnapPeaks: public StepDetection
{
protected:
    double prevPeak;
    bool peakFound;
    BeatTiming *beatTimer;
    PeakTimer peakTimer;
    double curPeakHeight;
    int whichAxisPeak;
    
    double xThresh, yThresh, zThresh;
    bool stepOnlyOnBeat;
    int peakcount;
    
public:
    SnapPeaks( int id1, std::string portz1, SignalAnalysis *s, BeatTiming *bt=NULL, bool onbeat = false) : StepDetection( id1, portz1, s )
    {

        peakcount = 0;
        
        xPeak = false;
        yPeak = false;
        zPeak = false;
        
        combinedPeak = false;
        lastFramePeak = false;
        
        ID1 = id1;
        port1 = portz1;
        
        sinceLastPeak = waitBetweenPeaks + 1;
        
        prevPeak = 0;
        peakFound = false;
        
        lastZPeak = false;
        
        beatTimer = bt;
        peakTimer.setWaitTime( beatTimer->getPulse8() ); //for space of a 16th note don't recognize another
        //tODO: must change when bpm changes
        
        curPeakHeight = 0; //current peak, of all axes
        
        whichAxisPeak = 0;
        
        xThresh = 0.75;
        yThresh = 1.0;
        zThresh = 0.4;
        
//        xThresh = 0.6;
//        yThresh = 0.8;
//        zThresh = 0.4;

        
        stepOnlyOnBeat = onbeat;

    };
    
    void setThreshes(double x, double y, double z)
    {
        xThresh = x;
        yThresh = y;
        zThresh = z;
    };
    
    bool getCombinedPeak()
    {
        return combinedPeak;
    };
    
    double getCurrentPeak()
    {
        return curPeakHeight;
    };
    
    //TODO: refactor eek!!!!
    std::vector<double> derivative(std::vector<double> input)
    {
        assert(input.size() > 2); //absolute min.
        
        std::vector<double> output;
        output.push_back(0);
        for(int i=1; i<input.size(); i++)
        {
            output.push_back( input[i-1] - input[i] );
        }
        return output;
    };

    
    void findpeaks(double input[], std::vector<double> &peaks, std::vector<int> &indices, int sz, int start )
    {
        /* from matlab
         y = y(:)';
         
         switch length(y)
         case 0
         ind = [];
         case 1
         ind = 1;
         otherwise
         dy = diff(y);
         not_plateau_ind = find(dy~=0);
         ind = find( ([dy(not_plateau_ind) 0]<0) & ([0 dy(not_plateau_ind)]>0) );
         ind = not_plateau_ind(ind);
         if y(1)>y(2)
         ind = [1 ind];
         end
         if y(end-1)<y(end)
         ind = [ind length(y)];
         end
         end
         
         if nargout > 1
         peaks = y(ind);
         end
         
         the important thing here is to test the cases at the edge, since this works with buffered sound 
         */
        std::vector<double> dy;
        StepDetection::derivative(input, dy, sz);
        
        //find non-zero
        std::vector<int> not_plateau_ind;
        for(int i=start; i<sz; i++)
        {
            if( dy[i]!=0 ) not_plateau_ind.push_back(i);
        }
        
        if(not_plateau_ind.empty()) return;
        for( int i=1; i<not_plateau_ind.size()-1; i++ )
        {
            if( ( dy[not_plateau_ind[i+1]] < 0 )  &  ( dy[not_plateau_ind[i]] > 0 )  )
            {
                indices.push_back( not_plateau_ind[i]  );
            }
        }
        
        /* expanding the buffer input to this function takes care of edge cases (rather than the matlab version) */
        for( int i=0; i<indices.size(); i++)
            peaks.push_back(input[indices[i]]);
    }
    
    bool findIC(int dataIndex, double &curpeak, double thresh=0.75, int takeDerivative=1 )
    {
        //        find z peaks
        std::vector<double> inputX;
        double dX[data1.size()];
        std::vector<double> xmin;
        std::vector<int> xminIndex;
        
        for(int i=0; i<data1.size(); i++)
        {
            inputX.push_back( data1[i]->getData(dataIndex) );
        }
        
        //take the derivative -- trying from just aceel to test
        for(int i=0; i<takeDerivative; i++){
            inputX = derivative(inputX);
        }
        
        for(int i=0; i<data1.size(); i++)
        {
            dX[i] = inputX[i];
        }
        
        int snapSample = data1.size() - std::min(getNewSampleCount(), int(SR/4)) - 5;
        bool FoundPeakX = false;
        findpeaks( dX, xmin, xminIndex, data1.size(), snapSample );
        
        //this method also finds the curPeakHeight
        curpeak = 0.0;
        for( int i=0; i<xminIndex.size(); i++ )
        {
            if(!FoundPeakX) FoundPeakX = (xminIndex[i] >= snapSample+4) && (xmin[i] > thresh );
            if((xminIndex[i] >= snapSample+4))
                curpeak = std::max( curpeak, xmin[i]);
        }
        
        
        return FoundPeakX;
    };
    
    int whichAxisWasPeak()
    {
        return whichAxisPeak;
    };
    
    //TODO read these in from database, yikes!! etc.
    double findNormalizedPeakHeight(double x, double y, double z)
    {
//        double xmax = 7.66269;
//        double ymax = 6.46104;
//        double zmax = 6.87796;
        
        //min as the thresh & the max is +5 standard deviations (from a long test run)... anything above is super high, so
        double scaledValueMaxX = 2.75;
        double scaledValueMaxY = 6;
        double scaledValueMaxZ = 1.85;
        
//        double scaledValueMaxX = 1.5;
//        double scaledValueMaxY = 3;
//        double scaledValueMaxZ = 1.5;
        
        //scale 0-1 for like to like comparisions... of course, this is only relevant in terms of steps
        double normX =  ( x - xThresh) / ( scaledValueMaxX - xThresh );
        double normY =  ( y - yThresh) / ( scaledValueMaxY - yThresh );
        double normZ =  ( z - zThresh) / ( scaledValueMaxZ - zThresh );
        
        //find out which axis had the highest peak & return that
        double p = normX;
        whichAxisPeak = 0;
        if( normX < normY )
        {
            if( normY < normZ )
            {
                whichAxisPeak = 2;
                p = normZ;
            }
            else
            {
                whichAxisPeak = 1;
                p = normY;
            }
        }
        else if( normX < normZ  )
        {
            whichAxisPeak = 2;
            p = normZ;
        }
        
        
        if(isnan(p)) p =0;
        return p;
    };
    
    virtual void update(float seconds)
    {
        SignalAnalysis::update(seconds);
        combinedPeak = false; //CHECK -- but should solve problem of multiple beeps
        if( data1.size() < windowSizeB ) return ;
        if( getNewSampleCount() <=0 ) return ; 
        peakTimer.update(seconds);
        curPeakHeight = NO_DATA;
        
        double peakx=0, peaky=0, peakz=0;
        combinedPeak = ( findIC(2, peakx, xThresh) || findIC(4, peaky, yThresh) || findIC(3, peakz, zThresh) );
        
        if( stepOnlyOnBeat ) combinedPeak = beatTimer->isOnBeat() && combinedPeak;
        
//        if ( beatTimer->isOnBeat() )
//        {
//            std::cout << "is it on: " << std::endl;
//        }
        
        combinedPeak = ( combinedPeak ) && ( peakTimer.readyForNextPeak() );
        if( combinedPeak )
        {
            peakcount++;
//            std::cout << "PEAK: " << peakcount << std::endl;
        }
        if(combinedPeak)
            peakTimer.peakHappened();
        curPeakHeight = findNormalizedPeakHeight(peakx, peaky, peakz);
    };
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
        
        std::vector<ci::osc::Message> msgs;

        
        return msgs;
    }
    

    
protected:
    double lerp(double a, double b, double f)
    {
        return a + f * (b - a);
    };
    
    bool lastZPeak;
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
    
    //previous was jerk -- try with snap
class SnapPeaksFromSnap: public SnapPeaks
{
    SnapPeaksFromSnap( int id1, std::string portz1, SignalAnalysis *s, BeatTiming *bt=NULL, bool onbeat = false) :
        SnapPeaks(  id1,  portz1,  s,  bt,  onbeat )
    {
     
    };
    
    virtual void update(float seconds)
    {
        SignalAnalysis::update(seconds);
        combinedPeak = false; //CHECK -- but should solve problem of multiple beeps
        if( data1.size() < windowSizeB ) return ;
        if( getNewSampleCount() <=0 ) return ;
        peakTimer.update(seconds);
        curPeakHeight = NO_DATA;
        
        double peakx=0, peaky=0, peakz=0;
        combinedPeak = ( findIC(2, peakx, xThresh, 2 ) || findIC(4, peaky, yThresh, 2 ) || findIC(3, peakz, zThresh, 2 ) );
        
        if( stepOnlyOnBeat ) combinedPeak = beatTimer->isOnBeat() && combinedPeak;
        
        combinedPeak = ( combinedPeak ) && ( peakTimer.readyForNextPeak() );

        if(combinedPeak)
            peakTimer.peakHappened();
        curPeakHeight = findNormalizedPeakHeight(peakx, peaky, peakz);
    };

};

    //TODO: add measure of how syncopated something is...
class FootOnset : public SignalAnalysisEventOutput
{
protected:
    SnapPeaks *leftFoot, *rightFoot;
    bool leftOnset, rightOnset;
    float stepsPerSampleSize;
    double curPeak, avgPeak;
    DataWindow stepPeaks;
    
    bool fakeStep;
    
//    std::vector<float> stepTimes; // in seconds
//    std::vector<float> stepPeaks; // in seconds
    
    //norm step count is 4 secs by default, longer window is steps, etc. over 45 secs by default.
    float window, windowLong;
    int stepCount, stepCountLong;
    int rightID, leftID;
    double timeSinceLastStep;

//    void updateSteps(float curTime)
//    {
//        bool inWindow = false;
//        while( stepTimes.size()>0 && !inWindow )
//        {
//            inWindow = stepTimes[0] >= ( curTime - window );
//            if(!inWindow)
//            {
//                stepTimes.erase(stepTimes.begin());
//                stepPeaks.erase(stepPeaks.begin());
//            }
//        }
//    };
    
public:
    enum MotionDataIndices { STEP_COUNT=0, AVG_PEAK=1, TIME_SINCE_LAST_STEP=2, STEP_COUNT_WINDOWSZ=3, STEP_COUNT_LONG=4, STEP_COUNT_WINDOWSIZE_LONG=5, IS_STEP=6, CUR_STEP_PEAK=7, STEP_LEFT=8, STEP_RIGHT=9,
        WHICH_AXIS_PEAK=10};
    
    FootOnset(SnapPeaks *left, SnapPeaks *right, int id1, int id2, float sampleSize = DEFAULT_FOOT_ONSET_STEP_COUNT_WINDOWSIZE, float sampleSizeLong = DEFAULT_FOOT_ONSET_STEP_COUNT_WINDOWSIZE_LONGER )
            :  SignalAnalysisEventOutput(NULL, 0, NULL)
    {
        leftFoot = left;
        rightFoot = right;
        leftOnset = false;
        rightOnset = false;
        window = sampleSize;
        stepsPerSampleSize = 0; //includes kicks... might be a problem with swinging legs
        stepCount = 0;
        avgPeak = 0;
        leftID = id1;
        rightID = id2;
        timeSinceLastStep = 0;
        windowLong = sampleSizeLong;
        
        stepPeaks.setWindowSize(sampleSizeLong);
        
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_COUNT));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::AVG_PEAK));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::TIME_SINCE_LAST_STEP));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_COUNT_WINDOWSZ));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_COUNT_LONG));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_COUNT_WINDOWSIZE_LONG));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::IS_STEP));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::CUR_STEP_PEAK));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_LEFT));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::STEP_RIGHT));
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, MotionDataIndices::WHICH_AXIS_PEAK));


        
        motionData[MotionDataIndices::STEP_COUNT]->setName("Step Count");
        motionData[MotionDataIndices::AVG_PEAK]->setName("Avg. Peak");
        motionData[MotionDataIndices::STEP_COUNT_WINDOWSZ]->setName("Long window size");
        motionData[MotionDataIndices::STEP_COUNT_LONG]->setName("longer window step count");
        motionData[MotionDataIndices::STEP_COUNT_WINDOWSIZE_LONG]->setName("longer window");
        motionData[MotionDataIndices::IS_STEP]->setName("is step");
        motionData[MotionDataIndices::CUR_STEP_PEAK]->setName("current step accel. peak");
        motionData[MotionDataIndices::WHICH_AXIS_PEAK]->setName("which axis was the peak");

        
        setMotionAnalysisMinMax(MotionDataIndices::STEP_COUNT, 0, 5 );
        setMotionAnalysisMinMax(MotionDataIndices::AVG_PEAK, 0, 1);
        setMotionAnalysisMinMax(MotionDataIndices::IS_STEP, 0, 1);
        setMotionAnalysisMinMax(MotionDataIndices::CUR_STEP_PEAK, 0, 1);
        setMotionAnalysisMinMax(MotionDataIndices::STEP_LEFT, 0, 1);
        setMotionAnalysisMinMax(MotionDataIndices::STEP_RIGHT, 0, 1);

        
        //window size doesn't change so set it now
        ( (MotionAnalysisEvent *)motionData[MotionDataIndices::STEP_COUNT_WINDOWSZ])->setValue(sampleSize);
        ( (MotionAnalysisEvent *)motionData[MotionDataIndices::STEP_COUNT_WINDOWSIZE_LONG])->setValue(0);

    };
    
    virtual void updateMotionData()
    {
        motionData[MotionDataIndices::STEP_COUNT]->setValue(stepCount);
        motionData[MotionDataIndices::AVG_PEAK]->setValue(avgPeak);
        motionData[MotionDataIndices::TIME_SINCE_LAST_STEP]->setValue(timeSinceLastStep);
        motionData[MotionDataIndices::STEP_COUNT_LONG]->setValue(stepCountLong);
        motionData[MotionDataIndices::STEP_COUNT_WINDOWSIZE_LONG]->setValue(stepPeaks.curWindowSize()); //during first section, etc. will be not full-size
        motionData[MotionDataIndices::IS_STEP]->setValue(isStepping()); //during first section, etc. will be not full-size
        motionData[MotionDataIndices::CUR_STEP_PEAK]->setValue(curPeak); //accel peak of current or last recorded step
        motionData[MotionDataIndices::STEP_LEFT]->setValue(isLeftOnset()); //accel peak of current or last recorded step
        motionData[MotionDataIndices::STEP_RIGHT]->setValue(isRightOnset()); //accel peak of current or last recorded step
        
        int whichAxis;
        if( isRightOnset() && !isLeftOnset() )
            whichAxis = rightFoot->whichAxisWasPeak();
        else if( isLeftOnset() && !isRightOnset() )
            whichAxis = leftFoot->whichAxisWasPeak();
        else
        {
            whichAxis = std::max( leftFoot->whichAxisWasPeak(), rightFoot->whichAxisWasPeak() );
        }
        motionData[MotionDataIndices::WHICH_AXIS_PEAK]->setValue( whichAxis ); //accel peak of current or last recorded step
    };
    
    void update(float seconds)
    {
        stepPeaks.update(seconds);
        
        bool lastLeftOnset = leftOnset;
        
        leftOnset = leftFoot->getCombinedPeak();
        rightOnset = rightFoot->getCombinedPeak();
        
        if(fakeStep)
        {
            fakeStep = false;
            leftOnset = rightOnset = true;
            //peak will still be 0 or whatever
        }
        
        if( leftOnset && rightOnset )
        {
            if ( !lastLeftOnset )
            {
                rightOnset = false;
            }
            else leftOnset = false;
        }
        
        if(leftOnset || rightOnset)
        {
            if( leftOnset )
                curPeak = leftFoot->getCurrentPeak();
            else curPeak = rightFoot->getCurrentPeak();
            stepPeaks.push_back(curPeak, seconds);
        }
        if( stepPeaks.size() > 0 )
        {
            timeSinceLastStep = seconds - stepPeaks.lastTime() ;
        }
        
        stepCount = stepPeaks.size(seconds, window);
        stepCountLong = stepPeaks.size(seconds, windowLong);
        avgPeak = stepPeaks.getAverageOverWindow(window, seconds);
        if(isnan(avgPeak)) avgPeak = 0; 
//        std::cout << "avgPeak: " << avgPeak << std::endl;
        updateMotionData();
//        std::cout << "after assign: " << motionData[1]->asDouble() << std::endl;
    };
    
    inline int getStepCount()
    {
        return stepCount;
    };
    
    inline bool isRightOnset()
    {
        return rightOnset;
    };

    inline bool isLeftOnset()
    {
        return leftOnset;
    };

    inline bool isStepping()
    {
        return ( rightOnset || leftOnset );
    };
    
    inline void createFakeStep()
    {
        fakeStep = true;
    }
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;
        
//don't send OSC... for now... maybe need to refactor this: CDB - 4/6/2015
        
//        ci::osc::Message msg;
//        
//        msg.setAddress(SHIMMER_STEP_DETECTION);
//        
//        msg.addIntArg(leftID);
//        msg.addIntArg(rightID);
//        
//        msg.addIntArg( getStepCount() );
//        msg.addIntArg( isLeftOnset() );
//        msg.addIntArg( isRightOnset() );
//        msg.addIntArg( isRightOnset() || isLeftOnset() );
//        msg.addFloatArg(curPeak);
//        msg.addFloatArg(timeSinceLastStep);
//
//
//        
//        msgs.push_back(msg);
    
        return msgs;
    }
    
};
    
};

#endif
