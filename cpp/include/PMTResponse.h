#pragma once

#include <iostream>
#include <string>
#include <cmath>

#include "MTRandom.h"

// Manage:
//
//  - generation of SPE
//  - timing resolution
//
//  for R12199_02 3'' PMT for mPMT
//
// The implementations are based on: 
//  - WCSimPMTObject.cc
//  - WCSimWCDigitizer.cc

class PMTResponse
{
    public:
        PMTResponse(const int);
        virtual ~PMTResponse();
        double GetRawSPE();
        float HitTimeSmearing(float);

    private:
        MTRandom *fRand;
        static const float fqpe0[501];
        float fTResConstant;
        float fTResMinimum;
        float fSclFacTTS;
};
