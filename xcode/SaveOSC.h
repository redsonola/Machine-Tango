//
//  SaveOSCToCSV.h
//
//
//  Created by courtney on 11/11/18.
//  Modified 1/8/2018 to with ITM style processing machine --  converted from 0.8.x from 0.9.x -- should think about updating
//

#ifndef SaveOSC_h
#define SaveOSC_h

#include <iostream>
#include <fstream>
#include "ReadCSV.h"

namespace InteractiveTango {
    
   class SaveOSC
    {
    protected:
        std::string filename;
    public:
        SaveOSC(std::string filename_)
        {
            filename = filename_;
        };
        
        void add(ci::osc::Message msg, float seconds)
        {
            std::ofstream oscfile;
            oscfile.open (filename, std::ios::out | std::ios::app );
            oscfile << seconds << "," << msg.getAddress() << ",";// << msg.getTypeTagAsString(); //note that the typetag string begins with a comma
            
            
            //this  is a hack bc in 0.8.x they don't save the typetag... soooo -- this is the fastest way to get this to convert from 9.x to 8.x
            for(int i=0; i<msg.getNumArgs(); i++)
            {
                ci::osc::ArgType type_ = msg.getArgType(i);
                switch (type_) {
                    case ci::osc::ArgType::TYPE_INT32:
                        oscfile <<"i";
                        break;
                    case ci::osc::ArgType::TYPE_FLOAT:
                        oscfile << "f";
                        break;
                        //                    case ci::osc::ArgType::DOUBLE:  //doesn't seem to exist in this OSC library
                        //                        oscfile << msg.getArgAsDouble(i);
                        //                        break;
                    case ci::osc::ArgType::TYPE_STRING:
                        oscfile << "s";
                        break;
                    default:
                        break;
                }
                
            }
            //end hack for rapid 8.x to 9.x conversion

            
            for(int i=0; i<msg.getNumArgs(); i++) // TODO: add more handlers for different osc types see: http://opensoundcontrol.org/spec-1_0-examples
            {
//                enum class ArgType : char { INTEGER_32 = 'i', FLOAT = 'f', DOUBLE = 'd', STRING = 's', BLOB = 'b', MIDI = 'm', TIME_TAG = 't', INTEGER_64 = 'h', BOOL_T = 'T', BOOL_F = 'F', CHAR = 'c', NULL_T = 'N', IMPULSE = 'I', NONE = NULL_T };
                
                //question --> should I update cinder 0.9?
                
                ci::osc::ArgType type_ = msg.getArgType(i);
                oscfile << "," ;
                switch (type_) {
                    case ci::osc::ArgType::TYPE_INT32:
                        oscfile << msg.getArgAsInt32(i);
                        break;
                    case ci::osc::ArgType::TYPE_FLOAT:
                        oscfile << msg.getArgAsFloat(i);
                        break;
//                    case ci::osc::ArgType::DOUBLE:
//                        oscfile << msg.getArgAsDouble(i);
//                        break;
                    case ci::osc::ArgType::TYPE_STRING:
                        oscfile << msg.getArgAsString(i);
                        break;
                    default:
                        break;
                }
            }
            oscfile << std::endl;
            oscfile.close();
        }
    };
    
    //utility class that's really a struct to just allow attaching the message with the timestamp
    class OSCMessageTimeStamp
    {
    public:
        float timeStamp;
        ci::osc::Message msg;
        
        OSCMessageTimeStamp(float stamp, ci::osc::Message m)
        {
            timeStamp = stamp;
            msg = m;
        };
    };
    
    //plays all saved OSC from session and sends it to where we are listening...
    //TODO: any kind of fastforwarding, rewinding, starting in time where the OSC starts should be coded by you
    class PlayOSC
    {
    protected:
        std::string filename;

        float brentAvg, courtneyAvg;
        float brentCurAvg, courtneyCurAvg;

        float whichSecond;
        
        ReadCSV csvFile;
        float lastTime;
        
        ci::osc::Sender  mSender;
        std::vector<OSCMessageTimeStamp> msgs;

