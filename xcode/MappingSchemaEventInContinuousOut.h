//
//  MappingSchemaEventInContinuousOut.h
//  InteractiveTangoReadFromAndroid
//
//  Created by courtney on 4/6/15.
//
//

#ifndef InteractiveTangoReadFromAndroid_MappingSchemaEventInContinuousOut_h
#define InteractiveTangoReadFromAndroid_MappingSchemaEventInContinuousOut_h

namespace InteractiveTango
{
    

class VolumePattern
{
public:
    enum PeakType { LIGHT, HEAVY };
    enum BusySparseType { SPARSE, NORM, BUSY };

    
    VolumePattern(std::vector<double> p, PeakType pt, BusySparseType bst)
    {
        _ptype = pt;
        _bstype = bst;
        _pattr = p;
    };
    
    int operator[](int i)
    {
        return at(i);
    };
    
    int at(int i)
    {
        assert( i < _pattr.size() && i>=0 );
        return _pattr[i];
    }

    size_t size()
    {
        return _pattr.size();
    };
    
    PeakType getPeakType()
    {
        return _ptype;
    };
    
    BusySparseType getBSType()
    {
        return _bstype;
    };
    
protected:
    std::vector<double> _pattr;
    PeakType _ptype;
    BusySparseType _bstype;
};
    

    //ok it isn't continuous, will change name
class OnsetPeakToAccentPatterns : public ContinousOutMappingSchema
{
protected:
    int _ID; //leader or follower, generally
//    int index;
    int selectedPattern;
    int lastStepVal;
    std::vector<std::vector<std::vector<double>>> orderedIndex;
    std::vector<VolumePattern *> patterns;
    bool shouldSend;
    
    int busySparse;
    int isStep;
    std::vector<float> busySparseMIDINumeratorNorm, busySparseMIDINumeratorPeaks;
    
    std::vector<double> startStop(int start, int size)
    {
        std::vector<double> v;
        for(int i=start; i<size; i++)
            v.push_back(i);
        return v;
    };
    
    //TODO: add these to database!! also could refactor.
    void createVolumePatterns()
    {
        
        std::vector<std::vector<double>> pat = {{ 1, 1, 1, 1  }, { 1, 1, 2, 1  } , { 1, 2, 1, 1  }, { 1, 2, 2, 1  }, { 1, 1, 2, 2  } , { 1, 2, 1, 2  },{ 1, 2, 2, 2  }, { 2, 1, 1, 1  }, { 2, 1, 2, 1  }, { 2, 2, 1, 1  }, { 2, 1, 2, 1  },  { 2, 2, 2, 1  }, { 2, 1, 2, 2  }, { 2, 2, 1, 1  }};
        
        int start = 0;
        patterns.push_back( new VolumePattern( pat[0]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::BUSY ) );
        patterns.push_back( new VolumePattern( pat[1]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::BUSY ) );
        patterns.push_back( new VolumePattern( pat[2]   , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::BUSY ) );
        std::vector<double> heavyBusy = startStop(start, patterns.size());
        start = patterns.size();
        
        patterns.push_back( new VolumePattern( pat[3]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::NORM ) );
        patterns.push_back( new VolumePattern( pat[4]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::NORM ) );
        patterns.push_back( new VolumePattern( pat[5]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::NORM ) );
        std::vector<double> heavyNorm = startStop(start, patterns.size());
        start = patterns.size();

        patterns.push_back( new VolumePattern( pat[6]  , VolumePattern::PeakType::HEAVY, VolumePattern::BusySparseType::SPARSE ) );
        std::vector<double> heavySparse = startStop(start, patterns.size());
        start = patterns.size();
        
        patterns.push_back( new VolumePattern( pat[7] , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::BUSY ) );
        std::vector<double> lightBusy = startStop(start, patterns.size());
        start = patterns.size();
        
        patterns.push_back( new VolumePattern( pat[8] , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::NORM ) );
        patterns.push_back( new VolumePattern( pat[9]  , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::NORM ) );
        patterns.push_back( new VolumePattern( pat[10] , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::NORM ) );
        std::vector<double> lightNorm = startStop(start, patterns.size());
        start = patterns.size();

        patterns.push_back( new VolumePattern( pat[11] , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::SPARSE ) );
        patterns.push_back( new VolumePattern( pat[12]  , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::SPARSE ) );
        patterns.push_back( new VolumePattern( pat[13]  , VolumePattern::PeakType::LIGHT, VolumePattern::BusySparseType::SPARSE ) );
        std::vector<double> lightSparse = startStop(start, patterns.size());
        
        //creating indices
        std::vector<std::vector<double>> light;
        light.push_back(lightSparse);
        light.push_back(lightNorm);
        light.push_back(lightBusy);
        
        std::vector<std::vector<double>> heavy;
        heavy.push_back(heavySparse);
        heavy.push_back(heavyNorm);
        heavy.push_back(heavyBusy);
        
        orderedIndex.push_back(light);
        orderedIndex.push_back(heavy);
    };
    
