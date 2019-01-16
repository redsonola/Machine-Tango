//
//  Dancers.h
//  InteractiveTangoReadFromAndroid
//
//  Created by courtney on 12/1/14.
//
//

#ifndef InteractiveTangoReadFromAndroid_Dancers_h
#define InteractiveTangoReadFromAndroid_Dancers_h

namespace InteractiveTango
{
    
    class TangoEntity : public SignalAnalysis //kinda like a UGEN, but not.
    {
//        virtual void update() = 0;
//        virtual double getAvgWindowedVar() = 0;
//        virtual double getAvgWindowedVarDiff()=0;
    protected:
        std::vector<MappingSchema *> mMappingSchemas;
        virtual void createSchemas(std::vector<SignalAnalysis * > *ugens) {};
    public:
        MappingSchema *getSchema(int i)
        {
            assert(i<mMappingSchemas.size() && i>=0);
            return mMappingSchemas[i];
        };
        
        enum WhichDancerType { LEADER1=0, FOLLOWER1=1, LEADER2=2, FOLLOWER2=3 };
        
        enum SongIDs{ FRAGMENTS = 0, POR_UNA_CABEZA = 1, GENERATED_PIAZZOLLA_TEST = 2 };
        
        enum WhichSchemas{ BUSY_SPARSE_LEADER=0, BUSY_SPARSE_FOLLOWER=1, ROOMSIZE_CONTINUOUS=2, POINTY_ROUNDED_CONT_LEADER=3, POINTY_ROUNDED_CONT_FOLLOWER=4, INSTRUMENT_POINTROUND_LEADER=5, INSTRUMENT_POINTROUND_FOLLOWER=6, LEADER_VOLUME=7, FOLLOWER_VOLUME=8, LEADER_SEND_BACK=9, FOLLOWER_SEND_BACK=10 }; //just one for now
        
        enum WhichMotionAnalysisParams{ COUPLE_STEPNUM=0, COUPLE_WINDOWED_VARDIFF=1, COUPLE_WINDOWED_VAR=2, CROSSCOVAR=3, COUPLE_ONSETPEAK=4,
            LEADER_WINDOWED_VAR=5, LEADER_STEPNUM=6, LEADER_WINDOWED_VARDIFF=7, LEADER_ONSETPEAK=8, FOLLOWER_WINDOWED_VAR=9,
            FOLLOWER_STEPNUM=10, FOLLOWER_WINDOWED_VARDIFF=11, FOLLOWER_ONSETPEAK=12, LEADER_BUSYSPARSE=13, FOLLOWER_BUSYSPARSE=14, LEADER_POINTYROUND=15, FOLLOWER_POINTYROUND=16, LEADER_IS_STEP=17, FOLLOWER_IS_STEP=18, LEADER_CUR_STEP_PEAK=19, FOLLOWER_CUR_STEP_PEAK=20, LEADER_IS_STEP_LEFT=21, FOLLOWER_IS_STEP_LEFT=22, LEADER_IS_STEP_RIGHT=23, FOLLOWER_IS_STEP_RIGHT=24, LEADER_LEFTFOOT_WINDOWEDVARDIFF=25, FOLLOWER_LEFTFOOT_WINDOWEDVARDIFF=26, LEADER_RIGHTFOOT_WINDOWEDVARDIFF=27, FOLLOWER_RIGHTFOOT_WINDOWEDVARDIFF=28, LEADER_STEPPEAK_AXIS=29, FOLLOWER_STEPPEAK_AXIS=30};
        
        enum WhichGestureMapping{ LEADER_RIGHT_CIRCLE=0, FOLLOWER_RIGHT_CIRCLE=1, LEADER_RIGHT_BOLEO=2, FOLLOWER_RIGHT_BOLEO=3, LEADER_LEFT_CIRCLE=4, FOLLOWER_LEFT_CIRCLE=5, LEADER_LEFT_BOLEO=6, FOLLOWER_LEFT_BOLEO=7 };
        
        std::string DancerTypeStr(WhichDancerType which)
        {
            std::string s;
            switch (which)
            {
                case LEADER1:
                    s = "Leader 1";
                    break;
                case FOLLOWER1:
                    s = "Follower 1";
                    break;
                case LEADER2:
                    s = "Leader 2";
                    break;
                case FOLLOWER2:
                    s = "Follower 2";
                    break;
                default:
                    break;
            };
            return s;
        };
    
    };
    
    class Dancer : public TangoEntity
    {
    protected:
        std::vector<UGEN * > rightFoot;
        std::vector<UGEN * > leftFoot;
        std::vector<UGEN * > back;
        
        //todo -- add other feet!
        RecognizedGestures *rightFootGestures;
        RecognizedGestures *leftFootGestures;
        
        std::vector<SignalAnalysisEventOutput * > vars, varDiffs;
        MotionDataAverage *totalVariance, *totalVarianceDiff;
        
        FootOnset *onsets;
        bool rightFootInit, leftFootInit, backInit, stepDInit, isSchemaInit;
        int rightFootID, leftFootID, backID, dancerID;
        BeatTiming *timer;
        
//        PointyVsSmoothEvent *pv;
//        BusyVsSparseEvent *bs;
//        RoomSizeAreaEvent *rs;
//        OrchThickVsThinEvent *ot;
                                        //fix
        SendOSCToWek *sendToWekinator, *sendToWekinatorLeft;
        
        WhichDancerType dancerType;
        
    public:
        enum BodyPart{ LeftFoot=0, RightFoot=1, Back=2 };
        enum WhichSignals{ INPUT_SIGNAL=0, LPFILTER5Hz=1, SNAP_PEAKS=2, LPFILTER15Hz=3, WINDOWED_VAR=4, WINDOWED_VARDIFF=5, SIGNAL_VECTOR=6, MINMAX_SIGNAL=7 };
        
        
        Dancer(BeatTiming *btimer, SendOSCToWek *sendTo, SendOSCToWek *sendToLeft){
            rightFootInit = false;
            leftFootInit = false;
            backInit = false;
            stepDInit = false;
            isSchemaInit = false;
            timer = btimer;
            sendToWekinator = sendTo;
            sendToWekinatorLeft = sendToLeft;
            rightFootGestures = NULL;
            leftFootGestures = NULL;
        };
        
        ~Dancer()
        {
            if(onsets != NULL)
            {
                delete onsets;
            }

            if(totalVariance != NULL)
            {
                delete totalVariance;
            }
            
            if(totalVarianceDiff != NULL)
            {
                delete totalVarianceDiff;
            }
            
            for(int i=0; i<vars.size(); i++)
            {
                if( vars[i] != NULL )
                {
                    delete vars[i];
                }
            }
            
            for(int i=0; i<varDiffs.size(); i++)
            {
                if( varDiffs[i] != NULL )
                {
                    delete varDiffs[i];
                }
            }
            
            
            delete rightFootGestures;
            delete leftFootGestures;
        };
        bool isInit()
        {
            return rightFootInit && leftFootInit && backInit;
        };
        
        
        int getRightFootSensorID()
        {
            return rightFootID;
        };
        
        int getLeftFootSensorID()
        {
            return leftFootID;
        };
        
        int getBackSensorID()
        {
            return backID;
        };
        
        WhichDancerType getDancerType()
        {
            return dancerType;
        };
        
        void setDancerType(WhichDancerType dtype)
        {
            dancerType = dtype;
        };

        
        SignalAnalysis *getSignal(int limb, int index)
        {
            std::vector<UGEN * > *signals;
            if ( limb == leftFootID )
            {
                signals = getLeftFoot();
            }
            else if( limb == rightFootID )
            {
                signals = getRightFoot();
            }
            else if( limb == backID )
            {
                signals = getBack();
            }
            else
            {
                std::cout << "WARNING: Dancer limb ID not found in getSignal()\n";
                return NULL;
            }
            
            
            if( index > signals->size() || index < 1 )
            {
                std::cout << "WARNING: index exceeds signal length in getSignal(). Index: "<<  index << ", limb id: " << limb << "\n";
                return NULL;
            }
            else
            {
                return (SignalAnalysis *) signals->at(index);
            }
            
        };
        
        //TODO -- add for all feet... etc.
        RecognizedGestures *getRootFootGestureRecognition()
        {
            return rightFootGestures;
        };
        
        RecognizedGestures *getLeftFootGestureRecognition()
        {
            return leftFootGestures;
        };
        
        void setDancerID(int did, std::vector<UGEN * > *ugens)
        {
            assert( timer != NULL );
            dancerID = did;
        };
        
        int getDancerID(){ return dancerID; };
        
