#pragma once

#include <string>
#include "MTRandom.h"
#include "HitTube.h"

using std::string;

// Manage:
//
//  - generation of SPE
//  - timing resolution
//
class PMTResponse
{
    public:
        virtual double GetRawSPE(const TrueHit* th=NULL, const HitTube* ht=NULL) = 0; 
        virtual float HitTimeSmearing(float) = 0;
        virtual void Initialize(int, const string &s="") = 0;
        virtual bool ApplyDE(const TrueHit* th=NULL, const HitTube* ht=NULL) = 0;

    protected:
        MTRandom *fRand;
        string fPMTType;
        float fSclFacTTS;
};

//  Based on the implementation of R12199_02 3'' PMT for mPMT in WCSim
//   - WCSimPMTObject.cc
class GenericPMTResponse : virtual public PMTResponse
{
    public:
        GenericPMTResponse(const int, const string &s="");
        GenericPMTResponse();
        virtual ~GenericPMTResponse();
        void Initialize(int, const string &s="");
        virtual double GetRawSPE(const TrueHit* th=NULL, const HitTube* ht=NULL);
        virtual float HitTimeSmearing(float);
        virtual bool ApplyDE(const TrueHit* th=NULL, const HitTube* ht=NULL);

    protected:
        void LoadCDFOfSPE(const string &s);
        float fqpe0[501];
        string fTxtFileSPECDF;

    private:
        float fTResConstant;
        float fTResMinimum;
};

class FlatAngularResponse : virtual public GenericPMTResponse
{
    public:
        FlatAngularResponse(float efficiency = 1.0);
        bool ApplyDE(const TrueHit* th=NULL, const HitTube* ht=NULL);
    
    private:
        float fEfficiency;
};

class LinearAngularResponse : virtual public GenericPMTResponse
{
    public:
        LinearAngularResponse(float effAtCosTheta0 = 1.0, float effAtCosTheta1 = 1.0);
        bool ApplyDE(const TrueHit* th=NULL, const HitTube* ht=NULL);
    
    private:
        float fEffAtCosTheta0;
        float fEffAtCosTheta1;
};

class ResponseBoxandLine20inchHQE : public virtual GenericPMTResponse
{
    public: 
        ResponseBoxandLine20inchHQE(int, const string &s="");
        ResponseBoxandLine20inchHQE();
        virtual ~ResponseBoxandLine20inchHQE();
        float HitTimeSmearing(float);
        void Initialize(int, const string &s="");

    private:
        static const double ksig_param[4];
        static const double klambda_param[2];
        double fhighcharge_param[2];
};

class ResponseBoxandLine20inchHQEFlat : public ResponseBoxandLine20inchHQE, FlatAngularResponse {
    public:
        ResponseBoxandLine20inchHQEFlat(float efficiency)
            : ResponseBoxandLine20inchHQE(), FlatAngularResponse(efficiency) {};
        
        ResponseBoxandLine20inchHQEFlat(float efficiency, int seed, const string &pmtname = "")
            : ResponseBoxandLine20inchHQE(seed, pmtname), FlatAngularResponse(efficiency) {};
};

class ResponseBoxandLine20inchHQELinear : public ResponseBoxandLine20inchHQE, LinearAngularResponse {
    public:
        ResponseBoxandLine20inchHQELinear(float effAtCosTheta0, float effAtCosTheta1)
            : ResponseBoxandLine20inchHQE(), LinearAngularResponse(effAtCosTheta0, effAtCosTheta1) {};

        ResponseBoxandLine20inchHQELinear(float effAtCosTheta0, float effAtCosTheta1, int seed, const string &pmtname="")
            : ResponseBoxandLine20inchHQE(seed, pmtname), LinearAngularResponse(effAtCosTheta0, effAtCosTheta1) {};
};

class Response3inchR14374 : virtual public GenericPMTResponse
{
    public: 
        Response3inchR14374(int, const string &s="");
        Response3inchR14374();
        virtual ~Response3inchR14374();
        float HitTimeSmearing(float);
        void Initialize(int, const string &s="");

    private:
        double fTimeResAt1PE;
};

class Response3inchR14374Flat : public Response3inchR14374, FlatAngularResponse {
    public:
        Response3inchR14374Flat(float efficiency)
            : Response3inchR14374(), FlatAngularResponse(efficiency) {};
        
        Response3inchR14374Flat(float efficiency, int seed, const string &pmtname = "")
            : Response3inchR14374(seed, pmtname), FlatAngularResponse(efficiency) {};
};

class Response3inchR14374Linear : public Response3inchR14374, LinearAngularResponse {
    public:
        Response3inchR14374Linear(float effAtCosTheta0, float effAtCosTheta1)
            : Response3inchR14374(), LinearAngularResponse(effAtCosTheta0, effAtCosTheta1) {};
        
        Response3inchR14374Linear(float effAtCosTheta0, float effAtCosTheta1, int seed, const string &pmtname="")
            : Response3inchR14374(seed, pmtname), LinearAngularResponse(effAtCosTheta0, effAtCosTheta1) {};
};