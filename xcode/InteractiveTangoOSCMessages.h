//
//  InteractiveTangoOSCMessages.h
//  InteractiveTangoMilonga
//
//  Created by courtney on 11/9/17.
//
//

//has port information and some other constants as well as OSC addresses used accross the system

#ifndef InteractiveTangoOSCMessages_h
#define InteractiveTangoOSCMessages_h

#define SHIMMER_DATA_OSC_ADDR "/shimmerdata"
#define BUS_OSC_ADDR "/c_set"
#define SHIMMER_NUM_ARGS 19
#define SHIMMER_PORT 57121
#define SC_LANG_PORT 57120
#define G_FORCE 9.80665
#define FRAMERATE 8000

#define TYPE_ACCEL 0
#define TYPE_GYR 1
#define TYPE_MAG 2
#define ANDROID_SENSOR 5

#define SELF_IPADDR "127.0.0.1"
#define PC_IPADDR "192.168.1.7"
#define WEK_ADDR "192.168.1.5"
#define OSC_LISTENING_PORT 3000
#define PHOTO_TIMING_PORT 3001

#define REVERB_PORT 3002
#define POINTY_VS_SMOOTH_PORT 3003
#define ORNAMENT_PORT 3004
#define CIRCLING_PORT 3005
#define LONGSTEP_PORT 3006
#define BACK_PORT 3007
#define BACK_PORT_FOLLOWER 3008

//SEND CONTINUOUS DATA AT 15Hz
#define OSC_SEND_AVG_SPEED 0.06666666


#define WEKINATOR_PORT 6448
#define WEKINATOR_PORT_DTW 6667 //hack TODO: fix
#define WEKINATOR_OUTPUT_PORT 12000


//defining defaults here for now, including analysis window sizes, etc.
#define DEFAULT_FOOT_ONSET_STEP_COUNT_WINDOWSIZE 4 //in seconds
#define DEFAULT_FOOT_ONSET_STEP_COUNT_WINDOWSIZE_LONGER 45 //in seconds


//OSC MESSAGES, receiving from android & sending to ableton.
#define INTERACTIVE_TANGO_PREFIX "/InteractiveTango"
#define OSC_ANDROID_SENSOR_DATA "/InteractiveTango/AndroidData"
#define OSC_SHIMMERDATA "/InteractiveTango/ShimmerData"
#define OSC_SHIMMERDATA_TO_SC "/InteractiveTango/ShimmerData/SC"
#define SHIMMER_DATA_QUAT "/InteractiveTango/ShimmerData"
#define ANDROID_ORIENTATION "/InteractiveTango/AndroidOrient"
#define SHIMMER_CROSS_COVARIATION  "/shimmerCrossCorrelation"
#define SHIMMER_STEP_DETECTION "/shimmerStepDetection"
#define SHIMMER_STEP_DETECTION2  "/shimmerStepDetection2"
#define SHIMMER_STATIONARY "/shimmerStationary"
#define SHIMMER_MIDSWING_DETECTION "/shimmerMidSwingDetection"
#define ANDROID_STEPDETECTION "/AndroidStepDetection"
#define ABLETON_SYNC "/AbletonTiming"
#define ABLETON_BPM "/AbletonBPM"
#define SHIMMER_WINDOWED_VARIATION "/ShimmerWindowedVariation"
#define SHIMMER_WINDOWED_VARIATION_DIFF "/ShimmerWindowedVariationDiff"
#define PHOTO_SYNC "/PhotoSync"
#define ABLETON_BUSYSPARSE_SCALE "/AbletonScale"

//OSC Messages to send to ableton
#define PLAY_CLIP "/AbletonPlayClip"
#define PLAY_MELODY_CLIP "/InteractiveTango/Melody/PlayClip"

//Depreciated -- No longer used
#define STEPTIME_REVERBTAIL "/InteractiveTango/StepTimeReverbTail"
#define SEND_GYRO_LONG_STEP "/InteractiveTango/SendGyroLongStep"
#define SEND_FREEZE "/InteractiveTango/FreezeMessage"
//----end deprec.


#define POINTY_VS_SMOOTH "/InteractiveTango/PointyVsSmooth"
#define ROOM_SIZE_GROUP_SIMILARITY "/InteractiveTango/RoomSize/Group"
#define HEAVY_TO_MIDI_VEL "/InteractiveTango/HeavyLightVelocityPattern"

//for circling adornos, sending to ableton
#define CIRCLING_VOLUME "/InteractiveTango/Circling/Volume"
#define CIRCLING_GYRO_VECTOR_LENGTH "/InteractiveTango/Circling/GyroVectorLength"
#define CIRCLING_ACCEL "/InteractiveTango/Circling/Accel"
#define CIRCLING_GYR "/InteractiveTango/Circling/Gyro"
#define PLAY_CIRCLING_CLIP "/InteractiveTango/Circling/PlayClip"


