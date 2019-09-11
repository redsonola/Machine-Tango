//
//  MachineTangoDancers.h
//  Machine Tango
//
//  Created by courtney on 9/13/18.
//

#ifndef MachineTangoDancers_h
#define MachineTangoDancers_h

//send footsteps or some threshold to create sound? -- activity correlating to volume maybe?
//send staccato/legato
//for now, don't send song information -- only send dancer information
namespace InteractiveTango
{

class MachineDanceFloor : public ExperimentalDanceFloor
{
protected:
    std::vector<ci::osc::Message> msgs;
    
public:
    MachineDanceFloor(BeatTiming *timer) : ExperimentalDanceFloor(timer)
    {
        
    }
    
    virtual void update(float seconds=0)
    {
        
        if( getCurrentMapping() == NONE ) return ;

        
        //send step information
        FootOnset * follower_onsets = couples[0]->getFollower()->getOnsets();
        FootOnset * leader_onsets = couples[0]->getLeader()->getOnsets();
        
        osc::Message footOnsetMessage;
        footOnsetMessage.setAddress(EXPMUSIC_FOOT_ONSETS);
        footOnsetMessage.addIntArg(leader_onsets->isStepping());
        footOnsetMessage.addIntArg(follower_onsets->isStepping());
        msgs.push_back(footOnsetMessage);
        
        
        //send busy/sparse information
        PerceptualEvent *fbs = (PerceptualEvent *) mMappingSchemas[MELODY_BS];
        PerceptualEvent *lbs = (PerceptualEvent *) mMappingSchemas[ACCOMPANIMENT_BS];
        
        osc::Message busySparseMessageFollower;
        busySparseMessageFollower.setAddress(BUSY_SPARSE_DANCERS);
        busySparseMessageFollower.addIntArg(1);
        busySparseMessageFollower.addIntArg(fbs->getCurMood(1.0, seconds));
        busySparseMessageFollower.addFloatArg(((BusyVsSparseEvent *)fbs)->findMoodfixed()/(float)fbs->getMaxMood());
        
        osc::Message busySparseMessageLeader;
        busySparseMessageLeader.setAddress(BUSY_SPARSE_DANCERS);
        busySparseMessageLeader.addIntArg(0);
        busySparseMessageLeader.addIntArg(lbs->getCurMood(1.0, seconds));
        busySparseMessageLeader.addFloatArg(((BusyVsSparseEvent *)lbs)->findMoodfixed()/(float)lbs->getMaxMood());

        
        msgs.push_back(busySparseMessageFollower);
        msgs.push_back(busySparseMessageLeader);
    }
    
    //player must be the first thing to send OSC.
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> curMsgs = msgs;
        msgs.clear();
        if( getCurrentMapping() == NONE ) return msgs;
        
//        msgs = player->getOSC(); not using player for thiis
        
        for( int i=0; i<mMappingSchemas.size(); i++ )
        {
            std::vector<ci::osc::Message> ms = mMappingSchemas[i]->getOSC();
            for( int j=0; j<ms.size(); j++ )
            {
                curMsgs.push_back(ms[j]);
            }
        }
//        
//        for(int i=0; i<curMsgs.size(); i++)
//            std::cout << curMsgs[i].getAddress() << std::endl;
        
        return curMsgs;
    };
};

}
#endif /* MachineTangoDancers_h */
