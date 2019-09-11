
//OK, originally I was going to do a magnetic time piece, but now I am going to do an experimental music piece which is more general
//This is my plan:
//OK I AM NOW DOING A MAGNETIC TIME PIECE. WOO.
//magnetic time is implemented in Max 8 rather than the C++ in this version -- but not w/Carlos Gardel

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
//#include "cinder/Capture.h"
#include "cinder/ip/Resize.h"
#include "cinder/Rand.h"
#include <fstream>
#include <sstream>

#include <assert.h>

#define _COUT_DEBUG_ 0


#include "OscSender.h"
#include "OscListener.h"

#include "InteractiveTangoOSCMessages.h"

#include "MIDIUtility.h"

#include "ReadCSV.h"
#include "ShimmerData.h"
#include "NotchData.h"
#include "VisualizeShimmer.h"
#include "ShimmerWrapper.h"

#include "SensorData.h"

#include "Containers.h"
#include "MotionAnalysisParameterRanges.h"
#include "MotionAnalysisOuput.h"

#include "BeatTiming.h"
#include "InteractiveTangoSignalAnalysis.h"
#include "InteractiveTangoStepDetection.h"
#include "PerceptualMappingScheme.h"
#include "PerceptualSchemas.h"
#include "GestureRecognizerUGENs.h"

#include "Instruments.h"
#include "SoundFile.h"
#include "MusicPlayer.h"
#include "MelodyGeneratorAlgorithm.h"
#include "FactorOracle.h"

#include "ChordGeneration.h"
#include "MelodyGenerator.h"

#include "SendOSCUGENs.h"
#include "MappingSchemaEventInContinuousOut.h"

//#include "TangoFragmentsSectionALoader.h"

#include "Dancers.h"

#include "ExperimentalMusicPlayer.h"
#include "ExperimentalMusicDancers.h"
#include "MachineTangoDancers.h"
#include "sequence_player.h"
#include "SaveOSC.h"

//#include "MagneticTime.h"

#define MAX_PORT 89898

#define SAVE_ALL_OSC //define if you want to save all the incoming OSC


using namespace ci;
using namespace ci::app;
using namespace std;

//InteractiveTango::MidiFileUtility midiFile;


//void MachineTango::setup()
//{
//    InteractiveTango::MelodyGenerator generator;
//    InteractiveTango::FactorOracle fo;
//    
//    fo.train("/Users/courtney/Documents/Interactive Tango Milonga/tango_melody_midi/Piazzola_Astor_one_line_mel.mid");
//    generator.addGeneratorAlgorithm(&fo);
//    
//}


class MachineTango : public App {
public:
    void setup();
    void keyDown( KeyEvent event );
    void update();
    void draw();
    SensorData * getSensor(std::string whichShimmer);
    
private:
    osc::Listener mListener;
    SensorData * getSensor(string deviceID, int which);
    void drawGrid(float size=100.0f, float step=2.0f);
    void sendPhotoSync(float seconds);
    
    std::vector<SensorData *> mSensorData;
    std::vector<VisualizeShimmer *> visualizers;
    
    std::vector<InteractiveTango::UGEN *> mUGENs;
    int mHoldShimmerIndex;
    int mHoldShimmerIndex2;
    
    ci::ColorA generateRandomColor();
    void screenshot();
    void loadSensor(std::string input);
    void parseFilenameForInfo(string fname, string *deviceID, int *whichID);
    void addStepDetection(int id1, int id2, std::string port1, std::string port2);
    virtual void prepareSettings(Settings *settings);
    void handleOSC(float seconds);
    void saveAllSensors();
    void updateSignalTree(float seconds, float pastSeconds);
    std::vector<ci::osc::Message> collectMessagesforDifferentPorts(std::string addr, std::vector<ci::osc::Message> *msgs, std::vector<ci::osc::Message> nmsgs);
    std::vector<ci::osc::Message> collectMessagesforDifferentPortsBack(std::vector<ci::osc::Message> *msgs);
    
    void loadChosenFiles();
    void loadFragmentsStaccatoSavedFiles();
    void loadPorUnaCabezaNormSavedFiles();
    void loadPorUnaCabezaStaccatoSavedFiles();
    void playPorUnaCabeza();
    void playFragments();
    void addPareja();
    void addDancer();
    void stopOSCToAbleton();
    void startOSCToAbleton();
    void stopStartOSCToAbleton();
    void restartSong();
    InteractiveTango::Pareja::SongIDs whichSongIsPlaying;
    void cycleThroughPlaylist();
    void changeSensorColor();
    void addMarkerToSensorFile(std::string marker);
    
    void setupSensorsForTest();
    void setupSensorsForTestTwoDancers();
    
    void printMenu();
    
    //for handling wii
    void handleWiiOSC(osc::Message message); // turn into commands in application
    void wiiToWekinator(osc::Message message); // forward commands to wekinator
    void printNormalWiiMenu();
    bool isWiiButtonDown(std::string addr, osc::Message message);
    
    //for machine learning in Wekinator, etc.
    bool isSendingToWekinator; //whether wii is sending messages to wekinator
    std::vector<osc::Sender *> wekinatorSenders;
    std::vector<osc::Sender *> wiiWekinatorSenders;
    std::vector<InteractiveTango::SendOSCToWek *> sendOSCToWekinators;
    int whichDTWWekinatorOutput;
    int selectedWekinatorSenderIndex;
    
    void printWekModeMenu();
    void printChangingParamsMenu();
    int addSendtoWekinator();
    void sendOSCInputsToWekinator(std::vector<ci::osc::Message> msgs, float seconds, int index );
    void handleOutputWekOSC( osc::Message message, float seconds );
    
    
    //use wii to change movement min/max params....
    bool isChangingMovementParams;
    void changeMovementParamsWithWii(osc::Message message);
    
    
    //if there are any OSC messages here, clear before proceeding.
    void clearResidualData();
    void clearListeningUDPSocket();
    
    int beatIndex; //for debug
    
    std::string mSaveFilePath;
    
    ci::CameraPersp     mCamera;
    Timer               mTimer;
    long                mIndex;
    bool                mShowMovie;
    bool				mPlayPrevious;
    
    bool                playing_done;
    
    bool                fakeBSLeaderMode;
    bool                fakeBSFollowerMode;
    bool                fakeBSAccompMode;

    
    std::string         whichShimmer;
    osc::Sender         syncSender;
    int                 selectedPareja; //for changing motion params, etc.
    
    //different senders for different messages
    //why? bc m4l is CRAZY
    osc::Sender         sender;
    osc::Sender         pVSsSender;
    osc::Sender         ornamentSender; //this sends the ornaments... OSC is getting dropped, so testing if this is why (too much sending at once);
    osc::Sender         circlingSender; //send circling steps
    osc::Sender         longStepSender; //send long step info (not yet mapped)
    osc::Sender         reverbSender;
    osc::Sender         backSender;
    osc::Sender         backSenderFollower;
    
    //saving OSC that the program receives
    InteractiveTango::SaveOSC *mSaveOSC;
    InteractiveTango::PlayOSC *mPlayOSC;
    
    
    void sendOSCMessages( std::vector<ci::osc::Message> msgs, float seconds  );
    
    //to send OSC messages, etc. differing rates, keep Hz down
    float pastseconds;
    float lastSentOSC;
    
    
    void addCrossCoVar(int id1, int id2, std::string p1, std::string p2);
    
    std::vector<InteractiveTango::Dancer *> mDancers;
    std::vector<InteractiveTango::Pareja *> mParejas;
    InteractiveTango::MachineDanceFloor *danceFloor;
    
    InteractiveTango::BeatTiming mBeatTimer;
    bool    receivedBeat;
    
    //add dancers automatically
    bool addLiveDancers;
    void addLivePareja(std::map<int, int> &l, std::map<int, int> &f, int lID, int fID);
    void addLiveDancer(SensorData *sensor, int sid);
    void addTestDancersNoSensors();
    