        void addSensorBodyPart(int idz, SensorData *sensor, std::vector<UGEN * > *ugens, BodyPart whichBody )
        {
            //which body part is the sensor of?
            std::vector<UGEN * > *bUgens;
            switch (whichBody)
            {
                case RightFoot:
                    bUgens = &rightFoot;
                    rightFootInit = true;
                    rightFootID = idz;
                    break;
                case LeftFoot:
                    bUgens = &leftFoot;
                    leftFootInit = true;
                    leftFootID = idz;
                    break;
                case Back:
                    bUgens = &back;
                    backInit = true;
                    backID = idz;
                    break;
                default:
                    break;
            };
        
            //add all the basic signal analysis that happens for ea. sensor
            InputSignal *signal_input = new InputSignal(idz, sensor->getWhichSensor() == ANDROID_SENSOR);
        
            signal_input->setInput(sensor);
        
            bUgens->push_back( signal_input );
            
        
            //was 20... worked with 20
            LPFilter5Hz5d *filter5 = new LPFilter5Hz5d(signal_input);
            SnapPeaks *peaks = new SnapPeaks( idz, sensor->getPort(), filter5, timer );
        
            bUgens->push_back( filter5 );
            bUgens->push_back( peaks );
        
            LPFilter15Hz *filter15 = new LPFilter15Hz(signal_input);
            WindowedVariance *variance = new WindowedVariance(filter15, SR / 4, idz, sensor->getPort() ) ; //changed for exp. music 1/4/2018
            
            WindowedVarianceDifference *varianceDiff = new WindowedVarianceDifference(variance, idz, sensor->getPort());
            
            //try filtered for 5 hz
            SignalVector *sv1, *sv2;
//            LPFilter5Hz5d *sigVec, *sigVecLeft;
            SignalVectorMinMax *sigVecMinMax, *sigVecMinMaxLeft;

            if( whichBody == RightFoot )
            {
                sv1  = new SignalVector(signal_input, idz);
//                sigVec = new LPFilter5Hz5d( sv1 ); //not used...
                sigVecMinMax = new SignalVectorMinMax(signal_input, idz);

                rightFootGestures = new RecognizedGestures(peaks, timer, 500, 4, false);
            }
            else if( whichBody == LeftFoot )
            {
                sv2 = new SignalVector(signal_input, idz);
//                sigVecLeft = new LPFilter5Hz5d( sv2 );
                sigVecMinMaxLeft = new SignalVectorMinMax(signal_input, idz);
                
                leftFootGestures = new RecognizedGestures(peaks, timer, 500, 4, false);

            }
//            InteractiveTango::LPFilter15Hz *f5SigVec = new InteractiveTango::LPFilter15Hz(sigVec);

            
            bUgens->push_back( filter15 );
            bUgens->push_back( variance );
            bUgens->push_back( varianceDiff );
            
            //copy over to main signal processing chain
            for(int i=0; i<bUgens->size(); i++)
            {
                ugens->push_back( bUgens->at(i) );
            }
            
            //this way for testing
            if( whichBody == RightFoot )
            {

                ugens->push_back( sv1 );
                bUgens->push_back( sv1 );
                ugens->push_back( sigVecMinMax );
                bUgens->push_back( sigVecMinMax );

                ugens->push_back( rightFootGestures );
                bUgens->push_back( rightFootGestures );
            }
            else if( whichBody == LeftFoot )
            {
                ugens->push_back( sv2 );
                
//                ugens->push_back( sigVecLeft ); //TODO: FIX
//                bUgens->push_back( sigVecLeft );
                ugens->push_back( sigVecMinMaxLeft );
                bUgens->push_back( sigVecMinMaxLeft );
                
                ugens->push_back( leftFootGestures );
                bUgens->push_back( leftFootGestures );
            }
            
            //add the step onset once both legs are added
            if( rightFootInit && leftFootInit && !stepDInit)
            {
                float footOnsetSampleSize = 3; // changed to see if can change window - half of previous
                onsets = new FootOnset( (SnapPeaks *) leftFoot.at(SNAP_PEAKS),(SnapPeaks *)rightFoot.at(SNAP_PEAKS), leftFootID , rightFootID,  footOnsetSampleSize);
                ugens->push_back(onsets);
                
                float bps = timer->getBPM() / (60.0) ;
                onsets->setMotionAnalysisMinMax(FootOnset::MotionDataIndices::STEP_COUNT, 0, (bps/2)*12 ); //estimates for now -- max is steping every beat for now, need to change constant number window 4 to a variable to, and also have this linked to change in bpm
                onsets->setMotionAnalysisMinMax(FootOnset::MotionDataIndices::AVG_PEAK, 0, 0.6); //estimates for now
                
                stepDInit = true;
            }
            
            //create the mapping shemas
            if(stepDInit && backInit && !isSchemaInit)
            {
                //assign var and dvar HERE
                vars.push_back( (SignalAnalysisEventOutput *) leftFoot.at(WhichSignals::WINDOWED_VAR) );
                vars.push_back( (SignalAnalysisEventOutput *) rightFoot.at(WhichSignals::WINDOWED_VAR) );
                vars.push_back( (SignalAnalysisEventOutput *) back.at(WhichSignals::WINDOWED_VAR) );
                
                varDiffs.push_back( (SignalAnalysisEventOutput *) leftFoot.at(WhichSignals::WINDOWED_VARDIFF) );
                varDiffs.push_back( (SignalAnalysisEventOutput *) rightFoot.at(WhichSignals::WINDOWED_VARDIFF) );
                varDiffs.push_back( (SignalAnalysisEventOutput *) back.at(WhichSignals::WINDOWED_VARDIFF) );
                
                totalVariance = new MotionDataAverage( vars );
                totalVarianceDiff = new MotionDataAverage( varDiffs );
                
                //set minMax -- TODO: shimmers have different motion values -- need to normalize (since Android doesn't & back values are
                //different
//                totalVarianceDiff->setMotionAnalysisMinMax(0, 0.0, 0.12);
                totalVariance->setMotionAnalysisMinMax(0, 0.0, 1.0);

                ugens->push_back(totalVariance);
                ugens->push_back(totalVarianceDiff);
                
                //add rights, then lefts --> legacy... both feet so don't have to change wek file TODO: CHANGE
                sendToWekinator->addSignalToSend( (SignalAnalysis *) rightFoot[WhichSignals::SIGNAL_VECTOR]);
                sendToWekinator->addSignalToSend( (SignalAnalysis *) rightFoot[WhichSignals::INPUT_SIGNAL]);

//                sendToWekinator->addSignalToSend( (SignalAnalysis *) leftFoot[WhichSignals::SIGNAL_VECTOR]);
//                sendTox   inator->addSignalToSend( (SignalAnalysis *) leftFoot[WhichSignals::INPUT_SIGNAL]);
                
                //------------------------------------------------------------------------------------
                //now left first
                sendToWekinatorLeft->addSignalToSend( (SignalAnalysis *) leftFoot[WhichSignals::SIGNAL_VECTOR]);
                sendToWekinatorLeft->addSignalToSend( (SignalAnalysis *) leftFoot[WhichSignals::INPUT_SIGNAL]);
                
//                sendToWekinatorLeft->addSignalToSend( (SignalAnalysis *) rightFoot[WhichSignals::SIGNAL_VECTOR]);
//                sendToWekinatorLeft->addSignalToSend( (SignalAnalysis *) rightFoot[WhichSignals::INPUT_SIGNAL]);


                
//                sendToWekinator->addSignalToSend( (SignalAnalysis *) rightFoot[WhichSignals::MINMAX_SIGNAL]);
                
                isSchemaInit = true;
            }
        };
        std::vector<UGEN * > * getRightFoot(){ return &rightFoot; };
        std::vector<UGEN * > * getLeftFoot(){ return &leftFoot; };
        std::vector<UGEN * > * getBack(){ return &back; };
        FootOnset * getOnsets(){ return onsets; };
        SignalAnalysisEventOutput *getAvgVarianceDiff(){ return totalVarianceDiff; };
        SignalAnalysisEventOutput *getAvgVariance(){ return totalVariance; };
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            return msgs;
        };
        
