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
#include <utility>
#include <algorithm>

using std::vector;
using std::pair;

class HitTube
{
    public: 
        HitTube(int);
        virtual ~HitTube();
        void SetTubeID(const int i){ fTubeID = i; }
        int GetTubeID() const { return fTubeID; }

        void AddRawPE(const float t, const int id=9999999)
            { 
                fPhotoElectrons.push_back(pair<float, int>(t, id));
                fNRawPE+=1; 
            }
        void AddDigiHit(const float t, const float q, const vector<int> &id=vector<int>())
            {
                fTimeDigi.push_back( t );
                fChargeDigi.push_back( q );
                fParentCompDigi.push_back( id );
                fNDigiHits+=1;
            }

        int GetNRawPE() const { return fNRawPE; }
        float GetTimeRaw(const int i) const { return fPhotoElectrons[i].first; }
        void SortTrueHits()
        {
            std::sort(fPhotoElectrons.begin(),
              fPhotoElectrons.end(), 
              [](const pair<float, int> &left, const pair<float,int> &right)
              { return left.first < right.first; }
             );
        }
        const vector<pair<float,int>>& GetPhotoElectrons() const { return fPhotoElectrons; }


        int GetNDigiHits() const { return fNDigiHits; }
        float GetTimeDigi(const int i) const { return fTimeDigi[i]; }
        float GetChargeDigi(const int i) const { return fChargeDigi[i]; }
        const vector<int>& GetParentCompositionDigi(const int i) const { return fParentCompDigi[i]; }

        
    private:
        int fNRawPE;
        vector<pair<float, int>> fPhotoElectrons;

        int fTubeID;
        int fNDigiHits;
        vector<float> fTimeDigi;
        vector<float> fChargeDigi;
        vector<vector<int>> fParentCompDigi;
};