    std::map<int, int> leader, follower, leader2, follower2;
    
    bool shouldSendOSC;
    
    //    Engine *ep; //matlab engine
    
    void setRealTimePriorityHigh();
    
};

////TEST THE MELODY GENERATORS
//void MachineTango::setup()
//{
//    InteractiveTango::FactorOracle leaderfo;
//    leaderfo.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.v3.mid", 1);
////     leaderfo.testTrain();
////    leaderfo.printTrain();
//    
//    for(int i=0; i<200; i++)
//    {
//        std::cout << leaderfo.generateNext().pitch << " ";
//    }
//    
//    
////    followerfo.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.v3.mid", 1);
////    follower_gen.addGeneratorAlgorithm(&followerfo);
////    generators.push_back(follower_gen);
//}



void MachineTango::prepareSettings(Settings *settings)
{
    // settings->setWindowSize(1000,800);
    settings->setTitle("Experimental Interactive Tango");
    settings->setFrameRate(FRAMERATE); //set fastest framerate
//    settings->setHighDensityDisplayEnabled( true );
    
}

void MachineTango::setup()
{
//setup to save OSC
#ifdef SAVE_ALL_OSC
    fs::path savePath  = getSaveFilePath();
    mSaveOSC = new InteractiveTango::SaveOSC(savePath.c_str());
#else
    mSaveOSC = NULL;
#endif
    
    //always starts null -- need to press 'p' to open an file to play the saved OSC
    mPlayOSC = NULL;
    
    
    //just in case
    clearListeningUDPSocket();
    
    //listening to sensor data
    mListener.setup( OSC_LISTENING_PORT );
    clearResidualData(); //even more just in case
    
    
    mSaveFilePath = "";
    
    // set up the camera for screen display
    mCamera.setEyePoint(vec3(5.0f, 10.0f, 10.0f));
    mCamera.lookAt(vec3(0.0f, 0.0f, 0.0f));
    mCamera.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 1000.0f);
    
    // start timer and init variables for mocap replay
    mPlayPrevious = false;
    mShowMovie = false;
    mIndex = 0;
    
    //init indices, which hold user-selected sensor indices (yes, includes Androids)
    mHoldShimmerIndex = -1;
    mHoldShimmerIndex2 = -1;
    
    //setup the OSC senders
    sender.setup( SELF_IPADDR, SC_LANG_PORT );
    reverbSender.setup( SELF_IPADDR, REVERB_PORT );
    pVSsSender.setup( SELF_IPADDR, POINTY_VS_SMOOTH_PORT );
    syncSender.setup( PC_IPADDR, PHOTO_TIMING_PORT );
    ornamentSender.setup(SELF_IPADDR, ORNAMENT_PORT );
    circlingSender.setup(SELF_IPADDR, CIRCLING_PORT ); //send circling steps
    longStepSender.setup(SELF_IPADDR, LONGSTEP_PORT ); //send long step info (not yet mapped)
    backSender.setup(SELF_IPADDR, BACK_PORT);
    
    //initialize tempo / timing information
    mBeatTimer.setBPM(100);
    mBeatTimer.setError(2.1);
    receivedBeat = false;
    
    danceFloor =  new InteractiveTango::MachineDanceFloor(&mBeatTimer);
    
    //setupSensorsForTest();
    //setupSensorsForTestTwoDancers();
    addLiveDancers = true;
    shouldSendOSC = true;
    playing_done = false;
    
    lastSentOSC = 0;
    pastseconds = 0;
    
    //wekinator initialization & setting wii state
    isSendingToWekinator = false;
    isChangingMovementParams=false;
    selectedPareja = 0; //start w 0;
//    printNormalWiiMenu();
    whichDTWWekinatorOutput = 1;
    selectedWekinatorSenderIndex = 0; //none created or connected
    //    outputWekListener.setup(WEKINATOR_OUTPUT_PORT);
    
    //init for use of rand() throughout
    std::srand(time(NULL));
    
    //start timer AFTER initialization...
    mTimer.start();
    
    fakeBSFollowerMode = false;
    fakeBSLeaderMode = false;
    fakeBSAccompMode = false;
    
    //setRealTimePriorityHigh();
}

void MachineTango::keyDown( KeyEvent event )
{
    if(fakeBSLeaderMode || fakeBSFollowerMode || fakeBSAccompMode)
    {
        
        if((event.getChar() != '1' && event.getChar() != '2' && event.getChar() != '3') ||
           !(fakeBSAccompMode && (event.getChar() == '4' || event.getChar() == '5') ))
        {
            
            std::cout << "Not a valid value to change busy/sparse. Exiting mode...";
        }
        
        char c[1]; c[0] = event.getChar();
        std::string s(c);
        double res = std::atof(s.c_str());
        
        if(fakeBSLeaderMode)
        {
            danceFloor->leaderFakeBusySparse(res);
            std::cout << "Set leader busy/sparse to :" << res << std::endl;
            
        }
        else if(fakeBSFollowerMode)
        {
            danceFloor->followerFakeBusySparse(res);
            std::cout << "Set follower busy/sparse to :" << res << std::endl;
        }
        else{
            danceFloor->accompFakeBusySparse(res);
            std::cout << "Set accompaniment busy/sparse to :" << res << std::endl;
        }
        
        fakeBSLeaderMode = false;
        fakeBSFollowerMode = false;
        fakeBSAccompMode = false;
        
    }
    
    
    if( event.getChar() == 's' )
    {
        mSaveFilePath = App::getSaveFilePath().c_str();
        for( int i=0; i<mSensorData.size(); i++)
        {
            mSensorData[i]->saveToFile(mSaveFilePath);
        }
        mBeatTimer.save(mSaveFilePath);
    }
    else if (event.getChar() == 'x')
    {
        stopStartOSCToAbleton();
    }
    else if( event.getChar() == 'z' )
    {
        restartSong();
    }
    else if( event.getChar() == 't' )
    {
        addTestDancersNoSensors();
    }
    else if( event.getChar() == 'l')
    {
        if(danceFloor->parejaCount() < 1)
        {
            std::cout << "Cannot create a fake step because there are no dancers on the floor\n";
            return;
        }
        else{
            std::cout << "Creating a fake step for leader...";
            danceFloor->leaderFakeStep();
            std::cout << "done\n";
            
        }
    }
    else if( event.getChar() == 'f')
    {
        if(danceFloor->parejaCount() < 1)
        {
            std::cout << "Cannot create a fake step because there are no dancers on the floor\n";
            return;
        }
        else{
            std::cout << "Creating a fake step for follower...";
            danceFloor->followerFakeStep();
            std::cout << "done\n";
            
        }
    }   else if( event.getChar() == 'b')
    {
        if(danceFloor->parejaCount() < 1)
        {
            std::cout << "Cannot create a fake busy/sparse because there are no dancers on the floor\n";
            return;
        }
        else{
            std::cout << "In fake busy/sparse for follower. Please choose a value 1-3\n";
            fakeBSFollowerMode = true;
        }
    }
    else if( event.getChar() == 'n')
    {
        if(danceFloor->parejaCount() < 1)
        {
            std::cout << "Cannot create a fake busy/sparse because there are no dancers on the floor\n";
            return;
        }
        else{
            std::cout << "In fake busy/sparse for leader. Please choose a value 1-3\n";
            fakeBSLeaderMode = true;
        }
    }
    else if( event.getChar() == 'm')
    {
        if(danceFloor->parejaCount() < 1)
        {
            std::cout << "Cannot create an accomp. fake busy/sparse because there are not enouh dancers on the floor\n";
            return;
        }
        else{
            std::cout << "In fake busy/sparse for leader. Please choose a value 1-3\n";
            fakeBSAccompMode = true;
        }
    }
    else if( event.getChar()=='p' )
    {
        fs::path openPath = getOpenFilePath();
        mPlayOSC = new InteractiveTango::PlayOSC(openPath.c_str(), SELF_IPADDR, OSC_LISTENING_PORT);
    }
}

