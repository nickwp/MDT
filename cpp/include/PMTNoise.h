#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <map>

#include "MTRandom.h"
#include "HitTubeCollection.h"

using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::map;

class PMTNoise 
{
    public :
        PMTNoise(const int);
        virtual ~PMTNoise();

        void SetNumberOfPMTs(const int);
        void SetMinTubeID(const int);
        void SetMaxTubeID(const int);
        void SetWindowLow(const double /* ns */);
        void SetWindowUp(const double /* ns */);
        void SetDarkRate(const double /* kHz */);
        void SetDarkMode(const int i){ fDarkMode = i; }

        void GenerateDarkNoise(double, double, bool isDPE=false, const bool verbose=false);
        int GetNumberOfGeneratedDarkHits() const;
        int GetNumberOfGeneratedDarkPhotoElectrons() const;
        int GetNoiseTube(const int i) const { return fNoisePMT[i]; }
        double GetNoiseTime(const int i) const { return fNoiseTime[i]; }

        void AddPhotoElectrons(HitTubeCollection*);
        void Add(HitTubeCollection*, double, double);
        void FindRanges(HitTubeCollection*, vector<pair<double, double>>&);

    private :
        MTRandom *fRand;
        int fNPMTs;
        int fMinTubeID;
        int fMaxTubeID;
        float fWinLow;
        float fWinUp;
        float fDarkRate;
        float fAveNoise;
        float fConvRate;
        float fWindow;
        int fDarkMode;

        int fNnoise;
        vector<int> fNoisePMT;
        vector<double> fNoiseTime;
};