        //TODO: FILL IN
        virtual void update(float seconds = 0)
        {
//            std::cout << ( ( MotionAnalysisEvent * ) totalVariance->getMotionData().at(0) )->asDouble() << std::endl;
//            playAccompaniment->update(seconds);
        };
    };
    
    class Pareja : public TangoEntity
    {
        protected:
            Dancer *leader;
            Dancer *follower;
        
            CrossCoVar *leaderLeftFollowerRightFeet;
            CrossCoVar *leaderRightFollowerLeftFeet;
            CrossCoVar *backs;
        
//            MotionIntegerDataAverage * heavyVsLightEventPareja, *busyVsSparsePareja; //schema for both dancers
//            std::vector<SignalAnalysisEventOutput *> heavyVsLights;
//            std::vector<SignalAnalysisEventOutput *> busyVsSparse;

            //this is averaging across dancers
            MotionDataAverage *totalVariance, *totalVarianceDiff, *avgFootOnsets, *avgCrossCoVariance;
            std::vector<SignalAnalysisEventOutput * > vars, varDiffs, onsets, crossco;
            std::vector<MotionAnalysisEvent *> mMotionAnalysisEvents; //all the events sent to mapping schemas --? for changing the ranges on the fly
        
            //temporarily here
//            MainMelodySection *melody;
//            AccompanimentSection *accompaniment;
//            std::vector<OrnamentationSection *> ornaments;
//        
//            MusicPlayer player;
//            TangoInstruments instruments;
        
            BeatTiming *mTimer;
        
            SectionASongCoupleParams songACoupleParams;
            SectionASongFollowerParams songAFollowerParams;
            SectionASongLeaderParams songALeaderParams;
        
            PorUnaCabezaCoupleParams porUnaCabezaCoupleParams;
            PorUnaCabezaFollowerParams porUnaCabezaFollowerParams;
            PorUnaCabezaLeaderParams porUnaCabezaLeaderParams;
        
            CirclingGestures   *followerRightFootCircling, *followerLeftFootCircling;
            BoleoGestures      *followerRightFootBoleo, *followerLeftFootBoleo;
            CirclingGestures   *leaderRightFootCircling, *leaderLeftFootCircling;
            BoleoGestures      *leaderRightFootBoleo, *leaderLeftFootBoleo;
            std::vector<GestureToMusic *> gestureMappings;

        
    
        
            void addCrossCoVar(std::vector<UGEN * > *ugens)
            {
                 leaderLeftFollowerRightFeet = new CrossCoVar( ( Filter *) leader->getLeftFoot()->at(Dancer::LPFILTER5Hz),
                                                                         ( Filter *) follower->getRightFoot()->at(Dancer::LPFILTER5Hz),
                                                                         ( WindowedVariance *) leader->getLeftFoot()->at(Dancer::WINDOWED_VAR),
                                                                         ( WindowedVariance *) follower->getRightFoot()->at(Dancer::WINDOWED_VAR),
                                                                         leader->getLeftFootSensorID(),
                                                                         follower->getRightFootSensorID(),
                                                                         "", //there is no port
                                                                         ""); //there is no port anymore
                
                
                 leaderRightFollowerLeftFeet = new CrossCoVar(( Filter *) leader->getRightFoot()->at(Dancer::LPFILTER5Hz),
                                                                         ( Filter *) follower->getLeftFoot()->at(Dancer::LPFILTER5Hz),
                                                                         ( WindowedVariance *) leader->getRightFoot()->at(Dancer::WINDOWED_VAR),
                                                                         ( WindowedVariance *) follower->getLeftFoot()->at(Dancer::WINDOWED_VAR),
                                                                         leader->getRightFootSensorID(),
                                                                         follower->getLeftFootSensorID(),
                                                                         "", //there is no port
                                                                         ""); //there is no port anymore
                
                
                 backs = new CrossCoVar(( Filter *) leader->getBack()->at(Dancer::LPFILTER5Hz),
                                                                         ( Filter *) follower->getBack()->at(Dancer::LPFILTER5Hz),
                                                                         ( WindowedVariance *)leader->getBack()->at(Dancer::WINDOWED_VAR),
                                                                         ( WindowedVariance *)follower->getBack()->at(Dancer::WINDOWED_VAR),
                                                                         leader->getBackSensorID(),
                                                                         follower->getBackSensorID(),
                                                                         "", //there is no port
                                                                         ""); //there is no port anymore
                ugens->push_back(leaderLeftFollowerRightFeet);
                ugens->push_back(leaderRightFollowerLeftFeet);
                ugens->push_back(backs);


            };
        
        
    public:
        
        
    
        //so that we can change them as per song, etc.  --> eventually as per individual

        
        std::vector<std::string> motionAnalysisParamsStrings = { "COUPLE_STEPNUM", "COUPLE_WINDOWED_VARDIFF", "COUPLE_WINDOWED_VAR", "CROSSCOVAR", "COUPLE_ONSETPEAK",
            "LEADER_WINDOWED_VAR", "LEADER_STEPNUM", "LEADER_WINDOWED_VARDIFF", "LEADER_ONSETPEAK", "FOLLOWER_WINDOWED_VAR",
            "FOLLOWER_STEPNUM", "FOLLOWER_WINDOWED_VARDIFF", "FOLLOWER_ONSETPEAK", "LEADER_BUSYSPARSE", "FOLLOWER_BUSYSPARSE", "LEADER_POINTYROUND", "FOLLOWER_POINTYROUND", "LEADER_IS_STEP", "FOLLOWER_IS_STEP", "LEADER_CUR_STEP_PEAK", "FOLLOWER_CUR_STEP_PEAK", "LEADER_IS_STEP_LEFT", "FOLLOWER_IS_STEP_LEFT", "LEADER_IS_STEP_RIGHT", "FOLLOWER_IS_STEP_RIGHT",
            "LEADER_LEFTFOOT_WINDOWEDVARDIFF", "FOLLOWER_LEFTFOOT_WINDOWEDVARDIFF", "LEADER_RIGHTFOOT_WINDOWEDVARDIFF", "FOLLOWER_RIGHTFOOT_WINDOWEDVARDIFF", "LEADER_STEPPEAK_AXIS", "FOLLOWER_STEPPEAK_AXIS"};
    
    protected:
        
        //for changing motion params dynamically
        SongIDs selectedSong;
        int selectedMotionParam;
        bool motionParamsChangeMax;
        float selectedMotionParamIncVal;
        
        std::fstream outfile, motionDataOutFile;
        
    public:
        
        inline Dancer *getLeader(){ return leader; };
        inline Dancer *getFollower(){ return follower; };
        inline MotionAnalysisEvent *getMotionMotionAnalysisOutput(WhichMotionAnalysisParams mParam)
        {
            return mMotionAnalysisEvents[mParam];
        };
        inline GestureToMusic *getGestureMapping(WhichGestureMapping mMap)
        {
            return gestureMappings[mMap];

        };
        inline MappingSchema *getMappingSchema(TangoEntity::WhichSchemas mMap)
        {
            return mMappingSchemas[mMap];
        };
        

        
        
        
        void saveCurrentMotionParams()
        {
            motionDataOutFile.open("motionDataParams.txt", std::ios::app);
            motionDataOutFile << "------------------------------------------\n";
            motionDataOutFile << "------------------------------------------\n";
            
            for(int i=0; i<motionAnalysisParamsStrings.size(); i++)
            {
                std::vector<double> coeffs = getMotionDataCoefficients((WhichMotionAnalysisParams)i);
                std::cout << motionAnalysisParamsStrings[i] << "  MIN: " << coeffs[0] << "  MAX: "<< coeffs[1] << std::endl;
            }
            motionDataOutFile.close();
        };
        
        //-----------------------------------
        //----START FOR CHANGING MOTION PARAMS DYNAMICALLY
        //----TODO: REFACTOR OUT!!!------
        //-----------------------------------
        
        //collections of the same motion parameter in diff. dancers in the pareja
        std::vector<WhichMotionAnalysisParams> steps = { Pareja::WhichMotionAnalysisParams::COUPLE_STEPNUM, WhichMotionAnalysisParams::LEADER_STEPNUM, WhichMotionAnalysisParams::FOLLOWER_STEPNUM };
        std::vector<WhichMotionAnalysisParams> windowedVar = { WhichMotionAnalysisParams::COUPLE_WINDOWED_VAR, WhichMotionAnalysisParams::LEADER_WINDOWED_VAR, WhichMotionAnalysisParams::FOLLOWER_WINDOWED_VAR };
        std::vector<WhichMotionAnalysisParams> windowedVarDiff = { WhichMotionAnalysisParams::COUPLE_WINDOWED_VARDIFF, WhichMotionAnalysisParams::LEADER_WINDOWED_VARDIFF, WhichMotionAnalysisParams::FOLLOWER_WINDOWED_VARDIFF };
        std::vector<WhichMotionAnalysisParams> onsetPeak = { WhichMotionAnalysisParams::COUPLE_ONSETPEAK, WhichMotionAnalysisParams::LEADER_ONSETPEAK, WhichMotionAnalysisParams::FOLLOWER_ONSETPEAK };
        
        


        std::string getSelectedMotionParamString()
        {
            return motionAnalysisParamsStrings[selectedMotionParam];
        };
        
        int getSelectedMotionParam()
        {
            return selectedMotionParam;
        };
        void incSelectedMotionParam()
        {
            selectedMotionParam++;
            if( selectedMotionParam >= motionAnalysisParamsStrings.size() )
                selectedMotionParam=0;
        };
        
        //TODO: streamline this code -- fix the params object
        //this is like the most hacky function EVER ARGH -- refactor!!!!!!!
        
        bool changeMotionMotionParamMax()
        {
            return motionParamsChangeMax;
        }
        void setChangeMotionMotionParamMax(bool changeMax)
        {
            motionParamsChangeMax = changeMax;
        }
        
        void changeParamValue(std::vector<double> val )
        {
            changeParamValue( (WhichMotionAnalysisParams) selectedMotionParam, val);
        };
        
        void motionParamIncValHundredth(){
            selectedMotionParamIncVal = 0.01;
        };
        
        void motionParamIncValTenth(){
            selectedMotionParamIncVal = 0.1;
        };
        
        void motionParamIncValOne(){
            selectedMotionParamIncVal = 1;
        };
        
        void motionParamIncValThousandth(){
            selectedMotionParamIncVal = 0.001;
        };
        
        float getSelectedMotionParamIncVal()
        {
            return selectedMotionParamIncVal;
        };
        
        void increaseCurMotionParam()
        {
            if(motionParamsChangeMax) increaseCurMotionParamMax();
            else increaseCurMotionParamMin();
        }
        
        void decreaseCurMotionParam()
        {
            if(motionParamsChangeMax) decreaseCurMotionParamMax();
            else decreaseCurMotionParamMin();
        }


        void increaseCurMotionParamMin()
        {
            std::vector<double> val = getMotionDataCoefficients((WhichMotionAnalysisParams)selectedMotionParam);
            val[0]+=selectedMotionParamIncVal;
            changeParamValue((WhichMotionAnalysisParams)selectedMotionParam, val);
        };
        
        void increaseCurMotionParamMax()
        {
            std::vector<double> val = getMotionDataCoefficients((WhichMotionAnalysisParams)selectedMotionParam);
            val[1]+=selectedMotionParamIncVal;
            changeParamValue((WhichMotionAnalysisParams)selectedMotionParam, val);
        };
        
        void decreaseCurMotionParamMin()
        {
            std::vector<double> val = getMotionDataCoefficients((WhichMotionAnalysisParams)selectedMotionParam);
            val[0]-=selectedMotionParamIncVal;
            changeParamValue((WhichMotionAnalysisParams)selectedMotionParam, val);
        };
        void decreaseCurMotionParamMax()
        {
            std::vector<double> val = getMotionDataCoefficients((WhichMotionAnalysisParams)selectedMotionParam);
            val[1]-=selectedMotionParamIncVal;
            changeParamValue((WhichMotionAnalysisParams)selectedMotionParam, val);
        };
        
        GeneralMotionParams *findCorrectParamsFromMotionParam(WhichMotionAnalysisParams param)
        {
            GeneralMotionParams *params;
            
            //set params depending on song and whether couple, leader, or follower
            if (selectedSong == SongIDs::FRAGMENTS )
            {
                if( param <=  WhichMotionAnalysisParams::COUPLE_ONSETPEAK)
                {
                    params = &songACoupleParams;
                }
                else if( param <=  WhichMotionAnalysisParams::LEADER_ONSETPEAK)
                {
                    params = &songALeaderParams;
                }
                else
                {
                    params = &songAFollowerParams;
                }
            }
            else //(selectedSong == SongIDs::POR_UNA_CABEZA )
            {
                if( param <=  WhichMotionAnalysisParams::COUPLE_ONSETPEAK)
                {
                    params = &porUnaCabezaCoupleParams;
                }
                else if( param <=  WhichMotionAnalysisParams::LEADER_ONSETPEAK)
                {
                    params = &porUnaCabezaLeaderParams;
                }
                else
                {
                    params = &porUnaCabezaFollowerParams;
                }
            }
            return params;
        };
        
        void changeParamValue(WhichMotionAnalysisParams param, std::vector<double> val )
        {
            GeneralMotionParams *params = findCorrectParamsFromMotionParam(param);
            
            //set the value of the coefficients
            if( std::find( steps.begin(), steps.end(), param ) != steps.end() )
            {
                params->stepNumCoEfficients = val;
            }
            else if(std::find( windowedVar.begin(), windowedVar.end(), param ) != windowedVar.end() )
            {
                params->windowedDiffVarCoEfficients = val;
            }
            else if(std::find( windowedVarDiff.begin(), windowedVarDiff.end(), param ) != windowedVarDiff.end() )
            {
                params->windowedDiffVarCoEfficients = val;
            }
            else if(std::find( onsetPeak.begin(), onsetPeak.end(), param ) != onsetPeak.end() )
            {
                params->footOnsetCoEfficients = val;
            }
            else params->crossCoVarCoEfficients = val;
    
            setMotionAnalysisParamsForSong(params);
            
        };
        
        //oh gawd needs refactoring
        std::vector<double> getMotionDataCoefficients()
        {
            return getMotionDataCoefficients((WhichMotionAnalysisParams)selectedMotionParam );
        };
        
        std::vector<double> getMotionDataCoefficients(WhichMotionAnalysisParams param )
        {
            GeneralMotionParams *params = findCorrectParamsFromMotionParam(param);
            std::vector<double> coefficients;
            
            //set the value of the coefficients
            if( std::find( steps.begin(), steps.end(), param ) != steps.end() )
            {
                coefficients = params->stepNumCoEfficients;
            }
            else if(std::find( windowedVar.begin(), windowedVar.end(), param ) != windowedVar.end() )
            {
                coefficients= params->windowedDiffVarCoEfficients;
            }
            else if(std::find( windowedVarDiff.begin(), windowedVarDiff.end(), param ) != windowedVarDiff.end() )
            {
                coefficients = params->windowedDiffVarCoEfficients;
            }
            else if(std::find( onsetPeak.begin(), onsetPeak.end(), param ) != onsetPeak.end() )
            {
                coefficients= params->footOnsetCoEfficients;
            }
            else coefficients = params->crossCoVarCoEfficients;
            
            return coefficients;
            
        };

        
        //-----------------------------------
        //----END FOR CHANGING MOTION PARAMS DYNAMICALLY TODO: REFACTOR OUT!!!------
        //-----------------------------------
        
        //for deciding on min/max values
        struct MinMaxP
        {
            double min=15000;
            double max=0;
            double avg=0;
            int count=0;
            double sum=0;
        };
        MinMaxP mmparams[13];
        
        
        void findMinMaxParam( int i )
        {
            double val = mMotionAnalysisEvents[i]->asDouble();
            
//            if (val != -10000) //-1000 is a dummy value
//            {
//                mmparams[i].min = std::min(val, mmparams[i].min);
//                mmparams[i].max = std::max(val, mmparams[i].max);
//
//                mmparams[i].count++;
//                mmparams[i].sum += val;
//                mmparams[i].avg =  mmparams[i].sum/ mmparams[i].count;
//            }
            outfile << val << "," <<  mMotionAnalysisEvents[i]->scaledValue() << ",";
        }
        
        //just record data for now
        void printMinMaxAvgMotionAnalysisEvents()
        {
            for(int i=0; i<mMotionAnalysisEvents.size(); i++)
            {
//                outfile  << motionAnalysisParamsStrings[i] << ",";
                findMinMaxParam(i);
                //std::cout << motionAnalysisParamsStrings[i] << "-- min:" << mmparams[i].min << " max:" << mmparams[i].max << " avg:" << mmparams[i].avg << std::endl;
            }
            outfile << std::endl;
            
        };


            Pareja(Dancer *l, Dancer *f, std::vector<UGEN * > *ugens, BeatTiming *timer)
            {
//                melody = NULL;
//                accompaniment = NULL;
                
                mTimer = timer;
                
                leader = l;
                follower = f;
                addCrossCoVar(ugens);
                averageAcrossDancers(ugens);
                createSchemas( ugens, timer );
                
                gestureMappings.push_back(leaderRightFootCircling);
                gestureMappings.push_back(followerRightFootCircling);
                gestureMappings.push_back(leaderRightFootBoleo);
                gestureMappings.push_back(followerRightFootBoleo);
                
                gestureMappings.push_back(leaderLeftFootCircling);
                gestureMappings.push_back(followerLeftFootCircling);
                gestureMappings.push_back(leaderLeftFootBoleo);
                gestureMappings.push_back(followerLeftFootBoleo);
                
                outfile.open("newStats.txt", std::ios::app);
                
                std::time_t  timev;
                struct tm * now = localtime( & timev );
                
                outfile << "--------------------------- " << " NEW DANCE ";
                outfile << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << std::endl;
                outfile << " ---------------------------" << std::endl; // TODO: add a timestamp
                for(int i=0; i<mMotionAnalysisEvents.size(); i++)
                {
                    outfile  << motionAnalysisParamsStrings[i] << "," << motionAnalysisParamsStrings[i] <<"_scaled, ";
                }
                outfile << std::endl;

//                //create music section.
//                loadCourtneyTangoSongNumberOneIntoPlayer(ugens, timer);
////                loadPorUnaCabezaIntoPlayer(ugens, timer);
//                ugens->push_back( &player );
                
                //dynamically change motion param code
                selectedMotionParam = 0; //start at beginning;
                selectedMotionParamIncVal = 0.1;
                motionParamsChangeMax = true;
            };
        
            ~Pareja()
            {
                outfile.close();
//                destroyCurSong();
            };
        
//            void destroyCurSong()
//            {
//                player.setMainMelody( NULL );
//                if( melody != NULL ) delete melody;
//                melody = NULL;
//                
//                player.clearCurAccompaniment(); //clear current accompaniments
//                if( accompaniment != NULL ) delete accompaniment;
//                accompaniment = NULL;
//                
//                player.clearCurOrnaments();
//                for( int i=0; i<ornaments.size(); i++ )
//                {
//                    if( ornaments[i] != NULL ) delete ornaments[i];
//                }
//                ornaments.clear();
//                
//                player.reset();
//            };
        

            void setMotionAnalysisParamsForSongCouple(GeneralMotionParams *couple_params)
            {
                mMotionAnalysisEvents[COUPLE_STEPNUM]->setCoEfficients(couple_params->stepNumCoEfficients);
                mMotionAnalysisEvents[COUPLE_WINDOWED_VARDIFF]->setCoEfficients(couple_params->windowedDiffVarCoEfficients);
                mMotionAnalysisEvents[COUPLE_WINDOWED_VAR]->setCoEfficients(couple_params->windowedVarCoEfficients);
                mMotionAnalysisEvents[CROSSCOVAR]->setCoEfficients(couple_params->crossCoVarCoEfficients);
                mMotionAnalysisEvents[COUPLE_ONSETPEAK]->setCoEfficients(couple_params->footOnsetCoEfficients);
                

            };
        
            //TODO: maybe set different min and max for left and right foots as well, instead of taking it from the average
        
            void setMotionAnalysisParamsForSongLeader(GeneralMotionParams *leader_params)
            {
                mMotionAnalysisEvents[LEADER_WINDOWED_VAR]->setCoEfficients(leader_params->windowedVarCoEfficients);
                mMotionAnalysisEvents[LEADER_STEPNUM]->setCoEfficients(leader_params->stepNumCoEfficients);
                mMotionAnalysisEvents[LEADER_WINDOWED_VARDIFF]->setCoEfficients(leader_params->windowedDiffVarCoEfficients);
                mMotionAnalysisEvents[LEADER_ONSETPEAK]->setCoEfficients(leader_params->footOnsetCoEfficients);
                mMotionAnalysisEvents[LEADER_CUR_STEP_PEAK]->setCoEfficients(leader_params->footOnsetCoEfficients); //same as average peak
                
                mMotionAnalysisEvents[LEADER_LEFTFOOT_WINDOWEDVARDIFF]->setCoEfficients(leader_params->windowedDiffVarCoEfficients);
                mMotionAnalysisEvents[LEADER_RIGHTFOOT_WINDOWEDVARDIFF]->setCoEfficients(leader_params->windowedDiffVarCoEfficients);



            };
            void setMotionAnalysisParamsForSongFollower(GeneralMotionParams *follower_params)
            {
                mMotionAnalysisEvents[FOLLOWER_WINDOWED_VAR]->setCoEfficients(follower_params->windowedVarCoEfficients);
                mMotionAnalysisEvents[FOLLOWER_STEPNUM]->setCoEfficients(follower_params->stepNumCoEfficients);
                mMotionAnalysisEvents[FOLLOWER_WINDOWED_VARDIFF]->setCoEfficients(follower_params->windowedDiffVarCoEfficients);
                mMotionAnalysisEvents[FOLLOWER_ONSETPEAK]->setCoEfficients(follower_params->footOnsetCoEfficients);
                mMotionAnalysisEvents[FOLLOWER_CUR_STEP_PEAK]->setCoEfficients(follower_params->footOnsetCoEfficients); //same as average peak
                
                mMotionAnalysisEvents[FOLLOWER_LEFTFOOT_WINDOWEDVARDIFF]->setCoEfficients(follower_params->windowedDiffVarCoEfficients);
                mMotionAnalysisEvents[FOLLOWER_RIGHTFOOT_WINDOWEDVARDIFF]->setCoEfficients(follower_params->windowedDiffVarCoEfficients);
            };
        
            void setMotionAnalysisParamsForSong(GeneralMotionParams *params)
            {
                if (params->getWhichDancer() == GeneralMotionParams::WhichDancer::couple)
                {
                    setMotionAnalysisParamsForSongCouple(params);
                }
                else if  (params->getWhichDancer() == GeneralMotionParams::WhichDancer::leader)
                {
                    setMotionAnalysisParamsForSongLeader(params);
                }
                else
                {
                    setMotionAnalysisParamsForSongFollower(params);
                }
                    
            }
        

        void setMotionAnalysisParamsForSong(TangoEntity::SongIDs songID)
        {
            if( songID == TangoEntity::SongIDs::FRAGMENTS )
            {
                setMotionAnalysisParamsForSong(&songACoupleParams, &songALeaderParams, &songAFollowerParams);
            }
            else if( songID == TangoEntity::SongIDs::POR_UNA_CABEZA )
            {
                setMotionAnalysisParamsForSong(&porUnaCabezaCoupleParams, &porUnaCabezaLeaderParams, &porUnaCabezaFollowerParams);
            }
        };

        
        void setMotionAnalysisParamsForSong(GeneralMotionParams *couple_params, GeneralMotionParams *leader_params, GeneralMotionParams *follower_params)
            {
                setMotionAnalysisParamsForSongCouple(couple_params);
                setMotionAnalysisParamsForSongLeader(leader_params);
                setMotionAnalysisParamsForSongFollower(follower_params);
            };
        
//            void loadCourtneyTangoSongNumberOneOrnaments()
//            {
//                OrnamentationSection *orn = new OrnamentCirclingSectionA( followerRightFootCircling,  mTimer, follower->getOnsets(), &instruments );
//                OrnamentationSection *boleo = new BoleoOrnamentationSection( followerRightFootBoleo, mTimer, follower->getOnsets(), &instruments );
//                
//                
//                OrnamentationSection *leaderorn = new OrnamentCirclingSectionA( leaderRightFootCircling,  mTimer, leader->getOnsets(), &instruments );
//                OrnamentationSection *leaderboleo = new BoleoOrnamentationSection( leaderRightFootBoleo, mTimer, leader->getOnsets(), &instruments );
//                
//                boleo->addSchema( mMappingSchemas[BUSY_SPARSE_FOLLOWER] );
//                leaderboleo->addSchema( mMappingSchemas[BUSY_SPARSE_LEADER] );
//                
////                orn->addSchema( mMappingSchemas[BUSY_SPARSE_FOLLOWER] );  
//                ornaments.push_back( orn );
//                ornaments.push_back( boleo );
//                ornaments.push_back( leaderorn );
//                ornaments.push_back( leaderboleo );
//                
//                player.addOrnament( orn );
//                player.addOrnament( boleo );
//                player.addOrnament( leaderorn );
//                player.addOrnament( leaderboleo );
//
//            };
        
//            void loadSong(std::vector<UGEN * > *ugens, BeatTiming *timer)
//            {
//                melody->addSchema( mMappingSchemas[BUSY_SPARSE_FOLLOWER] );  //not using to choose melodies at the MOMENT //choosing again 8/2014
//                mMappingSchemas[BUSY_SPARSE_FOLLOWER]->setName("Follower Busy Sparse");
//                melody->addSchema( mMappingSchemas[INSTRUMENT_POINTROUND_FOLLOWER] );
//            
////                accompaniment->addSchema( mMappingSchemas[THICK_THIN] ); //7-29 deleting for now... busy/spare more accurate... I think... look at later... 
//                accompaniment->addSchema( mMappingSchemas[BUSY_SPARSE_LEADER] );
//                mMappingSchemas[BUSY_SPARSE_LEADER]->setName("Leader Busy Sparse");
//                accompaniment->addSchema( mMappingSchemas[INSTRUMENT_POINTROUND_LEADER] );
//            
//                player.setMainMelody( melody );
//                player.addAccompaniment( accompaniment );
//            };
//        
//            void loadCourtneyTangoSongNumberOneIntoPlayer(std::vector<UGEN * > *ugens, BeatTiming *timer)
//            {
//                destroyCurSong();
//                melody = new MelodySectionA( timer, (FootOnset *) onsets[1], &instruments ); //so far only FOLLOWER... change?
//                accompaniment = new AccompanimentSectionA( timer, &instruments );
//                
////              ornaments
//                setMotionAnalysisParamsForSong(&songACoupleParams, &songALeaderParams, &songAFollowerParams);
//                loadSong(ugens, timer);
//                loadCourtneyTangoSongNumberOneOrnaments();
//                
//                selectedSong = SongIDs::FRAGMENTS;
//                std::cout << "Loaded Fragments\n";
//            };
//        
//            void loadPorUnaCabezaIntoPlayer(std::vector<UGEN * > *ugens, BeatTiming *timer)
//            {
//                destroyCurSong();
//                melody = new MelodyPorUnaCabeza( timer, (FootOnset *) onsets[1], &instruments ); //so far only FOLLOWER... change?
//                accompaniment = new AccompanimentPorUnaCabeza( timer, &instruments );
//                setMotionAnalysisParamsForSong(&porUnaCabezaCoupleParams, &porUnaCabezaLeaderParams, &porUnaCabezaFollowerParams);
//                loadSong(ugens, timer);
//                selectedSong = SongIDs::POR_UNA_CABEZA;
//                std::cout << "Loaded Por Una Cabexa\n"; 
//            };
        
            virtual std::vector<ci::osc::Message> getOSC()
            {
                std::vector<ci::osc::Message> msgs;
                return msgs;
            };
        
            virtual void averageAcrossDancers(std::vector<UGEN * > *ugens)
            {
                
                //push back variance data to create averages
                vars.push_back(leader->getAvgVariance());
                vars.push_back(follower->getAvgVariance());
                
                varDiffs.push_back(leader->getAvgVarianceDiff());
                varDiffs.push_back(follower->getAvgVarianceDiff());
                
                onsets.push_back(leader->getOnsets());
                onsets.push_back(follower->getOnsets());
                
                crossco.push_back(leaderLeftFollowerRightFeet);
                crossco.push_back(leaderRightFollowerLeftFeet);
                crossco.push_back(backs);
                
                //average movement variance across leader and follower
                totalVariance = new MotionDataAverage(vars);
                totalVarianceDiff = new MotionDataAverage(varDiffs);
                avgFootOnsets = new MotionDataAverage(onsets);
                avgCrossCoVariance = new MotionDataAverage(crossco);
                

                //add to ugens so that it is in the processing loop
                ugens->push_back(totalVariance);
                ugens->push_back(totalVarianceDiff);
                ugens->push_back(avgFootOnsets);
                ugens->push_back(avgCrossCoVariance);

            };
        
    protected:
        
           virtual void createSchemas(std::vector<UGEN * > *ugens, BeatTiming *timer) //TODO set default min and maxes in the signal analysis classes
            {
                
                //probably a more efficient way to do this
                MotionAnalysisEvent *stepNum = (MotionAnalysisEvent *)avgFootOnsets->getMotionData().at(FootOnset::MotionDataIndices::STEP_COUNT);
                MotionAnalysisEvent *winVarDiff = (MotionAnalysisEvent *)totalVarianceDiff->getMotionData().at(0);
                MotionAnalysisEvent *winVar = (MotionAnalysisEvent *)totalVariance->getMotionData().at(0);
                MotionAnalysisEvent *crosscovar = (MotionAnalysisEvent *)avgCrossCoVariance->getMotionData().at(0);
                MotionAnalysisEvent *avgOnsetPeak = (MotionAnalysisEvent *) avgFootOnsets->getMotionData().at(FootOnset::MotionDataIndices::AVG_PEAK);
                MotionAnalysisEvent *winVarDiffLeader = (MotionAnalysisEvent *) leader->getAvgVarianceDiff()->getMotionData().at(0);
                MotionAnalysisEvent *leaderStepNum = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_COUNT);
                MotionAnalysisEvent *leaderAvgPeak = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::AVG_PEAK);
                MotionAnalysisEvent *winVarDiffFollwer = (MotionAnalysisEvent *) follower->getAvgVarianceDiff()->getMotionData().at(0);
                MotionAnalysisEvent *followerStepNum = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_COUNT);
                MotionAnalysisEvent *followerAvgPeak = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::AVG_PEAK);

                MotionAnalysisEvent *followerIsStep = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::IS_STEP);
                MotionAnalysisEvent *leaderIsStep = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::IS_STEP);
                MotionAnalysisEvent *followerCurStepPeak = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::CUR_STEP_PEAK);
                MotionAnalysisEvent *leaderCurStepPeak = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::CUR_STEP_PEAK);
                
                MotionAnalysisEvent *leaderIsStepLeft = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_LEFT);
                MotionAnalysisEvent *followerIsStepLeft = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_LEFT);
                MotionAnalysisEvent *leaderIsStepRight = (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_RIGHT);
                MotionAnalysisEvent *followerIsStepRight = (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::STEP_RIGHT);
                
                MotionAnalysisEvent *leaderLeftVarDiff = (MotionAnalysisEvent *) ((WindowedVarianceDifference *)leader->getLeftFoot()->at(Dancer::WhichSignals::WINDOWED_VARDIFF))->getMotionData().at(WindowedVarianceDifference::MotionDataIndices::AVG);
                MotionAnalysisEvent *followerLeftVarDiff = (MotionAnalysisEvent *) ((WindowedVarianceDifference *)follower->getLeftFoot()->at(Dancer::WhichSignals::WINDOWED_VARDIFF))->getMotionData().at(WindowedVarianceDifference::MotionDataIndices::AVG);
                MotionAnalysisEvent *leaderRightVarDiff = (MotionAnalysisEvent *) ((WindowedVarianceDifference *)leader->getRightFoot()->at(Dancer::WhichSignals::WINDOWED_VARDIFF))->getMotionData().at(WindowedVarianceDifference::MotionDataIndices::AVG);
                MotionAnalysisEvent *followerRightVarDiff = (MotionAnalysisEvent *) ((WindowedVarianceDifference *)follower->getRightFoot()->at(Dancer::WhichSignals::WINDOWED_VARDIFF))->getMotionData().at(WindowedVarianceDifference::MotionDataIndices::AVG);
                
                mMotionAnalysisEvents.push_back(stepNum);
                mMotionAnalysisEvents.push_back(winVarDiff);
                mMotionAnalysisEvents.push_back(winVar);
                mMotionAnalysisEvents.push_back(crosscovar);
                mMotionAnalysisEvents.push_back(avgOnsetPeak);
                mMotionAnalysisEvents.push_back((MotionAnalysisEvent *) leader->getAvgVariance()->getMotionData().at(0));
                mMotionAnalysisEvents.push_back(leaderStepNum);
                mMotionAnalysisEvents.push_back((MotionAnalysisEvent *)winVarDiffLeader);
                mMotionAnalysisEvents.push_back(leaderAvgPeak);
                mMotionAnalysisEvents.push_back((MotionAnalysisEvent *) follower->getAvgVariance()->getMotionData().at(0));
                mMotionAnalysisEvents.push_back(followerStepNum);
                mMotionAnalysisEvents.push_back((MotionAnalysisEvent *)winVarDiffFollwer);
                mMotionAnalysisEvents.push_back(followerAvgPeak);
                
                

                //this is only for couple... for now