void MachineTango::stopStartOSCToAbleton()
{
    shouldSendOSC = !shouldSendOSC;
    if (shouldSendOSC) std::cout << "Sending OSC\n";
    else
    {
        restartSong();
        std::cout << "Stopped sending OSC\n";
    }
    std::flush(std::cout);
}

void MachineTango::startOSCToAbleton()
{
    shouldSendOSC = true;
    std::cout << "Sending OSC & Midi\n";
    danceFloor->startMidi();
    std::flush(std::cout);
}

void MachineTango::stopOSCToAbleton()
{
    shouldSendOSC = false;
    std::cout << "Stopped sending OSC & MIDI\n";
    danceFloor->stopMidi();
    restartSong();
    std::flush(std::cout);
}

void MachineTango::restartSong()
{
    //restart the song!
    danceFloor->restartPlayer();
    std::cout << "Re-starting song\n";
    std::flush(std::cout);
}

void MachineTango::setRealTimePriorityHigh()
{
    //http://www.yonch.com/tech/82-linux-thread-priority
    int ret;
    
    // We'll operate on the currently running thread.
    pthread_t this_thread = pthread_self();
    
    // struct sched_param is used to store the scheduling priority
    struct sched_param params;
    
    // We'll set the priority to the maximum.
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);

    std::cout << "Trying to set thread realtime prio = " << params.sched_priority << std::endl;
    
    // Attempt to set thread real-time priority to the SCHED_FIFO policy
    ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
    if (ret != 0) {
        // Print the error
        std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
        return;
    }
    
    // Now verify the change in thread priority
    int policy = 0;
    ret = pthread_getschedparam(this_thread, &policy, &params);
    if (ret != 0) {
        std::cout << "Couldn't retrieve real-time scheduling paramers" << std::endl;
        return;
    }
    
    // Check the correct policy was applied
    if(policy != SCHED_FIFO) {
        std::cout << "Scheduling is NOT SCHED_FIFO!" << std::endl;
    } else {
        std::cout << "SCHED_FIFO OK" << std::endl;
    }
    
    // Print thread scheduling priority
    std::cout << "Thread priority is " << params.sched_priority << std::endl;

}


void MachineTango::update()
{
    
    setpriority(PRIO_PROCESS, 0, 1);
    
    float seconds = mTimer.getSeconds() ;
    
    //play saved OSC messages, if valid.
    if( mPlayOSC != NULL )
        mPlayOSC->update(seconds);
    
    if( playing_done){
        std::cout << "Playing current sensor file is done\n";
        return; //don't update after everything is done playing
    }

    
    //clear current sensor data (more received via OSC)
    for(int i=0; i<mSensorData.size(); i++)
        mSensorData[i]->eraseData();
    
    //handle incoming OSC messages
    handleOSC(seconds);
    
    //update UGens
    for(int i=0; i<mSensorData.size() && !playing_done; i++)
    {
        mSensorData[i]->update(seconds);
        
        //check if done playing loaded sensor
        if( !addLiveDancers )
        {
            std::cout << "LoadedSensor functionality has been disabled\n";
//            LoadedSensor *s = (LoadedSensor *) mSensorData[i];
//            playing_done = s->done();
//            if(playing_done) std::cout << "Done playing sensor file.\n";
        }
        
    }
    mBeatTimer.update(seconds);
    
    //update signal processing tree and send OSC
    updateSignalTree(seconds, pastseconds);
    
    //clear memory
    for(int i=0; i<mSensorData.size(); i++)
    {
        mSensorData[i]->cleanupBuffer();
    }
    
    pastseconds = seconds;
}

void MachineTango::handleOSC(float seconds)
{
    //handles all the incoming OSC messages
    //this includes: sensor data from Shimmer and Android, beat/tempo messages from Ableton,
    
    setpriority(PRIO_PROCESS, 0, 1);
    
    receivedBeat = false;
    bool firstTime  = true;
    
    while( mListener.hasWaitingMessages() ) {
        osc::Message message;
        mListener.getNextMessage( &message );
        
        //save all received messages here
        if(mSaveOSC!=NULL)
            mSaveOSC->add(message, seconds); //add this line to save the OSC
        
        std::string addr = message.getAddress();
        
        if( !addr.compare( ABLETON_SYNC ) )
        {
            mBeatTimer.setLastBeat(seconds, message.getArgAsInt32(0), message.getArgAsInt32(1), message.getArgAsFloat(2), message.getArgAsFloat(2) );
            receivedBeat = true;
        }
        else if( !addr.compare( ABLETON_BPM ) )
        {
            mBeatTimer.setBPM(message.getArgAsFloat(0));
        }
        else if( !addr.compare( ABLETON_BUSYSPARSE_SCALE ) )
        {
            danceFloor->changeBusySparseScale(InteractiveTango::TangoEntity::WhichDancerType(message.getArgAsInt32(0)), message.getArgAsFloat(1));
        }
        else if( ! addr.find(WII_PREFIX) )
        {
            //            std::cout << "handling Wii OSC...\n";
            if( isSendingToWekinator )
                wiiToWekinator(message);
            else if ( isChangingMovementParams )
                changeMovementParamsWithWii(message);
            else
                handleWiiOSC(message);
        }
        else if( ! addr.find(WEKINATOR_OUTPUT_PREFIX) )
        {
            handleOutputWekOSC( message, seconds );
        }
        else if( !addr.compare(IPHONE_RESTART))
        {
            restartSong();
        }
        else if( !addr.compare(IPHONE_OSCMIDI_TOGGLE))
        {
            if(message.getArgAsFloat(0) == 1.0)
                startOSCToAbleton();
            else
                stopOSCToAbleton();
        }

        else
        {
            
            if( firstTime )
            {
                firstTime = false;
                //                std::cout << seconds << std::endl;
            }
            
            //sometimes this gets phantom data -- this MUST be enabled.
            if( !addLiveDancers ) return;
            
            ShimmerData *shimmer = new ShimmerData;
            std::string dID = message.getArgAsString(0) ;
            int whichShimmer = message.getArgAsInt32(1);
            shimmer->setData( 1, message.getArgAsFloat(2) ); //set timestamp from device
            shimmer->setData( 19, seconds); //set timestamp from this program
            
            SensorData *sensor = getSensor(dID, whichShimmer);
            
            if( !addr.compare( OSC_SHIMMERDATA ) )
            {
                ci::vec4 quat;
                
                for( int i=3; i<6; i++ )
                {
                    shimmer->setData( i-1, message.getArgAsFloat(i) );
                }
                for( int i=7; i<10; i++ )
                {
                    shimmer->setData( i+4, message.getArgAsFloat(i) );
                }
                for( int i=10; i<13; i++ )
                {
                    shimmer->setData(i+4,  message.getArgAsFloat(i) );
                }
                for( int i=13; i<17; i++ )
                {
                    quat[i-13] = message.getArgAsFloat(i);
                }
                //                std::cout << message.getAddress();
                //                std::cout << ": " <<  message.getArgAsString(0) << " , " << whichShimmer << " , " ;
                //                for(int i=3; i<message.getNumArgs(); i++)
                //                {
                //                    std::cout << message.getArgAsFloat(i) << " , ";
                //                }
                //                std::cout << std::endl;
                
                
                shimmer->setQuarternion( quat[0], quat[1], quat[2], quat[3]  );
                //                std::cout << dID << "," << whichShimmer << " quat: " << quat[0] << " , " << quat[1] << " , " << quat[2]  << " , " <<  quat[3] << std::endl ;
            }
            else if( !addr.compare( OSC_ANDROID_SENSOR_DATA ) )
            {
                int whichSignal = message.getArgAsInt32(3);
                int indexStart = 0;
                if( whichSignal == TYPE_ACCEL )
                {
                    indexStart = 2;
                }
                else if( whichSignal == TYPE_GYR  )
                {
                    indexStart = 11;
                }
                else if( whichSignal == TYPE_MAG )
                {
                    indexStart = 14;
                }
                
                for(int i=0; i<3; i++)
                {
                    shimmer->setData(i+indexStart,  message.getArgAsFloat(i+4) ); // TODO: check if right
                }
            }
            else if(!addr.compare( ANDROID_ORIENTATION  ) )
            {
                float oVals[3];
                for(int i=0; i<3; i++)
                {
                    oVals[i] = message.getArgAsFloat(i+3) ;
                }
                shimmer->setOrientationMatrix(oVals[0], oVals[1], oVals[2]);
            }
            else if( !addr.compare( ANDROID_STEPDETECTION ) )
            {
                std::cout << "OMFG Detected a step!!!!!!!\n";
            }
            
            
            if( !addr.compare( OSC_SHIMMERDATA )  || !addr.compare( OSC_ANDROID_SENSOR_DATA )  )
                sensor->addShimmer( shimmer );
            
        }
    }
}

