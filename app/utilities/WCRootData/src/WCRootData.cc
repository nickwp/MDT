#include "WCRootData.h"
#include "Configuration.h"

#include "TBranch.h"

WCRootData::WCRootData()
{
    fDetCentreY = 0.;
    fWCGeom = 0;
    fWCSimC = 0;
    fWCSimT = 0;
    fSpEvt = 0;
    fOutFileName = "";

    int mult_flag = 1;
    fHitTimeOffset = 0.;

    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>("TimeOffset", fHitTimeOffset);
    Conf->GetValue<int>("FlagMultDigits", mult_flag);

    fMultDigiHits = bool(mult_flag);
}

WCRootData::~WCRootData()
{
    if( fWCGeom ){ delete fWCGeom; fWCGeom = 0; }
    if( fWCSimT ){ delete fWCSimT; fWCSimT = 0; }
    if( fWCSimC ){ delete fWCSimC; fWCSimC = 0; }
    if( fSpEvt ){ delete fSpEvt; fSpEvt = 0; }
}


void WCRootData::GetGeometryInfo(const char *filename)
{
    TFile *f=TFile::Open(filename);
    TTree *t=(TTree*)f->Get("wcsimGeoT");
    t->SetBranchAddress("wcsimrootgeom", &fWCGeom);
    t->GetEntry( 0 );
    fDetCentreY = fWCGeom->GetWCOffset(1);
    cout<<" DetCentreY: " << fDetCentreY <<endl;

    f->Close();
}


void WCRootData::AddTrueHitsToMDT(MDTManager *mdt, float intTime)
{
    HitTubeCollection *hc = mdt->GetHitTubeCollection();

    const WCSimRootTrigger *aEvt = fSpEvt->GetTrigger(0);
    const int nCkovHits = aEvt->GetNcherenkovhits();

    TClonesArray *hitTimeArray = aEvt->GetCherenkovHitTimes();
    for(int iHit=0; iHit<nCkovHits; iHit++)
    {
        WCSimRootCherenkovHit *aHit=
                dynamic_cast<WCSimRootCherenkovHit*>((aEvt->GetCherenkovHits())->At(iHit));
    
        int timeArrayIndex = aHit->GetTotalPe(0);
        int peForTube = aHit->GetTotalPe(1);   
        int tubeID = aHit->GetTubeID();
        const int maxPE = timeArrayIndex + peForTube;
        for(int jPE=timeArrayIndex; jPE<maxPE; jPE++)
        {
            WCSimRootCherenkovHitTime *aHitTime=
                dynamic_cast<WCSimRootCherenkovHitTime*>(hitTimeArray->At(jPE));
    
            float truetime = aHitTime->GetTruetime();
            if( truetime<0. ){ continue; }
            if( aHitTime->GetParentID()<0 ){ continue; }

            // Add new hit tube
            if( !hc->HasTube(tubeID) )
            {
                hc->AddHitTube(tubeID);
            }
            (&(*hc)[tubeID])->AddRawPE(truetime+intTime, aHitTime->GetParentID());
        }
    }
    hc = 0;
}

void WCRootData::ReadFile(const char *filename)
{
    fWCSimC = new TChain("wcsimT");
    fWCSimC->Add(filename);
    fWCSimC->SetBranchAddress("wcsimrootevent", &fSpEvt);
    fWCSimC->SetAutoDelete();
}

void WCRootData::CloseFile()
{
    delete fWCSimC; fWCSimC = 0;
    delete fSpEvt; fSpEvt = 0;
}

int WCRootData::GetEntries() 
{
    return fWCSimC->GetEntries();
}

void WCRootData::GetEntry(int i)
{
    fWCSimC->GetEntry(i);
}


void WCRootData::CreateTree(const char *filename)
{
    fOutFileName = TString(filename);
    TFile *fout = new TFile(fOutFileName, "recreate");
    fout->SetCompressionLevel(2);

    Int_t branchStyle = 1;
    Int_t bufferSize = 64000;
    TString bName = "wcsimrootevent";
    TString bAddress = "WCSimRootEvent";

    fWCSimT = new TTree("wcsimT", "Processed WCSim Tree for pile-up");
    fSpEvt = new WCSimRootEvent();
    fSpEvt->Initialize();
    TTree::SetBranchStyle(branchStyle);
    TBranch *branch = fWCSimT->Branch(bName, bAddress, &fSpEvt, bufferSize, 2);
}

