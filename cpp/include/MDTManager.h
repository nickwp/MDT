#pragma once

#include "HitTube.h"
#include "PMTNoise.h"
#include "HitTubeCollection.h"
#include "HitDigitizer.h"
#include "TriggerInfo.h"
#include "TriggerAlgo.h"
#include "Configuration.h"

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

    private:
        TriggerAlgo *fTrigAlgo;
        TriggerInfo *fTrigInfo;
        HitDigitizer *fDgtzr;
        PMTNoise *fDark;
        HitTubeCollection *fPHC; 
};