void MachineTango::updateSignalTree(float seconds, float pastseconds)
{
    //update this first
    if( shouldSendOSC )
    {
        for(int i=0; i<mUGENs.size(); i++)
        {
            mUGENs[i]->update(seconds);
        }
    }
    //update dancer entities... TODO: move this to ugens, bc can now inherit from UGen?
    for( int i=0; i<mDancers.size(); i++ )
    {
        mDancers[i]->update();
    }
    for( int i=0; i<mParejas.size(); i++ )
    {
        mParejas[i]->update();
    }
    
    //send the rest of the OSC after the dance floor (music player) is sent so music player messages have priority
    if( shouldSendOSC )
    {
        if( danceFloor != NULL )
        {
            danceFloor->update(seconds); //must be last thing updated b4 wek is sent
            sendOSCMessages(danceFloor->getOSC(), seconds); //first one to send OSC
        }
        else std::cout << "dance floor is null\n";
        
        //ok now send OSC messages for rest of signal tree
        for(int i=0; i<mUGENs.size(); i++)
        {
            sendOSCMessages(mUGENs[i]->getOSC(), seconds);
        }
    }
    
    //update sendToWekinator -- must be at the end...
    for( int i=0; i<sendOSCToWekinators.size(); i++ )
    {
        sendOSCToWekinators[i]->update();
        std::vector<osc::Message> msgs = sendOSCToWekinators[i]->getOSC();
        sendOSCInputsToWekinator(msgs, seconds, i);
        //        if(msgs.size()!=0)
        //        std::cout << "i: " << i << " sigNum: " << sendOSCToWekinators[i]->getSignalCount() << "  sent: " << msgs.size() << " seconds  "  << seconds - pastSeconds << std::endl;
    }

}

void MachineTango::sendOSCMessages( std::vector<ci::osc::Message> msgs, float seconds  )
{
    //TODO: this should be refactored into something that makes sense
    //But now it sends signals to different ports and sends continuous data at a set rate, now 15Hz
    
    if( msgs.empty() ) return;
    
//    lastSentOSC += ( seconds - pastseconds );
    
    for(int i=0; i<msgs.size(); i++){
        sender.sendMessage(msgs[i]);
//        std::cout << msgs[i].getAddress() << std::endl;
    }

    
//    ci::osc::Sender *send;
//    bool shouldSend = false;
//    std::string firstAddress = msgs[0].getAddress();
//
//    std::vector<ci::osc::Message> ornament_msgs;
//    ornament_msgs = collectMessagesforDifferentPorts(CREATE_BOLEO_ORNAMENT, &msgs, ornament_msgs);
//
//    std::vector<ci::osc::Message> circling_msgs;
//    circling_msgs = collectMessagesforDifferentPorts(CIRCLING_VOLUME, &msgs, circling_msgs);
//    circling_msgs = collectMessagesforDifferentPorts(CIRCLING_GYRO_VECTOR_LENGTH, &msgs, circling_msgs);
//    circling_msgs = collectMessagesforDifferentPorts(CIRCLING_ACCEL, &msgs, circling_msgs);
//    circling_msgs = collectMessagesforDifferentPorts(CIRCLING_GYR, &msgs, circling_msgs);
//    circling_msgs = collectMessagesforDifferentPorts(PLAY_CIRCLING_CLIP, &msgs, circling_msgs);
//
//    std::vector<ci::osc::Message> longstep_msgs;
//    longstep_msgs = collectMessagesforDifferentPorts(LONGSTEP_ACCEL, &msgs, longstep_msgs);
//    longstep_msgs = collectMessagesforDifferentPorts(LONGSTEP_START, &msgs, longstep_msgs);
//    longstep_msgs = collectMessagesforDifferentPorts(LONGSTEP_END, &msgs, longstep_msgs);
//
//    std::vector<ci::osc::Message>  back_msgs = collectMessagesforDifferentPortsBack(&msgs);
//
//    std::vector<ci::osc::Message> reverb_msgs;
//    reverb_msgs = collectMessagesforDifferentPorts(ROOM_SIZE_GROUP_SIMILARITY, &msgs, reverb_msgs);
//
//    //solution FOR NOW -- refactor later
//
//    //init
//    send = NULL;
//
//    if( !firstAddress.compare( PLAY_CLIP ) || !firstAddress.compare( PLAY_MELODY_CLIP ) )
//    {
//        send = &sender;
//        shouldSend = true;
//    }
//    else
//    {
//        if(lastSentOSC > OSC_SEND_AVG_SPEED )
//        {
//            if( !firstAddress.compare( STEPTIME_REVERBTAIL ) )
//            {
//                send = &reverbSender;
//            }
//            else if( !firstAddress.compare( POINTY_VS_SMOOTH ) )
//            {
//                send = &pVSsSender;
//            }
//            shouldSend = true;
//            lastSentOSC = 0;
//        }
//        else
//        {
//            shouldSend = true;
//            send = &sender;
//
//
//        }
//    }
//
//    if( shouldSend )
//    {
//        for(int i=0; i<msgs.size(); i++)
//        {
//            if(send != NULL)
//            {
//                send->sendMessage(msgs[i]);
//            }
//        }
//        for(int i=0; i<ornament_msgs.size(); i++)
//        {
//            ornamentSender.sendMessage(ornament_msgs[i]);
//        }
//        for(int i=0; i<circling_msgs.size(); i++)
//        {
//            circlingSender.sendMessage(circling_msgs[i]);
//        }
//        //        for(int i=0; i<longstep_msgs.size(); i++)
//        //        {
//        //            longStepSender.sendMessage(longstep_msgs[i]);
//        //        }
//        for(int i=0; i<back_msgs.size(); i++)
//        {
//            backSender.sendMessage(back_msgs[i]);
//        }
//        for(int i=0; i<reverb_msgs.size(); i++)
//        {
//            reverbSender.sendMessage(reverb_msgs[i]);
//        }
//
//    }

}

void MachineTango::sendOSCInputsToWekinator(std::vector<ci::osc::Message> msgs, float seconds, int index )
{
    if( msgs.empty() ) return;
    
    ci::osc::Sender *send = wekinatorSenders[index];
    
    for(int i=0; i<msgs.size(); i++)
    {
        send->sendMessage(msgs[i]);
        
        
        //        std::cout << "Sending.. " << msgs[i].getAddress() << "   ";
        //        for( int j = 0; j<msgs[i].getNumArgs(); j++ )
        //        {
        //            std::cout << msgs[i].getArgAsFloat(j) << ",";
        //        }
        //        std::cout << std::endl;
    }
}

