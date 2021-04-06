#include "HitDigitizer.h"
#include "Configuration.h"
        
HitDigitizer::HitDigitizer(int seed) :
fPrecisionCharge(0.), 
fPrecisionTiming(0.1),
fEfficiency( 0.985 ),
fIntegWindow( 200. )
{
    fRand = new MTRandom( seed );
    fPMT =new PMTResponse( fRand->Integer(10000000) );
    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>("DigiHitIntegrationWindow", fIntegWindow);
    Conf->GetValue<float>("PrecisionTiming", fPrecisionTiming);
    Conf->GetValue<float>("PrecisionCharge", fPrecisionCharge);
}

HitDigitizer::~HitDigitizer()
{
    if( fPMT ){ delete fPMT; fPMT = NULL; } 
    if( fRand ){ delete fRand; fRand = NULL; }
}

void HitDigitizer::Digitize(HitTubeCollection *hc)
{
    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        this->DigitizeTube( &(*hc)() );
    }
}

// Based on WCSimWCDigitizerSKI::DigitizeHits in WCSimWCDigitizer.cc
void HitDigitizer::DigitizeTube(HitTube *aHT)
{
    const int NPE = aHT->GetNRawPE();
    vector<float> times = aHT->GetAllTimeRaw();
    vector<float> rawspe(NPE, 0.);
    std::sort(times.begin(), times.end());

    // Taken from WCSimWCDigitizerSKI::DigitizeHits
    double sumSPE = 0.;
    double tSmeared = -9999.;
    bool isAlive = false;
    double digiT = 0.;
    double digiQ = 0.;

    double intgr_srt = times[0];
    double intgr_end = intgr_srt+fIntegWindow;
    int nDigi = 0;

    for(int iPE=0; iPE<NPE; iPE++)
    {
        if( times[iPE]>=intgr_srt && times[iPE]<intgr_end )
        {
            sumSPE += fPMT->GetRawSPE();
        }
        else
        {
            digiT = intgr_srt;
            digiQ = sumSPE;
            this->ApplyThreshold(digiQ, isAlive);
            if( isAlive && nDigi==0 )
            {
                tSmeared = fPMT->HitTimeSmearing(digiQ);
                digiQ *= fEfficiency;
                digiT += tSmeared;
                digiQ = this->DoTruncate(digiQ, fPrecisionCharge);
                digiT = this->DoTruncate(digiT, fPrecisionTiming);
                aHT->AddDigiHit(digiT, digiQ);
                nDigi += 1;
            }
            sumSPE = 0.;
            intgr_srt = times[iPE];
            intgr_end = intgr_srt+fIntegWindow;
            sumSPE += fPMT->GetRawSPE();
        }
    }

    digiT = intgr_srt;
    digiQ = sumSPE;
    tSmeared = fPMT->HitTimeSmearing(digiQ);
    this->ApplyThreshold(digiQ, isAlive);
    if( isAlive )
    {
        digiQ *= fEfficiency;
        digiT += tSmeared ;
        digiQ = this->DoTruncate(digiQ, fPrecisionCharge);
        digiT = this->DoTruncate(digiT, fPrecisionTiming);
        aHT->AddDigiHit(digiT, digiQ);
    }
}


void HitDigitizer::ApplyThreshold(double& pe, bool& pass)
{
// Apply DAQ efficiency
// Taken from WCSimWCDigitizerSKI::Threshold 
// in WCSimWCDigitizer.hh
//
// NOTE: the input charge, pe, will be altered in this function
    pass=true;
    double x=pe+0.1;
    double thr=0.;
    if( x<1.1 )
    {
      thr = std::min(1.0,
		     -0.06374+x*(3.748+x*(-63.23+x*(452.0+x*(-1449.0+x*(2513.0
									+x*(-2529.+x*(1472.0+x*(-452.2+x*(51.34+x*2.370))))))))));
    } 
    else 
    {
      thr = 1.0;
    }

    if( thr<fRand->Rndm() )
    {
        pe=0.;
        pass=false;
    }
    else
    {
        pe += fRand->Gaus(0., 0.03);
    }
}

double HitDigitizer::DoTruncate(const double x, const double precision)
{
// Based on WCSimWCDigitizerBase::AddNewDigit in WCSimWCDigitizer.hh
// digitised hit information does not have infinite precision
// so need to round the charge and time information
//
// The following is based on WCSimWCDigitizerBas::Truncate
// in WCSimWCDigitizer.hh
    if(precision < 1E-10){ return x; }
    return precision * (int)(x/precision); 
}