//                PointyVsSmoothEvent *pv = new PointyVsSmoothEvent(timer,  stepNum,   winVarDiff );
//                BusyVsSparseEvent *bs = new BusyVsSparseEvent(timer,  winVar,   stepNum,   crosscovar );

//                RoomSizeAreaEvent *rs = new RoomSizeAreaEvent(timer,  stepNum,   crosscovar );
//                OrchThickVsThinEvent *ot =  new OrchThickVsThinEvent(timer,  stepNum,  winVar,  crosscovar );
                
                //replace with individual dancer pointy v. smooth 1/13/2016
                //PointyVsSmoothContinuous *pc = new PointyVsSmoothContinuous(timer,  stepNum, winVarDiff, avgOnsetPeak );
                
                
                //TODO: 2/22.2016 check these -- also one of the polyfits is badly off -- find out which one
                
                //OK FIX THIS SHIT!!!
//                enum WhichLineInSong{COUPLE=0, MELODY=1, ACCOMPANIMENT=2}; //may move to a pareja-class level enum?
                PointyVsSmoothContinuous *pcLeader = new PointyVsSmoothContinuous(timer, leaderStepNum, winVarDiffLeader, leaderAvgPeak, leader->getDancerID());
                pcLeader->setName("Leader Pointy vs. Smooth");
                
                