    enum Factors{ PEAK_SIZE=0, BUSY_SPARSE=1, STEP_LEFT=2, STEP_RIGHT=3, LEFT_WINVARDIFF=4, RIGHT_WINVARDIFF=5, WHICH_AXIS_PEAK=6 };
    double DIVISOR; // will top & bottom peak size values and discard middle... default 3
    double ps;
    int lastIndexSent;
    
    int lastPS; //TODO: disable if implement patterns on the receiving side
    int whichAxis;
    std::string xyzPeak;

public:
    OnsetPeakToAccentPatterns(MotionAnalysisEvent *peakSize, MotionAnalysisEvent * busySparse, MotionAnalysisEvent *isStepLeft, MotionAnalysisEvent *isStepRight, MotionAnalysisEvent *leftFootWinVarDiff, MotionAnalysisEvent *rightFootWinVarDiff , MotionAnalysisEvent *whichAxisPeak, int did) : ContinousOutMappingSchema(did)
    {
        mData.push_back(peakSize);
        mData.push_back(busySparse);
        mData.push_back(isStepLeft);
        mData.push_back(isStepRight);
        mData.push_back(leftFootWinVarDiff);
        mData.push_back(rightFootWinVarDiff);
        mData.push_back(whichAxisPeak);
        
        mWeights.push_back(0.3);
        mWeights.push_back(0.7);
        
        _ID = did;
        
        selectedPattern = -1;
        DIVISOR = 3.0;
        lastStepVal = 0;
        isStep = false;
        shouldSend = false;
        
        createVolumePatterns();
        float middleVal = std::round(1.0*(0.75));
        busySparseMIDINumeratorPeaks = { 1.0/2.0 , middleVal , 1.0 };
        
        //ohhhhkaaaayyy try this shit
        busySparseMIDINumeratorNorm.push_back( std::round(double(1.0/2.0)*0.75) );
        busySparseMIDINumeratorNorm.push_back( std::round(middleVal*0.75) );
        busySparseMIDINumeratorNorm.push_back(  middleVal );
        
        lastIndexSent = -1;
        lastPS = -1;
    };
    
