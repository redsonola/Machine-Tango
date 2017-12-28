//
//  ExperimentalMusicPlayer.h
//  MagneticGardel
//
//  Created by courtney on 11/17/17.
//
//

#ifndef ExperimentalMusicPlayer_h
#define ExperimentalMusicPlayer_h

#include "sequence_player.h"

#define MAX_DEAD_PLAYERS 50

namespace InteractiveTango
{
    
    class GeneratedMelodySection : public MainMelodySection
    {
    protected:
        MelodyGenerator *generator;
        float lastTimePlayed;
    public:
        
        GeneratedMelodySection (BeatTiming *timer, FootOnset *onset, MelodyGenerator *gen, Instruments *ins=NULL, float perWindowSize=1) : MainMelodySection(timer, onset, ins, perWindowSize)
        {
            generator = gen;
        };
        
        virtual void update(boost::shared_ptr<std::vector<int>> hsprofile, float seconds = 0)
        {
            if( fo->isStepping())
                update(seconds);
        }

        
        virtual void update(float seconds = 0)
        {
            curSeconds = seconds;
            
            //if busy/sparse = 1 then wait a bit (at least an eighth note) before re-genning
            float timeDiff = seconds - lastTimePlayed;
            float quarter = (1.0f / ((float) generator->getBPM() /  60.0f)) ;
            float eight = quarter / 2.0f;
            float sixteenth = eight / 2.0f;

            int bs = findBusySparse();
            
            if( timeDiff < sixteenth ) return; // no faster than sixteenth notes but don't place such a hard limit on density... see. 
            
            if(generator->oneToOne() && fo->isStepping() ) //wait for a foot onset to start
            {
                //ok now generate a melody
                generator->update(seconds);
            } //else implement a non-one to one mode solution
            else if( fo->isStepping())
            {
                generator->update(bs, seconds);
                lastTimePlayed = seconds; //beatTimer->getTimeInSeconds();
            }
        };
        
        
        std::vector<MidiNote> getBufferedNotes()
        {
            return generator->getCurNotes();
        }
        
         virtual std::vector<ci::osc::Message> getOSC()
        {
            //probably do nothing here... we'll see
            std::vector<ci::osc::Message> msgs = MainMelodySection::getOSC(); //this should return nothing.
            return msgs;
        }
        
        virtual void timesToRepeatSection()
        {
            song_structure.push_back(1); //only one section for now, but a 2nd section may draw upon different melodic material
        }
        
        double getTicksPerBeat()
        {
            return generator->getTicksPerBeat(0);
        }
        
    };
    
    class GeneratedAccompanmentSection : public AccompanimentSection
    {
    protected:
        ChordGeneration generator;
       std::vector<std::vector<MidiNote>> notes;
        int BEATSPERMEASURE;
        
    public:
        
        GeneratedAccompanmentSection(BeatTiming *timer, Instruments *ins) : AccompanimentSection(timer, ins)
        {
            BEATSPERMEASURE = 4;
        }
    
        //has to be updated with the harmony/section profile
        virtual void update(float seconds = 0 )
        {
            notes.clear();
            
            
            if( beatTimer->isOnBeat(0.0, seconds) ) //exactly on beat
            {
                beatsPlayed++;
            }

            if( beatsPlayed >= BEATSPERMEASURE) //TODO: fix hardcoding
            {
                beatsPlayed = 0;
                shouldStartFile = true;
                
                
                notes.push_back(generator.getNextChord());
                notes.push_back(generator.getBass());


                //switch the constant instrument
                if( phraseStart )
                {
                    phraseStart = false;
                    
                    continuingInstrumentsThroughPhrase.clear();
                    Orchestra *curOrch = curSoundFile->getOrchestration();
                    //                    assert( !curOrch->empty() );
                    for(int i=0; i<maxInstrumentsToHoldConstantThroughPhrase && i<curOrch->size(); i++)
                    {
                        continuingInstrumentsThroughPhrase.addInstrument(curOrch->getInstrViaIndex(i));
                    }
                }
                
            } else shouldStartFile = false;
        };
        
