// This class manages informaion about both true and digitized hits 
// This class assumed to be instanceated for each PMT
// The following information is stored:
//
//  - true hits (just photoelectrons)
//      - number of true hits
//      - true hit time 
//      - parent charged particle ID 
//          - negative value for noise 
//
//  - digitized hits
//      - number of digitized hits
//      - digitized hit time
//      - digitized charge (p.e)
//      - composition of true hits

#pragma once

#include <vector>

using std::vector;

class HitTube
{
    public: 
        HitTube(int);
        virtual ~HitTube();
        void SetTubeID(const int i){ fTubeID = i; }
        int GetTubeID() const { return fTubeID; }

        void AddRawPE(const float t)
            { 
                fTimeRaw.push_back( t ); 
                fNRawPE+=1; 
            }
        void AddDigiHit(const float t, const float q)
            {
                fTimeDigi.push_back( t );
                fChargeDigi.push_back( q );
                fNDigiHits+=1;
            }

        int GetNRawPE() const { return fNRawPE; }
        float GetTimeRaw(const int i) const { return fTimeRaw[i]; }
        const vector<float>& GetAllTimeRaw() { return fTimeRaw; }

        int GetNDigiHits() const { return fNDigiHits; }
        float GetTimeDigi(const int i) const { return fTimeDigi[i]; }
        float GetChargeDigi(const int i) const { return fChargeDigi[i]; }

        
    private:
        int fNRawPE;
        vector<float> fTimeRaw;

        int fTubeID;
        int fNDigiHits;
        vector<float> fTimeDigi;
        vector<float> fChargeDigi;
};