    virtual void update(float seconds = 0)
    {
        busySparse =  std::round( ((MotionAnalysisEvent *)mData[BUSY_SPARSE])->scaledValue() * (DIVISOR-1)  ) +1;
        ps =  ((MotionAnalysisEvent *)mData[PEAK_SIZE])->scaledValuePolyFit();
        
        //should send? only send once per step & when at extremes (light or heavy, not normal)
        isStep = mData[STEP_LEFT]->asInt() || mData[STEP_RIGHT]->asInt() ;
        shouldSend = isStep;
        
        if(isStep)
        {
            //averag in the winVarDiff of ea. leg
            MotionAnalysisEvent *winVarDiff;
            if( mData[STEP_LEFT]->asInt() ) winVarDiff = (MotionAnalysisEvent *) mData[LEFT_WINVARDIFF];
            else  winVarDiff = (MotionAnalysisEvent *) mData[RIGHT_WINVARDIFF];
            double wvd = winVarDiff->scaledValuePolyFit();
            
//            std::cout << "update: ps, " << ps << " wvd, " << wvd ;
            
            ps = std::round( (double(ps)*mWeights[0] + wvd*mWeights[1]) * (DIVISOR-1) ) + 1;
            
//            std::cout << "   final, " << ps << std::endl; 
            
            shouldSend = isStep && !lastStepVal && (ps==1 || ps==DIVISOR) ;
            
            if( shouldSend )
            {
                if(ps == 1) ps = 0; else ps=1; //convert to heavy v. light
                
//                std::cout << "ps: " << ps << " bs: " << busySparse-1 <<  " size: " << orderedIndex.size() ;
//                std::cout << " dimension ps: " << orderedIndex[ps].size() << std::endl;
                
                std::vector<double> p = orderedIndex[ps][busySparse-1];
                int index = std::rand() % p.size();
                shouldSend = !( index == lastIndexSent  && ps == lastPS) ;
                lastIndexSent = index;
                lastPS = ps;
                
                whichAxis = mData[WHICH_AXIS_PEAK]->asInt();
                if(whichAxis==0)
                    xyzPeak = "X";
                else if(whichAxis==1)
                    xyzPeak = "Y";
                else xyzPeak = "Z";
     
                
                selectedPattern = p[ index ];
            }
        }
        
        lastStepVal = isStep;

    };
    
    float limitPrecisionToThree(float num)
    {
        return float(std::round( num * 1000 )) / 1000.0;
    }
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
     
        std::vector<ci::osc::Message>  msgs;
        
        if(shouldSend || isStep)
        {
            ci::osc::Message msg;
            std::stringstream ss;
            ss << HEAVY_TO_MIDI_VEL << "/" << _ID;
            msg.setAddress(ss.str());
            
            if( shouldSend )
            {
                
//                std::cout<< "In OSC: " <<  ss.str() << ": "<< ","<<busySparse <<" , "<< ps
//                << " , " << xyzPeak << " , " << ((MotionAnalysisEvent *)mData[PEAK_SIZE])->asFloat() << " , ";

                for(int i=0; i<patterns[selectedPattern]->size(); i++)
                {
                    double numerator = busySparseMIDINumeratorPeaks[busySparse-1];
                    double denom = patterns[selectedPattern]->at(i) ;
                    
                    //add some random variation...
                    double res = limitPrecisionToThree(( numerator / denom )  + ((double(std::rand())/double(RAND_MAX))/10.0));
                    msg.addFloatArg( res );
                    
//                    if( i==0 ) std::cout << res << " , ";
                }
//                std::cout << std::endl;
                msgs.push_back( msg );
            }
            else if (isStep)
            {
                
//                std::cout<< "In OSC: " <<  ss.str() << ": "<< ps << ","<<busySparse
//                << ",";
                
                //yikes fudging it TODO: CLEAN!!!!!
                for(int i=0; i<4; i++)
                {
                    msg.addFloatArg( busySparseMIDINumeratorNorm[busySparse-1] + (std::rand()/(10.0*double(RAND_MAX))));
                    
//                    if( i==0 )
//                        std::cout <<  limitPrecisionToThree(busySparseMIDINumeratorNorm[busySparse-1] + ((double(std::rand())/double(RAND_MAX))/6.0)) << " , ";
                }
                
                msgs.push_back( msg );
//                std::cout << std::endl;
            }
        }
        
        return msgs;
    };


};
    
//send accel signal to ableton/m4l for a long step
class LongStepSendSignal : public ContinousOutMappingSchema {
protected:
    enum Factors{ STEP_LEFT=0, STEP_RIGHT=1 };
    Filter *filteredLeft, *filteredRight;
    float longStepDef;
    int firstSteps;  //don't do this after phrases end
    MainMelodySection *melody;
    bool shouldSend;
    bool lastStepRight;
    bool lastStepLeft;
    
