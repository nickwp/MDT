#include "MDTManager.h"
#include "MTRandom.h"

MDTManager::MDTManager(int seed)
{
    MTRandom *rndm = new MTRandom( seed );
      
    fTrigAlgo = new TriggerAlgo();
    fTrigInfo = new TriggerInfo();
    fPHC = new HitTubeCollection();
    fDark = new PMTNoise( rndm->Integer(1000000) );
    fDgtzr = new HitDigitizer( rndm->Integer(1000000) );

    delete rndm; rndm = NULL;
}

MDTManager::~MDTManager()
{
    if( fTrigAlgo ){ delete fTrigAlgo; fTrigAlgo = NULL; }
    if( fTrigInfo ){ delete fTrigInfo; fTrigInfo = NULL; }
    if( fPHC ){ delete fPHC; fPHC = NULL; }
    if( fDark ){ delete fDark; fDark = NULL; }
    if( fDgtzr ){ delete fDgtzr; fDgtzr = NULL; }
}

void MDTManager::DoAddDark()
{
    fDark->AddPhotoElectrons(fPHC);
}

void MDTManager::DoDigitize()
{
    fDgtzr->Digitize(fPHC);
}

void MDTManager::DoTrigger()
{
    fTrigAlgo->NDigits(fPHC, fTrigInfo);
}

void MDTManager::DoInitialize()
{
    fTrigInfo->Clear();
    fPHC->Clear();
}