void MachineTango::wiiToWekinator(osc::Message message)
{
    osc::Message wek_msg;
    bool send = false;
    
    if(isWiiButtonDown(WII_1, message))
    {
        wek_msg.setAddress(WEKINATOR_START_DTW_RECORDING);
        wek_msg.addIntArg(whichDTWWekinatorOutput);
        std::cout << "Wekinator start recording DTW\n";
        std::cout << "Current DTW Wekinator output: " << whichDTWWekinatorOutput << std::endl;
        
    std:stringstream ss; ss << selectedWekinatorSenderIndex << "," << whichDTWWekinatorOutput << ",";
        sendOSCToWekinators[selectedWekinatorSenderIndex]->setRecord(true, ss.str());
        
        send = true;
    }
    else if(isWiiButtonDown(WII_2, message))
    {
        wek_msg.setAddress(WEKINATOR_STOP_DTW_RECORDING);
        std::cout << "Wekinator stop recording DTW\n";
        sendOSCToWekinators[selectedWekinatorSenderIndex]->setRecord(false);
        
        send = true;
        
    }
    else if(isWiiButtonDown(WII_A, message))
    {
        wek_msg.setAddress(WEKINATOR_TRAIN);
        std::cout << "Wekinator train\n";
        send = true;
    }
    else if(isWiiButtonDown(WII_B, message))
    {
        wek_msg.setAddress(WEKINATOR_CANCELTRAIN);
        std::cout << "Wekinator Canceled train\n";
        send = true;
    }
    else if(isWiiButtonDown(WII_UP, message))
    {
        wek_msg.setAddress(WEKINATOR_STARTRUNNING);
        std::cout << "Wekinator start running\n";
        send = true;
    }
    else if(isWiiButtonDown(WII_DOWN, message))
    {
        wek_msg.setAddress(WEKINATOR_STOPRUNNING);
        std::cout << "Wekinator stop running\n";
        
        send = true;
    }
    else if(isWiiButtonDown(WII_HOME, message)) //restart this song
    {
        isSendingToWekinator = false;
        std::cout << "Stopped sending to Wekinator, wii back in Application mode\n";
        printNormalWiiMenu();
        
    }
    else if(isWiiButtonDown(WII_PLUS, message)) //restart this song
    {
        whichDTWWekinatorOutput++;
        std::cout << "Incremented DTW wekinator output, it is now: " << whichDTWWekinatorOutput << std::endl;
    }
    else if(isWiiButtonDown(WII_MINUS, message)) //restart this song
    {
        whichDTWWekinatorOutput--;
        std::cout << "Decremented DTW wekinator output, it is now: " << whichDTWWekinatorOutput << std::endl;
    }
    else if(isWiiButtonDown(WII_RIGHT, message)) //restart this song
    {
        selectedWekinatorSenderIndex++;
        if(selectedWekinatorSenderIndex >= wekinatorSenders.size()) selectedWekinatorSenderIndex = 0;
        std::cout << "Incremented wekinator sender index, it is now: " << selectedWekinatorSenderIndex << std::endl;
    }
    else if(isWiiButtonDown(WII_LEFT, message)) //restart this song
    {
        selectedWekinatorSenderIndex--;
        if(selectedWekinatorSenderIndex < 0) selectedWekinatorSenderIndex = 0;
        std::cout << "Decremented wekinator sender index, it is now: " << selectedWekinatorSenderIndex << std::endl;
    }
    
    if (send)
        wiiWekinatorSenders[selectedWekinatorSenderIndex]->sendMessage(wek_msg);

}

bool MachineTango::isWiiButtonDown(std::string addr, osc::Message message)
{
    //    std::cout << addr << "-" << message.getAddress() << std::endl;
    
    if(!message.getAddress().compare(addr))
    {
        return ( message.getArgAsFloat(0) == 1.0);
    }
    else return false;
}

