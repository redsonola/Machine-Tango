//
//  ExperimentalMusicDancers.h
//  MagneticGardel
//
//  Created by courtney on 11/17/17.
//  This class is for performing an experimental music piece I am performing. Organizes sensors into dancers, dancers into couples, into a dance floor
//  THe music player is created and connections are made here to song structures, etc.
//

#ifndef ExperimentalMusicDancers_h
#define ExperimentalMusicDancers_h




namespace InteractiveTango
{
    class ExperimentalPareja : public Pareja
    {
    public:
        ExperimentalPareja(Dancer *l, Dancer *f, std::vector<UGEN * > *ugens, BeatTiming *timer) :  Pareja(l,f,ugens, timer){
            createEXPSchemas(ugens, timer);
        }
        BusyVsSparseEvent *getCoupleBS() { return bsCouple; };

    protected:
        BusyVsSparseEvent *bsCouple; //for now...
        
        virtual void createEXPSchemas(std::vector<UGEN * > *ugens, BeatTiming *timer) //TODO set default min and maxes in the signal analysis classes
        {
//            Pareja::createSchemas(ugens, timer);
            
            std::cout << "setting up experimental pareja\n";

            //send feet sensors in real time
            SendSignal *sigs[] = {
                new SendSignal( (Filter *) getLeader()->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), getLeader()->getDancerID(), SEND_LEFTFOOT, 1  ),
                new SendSignal( (Filter *) getFollower()->getLeftFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), getFollower()->getDancerID(), SEND_LEFTFOOT, 1 ),
                new SendSignal( (Filter *) getLeader()->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), getLeader()->getDancerID(), SEND_RIGHTFOOT, 1  ),
                new SendSignal( (Filter *) getFollower()->getRightFoot()->at(Dancer::WhichSignals::LPFILTER15Hz), getFollower()->getDancerID(),  SEND_RIGHTFOOT, 1 ) };
            
            std::cout << "leader dancerid: " << getLeader()->getDancerID() << std::endl;
            
            for(int i=0; i<4; i++)
            {
                ugens->push_back(sigs[i]);
            }
            
            //since we use this for accompaniment
            bsCouple = new BusyVsSparseEvent(timer, mMotionAnalysisEvents[COUPLE_WINDOWED_VAR], mMotionAnalysisEvents[COUPLE_STEPNUM],  mMotionAnalysisEvents[CROSSCOVAR] );
            bsCouple->setName("Couple Busy Sparse");
            bsCouple->setMinMaxMood(1, 20); //more gradiations
            
            ugens->push_back(bsCouple);
        }
        

    };
    
    
    
    class ExperimentalDanceFloor : public DanceFloor
    {
    protected:
        std::vector<MelodyGenerator  *> generators;
        std::vector<MelodyGenerator *> followermel, leadermel;

        MelodyGenerator *follower_gen, *leader_gen;
        MelodyGenerator *follower_gen2, *leader_gen2, *leader_gen3, *follower_gen3;

        FactorOracle leaderfo, followerfo;
        FactorOracle leaderfo2, followerfo2;
        FactorOracle leaderfo3, followerfo3;

        MelodySection *leaderMelody;
        AccompanimentSection *accompSection;
    public:
        ExperimentalDanceFloor(BeatTiming *timer) : DanceFloor(timer)
        {
            if(player != NULL) delete player;
            player = new ExperimentalMusicPlayer();
            
            //TODO - make those numbers global variables for busy sparse melody range.
            follower_gen = new MelodyGenerator(1, 5);
            leader_gen =  new MelodyGenerator(1, 5);
            
            follower_gen2 = new MelodyGenerator(1, 5);
            leader_gen2 =  new MelodyGenerator(1, 5);
            
            follower_gen3 = new MelodyGenerator(1, 5);
            leader_gen3 =  new MelodyGenerator(1, 5);
        };
        
        virtual void loadCourtneyTangoSongNumberOneOrnaments()
        {
            //just in case -- TODO: refactor so that this is not necessary
            std::cout << "Cannot load Fragments ornaments -- Not relevant for this dance floor!\n" ;
        };
        
        void leaderFakeStep()
        {
            couples[0]->getLeader()->getOnsets()->createFakeStep();
        }
        
        void followerFakeStep()
        {
            couples[0]->getFollower()->getOnsets()->createFakeStep();
        }
        
        //start song from the beginning
        virtual void restartPlayer()
        {
            if(player==NULL)
                std::cout << "Song cannot reset as music player does not exist.\n";
            else
            {
                std::cout << "Song restarted.\n";
                ((ExperimentalMusicPlayer *)player)->resetSong();
            }
        }
        
        virtual void startMidi()
        {
            if(player==NULL)
                std::cout << "Song cannot start midi as music player does not exist.\n";
            else
            {
                std::cout << "Song sending midi.\n";
                ((ExperimentalMusicPlayer *)player)->startMidi();
            }
            
            
        }
        
        virtual void stopMidi()
        {
            if(player==NULL)
                std::cout << "Song cannot stop midi as music player does not exist.\n";
            else
            {
                std::cout << "Song stopping midi.\n";
                ((ExperimentalMusicPlayer *)player)->stopMidi();
            }
            
            
        }
        
        void followerFakeBusySparse(double m)
        {
            ((PerceptualEvent *) couples[0]->getMappingSchema(BUSY_SPARSE_FOLLOWER))->setFakeMood(m);
        }
        
        void leaderFakeBusySparse(double m)
        {
            ((PerceptualEvent *) couples[0]->getMappingSchema(BUSY_SPARSE_LEADER))->setFakeMood(m);
        }
        
        void accompFakeBusySparse(double m)
        {
            //convert to 1-20 -- hacky I know should fix -- numbers are from scaleBVSFrom20to5() in class ExperimentalAccompanimentSection
            if(m==2) m = 8;
            else if(m==3) m = 13;
            else if(m==4) m= 16;
            else m=20;
            
            ((PerceptualEvent *) ((ExperimentalPareja *)couples[0])->getCoupleBS())->setFakeMood(m);
        }
        
        void loadSong(BeatTiming *timer)
        {
            mMappingSchemas[MELODY_BS]->setName("Follower Busy Sparse");
            ( ( PerceptualEvent * ) couples[0]->getMappingSchema(BUSY_SPARSE_FOLLOWER) )->setMinMaxMood(1, 20); //more gradiations

            melody->addSchema( mMappingSchemas[MELODY_BS] );  //not using to choose melodies at the MOMENT //choosing again 8/2014
            melody->addSchema( mMappingSchemas[MELODY_INSTRUMENT_PR] );


            //leader once controlled only the harmonyr
            //                accompaniment->addSchema( mMappingSchemas[THICK_THIN] ); //7-29 deleting for now... busy/spare more accurate... I think... look at later...
            couples[0]->getMappingSchema(BUSY_SPARSE_LEADER)->setName("Leader Busy Sparse");
            leaderMelody->addSchema( couples[0]->getMappingSchema(BUSY_SPARSE_LEADER) );
            leaderMelody->addSchema( couples[0]->getMappingSchema(POINTY_ROUNDED_CONT_LEADER));
            ( ( PerceptualEvent * ) couples[0]->getMappingSchema(BUSY_SPARSE_LEADER) )->setMinMaxMood(1, 20); //more gradiations

            
            ((ExperimentalMusicPlayer *) player)->addGeneratedMelodySection( (GeneratedMelodySection *) melody );
            ((ExperimentalMusicPlayer *) player)->addGeneratedMelodySection( (GeneratedMelodySection *) leaderMelody );
            player->addAccompaniment(accompaniment);
            
//            
//            follower_gen.turnOn1to1();
//            leader_gen.turnOn1to1();

            //player->addAccompaniment( accompaniment ); // not yet implemented
        };
        
        virtual void loadCourtneyTangoSongNumberOneIntoPlayer()
        {
            //just in case -- TODO: refactor so that this is not necessary
            std::cout << "Cannot load Fragments song -- Not relevant for this dance floor!\n" ;
        };
        
        
        void loadGeneratedSong()
        {
            destroyCurSong();
            if( couples.size() <= 0 )
            {
                std::cout << "Warning! No couples on dance floor. Cannot load song! ";
            }
            
            //create new melody generator section -- TODO: REFACTOR!!!!!!!!
            leaderfo.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.v3.mid", 2);
            leader_gen->addGeneratorAlgorithm(&leaderfo);
            leader_gen->turnOn1to1();
            generators.push_back(leader_gen);
            
            followerfo.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.v3.mid", 1);
            follower_gen->addGeneratorAlgorithm(&followerfo);
            generators.push_back(follower_gen);
            
            leaderfo3.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango1.2.mid", 2);
            leader_gen3->addGeneratorAlgorithm(&leaderfo3);
            leader_gen3->turnOn1to1();
            generators.push_back(leader_gen3);
            
            followerfo3.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango1.2.mid", 1);
            follower_gen3->addGeneratorAlgorithm(&followerfo3);
            generators.push_back(follower_gen3);
            
            //create new melody generator section
            leaderfo2.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.Amaj.mid", 2);
            leader_gen2->addGeneratorAlgorithm(&leaderfo2);
            leader_gen2->turnOn1to1();
            generators.push_back(leader_gen2);
            
            followerfo2.train("/Users/courtney/Documents/Interactive Tango Milonga/EMTango_Melody/emtango.Amaj.mid", 1);
            follower_gen2->addGeneratorAlgorithm(&followerfo2);
            generators.push_back(follower_gen2);
            
            followermel.push_back(follower_gen);
            followermel.push_back(follower_gen3);
            followermel.push_back(follower_gen2);

            
            leadermel.push_back(leader_gen);
            leadermel.push_back(leader_gen3);
            leadermel.push_back(leader_gen2);

            
            melody = new GeneratedMelodySection( mTimer, melodyOnsetDancer->getOnsets(), followermel, couples[0]->getFollower()->getDancerID(), &instruments );
            ( ( GeneratedMelodySection * )melody )->setCoupleBS(  ( (ExperimentalPareja  *) couples[0] )->getCoupleBS() );
            std::vector<int> instrumentsForFollower = {1, 3, 2, 4, 2, 1};

            ((GeneratedMelodySection *)melody)->setExpInstrumentsforSections(instrumentsForFollower);
            
            leaderMelody = new GeneratedMelodySection( mTimer, couples[0]->getLeader()->getOnsets(), leadermel, couples[0]->getLeader()->getDancerID(), &instruments ); //added leader...
            ( ( GeneratedMelodySection * )leaderMelody )->setMelodySectionDecider(  (GeneratedMelodySection *) melody );
            std::vector<int> instrumentsForLeader = {2, 3, 3, 4, 2, 4};
            ((GeneratedMelodySection *)leaderMelody)->setExpInstrumentsforSections(instrumentsForLeader);
            
            accompaniment = new GeneratedAccompanmentSection( mTimer, &instruments );
            accompaniment->addSchema( ((ExperimentalPareja *) couples[0])->getCoupleBS() );
            ( ( GeneratedAccompanmentSection * )accompaniment )->setMelodySectionDecider(  (GeneratedMelodySection *) melody );

            
            for( int i=0; i<couples.size(); i++ )
                couples[i]->setMotionAnalysisParamsForSong(TangoEntity::SongIDs::FRAGMENTS); //just use settings for fragments for now
            
            loadSong(mTimer);
            selectedSong = TangoEntity::SongIDs::GENERATED_PIAZZOLLA_TEST;
            std::cout << "Loaded Generated Melody song\n";
        };
        
        void loadPorUnaCabezaIntoPlayer()
        {
            //just in case -- TODO: refactor so that this is not necessary
            std::cout << "Cannot load Por Una Cabeza song -- Not relevant for this dance floor!\n" ;
        }
        
    };

        
};

#endif /* ExperimentalMusicDancers_h */
