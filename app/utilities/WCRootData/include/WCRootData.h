#ifndef WCROOTDATA_H
#define WCROOTDATA_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <utility>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"

#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "TNRooTrackerVtx.hh"

#include "MDTManager.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::map;
using std::pair;

class WCRootData
{
    public:
        WCRootData();
        virtual ~WCRootData();

        void ReadFile(const char *);
        void CloseFile();

        void CreateTree(const char*);
        virtual void FillTree();
        virtual void WriteTree();

        void GetGeometryInfo(const char*);
        void CopyTree(const char*, const char*, const vector<bool>&v=vector<bool>());
        void SetOutFileName(const char *f){ fOutFileName = TString(f); }

        int GetEntries();
        void GetEntry(int);

        WCSimRootTrigger *GetTrigger(int i) const { return fSpEvt->GetTrigger(i); }

        void AddTrueHitsToMDT(MDTManager*, float offset_time=0.);
        void AddDigiHits(MDTManager*, int event_id=0);
        void AddTracks(const WCSimRootTrigger*, float offset_time=0.);

        const char *GetCurrentInputFileName() const { return fWCSimC->GetFile()->GetName(); }

    protected:
        TChain *fWCSimC;
        TTree *fWCSimT;
        WCSimRootEvent *fSpEvt;
        WCSimRootGeom *fWCGeom;

        Float_t fDetCentreY;

        float fHitTimeOffset;
        bool fMultDigiHits;

    private:
        TString fOutFileName;
};

#endif
