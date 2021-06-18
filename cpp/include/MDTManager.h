#pragma once

#include "HitTube.h"
#include "PMTNoise.h"
#include "HitTubeCollection.h"
#include "HitDigitizer.h"
#include "TriggerInfo.h"
#include "TriggerAlgo.h"
#include "Configuration.h"
#include "PMTAfterpulse.h"

class MDTManager
{
    public:
        MDTManager(int seed=78923);
        virtual ~MDTManager();

        HitTubeCollection* GetHitTubeCollection() { return fPHC; }
        TriggerInfo* GetTriggerInfo() { return fTrigInfo; }
        
        void DoAddDark();
        void DoDigitize();   
        void DoTrigger();
        void DoInitialize();
        void DoAddAfterpulse();

        void SetHitTubeCollection(HitTubeCollection*);

    private:
        TriggerAlgo *fTrigAlgo;
        TriggerInfo *fTrigInfo;
        HitDigitizer *fDgtzr;
        PMTResponse *fPMTResp;
        PMTNoise *fDark;
        HitTubeCollection *fPHC; 
        PMTAfterpulse *fAftpulse;
};