    float timeSinceLastStep;
    float stepTime;
    float lastSend;
    
public:
    LongStepSendSignal(BeatTiming *timer, MotionAnalysisEvent *isStepLeft, MotionAnalysisEvent *isStepRight, SignalAnalysis *fLeft=NULL, SignalAnalysis *fRight=NULL, int did=0) : ContinousOutMappingSchema(did)
    {
        mData.push_back(isStepLeft);
        mData.push_back(isStepRight);
        
        filteredLeft  = (Filter *) fLeft;
        filteredRight = (Filter *) fRight;
        
        filteredLeft->calcMotionData(true);
        filteredRight->calcMotionData(true);
        
        longStepDef = timer->getPulse8()*12 * 1; //default 1.5 bars
        melody = NULL;
        firstSteps = 0; //don't do this 1st 2 steps or while at the end of a phrase
        timeSinceLastStep = 0;
        stepTime = 0;
        
        lastStepRight = false;
        lastStepLeft = false;
        lastSend = false;
    };
    
    void setMelody(MainMelodySection *mel)
    {
        melody = mel;
    };
    
    virtual void update(float seconds = 0)
    {
        bool isStep = mData[STEP_LEFT]->asInt() || mData[STEP_RIGHT]->asInt() ;
        
        if( isStep )
        {
            lastStepLeft = mData[STEP_LEFT]->asInt();
            lastStepRight = mData[STEP_RIGHT]->asInt();

            timeSinceLastStep = 0;
            stepTime = seconds;
        }
        else timeSinceLastStep = seconds - stepTime;
        
//try w/o this for now
//        bool phraseWaiting = ( melody != NULL );
//        if( phraseWaiting ) phraseWaiting = melody->afterEndOfPhrase();
        
        lastSend = shouldSend;
        shouldSend = ( timeSinceLastStep >= longStepDef );
    };
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
        
        std::vector<ci::osc::Message>  msgs;
        if( shouldSend )
        {
            if(!lastSend)
            {
                ci::osc::Message msgStart;
                msgStart.setAddress(LONGSTEP_START);
                msgStart.addIntArg(_id);
                msgs.push_back(msgStart);
                
            }
            
            ci::osc::Message msg;
            msg.setAddress(LONGSTEP_ACCEL);
            
            Filter *f;
            if(lastStepLeft) f = filteredRight;
            else f = filteredLeft;
            
            msg.addIntArg(_id);
            for(int i=0; i<5; i++)
                msg.addFloatArg( f->getMotionData().at(i)->scaledValue());
            
            msgs.push_back(msg);
        }
        else if( lastSend )
        {
            ci::osc::Message msgEnd;
            msgEnd.setAddress(LONGSTEP_END);
            msgEnd.addIntArg(_id);
            msgs.push_back(msgEnd);
//            std::cout << LONGSTEP_END << std::endl;
        }
        return msgs;
    
    };
    
    
};
        
    
class PointyVsSmoothContinuous : public PerceptualContinuous
{
protected:
    MotionAnalysisEvent *peakS;
public:
    enum Factors{ STEP_NUM=0, WINVARDIFF=1, PEAK_SIZE=2 };
        
    PointyVsSmoothContinuous(BeatTiming *timer, MotionAnalysisEvent *stepNum,  MotionAnalysisEvent *winVarDiff, MotionAnalysisEvent *peakSize,
                              long p_id=0, double window_size = 1.5 ) : PerceptualContinuous(timer, window_size)
    {
        //create set of factors
        mData.push_back( stepNum );
        mData.push_back( winVarDiff );
        mData.push_back( peakSize ); //just look at values for now
        peakS = peakSize;
            
        //change from 0 to 1 FOR NOW
        setMinMaxMood(0, 1);
            
        //add weights to those factors
        mWeights.push_back(0.15);
        mWeights.push_back(0.55);
        mWeights.push_back(0.30);
        
        //set ID --> to distinguish between dancers, etc.
        _ID = p_id;
        
    };
//    
//    virtual double findMood()
//    {
//        double equalWeight = ( (double) maxMood )/( (double) mData.size() );
//        double mood = 0;
//        for(int i=0; i<mWeights.size(); i++)
//        {
//            mood = mood + (mWeights[i]* ((MotionAnalysisEvent *)mData[i])->scaledValuePolyFit())*equalWeight;         }
//        return mood;
//    };
//    
    virtual void determineMood()
    {
        curMood = std::max(findMood(), (double) minMood);
        curMood = std::min(curMood, maxMood);
    };
    