//                winVarDiffFollower->setMinMax(0, 0.05);
                PointyVsSmoothContinuous *pcFollower = new PointyVsSmoothContinuous(timer, followerStepNum, winVarDiffFollwer, followerAvgPeak, follower->getDancerID());
                pcFollower->setName("Follower Pointy vs. Smooth");
                
                BusyVsSparseEvent *bsLeader = new BusyVsSparseEvent(timer,mMotionAnalysisEvents[LEADER_WINDOWED_VAR], leaderStepNum,   crosscovar );
                bsLeader->setName("Leader BvS");
                BusyVsSparseEvent *bsFollower = new BusyVsSparseEvent(timer, mMotionAnalysisEvents[FOLLOWER_WINDOWED_VAR], followerStepNum,  crosscovar );
                bsFollower->setName("Follower BvS");
                
                followerRightFootCircling = new CirclingGestures( follower->getRootFootGestureRecognition() , timer, (SignalAnalysis *) follower->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), TangoEntity::WhichDancerType::FOLLOWER1 );
                followerRightFootBoleo = new BoleoGestures( ( SnapPeaks * ) follower->getRightFoot()->at(Dancer::WhichSignals::SNAP_PEAKS), follower->getRootFootGestureRecognition(), mTimer );
            
                leaderRightFootCircling = new CirclingGestures( leader->getRootFootGestureRecognition() , timer, (SignalAnalysis *) leader->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), TangoEntity::WhichDancerType::LEADER1 );
                leaderRightFootBoleo = new BoleoGestures( ( SnapPeaks * ) leader->getRightFoot()->at(Dancer::WhichSignals::SNAP_PEAKS), leader->getRootFootGestureRecognition(), mTimer );
                
                followerLeftFootCircling = new CirclingGestures( follower->getLeftFootGestureRecognition() , timer, (SignalAnalysis *) follower->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), TangoEntity::WhichDancerType::FOLLOWER1 );
                followerLeftFootBoleo = new BoleoGestures( ( SnapPeaks * ) follower->getLeftFoot()->at(Dancer::WhichSignals::SNAP_PEAKS), follower->getLeftFootGestureRecognition(), mTimer );
                
                leaderLeftFootCircling = new CirclingGestures( leader->getLeftFootGestureRecognition() , timer, (SignalAnalysis *) leader->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), TangoEntity::WhichDancerType::LEADER1 );
                leaderLeftFootBoleo = new BoleoGestures( ( SnapPeaks * ) leader->getLeftFoot()->at(Dancer::WhichSignals::SNAP_PEAKS), leader->getLeftFootGestureRecognition(), mTimer );

                

                PointyVsSmoothInstrumentEvent *pvsInstrLeader = new PointyVsSmoothInstrumentEvent(timer, leaderStepNum, winVarDiffLeader, leaderAvgPeak);
                pvsInstrLeader->setName(" Leader P v C Instrument ");
                PointyVsSmoothInstrumentEvent *pvsInstrFollower = new PointyVsSmoothInstrumentEvent(timer, followerStepNum, winVarDiffFollwer, followerAvgPeak);
                pvsInstrFollower->setName(" Follower P v C Instrument ");
                
                //add motion data from perceptual events
                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) bsLeader->getMotionDataItem(0) );
                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) bsFollower->getMotionDataItem(0) );
                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) pcLeader->getMotionDataItem(0) );
                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) pcFollower->getMotionDataItem(0) );
                mMotionAnalysisEvents.push_back(  leaderIsStep  );
                mMotionAnalysisEvents.push_back(  followerIsStep  );
                mMotionAnalysisEvents.push_back(  leaderCurStepPeak  );
                mMotionAnalysisEvents.push_back(  followerCurStepPeak  );
                mMotionAnalysisEvents.push_back(  leaderIsStepLeft  );
                mMotionAnalysisEvents.push_back(  followerIsStepLeft  );
                mMotionAnalysisEvents.push_back(  leaderIsStepRight  );
                mMotionAnalysisEvents.push_back(  followerIsStepRight  );
                
                mMotionAnalysisEvents.push_back(  leaderLeftVarDiff  );
                mMotionAnalysisEvents.push_back(  followerLeftVarDiff  );
                mMotionAnalysisEvents.push_back(  leaderRightVarDiff  );
                mMotionAnalysisEvents.push_back(  followerRightVarDiff  );

                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) leader->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::WHICH_AXIS_PEAK)  );
                mMotionAnalysisEvents.push_back(  (MotionAnalysisEvent *) follower->getOnsets()->getMotionData().at(FootOnset::MotionDataIndices::WHICH_AXIS_PEAK)  );

                
                for(int i=0; i<mMotionAnalysisEvents.size(); i++)
                {
                    mMotionAnalysisEvents[i]->setName( motionAnalysisParamsStrings[i] );
                }
                
                LongStepSendSignal *leaderLSSS = new LongStepSendSignal(timer, mMotionAnalysisEvents[LEADER_IS_STEP_LEFT], mMotionAnalysisEvents[LEADER_IS_STEP_RIGHT], (SignalAnalysis *) leader->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), (SignalAnalysis *) leader->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), leader->getDancerID() );
                
                LongStepSendSignal *followerLSSS = new LongStepSendSignal(timer, mMotionAnalysisEvents[FOLLOWER_IS_STEP_LEFT], mMotionAnalysisEvents[FOLLOWER_IS_STEP_RIGHT], (SignalAnalysis *) follower->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), (SignalAnalysis *) follower->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), follower->getDancerID() );

                //add room size continuous
                RoomSizeAreaContinuous *roomSize = new RoomSizeAreaContinuous(timer);
                roomSize->addPareja(crosscovar, mMotionAnalysisEvents[LEADER_BUSYSPARSE], mMotionAnalysisEvents[FOLLOWER_BUSYSPARSE], mMotionAnalysisEvents[LEADER_POINTYROUND], mMotionAnalysisEvents[FOLLOWER_POINTYROUND]);
                
                OnsetPeakToAccentPatterns *optapFollower = new OnsetPeakToAccentPatterns(mMotionAnalysisEvents[FOLLOWER_CUR_STEP_PEAK], mMotionAnalysisEvents[FOLLOWER_BUSYSPARSE], mMotionAnalysisEvents[FOLLOWER_IS_STEP_LEFT], mMotionAnalysisEvents[FOLLOWER_IS_STEP_RIGHT], mMotionAnalysisEvents[FOLLOWER_LEFTFOOT_WINDOWEDVARDIFF], mMotionAnalysisEvents[FOLLOWER_RIGHTFOOT_WINDOWEDVARDIFF], mMotionAnalysisEvents[FOLLOWER_STEPPEAK_AXIS], TangoEntity::WhichDancerType::FOLLOWER1 );
                
                OnsetPeakToAccentPatterns *optapLeader = new OnsetPeakToAccentPatterns(mMotionAnalysisEvents[LEADER_CUR_STEP_PEAK], mMotionAnalysisEvents[LEADER_BUSYSPARSE], mMotionAnalysisEvents[LEADER_IS_STEP_LEFT], mMotionAnalysisEvents[LEADER_IS_STEP_RIGHT], mMotionAnalysisEvents[LEADER_LEFTFOOT_WINDOWEDVARDIFF], mMotionAnalysisEvents[LEADER_RIGHTFOOT_WINDOWEDVARDIFF], mMotionAnalysisEvents[LEADER_STEPPEAK_AXIS], TangoEntity::WhichDancerType::LEADER1 );
                
                
                //send back sensors in real time
                SendSignal *followerSendBack = new SendSignal( (Filter *) follower->getBack()->at(Dancer::WhichSignals::LPFILTER15Hz), follower->getDancerID() );
                SendSignal *leaderSendBack = new SendSignal( (Filter *) leader->getBack()->at(Dancer::WhichSignals::LPFILTER15Hz), leader->getDancerID()  );
                
                //add to signal processing tree
