#include "PMTResponse.h"
#include "Configuration.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

using std::string;
using std::ifstream;
using std::stringstream;
using std::vector;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
GenericPMTResponse::GenericPMTResponse(int seed, const string &pmtname)
{
    fSclFacTTS = 1.0;
    this->Initialize(seed, pmtname);
}

GenericPMTResponse::GenericPMTResponse()
{
    fSclFacTTS = 1.0;
}

GenericPMTResponse::~GenericPMTResponse()
{
    if( !fRand ){ delete fRand; fRand=NULL; }
}

void GenericPMTResponse::Initialize(int seed, const string &pmtname)
{
    fRand=new MTRandom(seed);
    fPMTType = pmtname;

    fTResConstant = 1.890;
    fTResMinimum = 0.32;

    map<string, string> s;
    s["TimingResConstant"] = "TimingResConstant";
    s["TimingResMinimum"] = "TimingResMinimum"; 
    s["ScalFactorTTS"] = "ScalFactorTTS";
    s["SPECDFFile"] = "SPECDFFile";

    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }

    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>(s["TimingResConstant"], fTResConstant);
    Conf->GetValue<float>(s["TimingResMinimum"], fTResMinimum);
    Conf->GetValue<float>(s["ScalFactorTTS"], fSclFacTTS);
    Conf->GetValue<string>(s["SPECDFFile"], fTxtFileSPECDF);

    this->LoadCDFOfSPE(fTxtFileSPECDF);
}


double GenericPMTResponse::GetRawSPE(const TrueHit* th, const HitTube* ht)
{
    int i;
    double random1=fRand->Rndm();
    for(i = 0; i < 501; i++){
      if( random1<=*(fqpe0+i) ){ break; }
    }
    return (double(i-50) + fRand->Rndm())/22.83;
} 

bool GenericPMTResponse::ApplyDE(const TrueHit* th, const HitTube *ht)
{
    return true;
}

//// Currently based on 8" (instead of 20")
//// But shifted to requirements (2ns TTS FWHM) for 1 pe
float GenericPMTResponse::HitTimeSmearing(float Q) 
{
    Q = (Q > 0.5) ? Q : 0.5;
    float timingResolution = 0.5*fSclFacTTS*(0.33 + sqrt(fTResConstant/Q));
    if( timingResolution<fTResMinimum ){ timingResolution = fTResMinimum; }
    return fRand->Gaus(0.0,timingResolution);
}

void GenericPMTResponse::LoadCDFOfSPE(const string &filename)
{
    ifstream ifs(filename.c_str());
    string aLine;
    vector<float> qCDF;
    while( std::getline(ifs, aLine) )
    {
        if( aLine[0] == '#' ){ continue; }
        stringstream ssline(aLine);
        string item;
        while (getline(ssline, item, ssline.widen(' ')))
        {
            qCDF.push_back( atof(item.c_str()) );
        }
    }
    ifs.close();

    const unsigned int nBin = qCDF.size();
    if( nBin!=501 )
    {
        cout<<" [ERROR] PMTResponse::LoadCDFOfSPE" <<endl;
        cout<<"  - Different format: " << filename <<endl;
        cout<<"  -> EXIT" <<endl;
        exit(-1);
    }
    for(unsigned int i=0; i<nBin; i++)
    {
        fqpe0[i] = qCDF[i];
    }
}

///////////////////////////////////////////////////////////////////

FlatAngularResponse::FlatAngularResponse(float efficiency) : fEfficiency(efficiency)
{
}

bool FlatAngularResponse::ApplyDE(const TrueHit *th, const HitTube *ht) {
    return fRand->Rndm() > fEfficiency;
}

///////////////////////////////////////////////////////////////////

LinearAngularResponse::LinearAngularResponse(float effAtCosTheta0, float effAtCosTheta1)
        : fEffAtCosTheta0(effAtCosTheta0), fEffAtCosTheta1(effAtCosTheta1)
{
}

bool LinearAngularResponse::ApplyDE(const TrueHit *th, const HitTube *ht) {
    float cosTheta = 0;
    float normPMT = 0;
    float normPhoton = 0;
    for(int i=0; i<3; i++){
        float photonDir = th->GetStartPosition(i) - ht->GetPosition(i);
        cosTheta += photonDir*ht->GetOrientation(i);
        normPMT += ht->GetOrientation(i)*ht->GetOrientation(i);
        normPhoton += photonDir*photonDir;
    }
    cosTheta /= std::sqrt(normPhoton)*std::sqrt(normPMT);
    float eff = fEffAtCosTheta0 + cosTheta * (fEffAtCosTheta1 - fEffAtCosTheta0);
    return fRand->Rndm() < eff;
}