void MachineTango::changeMovementParamsWithWii(osc::Message message)
{
    
    if(isWiiButtonDown(WII_B, message) )
    {
        isChangingMovementParams = false;
        std::cout << "Stopped changing movement params. In normal application mode\n";
        
        printNormalWiiMenu();
    }
    if(isWiiButtonDown(WII_A, message))
    {
        mParejas[selectedPareja]->incSelectedMotionParam();
        std::cout << "New selected motion param: " << mParejas[selectedPareja]->getSelectedMotionParamString() << std::endl;
    }
    else if(isWiiButtonDown(WII_HOME, message))
    {
        selectedPareja++;
        if(selectedPareja >= mParejas.size()) selectedPareja = 0;
        
        std::cout << "New selected pareja: " << selectedPareja << std::endl;
    }
    else if(isWiiButtonDown(WII_UP, message))
    {
        mParejas[selectedPareja]->increaseCurMotionParamMax();
        std::cout << mParejas[selectedPareja]->getSelectedMotionParamString() << " max value is now:" << mParejas[selectedPareja]->getMotionDataCoefficients().at(1) << std::endl;
    }
    else if(isWiiButtonDown(WII_DOWN, message))
    {
        mParejas[selectedPareja]->decreaseCurMotionParamMax();
        std::cout << mParejas[selectedPareja]->getSelectedMotionParamString() << " max value is now:" << mParejas[selectedPareja]->getMotionDataCoefficients().at(1) << std::endl;
    }
    else if(isWiiButtonDown(WII_RIGHT, message))
    {
        mParejas[selectedPareja]->increaseCurMotionParamMin();
        std::cout << mParejas[selectedPareja]->getSelectedMotionParamString() << " min value is now:" << mParejas[selectedPareja]->getMotionDataCoefficients().at(0) << std::endl;
    }
    else if(isWiiButtonDown(WII_LEFT, message))
    {
        mParejas[selectedPareja]->decreaseCurMotionParamMin();
        std::cout << mParejas[selectedPareja]->getSelectedMotionParamString() << " min value is now:" << mParejas[selectedPareja]->getMotionDataCoefficients().at(0) << std::endl;
        
    }
    else if(isWiiButtonDown(WII_1, message))
    {
        mParejas[selectedPareja]->motionParamIncValTenth();
        std::cout << "Set the increment to change by 0.1 \n";
    }
    else if(isWiiButtonDown(WII_2, message))
    {
        mParejas[selectedPareja]->motionParamIncValHundredth();
        std::cout << "Set the increment to change by 0.01 \n";
    }
    else if(isWiiButtonDown(WII_PLUS, message))
    {
        mParejas[selectedPareja]->saveCurrentMotionParams();
        std::cout << "Saved current motion params in text file\n";
    }
}
    
    //control this ITM application via wiimote
    void MachineTango::handleWiiOSC(osc::Message message)
    {
        
        if(isWiiButtonDown(WII_1, message)) //stop or start sending OSC
        {
            startOSCToAbleton();
        }
        else if(isWiiButtonDown(WII_2, message)) //restart this song
        {
            stopOSCToAbleton();
        }
        else if(isWiiButtonDown(WII_HOME, message)) //send to Wekinator
        {
            isSendingToWekinator = true;
            isChangingMovementParams = false;
            
            std::cout << "Sending to Wekinator now -- note wii state in this mode also\n";
            std::cout << "Press 'HOME' again to exit mode\n";
            printWekModeMenu();
        }
        //    else if(isWiiButtonDown(WII_B, message))
        //    {
        //        if( mParejas.size() <= 0 ){
        //            std::cout << "There are no parejas entered in... must enter a pareja to change motion params\n";
        //
        //        }
        //        else
        //        {
        //            isChangingMovementParams = true;
        //            std::cout << "Current Pareja is: " << selectedPareja << std::endl;
        //            std::cout << "Changing movement params now -- note wii state in this mode also\n";
        //            std::cout << "Press 'B' again to exit mode\n";
        //
        //            printChangingParamsMenu();
        //        }
        //    }
        else if(isWiiButtonDown(WII_A, message))
        {
            cycleThroughPlaylist();
        }
        else if(isWiiButtonDown(WII_UP, message))
        {
            addMarkerToSensorFile("START DANCE");
        }
        else if(isWiiButtonDown(WII_DOWN, message))
        {
            addMarkerToSensorFile("END DANCE");
        }
        else if(isWiiButtonDown(WII_RIGHT, message))
        {
            addMarkerToSensorFile("START MOVE");
        }
        else if(isWiiButtonDown(WII_LEFT, message))
        {
            addMarkerToSensorFile("END MOVE");
        }
    }


    void MachineTango::handleOutputWekOSC(osc::Message message, float seconds)
    {
        
        //remove '/wek' prefix + next '/'
        std::string addr = message.getAddress();
        std::string prefix = WEKINATOR_OUTPUT_PREFIX;
        addr = addr.substr( prefix.size()+1 );
        
        std::string sDancer = addr.substr(0, addr.find('/'));
        int whichDancer = std::atoi( sDancer.c_str() );
        InteractiveTango::Dancer::BodyPart whichBodyPart = InteractiveTango::Dancer::BodyPart::RightFoot; //init -- TODO: FIX WITH NONE
        
        if( addr.find( WEKINATOR_OUTPUTS ) != -1 ) return; //leave
        
        
        if(_COUT_DEBUG_)
        {
            if(whichDancer == 0)
            {
                std::cout << " Leader,";
            }
            else{
                std::cout << " Follower,";
            }
        }
        
        
        //ok, now find if right foot -- umm, so only doing right foot now
        if( addr.find(WEKINATOR_RIGHTFOOT) != -1 )
        {
            whichBodyPart = InteractiveTango::Dancer::BodyPart::RightFoot;
            if(_COUT_DEBUG_) std::cout << " right foot: ";
        }
        else if( addr.find(WEKINATOR_LEFTFOOT) != -1 )
        {
            whichBodyPart = InteractiveTango::Dancer::BodyPart::LeftFoot;
            if(_COUT_DEBUG_) std::cout << " left foot: ";
        }
        
        if( addr.find( WEKINATOR_OUTPUTS ) != -1 )
        {
            //don't anything yet......
        }
        else
        {
            InteractiveTango::TangoGestures gesture = InteractiveTango::TangoGestures::STILLNESS;
            
            if( addr.find( WEKINATOR_STILLNESS ) != -1 )
            {
                gesture = InteractiveTango::TangoGestures::STILLNESS;
                if(_COUT_DEBUG_) std::cout << "stillness\n";
            }
            else if( addr.find( WEKINATOR_CIRCLING ) != -1 )
            {
                gesture = InteractiveTango::TangoGestures::CIRCLING;
                if(_COUT_DEBUG_) std::cout << "circling\n";
                
            }
            else if( addr.find( WEKINATOR_BOLEO ) != -1 )
            {
                gesture = InteractiveTango::TangoGestures::BOLEO;
                if(_COUT_DEBUG_) std::cout << "boleo\n";
                
            }
            else if( addr.find( WEKINATOR_STEPS ) != -1 )
            {
                gesture = InteractiveTango::TangoGestures::STEPS;
                if(_COUT_DEBUG_) std::cout << "steps\n";
                
            }
            else if( addr.find( WEKINATOR_OCHOS ) != -1 )
            {
                gesture = InteractiveTango::TangoGestures::OCHOS;
                if(_COUT_DEBUG_) std::cout << "ochos\n";
            }
            
            if( whichBodyPart == InteractiveTango::Dancer::BodyPart::RightFoot  )
            {
                mDancers[whichDancer]->getRootFootGestureRecognition()->addRecognizedGesture(gesture, seconds);
            }
            else if( whichBodyPart == InteractiveTango::Dancer::BodyPart::LeftFoot )
            {
                mDancers[whichDancer]->getLeftFootGestureRecognition()->addRecognizedGesture(gesture, seconds);
            }
            
        }
    };

std::vector<ci::osc::Message> MachineTango::collectMessagesforDifferentPorts(std::string addr, std::vector<ci::osc::Message> *msgs, std::vector<ci::osc::Message> nmsgs)
{
    
    int i = 0;
    while ( i < msgs->size() )
    {
        std::vector<int> toDelete;
        //boleo messages (last ones) are getting lost -- trying to see if sending through another port helps.
        if( !(msgs->at(i).getAddress().compare(addr) ) )
        {
            nmsgs.push_back(msgs->at(i));
            msgs->erase(msgs->begin()+i);
        }
        else
        {
            i++;
        }
    }
    return nmsgs;
};

std::vector<ci::osc::Message> MachineTango::collectMessagesforDifferentPortsBack(std::vector<ci::osc::Message> *msgs)
{
    std::vector<ci::osc::Message> nmsgs;
    int i = 0;
    std::vector<int> toDelete;
    
    while ( i < msgs->size() )
    {
        //boleo messages (last ones) are getting lost -- trying to see if sending through another port helps.
        if( !(msgs->at(i).getAddress().compare(SEND_BACK)) || !(msgs->at(i).getAddress().compare(SEND_LEFTFOOT)) || !(msgs->at(i).getAddress().compare(SEND_RIGHTFOOT) ) )
        {
            nmsgs.push_back(msgs->at(i));
//            std::cout <<  "messages: " << msgs->at(i).getAddress() << std::endl;
            toDelete.push_back(i);
        }
        i++;
    }
    
    //delete from msgs -- since previous had an error, only with send-back... why? WHY? I DO NOT KNOW.
    int j = 0;
    int minus =0;
    while( j < toDelete.size() )
    {
        msgs->erase(msgs->begin() + (toDelete[j]-minus));
        minus++;
        j++;
    }
    
    return nmsgs;
};

void MachineTango::clearListeningUDPSocket()
{
 //TODO -- later add the libraries to make this work - don't know if this even changed anything.
    
    //get any waiting messages now.
//    int fd;
//    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//        perror("cannot create socket");
//        return;
//    }
//    struct sockaddr_in myaddr;
//    memset((char *)&myaddr, 0, sizeof(myaddr));
//    myaddr.sin_family = AF_INET;
//    myaddr.sin_addr.s_addr = htonl(0x7f000001);
//    myaddr.sin_port = htons(OSC_LISTENING_PORT);
//    
//    if ( ::bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0 ) {
//        perror("bind failed");
//        return ;
//    }
//    
//    ::close(fd);
};

void MachineTango::addMarkerToSensorFile(std::string marker)
{
    if( mSensorData.size() <= 0 ) return;
    
    std::cout << "This function has been depreciated and does not do a thing.\n";
    
//    float seconds = mTimer.getSeconds() ;
//
//    for(int i=0; i<mSensorData.size(); i++)
//    {
//        mSensorData[i]->markerInFile(marker, seconds);
//    }
//    std::cout << "Added marker " << marker << "in file at " << seconds << "seconds.\n";
}

void MachineTango::printWekModeMenu()
{
    std::cout <<" CURRENT WII MENU \n";
    std::cout << "Wii Button 1 -- Wekinator start recording DTW\n";
    std::cout << "Wii Button 2 -- Wekinator stop recording DTW\n";
    std::cout << "Wii Button A -- Wekinator train\n";
    std::cout << "Wii Button B -- Wekinator Canceled train\n";
    std::cout << "Wii Button Up -- Wekinator start running\n";
    std::cout << "Wii Button Down -- Wekinator stop running\n";
    std::cout << "Wii Button Home -- Stop sending to Wekinator, wii back in Application mode\n";
}
void MachineTango::printNormalWiiMenu()
{
    std::cout <<" CURRENT WII MENU \n";
    std::cout <<" Wii Button 1 -- Start/Stop sending OSC to Ableton\n";
    std::cout <<" Wii Button 2 -- Restart song\n";
    std::cout <<" Wii Button A -- Cycle through songs (change current song)\n";
    std::cout <<" Wii Button Home -- Sending to Wekinator Mode\n";
    std::cout <<" Wii Button B -- Changing Motion Params Mode\n";
    std::cout <<" Wii Button Up -- Marker in file denoting dance start\n";
    std::cout <<" Wii Button Down -- Marker in file denoting dance end\n";
    std::cout <<" Wii Button Right -- Marker in file denoting move start\n";
    std::cout <<" Wii Button Left -- Marker in file denoting move end\n";
    
    
}