        std::vector<MidiNote> getBufferedNotes(int i=0)
        {
            return notes[i];
        }
        size_t voiceCount()
        {
            return notes.size();
        }
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            return msgs; 
        }
    };
    
    class ExperimentalMusicPlayer : public MusicPlayer, public mm::MidiSequencePlayerResponder
    {
    protected:
        MidiOutUtility midiOut; //for now have the player own it... hmmmmmmmm....
        std::vector<mm::MidiSequencePlayer *> players;
        std::vector<int> deletePlayerTag;
        double ticksPerBeat;


    public:
        ExperimentalMusicPlayer() : MusicPlayer()
        {
            main_melody = NULL;
        }
        
        void addGeneratedMelodySection(GeneratedMelodySection *section)
        {
            if(main_melody==NULL)
            {
                setMainMelody(section);
            }
            else
            {
                addCounterMelody(section);
            }
        }
        
//        virtual void update(float seconds = 0)
//        {
//            MusicPlayer::update(seconds);
//
//            //ok, send the midi now... may change tho...
//            
//            
//        }
        
        
        virtual void update(float seconds = 0)
        {
            if(main_melody == NULL) return;
            
            boost::shared_ptr<std::vector<int>> hsprofile(new std::vector<int> ); //empty one for main melody... should fix that setup
            Orchestra curMelodyOrchestration;
            
            main_melody->update(hsprofile, seconds);
            hsprofile.reset();
            hsprofile = main_melody->getHarmonySectionProfile();
            curMelodyOrchestration.addfromOtherOrchestra( main_melody->getOrchestration() );
            
            
            //use last harmony profile for melody that is not playing, etc., if not first one
            if( hsprofile.get() == NULL ) hsprofile = curHarmonyProfile;
            

            for(int i=0; i<c_melodies.size(); i++)
            {
                c_melodies[i]->update(hsprofile, seconds);
                curMelodyOrchestration.addfromOtherOrchestra( c_melodies[i]->getOrchestration() );
            }
                
            for(int i=0; i<accompaniments.size(); i++)
            {
                if( main_melody->isStartOfPhrase() )
                    accompaniments[i]->startPhrase();
                ((GeneratedAccompanmentSection *)accompaniments[i])->update(seconds);
            }
//
//          //TODO: OK FIX SOON!!!!!
//          for(int i=0; i<ornaments.size(); i++)
//          {
//             ornaments[i]->update(hsprofile, &curMelodyOrchestration, seconds);
//          }
//        }
            curHarmonyProfile.reset();
            curHarmonyProfile = hsprofile;
        
            sendMidiMessages();
            deleteDeadPlayers();

    };
        
        
        
        virtual void playerStopped(int tag)
        {
//            std::cout << "putting in queue to delete...\n";
            
            //cannot delete immediately upon this event call as will cause seg fault shit since this would be deleleting the object that is calling that function.
            deletePlayerTag.push_back(tag);

        }
        
        //so, delete these after 10 or 12
        void deleteDeadPlayers()
        {
            while (deletePlayerTag.size() > MAX_DEAD_PLAYERS)
            {
                int tag = deletePlayerTag[0];
                int i=0;
                bool found = false;
                while (!found && i<players.size())
                {
                    found = players[i]->getTag() == tag;
                    i++;
                }

                if(found)
                {
                    delete players[i-1];
                    players.erase(players.begin()+i-1);
                }
                deletePlayerTag.erase(deletePlayerTag.begin());
            }
            
            
        }
    
    
        virtual void sendNoteSeq(std::vector<MidiNote> notes, int channel )
        {
            if(notes.size() <= 0) return;
            
            mm::MidiSequencePlayer *player = new mm::MidiSequencePlayer(*midiOut.getOut());
            
            for(int i=0; i<notes.size(); i++)
            {
//                if(notes[i].tick > 0){
                    //mm::MessageType::NOTE_ON
                    //        MidiMessage(const uint8_t b1, const uint8_t b2, const uint8_t b3, const double ts = 0) : timestamp(ts) { data = {b1, b2, b3}; }

                    std::shared_ptr<mm::MidiMessage> m(mm::MakeNoteOnPtr(channel,notes[i].pitch, notes[i].velocity));
//                    std::cout << "Sending to sequencer: note:" <<notes[i].pitch << "," << notes[i].velocity << std::endl;
                    std::shared_ptr<mm::MidiPlayerEvent> ev(new mm::MidiPlayerEvent(notes[i].tick, m, 1)); //MidiPlayerEvent(double t,std::shared_ptr<MidiMessage> m, int track)
                    
                    ev->channel = channel;
                    ev->tick = notes[i].tick;
                    
                    player->addTimestampedEvent(1, time(NULL), ev);
                    
                    
//                }
//                else midiOut.send(notes[i], channel);
            }
            
//            if(notes.size() <= 1) return ;
            
            player->setBeatTiming(main_melody->getTimer());
            player->setTag(players.size());
            player->setResponder(this);
            player->setTicksPerBeat(ticksPerBeat);
            player->start(); //start it
            players.push_back(player);
        }
        
        
        //OK this is bogus and ridic refactor ASAP
        virtual void sendMidiMessages()
        {
            //collect all of the current midi notes
            std::vector<MidiNote> notes;
            if(main_melody != NULL)
            {
                notes = ((GeneratedMelodySection *) main_melody)->getBufferedNotes();
            }
            ticksPerBeat = ((GeneratedMelodySection *) main_melody)->getTicksPerBeat();
            
            //now send them
            sendNoteSeq(notes, 1);
            
//            std::cout << "Sending Follower notes:" << notes.size();

            
            notes.clear();
            
            for(int i=0; i<c_melodies.size(); i++)
            {
                notes = ((GeneratedMelodySection *) c_melodies[i])->getBufferedNotes();
                
                sendNoteSeq(notes, 2);
//                std::cout << "Sending Leader notes:" << notes.size();
                
                notes.clear();
            }
            
            for(int i=0; i<accompaniments.size(); i++)
            {
                for(int j=0; j<((GeneratedAccompanmentSection *) accompaniments[i])->voiceCount(); j++){
                notes = ((GeneratedAccompanmentSection *) accompaniments[i])->getBufferedNotes(j);
                    sendNoteSeq(notes, 3+j); //TODO: change for multiple accompaniment sections, altho not relevant
                }
            }
        }
        

        virtual std::vector<ci::osc::Message> getOSC()
        {
            //probably do nothing here... we'll see
            std::vector<ci::osc::Message> msgs = MusicPlayer::getOSC(); //this should return nothing.
            return msgs;
        }
        
 
    

    };
}

#endif /* ExperimentalMusicPlayer_h */