///////////////////////////////////////////////////////////////////
const double ResponseBoxandLine20inchHQE::ksig_param[4] = {0.6314, 0.06260, 0.5711,23.96};
const double ResponseBoxandLine20inchHQE::klambda_param[2] = {0.4113, 0.07827};

ResponseBoxandLine20inchHQE::ResponseBoxandLine20inchHQE(int seed, const string &pmtname)
{
    this->Initialize(seed, pmtname);
}

ResponseBoxandLine20inchHQE::ResponseBoxandLine20inchHQE()
{
}

ResponseBoxandLine20inchHQE::~ResponseBoxandLine20inchHQE()
{
}

void ResponseBoxandLine20inchHQE::Initialize(int seed, const string &pmtname)
{
    fPMTType = pmtname;
    fRand=new MTRandom(seed);

    fhighcharge_param[0] = 2*ksig_param[0]*ksig_param[1]*ksig_param[3]*sqrt(ksig_param[3])*exp(-ksig_param[1]*ksig_param[3]);
    fhighcharge_param[1] = ksig_param[0]*((1-2*ksig_param[1]*ksig_param[3])*exp(-ksig_param[1]*ksig_param[3])+ksig_param[2]);

    map<string, string> s;
    s["ScalFactorTTS"] = "ScalFactorTTS";
    s["SPECDFFile"] = "SPECDFFile";

    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }
    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>(s["ScalFactorTTS"], fSclFacTTS);
    Conf->GetValue<string>(s["SPECDFFile"], fTxtFileSPECDF);
    this->LoadCDFOfSPE(fTxtFileSPECDF);
}


float ResponseBoxandLine20inchHQE::HitTimeSmearing(float Q) 
{
  double sigma_lowcharge = ksig_param[0]*(exp(-ksig_param[1]*Q)+ksig_param[2]);
  double sigma_highcharge = fhighcharge_param[0]/sqrt(Q) + fhighcharge_param[1];
  double sigma = sigma_lowcharge*(Q<ksig_param[3])+sigma_highcharge*(Q>ksig_param[3]);
  double lambda = klambda_param[0]+klambda_param[1]*Q;
  return fRand->Gaus(-0.2, sigma)-1./lambda*log(1-fRand->Rndm());
}

///////////////////////////////////////////////////////////////////
Response3inchR14374::Response3inchR14374(int seed, const string &pmtname)
{
    fTimeResAt1PE = 0.6; // B. Quilain, to match the TTS = 1.4ns (sigma at 0.6) measured at 1 p.e. 
    this->Initialize(seed, pmtname);
}

Response3inchR14374::Response3inchR14374()
{
    fTimeResAt1PE = 0.6; // B. Quilain, to match the TTS = 1.4ns (sigma at 0.6) measured at 1 p.e. 
}



Response3inchR14374::~Response3inchR14374()
{
}

void Response3inchR14374::Initialize(int seed, const string &pmtname)
{
    fPMTType = pmtname;
    fRand = new MTRandom(seed);

    map<string, string> s;
    s["ScalFactorTTS"] = "ScalFactorTTS";
    s["SPECDFFile"] = "SPECDFFile";
    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }
    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>(s["ScalFactorTTS"], fSclFacTTS);
    Conf->GetValue<string>(s["SPECDFFile"], fTxtFileSPECDF);
    this->LoadCDFOfSPE(fTxtFileSPECDF);
}

float Response3inchR14374::HitTimeSmearing(float Q)
{
// Use a tentative Q dependence proposed by B. Quilain,
//  - Q<0.5.: force Q to be 0.5 to avoid divergence of timing resolution
//      - This comes from some PMT types's implementation in WCSimPMTObject.cc
//  - Q<10p.e.: timing resolution follows TTS/sqrt(Q)
//  - Q>=10p.e.: no charge dependence, so use a constant value of TTS/sqrt(10p.e.)
    //Q = (Q > 0.5) ? Q : 0.5;
    //if( Q>10. ){ Q = 10.; }
    //float timingResolution = (fTimeResAt1PE/sqrt(Q))*fSclFacTTS;
    float timingResolution = 0.6*fSclFacTTS;
    return fRand->Gaus(0., timingResolution);
}
