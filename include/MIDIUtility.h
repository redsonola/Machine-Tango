//
//  MIDIUtility.h
//  MagneticGardel
//
//  Created by courtney on 11/7/17.
//
//

#ifndef MIDIUtility_h
#define MIDIUtility_h

#include <chrono>
#include <random>
#include <fstream>
#include <string>


//for reference
//inline MidiMessage MakeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
//{
//    return MidiMessage(MakeCommand(MessageType::NOTE_ON, channel), note, velocity);
//}

#include "MidiFile.h"

#include "modernmidi.h"
#include "port_manager.h"
#include "midi_input.h"
#include "midi_output.h"
#include "midi_message.h"
#include "midi_event.h"
#include "midi_utils.h"

namespace InteractiveTango {

#define MIDI_MESSAGE_DATA_PITCH_NOTE_ON 1
#define MIDI_MESSAGE_DATA_VEL_NOTE_ON 2
    
#define INDEX_FROM_MELODY_END_COUNTS_AS_LAST 4

    class MidiNote
    {
    public:
        MidiNote(int p = 60, int vel = 100, double t =0, double d = 0.25, bool l=false)
        {
            pitch = p;
            velocity = vel;
            tick = t;
            duration = d;
            channel = -1; //if -1 use default
            oneOftheLastMelodyNotes = l; 
        }
        
        double tick;
        double absTick;
        double duration;
        int    pitch;
        int velocity;
        double tpb; //ticks per beat
        int channel; //which midi channel
        int oneOftheLastMelodyNotes;
        
        bool operator==(MidiNote note)
        {
            return (tick == note.tick && pitch == note.pitch);
        }
    };

    
class MidiFileUtility
{

private:


    int track;
    double microsecondsPerBeat;
    double beatsPerMinute;
    double msPerTick;
    double timeSigNumerator;
    double timeSigDenom;
    double ticksPerBeat;
    
    double lastTick; //the last note, at what tick is it?
    
    std::vector<std::vector<MidiNote>> melody;
    
public:
    
    void readMidiFile(std::string midifile)
    {
        MidiFile reader;
        int status = reader.read(midifile);
        if( status == 0 )
            std::cout << "Could not open file: " << midifile << std::endl;
        track=0;
        beatsPerMinute = -1;
        reader.splitTracks(); //just in case

        for(int i=0; i<reader.getTrackCount(); i++)
        {
            track = i;
            melody.push_back(std::vector<MidiNote>());
            convertToMelody(reader);

        }
        fixTicks(reader);
        setLastMidiNotes();
        
        std::cout  << "Track size of melody: " << melody.size() << "\n";
        
        ticksPerBeat = reader.getTicksPerQuarterNote();
//        std::cout << "beatsPerMinute:" << beatsPerMinute << std::endl;
        
    };
    
    double getBPM()
    {
        return beatsPerMinute;
    }
    
    double getTicksPerBeat()
    {
        return ticksPerBeat;
    }
    
    std::vector<MidiNote> getMelody(int track)
    {
        try {
            return melody[track];
        }
        catch(...)
        {
            std::cerr << "Track " << track << " does not exist!!\n";
            std::vector<MidiNote> empty;
            return empty;
        }
    }
    
    //returns melody note that is at the checked tick, if not absolute, will look for the one that closest after rather than closest before
    //if not found, returns null
    MidiNote *getMelodyNoteAtAbsTick(int track, double tick, int indexOfLastChecked=0)
    {
        std::vector<MidiNote> mel = getMelody(track);
        int i = indexOfLastChecked;
        bool  found = false;
        MidiNote *note = NULL;
        while(!found && i < mel.size() )
        {
            found  = mel[i].absTick >= tick;
            i++;
        }
        if(found) note = &mel[i-1];
        return note;
    }
    
    std::vector<MidiNote *>getAccompNotesAtAbsTick(int track, double tickStart, double tickEnd, int indexOfLastChecked=0)
    {
        std::vector<MidiNote> accompFile  = getMelody(track); //this gets everything all the track
        std::vector<MidiNote *> accomp;
        
//        std::cout << "track: " << track << " tickStart: " << tickStart << " tickEnd: " << tickEnd << " indexOf " << indexOfLastChecked << std::endl;
        
        int i = indexOfLastChecked;
        bool found = false;
        while(i < accompFile.size() && !found )
        {
//            std::cout << accompFile[i].pitch << " " << accompFile[i].tick << " " << accompFile[i].absTick << "\n";

            try{
                found  = accompFile[i].absTick > tickEnd;
                if(!found && accompFile[i].absTick >= tickStart)
                {
                    accomp.push_back(&accompFile[i]);
                }
                i++;
            }
            catch(...)
            {
                std::cerr << "problem with getting notes\n";
                std::cout << "i: " << i << " accompFile size " << accompFile.size() << std::endl;
                exit(1);
            }
        }
        return accomp;
    }

