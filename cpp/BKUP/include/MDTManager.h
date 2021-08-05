#pragma once

#include <map>

#include "HitTube.h"
#include "PMTNoise.h"
#include "HitTubeCollection.h"
#include "HitDigitizer.h"
#include "TriggerInfo.h"
#include "TriggerAlgo.h"
#include "Configuration.h"
#include "PMTAfterpulse.h"
#include "MTRandom.h"

using std::map;

class MDTManager
{
    public:
        MDTManager(int seed=78923);
        virtual ~MDTManager();

        HitTubeCollection* GetHitTubeCollection(const string &s) { return fPHC[s]; }
        TriggerInfo* GetTriggerInfo() { return fTrigInfo; }
        
        void RegisterPMTType(const string&, PMTResponse*);

        void DoInitialize();
        void DoAddDark(const string&);
        void DoDigitize(const string&);   
        void DoTrigger(const string&);
        void DoAddAfterpulse(const string&);
        
        void SetHitTubeCollection(HitTubeCollection*, const string&);
        bool HasThisPMTType(const string&);


    private:
        TriggerAlgo *fTrigAlgo;
        TriggerInfo *fTrigInfo;
        HitDigitizer *fDgtzr;
        MTRandom *fRndm;

        map<string, PMTResponse*> fPMTResp;
        map<string, PMTNoise*> fDark;
        map<string, HitTubeCollection*> fPHC; 
//        map<string, PMTAfterpulse*> fAftpulse;
};