//                ugens->push_back(pv);
                
                //-------the dance floor class will update these

                ugens->push_back(bsLeader);
                ugens->push_back(bsFollower);
                
//                ugens->push_back(bs);

                //These are no longer used...
//                ugens->push_back(rs);
//                ugens->push_back(ot);
                
                //1/13/2016 -- do individual instead for staccato/legato
//                ugens->push_back(pc);
                ugens->push_back(pcLeader);
                ugens->push_back(pcFollower);
                
                ugens->push_back( followerRightFootCircling ); 
                ugens->push_back( followerRightFootBoleo );
                ugens->push_back( leaderRightFootCircling );
                ugens->push_back( leaderRightFootBoleo );
                
                ugens->push_back( followerLeftFootCircling );
                ugens->push_back( followerLeftFootBoleo );
                ugens->push_back( leaderLeftFootCircling );
                ugens->push_back( leaderLeftFootBoleo );

                //-------the dance floor class will update these
                
                ugens->push_back( roomSize );
                
                ugens->push_back(pvsInstrLeader);
                ugens->push_back(pvsInstrFollower);
//                
                ugens->push_back( optapLeader );
                ugens->push_back( optapFollower );
                
                ugens->push_back( leaderSendBack );
                ugens->push_back( followerSendBack );
    
                //this is not implemented yet