    virtual double findMood()
    {
        double mood = 0;
        for(int i=0; i<mWeights.size(); i++)
        {
            mood = mood + (mWeights[i]* ((MotionAnalysisEvent *)mData[i])->scaledValuePolyFit()); //CHANGED 2/22/2016 //changed scaled value... -- //TODO: FIX
        }
        return mood;
    };
    
    virtual std::vector<ci::osc::Message> getOSC()
    {

        std::vector<ci::osc::Message>  msgs; 
        ci::osc::Message msg;
        
        std::stringstream ss;
        ss << POINTY_VS_SMOOTH << "/" << _ID;
        
        msg.setAddress(ss.str());
//        msg.addIntArg(_ID); //send ID -- this is really ID of the user of this mapping, eg. which dancer or couple --> 0 is sent to everything
        msg.addFloatArg(getCurMood());
        //std::cout << "getCurrentMood: " << getCurMood() << std::endl;
        
//        if( !getName().compare("Follower Pointy vs. Smooth") );

//         std::cout << "ID: " << _ID << " " << getName() << "," << getCurMood() <<std::endl;
//        
//        std::cout <<"StepNum: " << ((MotionAnalysisEvent *)mData[0])->scaledValuePolyFit() << "    ";
//        std::cout <<"WinVarDiff: " << ((MotionAnalysisEvent *)mData[1])->scaledValuePolyFit() << "    ";
//        std::cout <<"peakSize: " << ((MotionAnalysisEvent *)mData[2])->scaledValuePolyFit() << "\n";
        
        
        msgs.push_back( msg );

        return msgs;
 
    };
};
    
    
    //this measures spatial and mood similarity between dancers (they should be related)
    //more useful for when there is more than 1 dancer
    //this is for the whole dance floor
    class RoomSizeAreaContinuous : public PerceptualContinuous
    {
    public:
        enum Factors{ CROSSCOVAR=0, BUSY_SPARSE_BTW_PAREJAS=1, POINTY_ROUNDED_PAREJAS=2} ;//, POINTY_ROUNDED_BTW_LEADERS=3, POINTY_ROUNDED_BTW_FOLLOWERS=4, BUSY_SPARSE_BTW_LEADERS=5, BUSY_SPARSE_BTW_FOLLOWERS=6 };
        
        int factorSize;
        int locationOfCrossCoVarAndNumberOfFactorsPerCouple;
        int parejaCount;
        
        RoomSizeAreaContinuous( BeatTiming *timer, double window_size = 2.5 ) : PerceptualContinuous(timer, window_size)
        {
            //change from 1 to 3 FOR NOW
            setMinMaxMood(0, 1);
            
            //add weights to those factors
            mWeights.push_back(0.50);
            mWeights.push_back(0.40);
            mWeights.push_back(0.10);
            
            //fudgin' it
            factorSize = mWeights.size();
            locationOfCrossCoVarAndNumberOfFactorsPerCouple = 5; //TODO: change when add more parejas
            parejaCount = 0;
        };
        
        
        //for only one couple now -- TODO: multiply for many
        void addPareja(MotionAnalysisEvent *crosscovar, MotionAnalysisEvent *bsleader, MotionAnalysisEvent *bsfollower, MotionAnalysisEvent *prleader, MotionAnalysisEvent *prfollower)
        {
            mData.push_back(crosscovar);
            
            mData.push_back(bsleader);
            mData.push_back(bsfollower);
            
            
            mData.push_back(prleader);
            mData.push_back(prfollower);
            
            parejaCount++;
        }
        
        virtual void determineMood()
        {
            curMood = std::max(findMood(), (double) minMood);
            curMood = std::min(curMood, maxMood);
        };
        
        double perceptualEventDistanceMeasure(double val1, double val2)
        {
            //whelp, find the difference -- flip so that 0 -- least similar & 1 most similar -- note: this is exponiental
            return ( 1 - (  (val1-val2) * (val1-val2) ) );
        };
        
        std::vector<double> avgAcrossDancers()
        {
//            assert( mData.size() % locationOfCrossCoVarAndNumberOfFactorsPerCouple ); //TODO: check
            std::vector<double> factors;
            
            //avg the crosscovar across dancers
            factors.push_back(0);
            for( int i=0; i< parejaCount ; i++ )
            {
                factors[CROSSCOVAR]+= ((MotionAnalysisEvent *) mData[i*locationOfCrossCoVarAndNumberOfFactorsPerCouple])->scaledValuePolyFit();
            }
            factors[CROSSCOVAR] /= parejaCount;
            
            //find a distance measure between remaining distance measures, which for now involve 2 people.
            for (int f=1; f<factorSize; f++ )
            {
                factors.push_back(0.0);
                for( int p=0; p<parejaCount; p++ )
                {
                    factors[f] += perceptualEventDistanceMeasure( mData[f + (p*locationOfCrossCoVarAndNumberOfFactorsPerCouple) ]->scaledValue(),
                                                                        mData[f+(p*locationOfCrossCoVarAndNumberOfFactorsPerCouple)+1]->scaledValue() );
                }
                factors[f] /= parejaCount;
            }
            
            return factors;
 
        };
        
        virtual double findMood()
        {
            std::vector<double> factors = avgAcrossDancers();
            
            double mood = 0;
            for(int i=0; i<mWeights.size(); i++)
            {
                mood = mood + ( mWeights[i] * factors[i] );
            }
            return mood;
        };
    
        virtual std::vector<ci::osc::Message> getOSC()
        {
        
            std::vector<ci::osc::Message>  msgs;
            ci::osc::Message msg;
            msg.setAddress(ROOM_SIZE_GROUP_SIMILARITY);
            msg.addFloatArg(getCurMood());
            msgs.push_back( msg );
//            std::cout << ROOM_SIZE_GROUP_SIMILARITY << ": " << getCurMood() <<  "  CrossCoVar: " << ( ( MotionAnalysisEvent * ) mData[0])->scaledValuePolyFit()  <<"Busy Sparse:" << mData[1]->scaledValue() << " , " << mData[2]->scaledValue() <<  "   P v C :" << mData[3]->scaledValue() << " , " << mData[4]->scaledValue() << std::endl;
            return msgs;
        
        };
    
};
    
    //lower sample rate of signal -- only send at a fraction of send rate -- send an avg --
    //TODO: PUT in better place -- convert to signal analysis, UNGEN, etc. DO WHEN TIME!! REALLY DO THIS!
    class DegradeMotionData
    {
        int divisor;
        int index;
        std::vector<std::vector<double>> buf;
        Filter *filtered;
        std::vector<float> avgs;
        
    public:
        DegradeMotionData(Filter *signal = NULL, int div = 1)
        {
            divisor = div;
            index = 0;
            
            for( int i=0; i<signal->motionDataSize(); i++ )
            {
                std::vector<double> md;
                buf.push_back(md);
            }
            filtered = signal;
        };
        
        void setDiv(int d)
        {
            divisor = d;
        };
        
        void update()
        {
            index++;
            index = index % divisor;
            
            for( int i=0; i<filtered->motionDataSize(); i++ )
            {
                buf[i].push_back( filtered->getMotionDataItem(i)->scaledValue() );
            }
            
            if( shouldSend() )
            {
                avgs = calcAndEmptyBuffers();
            }
        };
        
        size_t getDataSize(){ return filtered->motionDataSize(); };
        
        double getAvg(int i)
        {
            double sum = 0;
            for(int j=0; j<buf[i].size(); j++)
            {
                sum += buf[i][j];
            }
            sum /= buf[i].size();
            buf[i].clear();
            
            return sum;
        };
        int returnSR(){ return FRAMERATE / divisor; };
        bool shouldSend(){ return ( index == 0 ); };
        
        
        std::vector<float> valuesForOSC()
        {
            return avgs;
        }
        
        std::vector<float> calcAndEmptyBuffers()
        {
            std::vector<float> vals;
            for( int i=0; i<filtered->motionDataSize(); i++ )
            {
                vals.push_back( float(getAvg(i)) );
            }
            return vals;
        };
       
    };
    
    //---------------
    
 
    //TODO: refactor this -- need to finish in a few days, so, this tacked on solution
    class SlopeSignalDegraded
    {
    protected:
        DegradeMotionData *dmd;
        std::vector<float> lastVal;
        std::vector<float> curSlope;
    public:
        SlopeSignalDegraded(DegradeMotionData *d)
        {
            dmd = d;
            
            for(int i=0; i<dmd->getDataSize(); i++)
            {
                lastVal.push_back(0);
                curSlope.push_back(0);
            }
        };
        
        virtual void findDifference(std::vector<float> curVal, std::vector<float> &lval)
        {
            for(int i=0; i<curVal.size(); i++)
            {
                curSlope[i] = curVal[i] - lastVal[i];
            }
            lval = curVal;
        };
        
        virtual void update()
        {
            if( dmd->shouldSend() )
            {
                std::vector<float> curVal = dmd->valuesForOSC();
                findDifference(curVal, lastVal);
            }
        };
        
        virtual bool shouldSend()
        {
            return dmd->shouldSend();
        }
        
        virtual size_t getDataSize()
        {
            return dmd->getDataSize();
        }
        
        virtual std::vector<float> valuesForOSC()
        {
            return curSlope;
        };
    };
    
    //--------------------------
    class DegradedAccelSignal : public SlopeSignalDegraded
    {
    protected:
        SlopeSignalDegraded *slope;
    public:
        DegradedAccelSignal(SlopeSignalDegraded *s, DegradeMotionData *d) : SlopeSignalDegraded(d)
        {
            slope = s;
        };
        
        virtual void update()
        {
            if( slope->shouldSend() )
            {
                std::vector<float> curVal = slope->valuesForOSC();
                findDifference(curVal, lastVal);
            }
        };
    };
    //--------------------------
    class SlopeEnergySignal
    {
    protected:
        SlopeSignalDegraded *slope;
        std::vector<std::vector<float> *> buf;
        std::vector<float> energy;

        int window;
    public:
        SlopeEnergySignal( SlopeSignalDegraded *s, int w=10 )
        {
            slope = s;
            window = w;
            
            for(int i=0; i<slope->getDataSize(); i++)
            {
                energy.push_back(0);
                buf.push_back(new std::vector<float>);
            }
        };
        
        void updateBuffers(std::vector<float> vals)
        {
            if (vals.size() != buf.size())
            {
                std::cout << "vals size "<< vals.size()<< " incorrect\n";
                std::abort();
            }
            for(int i=0; i<buf.size(); i++)
            {
                if(buf[i]->size() >= window)
                {
                    buf[i]->erase(buf[i]->begin());
                }
                buf[i]->push_back(vals[i]);
            }
        };
        
        size_t getDataSize()
        {
            return slope->getDataSize();
        }
        std::vector<float> valuesForOSC()
        {
            return energy;
        };
        
        void calcEnergy()
        {
            for( int i=0; i<getDataSize(); i++ )
            {
                float sum = 0;
                for( int j=0; j<buf[i]->size(); j++ )
                {
                    sum += buf[i]->at(j)*buf[i]->at(j);
                }
                energy[i] = std::sqrt(sum) / buf[i]->size();
            }
        };
        
        
        void update()
        {
            if( slope->shouldSend() )
            {
                updateBuffers( slope->valuesForOSC() );
                calcEnergy();
            }
        };
    
    };
    
    //--------------------------
