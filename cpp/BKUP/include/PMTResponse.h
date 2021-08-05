#pragma once

#include <string>
#include "MTRandom.h"

using std::string;

// Manage:
//
//  - generation of SPE
//  - timing resolution
//
class PMTResponse
{
    public:
        virtual double GetRawSPE() = 0; 
        virtual float HitTimeSmearing(float) = 0;

    protected:
        virtual void Initialize() = 0;

        MTRandom *fRand;
        string fPMTType;
        float fSclFacTTS;
};

//  Based on the implementation of R12199_02 3'' PMT for mPMT in WCSim
//   - WCSimPMTObject.cc
class GenericPMTResponse : public PMTResponse
{
    public:
        GenericPMTResponse(const int, const string &s="");
        GenericPMTResponse();
        virtual ~GenericPMTResponse();
        virtual double GetRawSPE();
        virtual float HitTimeSmearing(float);

    protected:
        void LoadCDFOfSPE(const string &s);
        float fqpe0[501];
        string fTxtFileSPECDF;

    private:
        void Initialize();
        float fTResConstant;
        float fTResMinimum;
};


class ResponseBoxandLine20inchHQE : public GenericPMTResponse
{
    public: 
        ResponseBoxandLine20inchHQE(const int, const string &s="");
        virtual ~ResponseBoxandLine20inchHQE();
        float HitTimeSmearing(float);

    private:
        void Initialize();
        static const double ksig_param[4];
        static const double klambda_param[2];
        double fhighcharge_param[2];
};

class Response3inchR14374 : public GenericPMTResponse
{
    public: 
        Response3inchR14374(const int, const string &s="");
        virtual ~Response3inchR14374();
        float HitTimeSmearing(float);

    private:
        void Initialize();
        double fTimeResAt1PE;
};