//                ugens->push_back( leaderLSSS );
//                ugens->push_back( followerLSSS );

                
                
                mMappingSchemas.push_back(bsLeader);
                mMappingSchemas.push_back(bsFollower);
                
                mMappingSchemas.push_back(roomSize);

                mMappingSchemas.push_back(pcLeader);
                mMappingSchemas.push_back(pcFollower);
                
                mMappingSchemas.push_back(pvsInstrLeader);
                mMappingSchemas.push_back(pvsInstrFollower);
                
                
                //push_back onset patterns
                mMappingSchemas.push_back( (MappingSchema *) optapLeader);
                mMappingSchemas.push_back((MappingSchema *)  optapFollower);
                
                mMappingSchemas.push_back((MappingSchema *)   leaderSendBack );
                mMappingSchemas.push_back((MappingSchema *)   followerSendBack );


            };
    public:
        
            virtual void update(float seconds = 0)
            {
                //show averages
                if ( (leader != NULL) && (follower != NULL) )
                {
                    printMinMaxAvgMotionAnalysisEvents();
                }
            };
        

    
    };
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    class DanceFloor : public TangoEntity
    {
    public:
        enum CurrentMapping{ NONE= -1, ONE_COUPLE=0, TWO_COUPLES=1, TWO_COUPLES_ALT=2  };
        
        //leader back sensors are at the end, but are variable
        enum WhichSongMapping{ MELODY_BS=0, MELODY_INSTRUMENT_PR=1, ACCOMPANIMENT_BS=2, ACCOMPANIMENT_INSTRUMENT_PR=3, CONT_MELODY_PC=4, CONT_ACCOMP_PC=5, ROOM_SIZE=6,  MELODY_VOL=7, ACCOMP_VOL=8 };
        
    protected:
        std::vector<Pareja *> couples;
        
        MainMelodySection *melody;
        AccompanimentSection *accompaniment;
        std::vector<OrnamentationSection *> ornaments;
        MusicPlayer *player;
        TangoInstruments instruments;
        BeatTiming *mTimer;
        
        TangoEntity::SongIDs selectedSong;
        
        CurrentMapping curMapping;
        
        Dancer *melodyOnsetDancer; //the dancer who sets off melodic fragments... generally a follower
        std::vector<MappingSchema *> mMappingSchemas;
        
    public:

        DanceFloor(BeatTiming *timer) : TangoEntity()
        {

            melody = NULL;
            accompaniment = NULL;
            
            mTimer = timer;
            
            curMapping = NONE;
            
           player = new MusicPlayer();
                        
        };
        

        size_t parejaCount(){ return couples.size(); };
        
        int getLeaderDancerID(int i){ return couples[i]->getLeader()->getDancerID(); };
        int getFollowerDancerID(int i){ return couples[i]->getFollower()->getDancerID(); };
        
        void changeBusySparseScale(WhichDancerType which, float newScale)
        {
            MappingSchema *mappingSchema;
            
            if( which == WhichDancerType::LEADER1 )
            {
                mappingSchema = couples[0]->getMappingSchema(BUSY_SPARSE_LEADER);
            }
            else if ( which == WhichDancerType::FOLLOWER1 )
            {
                mappingSchema = couples[0]->getMappingSchema(BUSY_SPARSE_FOLLOWER);
            }
            else if( which == WhichDancerType::LEADER2 && curMapping == CurrentMapping::TWO_COUPLES )
            {
                mappingSchema = couples[1]->getMappingSchema(BUSY_SPARSE_LEADER);
            }
            else if ( which == WhichDancerType::FOLLOWER2 && curMapping == CurrentMapping::TWO_COUPLES  )
            {
                mappingSchema = couples[1]->getMappingSchema(BUSY_SPARSE_FOLLOWER);
            }
            mappingSchema->setScale(newScale);
            
            std::cout << DancerTypeStr(which) << " scale changed to: " << newScale << std::endl;

        };
        
        
        void clearCouples()
        {
            couples.clear();
            curMapping = CurrentMapping::NONE;
        };
        
        virtual void addPareja(Pareja *pareja)
        {
            couples.push_back(pareja);

        
            //reset and redo mappings based on number in the system
           if( couples.size() == 1 )
           {
               mappingForOneCouple();
               curMapping = CurrentMapping::ONE_COUPLE;
           }
           else if( couples.size() == 2 )
           {
               mappingForTwoCouples();
               curMapping = CurrentMapping::TWO_COUPLES;
           }
        };
        

        inline CurrentMapping getCurrentMapping(){ return curMapping; };
        
        //player must be the last thing to update
        virtual void update(float seconds)
        {
            if( getCurrentMapping() == NONE ) return;
       
// mapping schemas should update in ugen collection now --> 1/19/2017
//            for( int i=0; i<mMappingSchemas.size(); i++ )
//            {
//                mMappingSchemas[i]->update(seconds);
//            }
            
            player->update(seconds);
        };
        
        //player must be the first thing to send OSC.
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            if( getCurrentMapping() == NONE ) return msgs;
            
            msgs = player->getOSC();

            
//            for( int i=0; i<mMappingSchemas.size(); i++ )
//            {
//                std::vector<ci::osc::Message> ms = mMappingSchemas[i]->getOSC();
//                for( int j=0; j<ms.size(); j++ )
//                {
//                    msgs.push_back(ms[j]);
//                }
//            }
            return msgs;
        };
        
        void initMappingVector()
        {
            //the sending android back info will be for every dancer
            for(int i=MELODY_BS; i<=ACCOMP_VOL + couples.size()*2; i++ )
            {
                mMappingSchemas.push_back(NULL);
            }
        };
        
        
        void mappingForOneCouple()
        {
            mMappingSchemas.clear(); //clear previous mapping
            initMappingVector();
            
            melodyOnsetDancer = couples[0]->getFollower();
            
            mMappingSchemas[MELODY_BS] = couples[0]->getMappingSchema(BUSY_SPARSE_FOLLOWER);
            mMappingSchemas[MELODY_INSTRUMENT_PR]  = couples[0]->getMappingSchema(INSTRUMENT_POINTROUND_FOLLOWER);
            mMappingSchemas[ACCOMPANIMENT_BS]  = couples[0]->getMappingSchema(BUSY_SPARSE_LEADER);
            mMappingSchemas[ACCOMPANIMENT_INSTRUMENT_PR]  = couples[0]->getMappingSchema( INSTRUMENT_POINTROUND_LEADER                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       );
            mMappingSchemas[CONT_MELODY_PC] = couples[0]->getMappingSchema( POINTY_ROUNDED_CONT_FOLLOWER );
            mMappingSchemas[CONT_ACCOMP_PC] = couples[0]->getMappingSchema( POINTY_ROUNDED_CONT_LEADER );
            mMappingSchemas[MELODY_VOL] = couples[0]->getMappingSchema( LEADER_VOLUME );
            mMappingSchemas[ACCOMP_VOL] = couples[0]->getMappingSchema( FOLLOWER_VOLUME );
            
            mMappingSchemas[ACCOMP_VOL+1] = couples[0]->getMappingSchema( LEADER_SEND_BACK );
            mMappingSchemas[ACCOMP_VOL+2] = couples[0]->getMappingSchema( FOLLOWER_SEND_BACK );
            
            //set up reverb
            mMappingSchemas[DanceFloor::WhichSongMapping::ROOM_SIZE] = couples[0]->getMappingSchema( ROOMSIZE_CONTINUOUS );
        };
        
        void mappingForTwoCouples()
        {
            mMappingSchemas.clear(); //clear previous mapping
            initMappingVector();
            
            //couple 1
            //follower
            //    - melody onsets
            //    - continuous pointy / round
            //    - onset peak / volume
            //leader 1
            //    - continuous pointy / round
            //    - onset peak / volume
            melodyOnsetDancer = couples[1]->getFollower();
            mMappingSchemas[MELODY_VOL] = couples[1]->getMappingSchema( LEADER_VOLUME );
            mMappingSchemas[ACCOMP_VOL] = couples[1]->getMappingSchema( FOLLOWER_VOLUME );
            mMappingSchemas[MELODY_INSTRUMENT_PR]  = couples[1]->getMappingSchema(INSTRUMENT_POINTROUND_FOLLOWER);
            mMappingSchemas[ACCOMPANIMENT_INSTRUMENT_PR]  = couples[1]->getMappingSchema(INSTRUMENT_POINTROUND_LEADER);


            
            //couples send back info
            mMappingSchemas[ACCOMP_VOL+1] = couples[1]->getMappingSchema( LEADER_SEND_BACK );
            mMappingSchemas[ACCOMP_VOL+2] = couples[1]->getMappingSchema( FOLLOWER_SEND_BACK );

            
            //set up reverb
            mMappingSchemas[ROOM_SIZE] = couples[1]->getMappingSchema( ROOMSIZE_CONTINUOUS );

            
            //couple 2
            //follower
            //    - busy/sparse
            //    - instrument pointy / round
            //leader 2
            //    - busy/sparse
            //    - instrument pointy / round
            mMappingSchemas[MELODY_BS] = couples[0]->getMappingSchema(BUSY_SPARSE_FOLLOWER);
            mMappingSchemas[ACCOMPANIMENT_BS]  = couples[0]->getMappingSchema(BUSY_SPARSE_LEADER);
            mMappingSchemas[CONT_MELODY_PC] = couples[0]->getMappingSchema( POINTY_ROUNDED_CONT_LEADER );
            mMappingSchemas[CONT_ACCOMP_PC] = couples[0]->getMappingSchema( POINTY_ROUNDED_CONT_FOLLOWER );

            
            //all couples send back info
            mMappingSchemas[ACCOMP_VOL+3] = couples[0]->getMappingSchema( LEADER_SEND_BACK );
            mMappingSchemas[ACCOMP_VOL+4] = couples[0]->getMappingSchema( FOLLOWER_SEND_BACK );
        };
        
        void mappingForTwoCouplesAlt()
        {
            //TBA
        };
        
        
        void destroyCurSong()
        {
            player->setMainMelody( NULL );
            if( melody != NULL ) delete melody;
            melody = NULL;
            
            player->clearCurAccompaniment(); //clear current accompaniments
            if( accompaniment != NULL ) delete accompaniment;
            accompaniment = NULL;
            
            player->clearCurOrnaments();
            for( int i=0; i<ornaments.size(); i++ )
            {
                if( ornaments[i] != NULL ) delete ornaments[i];
            }
            ornaments.clear();
            
            player->reset();
        };
        
        ~DanceFloor()
        {
            destroyCurSong();
            delete player;
        };
        
    
        
        void loadCourtneyTangoSongNumberOneOrnaments()
        {
            //TODO: refactor
            for( int i=0; i<couples.size(); i++ )
            {
                
                //NOTE -- disabled circling
//                OrnamentationSection *orn = new OrnamentCirclingSectionA( couples[i]->getGestureMapping(FOLLOWER_RIGHT_CIRCLE),  mTimer, couples[i]->getFollower()->getOnsets(), &instruments );
                OrnamentationSection *boleo = new BoleoOrnamentationSection( couples[i]->getGestureMapping(FOLLOWER_RIGHT_BOLEO), mTimer, couples[i]->getFollower()->getOnsets(), &instruments );
//                OrnamentationSection *leaderorn = new OrnamentCirclingSectionA( couples[i]->getGestureMapping(LEADER_RIGHT_CIRCLE),  mTimer, couples[i]->getLeader()->getOnsets(), &instruments );
                OrnamentationSection *leaderboleo = new BoleoOrnamentationSection( couples[i]->getGestureMapping(LEADER_RIGHT_BOLEO), mTimer, couples[i]->getLeader()->getOnsets(), &instruments );
                
//                OrnamentationSection *ornL = new OrnamentCirclingSectionA( couples[i]->getGestureMapping(FOLLOWER_LEFT_CIRCLE),  mTimer, couples[i]->getFollower()->getOnsets(), &instruments );
                OrnamentationSection *boleoL = new BoleoOrnamentationSection( couples[i]->getGestureMapping(FOLLOWER_LEFT_BOLEO), mTimer, couples[i]->getFollower()->getOnsets(), &instruments );
//                OrnamentationSection *leaderornL = new OrnamentCirclingSectionA( couples[i]->getGestureMapping(LEADER_LEFT_CIRCLE),  mTimer, couples[i]->getLeader()->getOnsets(), &instruments );
                OrnamentationSection *leaderboleoL = new BoleoOrnamentationSection( couples[i]->getGestureMapping(LEADER_LEFT_BOLEO), mTimer, couples[i]->getLeader()->getOnsets(), &instruments );

            
                boleo->addSchema( couples[i]->getMappingSchema( BUSY_SPARSE_FOLLOWER ) );
                leaderboleo->addSchema( couples[i]->getMappingSchema( BUSY_SPARSE_LEADER ) );
                boleoL->addSchema( couples[i]->getMappingSchema( BUSY_SPARSE_FOLLOWER ) );
                leaderboleoL->addSchema( couples[i]->getMappingSchema( BUSY_SPARSE_LEADER ) );

            
//                ornaments.push_back( orn );
                ornaments.push_back( boleo );
//                ornaments.push_back( leaderorn );
                ornaments.push_back( leaderboleo );
//                ornaments.push_back( ornL );
                ornaments.push_back( boleoL );
//                ornaments.push_back( leaderornL );
                ornaments.push_back( leaderboleoL );
            
//                player.addOrnament( orn );
                player->addOrnament( boleo );
//                player.addOrnament( leaderorn );
                player->addOrnament( leaderboleo );
//                player.addOrnament( ornL );
                player->addOrnament( boleoL );
//                player.addOrnament( leaderornL );
                player->addOrnament( leaderboleoL );
            }
        };
        
        
        void loadSong(BeatTiming *timer)
        {
            melody->addSchema( mMappingSchemas[MELODY_BS] );  //not using to choose melodies at the MOMENT //choosing again 8/2014
            mMappingSchemas[MELODY_BS]->setName("Melody Busy Sparse");
            melody->addSchema( mMappingSchemas[MELODY_INSTRUMENT_PR] );
            
            //                accompaniment->addSchema( mMappingSchemas[THICK_THIN] ); //7-29 deleting for now... busy/spare more accurate... I think... look at later...
            accompaniment->addSchema( mMappingSchemas[ACCOMPANIMENT_BS] );
            mMappingSchemas[ACCOMPANIMENT_BS]->setName("Leader Busy Sparse");
            accompaniment->addSchema( mMappingSchemas[ACCOMPANIMENT_INSTRUMENT_PR] );
            
            player->setMainMelody( melody );
            player->addAccompaniment( accompaniment );
        };
        