//for boleo ornaments send to ableton/m4l to generate the melodies
#define CREATE_BOLEO_ORNAMENT "/InteractiveTango/BoleoOrnament"

//for long step continuous control
#define LONGSTEP_ACCEL "/InteractiveTango/LongStep/Accel"
#define LONGSTEP_START "/InteractiveTango/LongStep/Start"
#define LONGSTEP_END "/InteractiveTango/LongStep/End"
#define SEND_BACK "/InteractiveTango/BackSignal"
#define SEND_LEFTFOOT "/InteractiveTango/LeftFootSignal"
#define SEND_RIGHTFOOT "/InteractiveTango/RightFootSignal"

//for sending busy spase to max 7
#define BUSY_SPARSE_PERCUSSION "/InteractiveTango/BusySparse/Beats"
#define BUSY_SPARSE_DANCERS "/InteractiveTango/BusySparse/Dancers"
#define EXPMUSIC_HARMONY "/InteractiveTango/Experimental/Harmony"
#define EXPMUSIC_INTROFILL "/InteractiveTango/Experimental/HarmonyIntroFill"
#define EXPMUSIC_CLOSEFILL "/InteractiveTango/Experimental/HarmonyCloseFill"
#define EXPMUSIC_MELODY_INSTRUMENT "/InteractiveTango/Experimental/Melody/Instrument"
#define EXPMUSIC_ACCOMP_INSTRUMENT "/InteractiveTango/Experimental/Accompaniment/Instrument"
#define EXPMUSIC_SECTION "/InteractiveTango/Experimental/Section"
#define EXPMUSIC_FOOT_ONSETS "/InteractiveTango/Experimental/FootOnset"


//Wekinator messages to send
#define WEKINATOR_RECORD "/wekinator/control/startRecording"
#define WEKINATOR_STOP_RECORD "/wekinator/control/stopRecording"
//#define WEKINATOR_OUTPUTS "/wekinator/control/outputs" // (with list of floats): Send Wekinator current output values to populate the boxes/sliders on its GUI. Attach one float per output, in order.
#define WEKINATOR_START_DTW_RECORDING "/wekinator/control/startDtwRecording" // (with 1 int) : Start recording dynamic time warping examples for the gesture type given by the int (gesture types are indexed starting from 1)
#define WEKINATOR_STOP_DTW_RECORDING "/wekinator/control/stopDtwRecording" // Stop recording dynamic time warping examples (no int required)
#define WEKINATOR_TRAIN "/wekinator/control/train" // Train on current examples
#define WEKINATOR_CANCELTRAIN "/wekinator/control/cancelTrain" // Cancel current training (if it’s in progress)
#define WEKINATOR_STARTRUNNING "/wekinator/control/startRunning" // Start running (if possible)
#define WEKINATOR_STOPRUNNING "/wekinator/control/stopRunning" // Stop running (if currently running)
#define WEKINATOR_INPUTS "/wek/inputs"

//receiving from wekinator
//                          124.052216 402.401855 1013.871826 591.93988 1981.942871 inf inf inf
//#define WEKINATOR_RIGHTFOOT "right_foot"
//#define WEKINATOR_STILLNESS "/wek/1/right_foot/stillness"
//#define WEKINATOR_CIRCLING "/wek/1/right_foot/circling"
//#define WEKINATOR_BOLEO "/wek/1/right_foot/boleo"
//#define WEKINATOR_STEPS "/wek/1/right_foot/steps"
//#define WEKINATOR_OCHOS "/wek/1/right_foot/ochos"
#define WEKINATOR_OUTPUT_PREFIX "/wek"
#define WEKINATOR_OUTPUTS "outputs"
#define WEKINATOR_RIGHTFOOT "/right_foot"
#define WEKINATOR_LEFTFOOT "/left_foot"
#define WEKINATOR_STILLNESS "/stillness"
#define WEKINATOR_CIRCLING "/circling"
#define WEKINATOR_BOLEO "/boleo"
#define WEKINATOR_STEPS "/steps"
#define WEKINATOR_OCHOS "/ochos"


//Wii messages from OSCulator
#define WII_PREFIX "/wii"
#define WII_1 "/wii/1/button/1"
#define WII_2 "/wii/1/button/2"
#define WII_A "/wii/1/button/A"
#define WII_B "/wii/1/button/B"
#define WII_DOWN "/wii/1/button/Down"
#define WII_HOME "/wii/1/button/Home"
#define WII_LEFT "/wii/1/button/Left"
#define WII_MINUS "/wii/1/button/Minus"
#define WII_PLUS "/wii/1/button/Plus"
#define WII_RIGHT "/wii/1/button/Right"
#define WII_UP "/wii/1/button/Up"

//TouchOSC messages from the iphone
#define IPHONE_RESTART "/InteractiveTango/Restart"
#define IPHONE_OSCMIDI_TOGGLE "/InteractiveTango/OscMidiToggle"




#endif /* InteractiveTangoOSCMessages_h */