//    class DegradedLineTrigger
//    {
//        DegradedLineTrigger(SlopeEnergySignal)
//        
//        
//    };
    
    //----------------------------
    //----------------------------
//    class DegradedFastBuildSlowDecay
//    {
//    protected:
//        SlopeEnergySignal *energy;
//        std::vector<std::vector<float> *> buf;
//        std::vector<float> output;
//        int window;
//    public:
//        DegradedFastBuildSlowDecay(SlopeEnergySignal *e, int w = 20)
//        {
//            energy = e;
//            
//            for(int i=0; i<energy->getDataSize(); i++)
//            {
//                output.push_back(0);
//                buf.push_back(new std::vector<float>);
//            }
//            window = w;
//        };
//        
//        virtual size_t getDataSize()
//        {
//            return energy->getDataSize();
//        };
//        
//        std::vector<float> valuesForOSC()
//        {
//            return output;
//        };
//        
//        
//        void  updateAccumulator(val)
//        {
//            if( val > accum )
//            {
//                accum = val;
//            }
//                    if( accum > thresh )
//                    {
//                        var index = buf.length-1;
//                        var perc = (val-thresh) * decrement;
//                        while( index >= 0 && perc > 0 )
//                        {
//                            buf[index] = buf[index] + perc;
//                            perc = perc * decrement;
//                            index--;
//                        }
//                    }
//                }
//        
//        void updateBuffers()
//        {
//        
//        }
//        
//        virtual void update()
//        {
//            if( energy->shouldSend() )
//            {
//                
//            }
//        };
//
//    };
    
    
    //--------------------------
    //--------------------------
    
    
    //send accel signal to ableton/m4l for a long step, at a hertz rate
    //also send slope
    class SendSignal : public ContinousOutMappingSchema {
    protected:
        Filter *filteredSignal;
        DegradeMotionData *dmd;
        SlopeSignalDegraded *slope;
        DegradedAccelSignal *accel;

        SlopeEnergySignal *energy;
        SlopeEnergySignal *accelEnergy;
        
        std::string oscMessage;

        int _id;
    public:
        SendSignal( Filter *signal, int did = 0, std::string oMsg = SEND_BACK, int srDiv = 6 ) : ContinousOutMappingSchema(did)
        {
            filteredSignal  = (Filter *) signal;
            filteredSignal->calcMotionData(true);
            _id = did;
            
            dmd = new DegradeMotionData(signal, srDiv);
            slope = new SlopeSignalDegraded(dmd);
            energy = new SlopeEnergySignal(slope);
            accel = new DegradedAccelSignal(slope, dmd);
            accelEnergy = new SlopeEnergySignal(accel);
            
            oscMessage = oMsg; //default
        };
        
        ~SendSignal()
        {
            delete dmd;
        };
        
        virtual void update(float seconds = 0)
        {
            dmd->update();
            slope->update();
            energy->update();
            accel->update();
            accelEnergy->update();
        };
        
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            
            std::vector<ci::osc::Message>  msgs;
//            std::cout << oscMessage << " getting OSC\n";
            if( dmd->shouldSend() )
            {
                OSCMessageITM msg;
                
                msg.setAddress(oscMessage);
                msg.addIntArg(_id);
//                std::vector<float> vals = dmd->valuesForOSC();
//                std::vector<float> slopeVals = slope->valuesForOSC();
                std::vector<float> energyVals = energy->valuesForOSC();
                std::vector<float> accelVals = accelEnergy->valuesForOSC();

//                for(int i=0; i<vals.size(); i++)
//                    msg.addFloatArg( vals[i] );
//                
//                for(int i=0; i<slopeVals.size(); i++)
//                    msg.addFloatArg( slopeVals[i] );
                
                for(int i=0; i<energyVals.size(); i++)
                    msg.addFloatArg( energyVals[i] );

                for(int i=0; i<accelVals.size(); i++)
                    msg.addFloatArg( accelVals[i] );
                
                msgs.push_back(msg.getMessage());
            }
            return msgs;
        };
        
        
    };
    
    

    
};

#endif