void WCRootData::AddDigiHits(MDTManager *mdt, int eventID)
{
    TriggerInfo *ti = mdt->GetTriggerInfo();
    HitTubeCollection *hc = mdt->GetHitTubeCollection();

    WCSimRootTrigger* anEvent = fSpEvt->GetTrigger(0);

    const int nTriggers = ti->GetNumOfTrigger(); 
    for(int iTrig=0; iTrig<nTriggers; iTrig++) 
    {
        TriggerType_t trigType = kTriggerNDigits;
        if( ti->GetType(iTrig)==(int)TriggerType::eFailure )
        {
            trigType = kTriggerFailure; 
        }

        if( iTrig>=1 )
        {
            fSpEvt->AddSubEvent();
            anEvent = fSpEvt->GetTrigger(iTrig);
            anEvent->SetHeader(eventID, 0, 0, iTrig+1);
            anEvent->SetMode(0);
        }
        vector<Float_t> info(1, ti->GetNHits(iTrig));
        anEvent->SetTriggerInfo(trigType, info);

        const float triggerTime = ti->GetTriggerTime(iTrig);
        const float tWinLowEdge = ti->GetLowEdge(iTrig);
        const float tWinUpEdge = ti->GetUpEdge(iTrig);

        int nHits = 0;
        float sumQ = 0.;
        map<int, bool> tubeCount;
        for(hc->Begin(); !hc->IsEnd(); hc->Next())
        {
            HitTube *aPH = &(*hc)();
            int tubeID = aPH->GetTubeID();
            int nCount = 0;
            for(int i=0; i<aPH->GetNDigiHits(); i++)
            {
                float t = aPH->GetTimeDigi(i);

                // Need to be updated 
                int mPMT_module_id = 0;
                int mPMT_pmt_id = 0;
                if( t>=tWinLowEdge && t<=tWinUpEdge )
                {
                    bool doFill = false;
                 
                    // The first hit is always allowed to be saved.
                    // Later hits depend on your setting
                    if( tubeCount.count(tubeID)==0 ){ doFill = true; } 
                    else { doFill = fMultDigiHits ? true : false; }  

                    if( doFill ) 
                    {
                        vector<int> true_pe_comp = aPH->GetParentCompositionDigi(i);
                        t = t +  fHitTimeOffset - triggerTime;
                        float q = aPH->GetChargeDigi(i);
                        anEvent->AddCherenkovDigiHit(q,
                                                     t,
                                                     tubeID,
                                                     mPMT_module_id,
                                                     mPMT_pmt_id,
                                                     true_pe_comp);
                        nHits += 1;
                        sumQ += q;
                        nCount+=1;
                        tubeCount[tubeID] = 1;
                    }
                }
            } // digi hit loop
            aPH = NULL;
        } // PMT loop
        anEvent->SetNumDigitizedTubes(nHits); 
        anEvent->SetSumQ(sumQ);
        WCSimRootEventHeader *eh = anEvent->GetHeader();
        eh->SetDate( int(triggerTime) );
    }
}

void WCRootData::FillTree()
{
    TFile *f = fWCSimT->GetCurrentFile();
    f->cd();
    fWCSimT->Fill();     
    fWCSimT->Write("",TObject::kOverwrite);
    fSpEvt->ReInitialize();
}


void WCRootData::AddTracks(const WCSimRootTrigger *aEvtIn, float offset_time)
{
    WCSimRootTrigger *aEvtOut = fSpEvt->GetTrigger(0);
    TClonesArray *tracks = aEvtIn->GetTracks();
    const int ntrack = tracks->GetEntries();
    for(int i=0; i<ntrack; i++)
    {
        WCSimRootTrack *aTrack = (WCSimRootTrack*)tracks->At(i);
        Float_t   dir[3];
        Float_t   pdir[3];
        Float_t   stop[3];
        Float_t   start[3];
        Int_t     ipnu = aTrack->GetIpnu();
        Int_t     flag = aTrack->GetFlag();
        Float_t   m = aTrack->GetM();
        Float_t   p = aTrack->GetP();
        Float_t   E = aTrack->GetE();
        Int_t     startvol = aTrack->GetStartvol();
        Int_t     stopvol = aTrack->GetStopvol();
        for(int j=0; j<3; j++)
        {
            dir[j] = aTrack->GetDir(j); 
            pdir[j] = aTrack->GetPdir(j);
            stop[j] = aTrack->GetStop(j);
            start[j] = aTrack->GetStart(j);
        }
        Int_t     parenttype = aTrack->GetParenttype();
        Float_t   time = aTrack->GetTime() + offset_time;
        Int_t     id = aTrack->GetId();
        Int_t     idPrnt = aTrack->GetParentId();

        aEvtOut->AddTrack(ipnu, 
				          flag, 
				          m, 
				          p, 
				          E, 
				          startvol, 
				          stopvol, 
				          dir, 
				          pdir, 
				          stop,
				          start,
				          parenttype,
				          time,
				          id,
                          idPrnt);
    }
}