void MachineTango::printChangingParamsMenu()
{
    std::cout <<" CURRENT WII MENU \n";
    std::cout << "Wii Button B -- return to Normal Application mode\n";
    std::cout << "Wii Button A -- cycle through the motion params\n";
    std::cout << "Wii Button Home -- cycle through parejas\n";
    std::cout << "Wii Button UP -- increase the MAX parameter by the selected inc. value \n";
    std::cout << "Wii Button DOWN -- decrease the MAX parameter by the selected inc. value \n";
    std::cout << "Wii Button RIGHT -- increase the MIN parameter by the selected inc. value \n";
    std::cout << "Wii Button LEFT -- decrease the MIN parameter by the selected inc. value \n";
    std::cout << "Wii Button 1 -- Set the increment to change by 0.1 \n";
    std::cout << "Wii Button 2 -- Set the increment to change by 0.01 \n";
    std::cout << "Wii Button PLUS -- Save current motion params in text file\n";
    
}

//TODO -- should kill this since will not use these songs
//InteractiveTango::Pareja::SongIDs whichSongIsPlaying;
void MachineTango::cycleThroughPlaylist()
{
    //just 2 songs... TODO: fix for MORE songs
//    
//    std::cout << "Cyling through playlist...\n";
//    if (whichSongIsPlaying == InteractiveTango::Pareja::SongIDs::POR_UNA_CABEZA)
//    {
//        playFragments();
//    }
//    else
//    {
//        playPorUnaCabeza();
//    }
    
};

SensorData * MachineTango::getSensor(string deviceID, int which)
{
    bool found = false;
    int index = 0;
    
    while( !found && index < mSensorData.size() )
    {
        found = mSensorData[index]->same( deviceID, which );
        index++;
    }
    
    if(!found)
    {
        std::cout << "deviceID:" << deviceID << "   which: " <<  which  <<  std::endl;
        
        VisualizeShimmer *visualizer = new VisualizeShimmer();
        ColorA c = generateRandomColor();
        visualizer->setColor( c );
        visualizer->setBounds(app::App::getWindowSize());
        visualizers.push_back(visualizer);
        
        SensorData *sensor = new SensorData( deviceID, which, visualizer );
        mSensorData.push_back(sensor);
        
        //add to dancer if relevant
        if( addLiveDancers )
        {
            addLiveDancer(sensor, mSensorData.size()-1);
        }
        
        
        if(!mSaveFilePath.empty()) sensor->saveToFile(mSaveFilePath);
        return sensor;
    }
    else return mSensorData[index-1];
};

ci::ColorA MachineTango::generateRandomColor()
{
    //gen random color, to ID sensors on screen
    
    float r, g, b;
    
    r = Rand::randFloat();
    g = Rand::randFloat();
    b = Rand::randFloat();
    
    return ColorA(r, g, b, 1);
}

//adds just one couple
void MachineTango::addLiveDancer(SensorData *sensor, int sid)
{
    std::string leaderName = "Brent";
    std::string followerName = "Courtney";
    std::string leader2Name = "Leader2";
    std::string follower2Name = "Follower2";
    
    if (!leaderName.compare(sensor->getDeviceID()))
    {
        switch( sensor->getWhichSensor() )
        {
            case 0:
                leader.insert(std::pair<int, int>(0, sid));
                break;
            case 1:
                leader.insert(std::pair<int, int>(1, sid));
                break;
            case 5:
                leader.insert(std::pair<int, int>(2, sid));
                break;
            default:
                break;
        }
    }
    else if ( !followerName.compare( sensor->getDeviceID() ) )
    {
        switch( sensor->getWhichSensor() )
        {
            case 0:
                follower.insert(std::pair<int, int>(0, sid));
                break;
            case 1:
                follower.insert(std::pair<int, int>(1, sid));
                break;
            case 5:
                follower.insert(std::pair<int, int>(2, sid));
                break;
            default:
                break;
        }
    }
    else if (!leader2Name.compare(sensor->getDeviceID()))
    {
        switch( sensor->getWhichSensor() )
        {
            case 0:
                leader2.insert(std::pair<int, int>(0, sid));
                break;
            case 1:
                leader2.insert(std::pair<int, int>(1, sid));
                break;
            case 5:
                leader2.insert(std::pair<int, int>(2, sid));
                break;
            default:
                break;
        }
    }
    else if ( !follower2Name.compare( sensor->getDeviceID() ) )
    {
        switch( sensor->getWhichSensor() )
        {
            case 0:
                follower2.insert(std::pair<int, int>(0, sid));
                break;
            case 1:
                follower2.insert(std::pair<int, int>(1, sid));
                break;
            case 5:
                follower2.insert(std::pair<int, int>(2, sid));
                break;
            default:
                break;
        }
    }
    
    //have all the sensors and androids connected, create dancers and pareja -- TODO: make for any # of dancers!
    addLivePareja(leader, follower, 0, 1);
    addLivePareja(leader2, follower2, 2, 3);
    
    //switch couple order if needed so that mappings stay consistent between devices
    if(danceFloor->parejaCount() == 2)
    {
        if( danceFloor->getLeaderDancerID(0) != 0 )
        {
            danceFloor->clearCouples();
            danceFloor->addPareja(mParejas[1]);
            danceFloor->addPareja(mParejas[0]);
        }
    }
}

void MachineTango::addTestDancersNoSensors()
{
    std::cout << "Adding a test couple to floor to be controlled via keyboard...\n";
    
    int wekIndexStart = sendOSCToWekinators.size();
    
    //create wekinator senders -- right legs then left legs for ea. couple... legacy TODO: fix!!!
    for( int i=wekIndexStart; i<wekIndexStart+4; i++ )
        addSendtoWekinator();
    
    
    //create fake sensors
    SensorData * leaderback = getSensor("Brent", 5);
    SensorData * leaderright = getSensor("Brent", 2);
    SensorData * leaderleft = getSensor("Brent", 1);
    
    //add the leader
    InteractiveTango::Dancer *dancer = new InteractiveTango::Dancer(&mBeatTimer, sendOSCToWekinators[wekIndexStart], sendOSCToWekinators[wekIndexStart+2]);
    dancer->setDancerType(InteractiveTango::TangoEntity::WhichDancerType::LEADER1);
    dancer->addSensorBodyPart(0, leaderleft, &mUGENs, InteractiveTango::Dancer::LeftFoot);
    dancer->addSensorBodyPart(1, leaderright, &mUGENs, InteractiveTango::Dancer::RightFoot);
    dancer->addSensorBodyPart(2, leaderback, &mUGENs, InteractiveTango::Dancer::Back);
    dancer->setDancerID( 0, &mUGENs );
    mDancers.push_back(dancer);

    std::cout << "Added Brent as dancer\n";
    
    //add the follower
    
    //create fake sensors
    SensorData * followerback = getSensor("Courtney", 5);
    SensorData * followerright = getSensor("Courtney", 2);
    SensorData * followerleft = getSensor("Courtney", 1);
    
    InteractiveTango::Dancer *dancer2 = new InteractiveTango::Dancer(&mBeatTimer, sendOSCToWekinators[wekIndexStart+1], sendOSCToWekinators[wekIndexStart+3]);
    dancer->setDancerType(InteractiveTango::TangoEntity::WhichDancerType::FOLLOWER1);
    dancer2->addSensorBodyPart(3, followerleft, &mUGENs, InteractiveTango::Dancer::LeftFoot);
    dancer2->addSensorBodyPart(4, followerright, &mUGENs, InteractiveTango::Dancer::RightFoot);
    dancer2->addSensorBodyPart(5, followerback, &mUGENs, InteractiveTango::Dancer::Back);
    dancer2->setDancerID( 1, &mUGENs );
    mDancers.push_back(dancer2);
    
    std::cout << "Added Courtney as dancer\n";
    
    //add the dancer
    InteractiveTango::Pareja *pareja = new InteractiveTango::ExperimentalPareja(mDancers[mDancers.size()-2], mDancers[mDancers.size()-1], &mUGENs, &mBeatTimer );
    mParejas.push_back(pareja);
    danceFloor->addPareja(pareja);
    std::cout << "Incluye la pareja\n";
    
    //saving all the sensor info
    //TODO: make this work again.
    //       saveAllSensors();
    
    
    danceFloor->loadGeneratedSong();

}