    public:
                                                                                                   // LOCALPORT, DESTHOST, DESTPORT
        PlayOSC( std::string _fname, std::string desthost, int destport, int localport=0)  : csvFile(filename)
        {
            filename = _fname;
            lastTime = 0;
            csvFile.init(filename);
            
            //, mSender(localport, desthost, destport)
            
//            try{
//                mSender.bind();
//            }
//            catch( ci::osc::Exception &e)
//            {
//                CI_LOG_E( "PlayOSC: Error binding" << e.what() << " val: " << e.value() );
//            }
            
            //reversion to 0.8x
            mSender.setup( desthost, destport );

            brentAvg=0;
            courtneyAvg=0;
            brentCurAvg=0;
            courtneyCurAvg=0;
            whichSecond=230;
        }
        
        ~PlayOSC()
        {
            csvFile.close();
        }
        
        //creates an OSC message, assuming a format.
        ci::osc::Message createMsg(std::vector<std::string> tokens)
        {
            ci::osc::Message msg;
            msg.setAddress( tokens[1] );
//            std::cout << tokens[1] << std::endl;
            std::string tag = tokens[2];
            
            //only handles what SaveOSC writes
            for(int i=3; i<tokens.size(); i++)
            {
                if(tag[i-3]=='i')
                {
                    int arg = std::atoi(tokens[i].c_str());
                    msg.addIntArg(arg);
                }
                else if(tag[i-3]=='f')
                {
                    float arg = std::atof(tokens[i].c_str());
                    msg.addFloatArg(arg);
                }
                else if(tag[i-3]=='d')
                {
                    double arg = std::atof(tokens[i].c_str()); //this is a hack - fix if this bothers you
                    msg.addFloatArg(arg);
                }
                else if(tag[i-3]=='s')
                {
                    msg.addStringArg(tokens[i]);
                }
            }
            return msg;
        }
        
        //sends buffered OSC with a timestamp >= time in seconds
        void sendBufferedOSC(float seconds)
        {
            for(int i=msgs.size()-1; i>=0; i--) //TODO: make more efficient by using while loop
            {
//                std::cout << msgs[i].timeStamp ;
//                if(msgs[i].timeStamp >= seconds)
//                {
                if(msgs[i].msg.getAddress().find("ShimmerData")!=-1 ){//&& msgs[i].timeStamp >= 200.0 && msgs[i].timeStamp <= 400.0 ){
                        int which = 0;
                        if(msgs[i].msg.getArgAsString(0).find("Brent")==-1) which = 1;
                    
                        if(which==0) brentCurAvg++;
                        else courtneyCurAvg++;
                    
//                        std::cout << msgs[i].msg.getAddress() << "," << msgs[i].timeStamp << "," << which <<"\n";
                        whichSecond = seconds;

                }
                
//                    mSender.send(msgs[i].msg);
                    mSender.sendMessage(msgs[i].msg); //change to 0.8.x
                    msgs.erase(msgs.begin() + i);
//                }
//                else std::cout << "not sending...\n";
            }
//            std::cout << "Courtney Average:" << courtneyCurAvg / whichSecond <<"\n";
//            std::cout << "Brent Average:" << brentCurAvg / whichSecond <<"\n";

        }
        
        //reads through file, loads OSC messages, and sends the OSC according to the input time in seconds.
        void update(float seconds)
        {
//            std::cout << "updating:" << lastTime << "," << csvFile.eof() << "\n";

            //1. read file, update buffer
            while(!csvFile.eof() && lastTime <= seconds)
            {
//                std::cout << "in here\n";
                std::vector<std::string> tokens = csvFile.getTokensInLine();
                
                if(tokens.size() >=3 )
                {
                    //create OSC message & send when ready
                    lastTime = std::atof(tokens[0].c_str());
                    ci::osc::Message msg = createMsg(tokens);
                
                    msgs.push_back(OSCMessageTimeStamp(lastTime, msg));
                }
            }
            
            //2. send buffered OSC messages
            sendBufferedOSC(seconds);
        }
        
    };
};


#endif /* SaveOSCToCSV_h */
