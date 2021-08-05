#include "MDTManager.h"

MDTManager::MDTManager(int seed)
{
    fRndm = new MTRandom( seed );
    fTrigAlgo = new TriggerAlgo();
    fTrigInfo = new TriggerInfo();
    fDgtzr = new HitDigitizer( fRndm->Integer(1000000) );

    fPMTResp.clear();
    fDark.clear();
    fPHC.clear();
}

MDTManager::~MDTManager()
{
    if( fTrigAlgo ){ delete fTrigAlgo; fTrigAlgo = NULL; }
    if( fTrigInfo ){ delete fTrigInfo; fTrigInfo = NULL; }
    if( fRndm ){ delete fRndm; fRndm = NULL; }

    map<string, PMTResponse*>::iterator iPMTResp;
    for(iPMTResp=fPMTResp.begin(); iPMTResp!=fPMTResp.end(); iPMTResp++)
    {
        std::cout<<" Deleting" << iPMTResp->first <<std::endl;
        delete iPMTResp->second; iPMTResp->second = NULL;
    }
    fPMTResp.clear();

    map<string, PMTNoise*>::iterator iDark;
    for(iDark=fDark.begin(); iDark!=fDark.end(); iDark++)
    {
        delete iDark->second; iDark->second = NULL;
    }
    fDark.clear();

    map<string, HitTubeCollection*>::iterator iPHC; 
    for(iPHC=fPHC.begin(); iPHC!=fPHC.end(); iPHC++)
    {
        delete iPHC->second; iPHC->second = NULL;
    }
    fPHC.clear();
}

void MDTManager::DoAddDark(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDark[pmtname]->AddPhotoElectrons(fPHC[pmtname]);
    }
}

void MDTManager::DoDigitize(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDgtzr->Digitize(fPHC[pmtname], fPMTResp[pmtname]);
    }
}

void MDTManager::DoTrigger(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fTrigAlgo->NDigits(fPHC[pmtname], fTrigInfo);
    }
}

void MDTManager::DoAddAfterpulse(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDark[pmtname]->AddAfterpulse(fPHC[pmtname], fDgtzr, fPMTResp[pmtname]);
    }
}

void MDTManager::DoInitialize()
{
    fTrigInfo->Clear();
    map<string, HitTubeCollection*>::iterator iPHC; 
    for(iPHC=fPHC.begin(); iPHC!=fPHC.end(); iPHC++)
    {
        iPHC->second->Clear();
    }
}

void MDTManager::SetHitTubeCollection(HitTubeCollection *hc, const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        if( fPHC[pmtname] ){ delete fPHC[pmtname]; fPHC[pmtname] = NULL; }
        fPHC[pmtname] = hc;
    }
}

void MDTManager::RegisterPMTType(const string &pmtname, PMTResponse *pmtResp)
{
    fPHC[pmtname] = new HitTubeCollection();
    fDark[pmtname] = new PMTNoise(fRndm->Integer(1000000), pmtname);
    //fPMTResp[pmtname] =new PMTResponse(fRndm->Integer(10000000), pmtname);
    //fPMTResp[pmtname] =new GenericPMTResponse(fRndm->Integer(10000000), pmtname);
    //
    fPMTResp[pmtname] = new ResponseBoxandLine20inchHQE(fRndm->Integer(10000000), pmtname);
    fPMTResp[pmtname] = 
}

bool MDTManager::HasThisPMTType(const string &pmtname)
{
    bool b = true;
    if( fPHC.count(pmtname)==0 )
    {   
        std::cout<<" [ERROR] MDTManager::HasPMTType " <<std::endl;
        std::cout<<" PMT type: " << pmtname <<" is not registered" <<endl;
        b = false;
    }
    return b;
}