void WCRootData::WriteTree()
{
    TFile *f = fWCSimT->GetCurrentFile();
    f->cd();
    fWCSimT->Write();
    f->Close();
}


void WCRootData::CopyTree(const char *filename,
                          const char *treename,
                          const vector<bool> &savelist)
{

	TFile *fin = TFile::Open(filename);
	if( !fin->FindKey(treename) )
	{
		fin->Close();
		std::cout<<" [ERROR] WCRootData::CopyTree " <<std::endl;
		std::cout<<"  - No " << treename <<" tree is saved in : " << filename <<std::endl;
        exit(-1);
	}

    // Copy all the entries
    if( savelist.size()==0 )
    {
        if( treename!="Settings" )
        {
            TTree *tin = (TTree*)fin->Get(treename);

            TFile *fout = TFile::Open(fOutFileName, "update");
            fout->cd();
            TTree *tout = tin->CloneTree(-1, "fast");
            tout->Write();
            fout->Close();
        }
        else // This is a special treatment
        {
            TTree *tin = (TTree*)fin->Get(treename);

            TBranch *bWCXRotation = tin->GetBranch("WCXRotation");
            TBranch *bWCYRotation = tin->GetBranch("WCYRotation");
            TBranch *bWCZRotation = tin->GetBranch("WCZRotation");
            TBranch *bWCDetCentre = tin->GetBranch("WCDetCentre");
            TBranch *bWCDetRadius = tin->GetBranch("WCDetRadius");
            TBranch *bWCDetHeight = tin->GetBranch("WCDetHeight");

            Float_t WCXRotation[3] = {0.};
            Float_t WCYRotation[3] = {0.};
            Float_t WCZRotation[3] = {0.};
            Float_t WCDetCentre[3] = {0.};
            Float_t WCDetRadius = 0.;
            Float_t WCDetHeight = 0.;

            bWCXRotation->SetAddress(WCXRotation);
            bWCYRotation->SetAddress(WCYRotation);
            bWCZRotation->SetAddress(WCZRotation);
            bWCDetCentre->SetAddress(WCDetCentre);
            bWCDetRadius->SetAddress(&WCDetRadius);
            bWCDetHeight->SetAddress(&WCDetHeight);

            bWCXRotation->GetEntry(0);
            bWCYRotation->GetEntry(0);
            bWCZRotation->GetEntry(0);
            bWCDetCentre->GetEntry(0);
            bWCDetRadius->GetEntry(0);
            bWCDetHeight->GetEntry(0);

            TFile *fout = TFile::Open(fOutFileName, "update");
            fout->cd();
            TTree *tout = new TTree("Settings", "");
            tout->Branch("WCXRotation", WCXRotation, "WCXRotation[3]/F");
            tout->Branch("WCYRotation", WCYRotation, "WCYRotation[3]/F");
            tout->Branch("WCZRotation", WCZRotation, "WCZRotation[3]/F");
            tout->Branch("WCDetCentre", WCDetCentre, "WCDetCentre[3]/F");
            tout->Branch("WCDetRadius", &WCDetRadius, "WCDetRadius/F");
            tout->Branch("WCDetHeight", &WCDetHeight, "WCDetHeight/F");
            tout->Fill();
            tout->Write();
            fout->Close();
        }
    }
    else
    {
        TTree *tin = (TTree*)fin->Get(treename);

        TFile *fout = TFile::Open(fOutFileName, "update");
        fout->cd();
        TTree *tout = tin->CloneTree(0);

        const int nEntries = tin->GetEntries();
        for(int iEntry=0; iEntry<nEntries; iEntry++)
        {
            tin->GetEntry(iEntry);
            if( !savelist[iEntry] ){ continue; }
            fout->cd();
            tout->Fill();
        }
        fout->cd();
        tout->Write();
        fout->Close();
    }
    fin->Close();
}