    void convertToMelody(MidiFile& midifile) {
        midifile.absoluteTicks();
        if (track < 0 || track >= midifile.getNumTracks()) {
            cout << "Invalid track: " << track << " Maximum track is: "
            << midifile.getNumTracks() - 1 << endl;
        }
        int numEvents = midifile.getNumEvents(track);
        
        vector<int> state(128);   // for keeping track of the note states
        
        int i;
        for (i=0; i<128; i++) {
            state[i] = -1;
        }
        
        melody[track].reserve(numEvents);
        melody[track].clear();
        
        MidiNote mtemp;
        int command;
        int pitch;
        int velocity;
        
        
        for (i=0; i<numEvents; i++) {
            command =midifile[track][i][0] & 0xf0;
            MidiEvent event = midifile[track][i];
            
            if(track==0 && beatsPerMinute==-1)
            {
             beatsPerMinute = event.getTempoBPM();
            }
            else if (command == 0x90) {
                pitch = midifile[track][i][1];
                velocity = midifile[track][i][2];
                if (velocity == 0) {
                    // note off
                    goto noteoff;
                } else {
                    // note on
                    state[pitch] = midifile[track][i].tick;
                }
            } else if (command == 0x80) {
                // note off
                pitch = midifile[track][i][1];
            noteoff:
                if (state[pitch] == -1) {
                   continue;
                }
                mtemp.tick = state[pitch];
                mtemp.duration = midifile[track][i].tick - state[pitch];
                mtemp.pitch = pitch;
                mtemp.velocity = velocity;
                mtemp.tpb = midifile.getTicksPerQuarterNote();
                melody[track].push_back(mtemp);
                state[pitch] = -1;
            }
        }
    }
    double getLastTick()
    {
        return lastTick;
    }
    
    void fixTicks(MidiFile& midifile)
    {
        std::vector<std::vector<MidiNote>> tmp;
        lastTick = 0;
        for(int track=0; track<midifile.getTrackCount(); track++)
        {
            tmp.push_back(std::vector<MidiNote>());
            if(melody[track].size() > 0){
                melody[track].at(0).absTick=0;
            tmp[track].push_back(melody[track].at(0));
            for(int i=1; i<melody[track].size(); i++)
            {
                double newTicks = melody[track].at(i).tick - melody[track].at(i-1).tick;
                tmp[track].push_back(melody[track].at(i));
                tmp[track].at(i).tick = newTicks;
                tmp[track].at(i).absTick = melody[track].at(i).tick;
                if(melody[track].at(i).tick > lastTick) lastTick = melody[track].at(i).tick;
            }
            }
        }
        melody = tmp;
    }
    
    void setLastMidiNotes()
    {
        //std::vector<std::vector<MidiNote>> melody;
        for(int i=1; i<melody.size(); i++)
        {
            std::vector<MidiNote> track = melody[i];
            int sz;
            if(track.size() > INDEX_FROM_MELODY_END_COUNTS_AS_LAST)
                sz = track.size()-INDEX_FROM_MELODY_END_COUNTS_AS_LAST;
            else sz = track.size();
            for(int j=sz;j < track.size(); j++ )
            {
                track[j].oneOftheLastMelodyNotes = true;
            }
        }
    }
    
};
    
class MidiOutUtility
{
protected:
    
    mm::MidiOutput *midiout;
public:
   MidiOutUtility(std::string portname = "ITM Port 1")
 //   MidiOutUtility(std::string portname = "IAC Bus 1")
    {
        midiout = new mm::MidiOutput("midiout");
        bool success = midiout->openVirtualPort(portname);
        if(!success)
        {
            std::cout << "Failure opening named port: " << portname << std::endl;
            std::cout << "Attempting to open port 0 instead... ";
            success = midiout->openPort( 0 );
            if(success)
            {
                std::cout << "done\n";
            }
            else
            {
                std::cout << "failed. Ending attempts.\n";
                return;
            }
        }
        else std::cout << "Opened " << portname << std::endl;
    }
    
    ~MidiOutUtility()
    {
        delete midiout;
    }
    
    void send(MidiNote note, int channel=1)
    {
//        std::cout << "note: " << note.pitch << "," << channel << std::endl;
        if(note.channel > -1)
            midiout->send(mm::MakeNoteOn(note.channel, note.pitch, note.velocity));
        else
             midiout->send(mm::MakeNoteOn(channel, note.pitch, note.velocity));
    };
    
    
    mm::MidiOutput *getOut(){ return midiout; }
};
    

}

#endif /* MIDIUtility_h */