//not used in this version
//        void loadCourtneyTangoSongNumberOneIntoPlayer()
//        {
//            destroyCurSong();
//            if( couples.size() <= 0 )
//            {
//                std::cout << "Warning! No couples on dance floor. Cannot load song! ";
//            }
//
//
//            melody = new MelodySectionA( mTimer, melodyOnsetDancer->getOnsets() , &instruments ); //so far only FOLLOWER... change?
//            accompaniment = new AccompanimentSectionA( mTimer, &instruments );
//
//            //              ornaments
//            for( int i=0; i<couples.size(); i++ )
//                couples[i]->setMotionAnalysisParamsForSong(TangoEntity::SongIDs::FRAGMENTS);
//
//            loadSong(mTimer);
//            loadCourtneyTangoSongNumberOneOrnaments();
//
//            selectedSong = TangoEntity::SongIDs::FRAGMENTS;
//            std::cout << "Loaded Fragments\n";
//        };
//
//        void loadPorUnaCabezaIntoPlayer()
//        {
//            destroyCurSong();
//            if( couples.size() <= 0 )
//            {
//                std::cout << "Warning! No couples on dance floor. Cannot load song! ";
//            }
//
//
//            melody = new MelodyPorUnaCabeza( mTimer, melodyOnsetDancer->getOnsets(), &instruments ); //so far only FOLLOWER... change?
//            accompaniment = new AccompanimentPorUnaCabeza( mTimer, &instruments );
//
//            for( int i=0; i<couples.size(); i++ )
//                couples[i]->setMotionAnalysisParamsForSong(TangoEntity::SongIDs::POR_UNA_CABEZA);
//
//            loadSong(mTimer);
//            selectedSong = TangoEntity::SongIDs::POR_UNA_CABEZA;
//            std::cout << "Loaded Por Una Cabeza\n";
//        };
        
        //temp here, I think....... this restarts the music player to the beginning
        virtual void restartPlayer()
        {
            player->reset();
        }


    };

};



#endif