void MachineTango::addLivePareja(std::map<int, int> &l, std::map<int, int> &f, int lID, int fID)
{
    if(f.size()==3 && l.size() ==3)
    {
        int wekIndexStart = sendOSCToWekinators.size();
        
        //create wekinator senders -- right legs then left legs for ea. couple... legacy TODO: fix!!!
        for( int i=wekIndexStart; i<wekIndexStart+4; i++ )
            addSendtoWekinator();
        
        //add the leader
        InteractiveTango::Dancer *dancer = new InteractiveTango::Dancer(&mBeatTimer, sendOSCToWekinators[wekIndexStart], sendOSCToWekinators[wekIndexStart+2]);
        dancer->setDancerType(InteractiveTango::TangoEntity::WhichDancerType::LEADER1);
        dancer->addSensorBodyPart(l[0], mSensorData[l[0]], &mUGENs, InteractiveTango::Dancer::LeftFoot);
        dancer->addSensorBodyPart(l[1], mSensorData[l[1]], &mUGENs, InteractiveTango::Dancer::RightFoot);
        dancer->addSensorBodyPart(l[2], mSensorData[l[2]], &mUGENs, InteractiveTango::Dancer::Back);
        dancer->setDancerID( lID, &mUGENs );
        mDancers.push_back(dancer);
        
        
        //add dancer/limb info to sensor data, so that it is saved in the file
        mSensorData[l[0]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::LeftFoot);
        mSensorData[l[1]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::RightFoot);
        mSensorData[l[2]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::Back);
        
        std::cout << "Added Brent as dancer\n";
        
        //add the follower
        InteractiveTango::Dancer *dancer2 = new InteractiveTango::Dancer(&mBeatTimer, sendOSCToWekinators[wekIndexStart+1], sendOSCToWekinators[wekIndexStart+3]);
        dancer->setDancerType(InteractiveTango::TangoEntity::WhichDancerType::FOLLOWER1);
        dancer2->addSensorBodyPart(f[0], mSensorData[f[0]], &mUGENs, InteractiveTango::Dancer::LeftFoot);
        dancer2->addSensorBodyPart(f[1], mSensorData[f[1]], &mUGENs, InteractiveTango::Dancer::RightFoot);
        dancer2->addSensorBodyPart(f[2], mSensorData[f[2]], &mUGENs, InteractiveTango::Dancer::Back);
        dancer2->setDancerID( fID, &mUGENs );
        mDancers.push_back(dancer2);
        
        //add dancer/limb info to sensor data, so that it is saved in the file
        mSensorData[f[0]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::LeftFoot);
        mSensorData[f[1]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::RightFoot);
        mSensorData[f[2]]->setDancerLimb(mDancers.size(), InteractiveTango::Dancer::Back);
        
        std::cout << "Added Courtney as dancer\n";
        
        //add the dancer
        InteractiveTango::Pareja *pareja = new InteractiveTango::ExperimentalPareja(mDancers[mDancers.size()-2], mDancers[mDancers.size()-1], &mUGENs, &mBeatTimer );
        mParejas.push_back(pareja);
        danceFloor->addPareja(pareja);
        std::cout << "Added a pareja\n";
        
        //saving all the sensor info
        //TODO: make this work again.
        //       saveAllSensors();
        
        
        danceFloor->loadGeneratedSong();
        
        
        //add an extra value to show done with adding pair //TODO: FIX!!! REFACTOR
        f.insert(std::pair<int, int>(3, 0));
        l.insert(std::pair<int, int>(3, 0));
    }
    
}

//an attempt to clear the udpsocket of past OSC data --> since this APPEARS to be happening & f'in shiz up?
void MachineTango::clearResidualData()
{
    while( mListener.hasWaitingMessages() ) {
        osc::Message message;
        mListener.getNextMessage( &message );
    }
};

int MachineTango::addSendtoWekinator()
{
    
    //SWITCH!!!!!!!!!!!!!!!!
    //    osc::Sender *s = new osc::Sender();
    //    s->setup(WEK_ADDR, WEKINATOR_PORT + sendOSCToWekinators.size());
    //    wekinatorSenders.push_back(s);
    //
    //    osc::Sender *s2 = new osc::Sender();
    //    s2->setup(WEK_ADDR, WEKINATOR_PORT_DTW - sendOSCToWekinators.size());
    //    wiiWekinatorSenders.push_back(s2);
    
    osc::Sender *s = new osc::Sender();
    s->setup(WEK_ADDR, WEKINATOR_PORT + sendOSCToWekinators.size());
    wekinatorSenders.push_back(s);
    
    osc::Sender *s2 = new osc::Sender();
    s2->setup(WEK_ADDR, WEKINATOR_PORT_DTW - sendOSCToWekinators.size());
    wiiWekinatorSenders.push_back(s2);
    
    int index = sendOSCToWekinators.size();
    std::stringstream ss;
    ss << WEKINATOR_INPUTS << "/" << index;
    InteractiveTango::SendOSCToWek  *so = new InteractiveTango::SendOSCToWek( ss.str() );
    sendOSCToWekinators.push_back(so);
    
    return index;
}

void MachineTango::draw()
{
    //clear out the window with black
    if(!receivedBeat)
        gl::clear(Color(0, 0, 0));
    else
        gl::clear(Color(1, 0, 0));
    gl::color(1, 1, 0, 1);
    ci::gl::setMatrices(mCamera);
    
    // enable the depth buffer (after all, we are doing 3D)
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    // draw the grid on the floor
    drawGrid();
    
    gl::color(1,0,1,1);
    ci::gl::drawSphere(ci::vec3(0, 0, 0), 0.05f);
    
    for (int i = 0; i < visualizers.size(); i++)
    {
        //        if( shimmers[i]->isStarted() )
        //        {
        ci::gl::setMatrices(mCamera);
        visualizers[i]->draw();
        //        }
    }
    
    gl::color(1, 1, 1, 1);
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    
}

void MachineTango::drawGrid(float size, float step)
{
    //draw 3d grid on the screen
    
    gl::color(Colorf(0.2f, 0.2f, 0.2f));
    for (float i = -size; i <= size; i += step)
    {
        gl::drawLine(vec3(i, 0.0f, -size), vec3(i, 0.0f, size));
        gl::drawLine(vec3(-size, 0.0f, i), vec3(size, 0.0f, i));
    }
}

CINDER_APP( MachineTango, RendererGl,
           []( MachineTango::Settings *settings ) //note: this part is to fix the display after updating OS X 1/15/18
           {
               settings->setHighDensityDisplayEnabled( true );
               settings->setTitle("Machine Tango");
               settings->setFrameRate(FRAMERATE); //set fastest framerate
           } )
