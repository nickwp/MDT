#include "HitTube.h"

HitTube::HitTube(int id)
{
    fNRawPE=0;
    fTimeRaw.clear();

    fNDigiHits=0;
    fTimeDigi.clear();
    fChargeDigi.clear();

    fTubeID = id;
}


HitTube::~HitTube()
{
    vector<float>().swap( fTimeRaw );
    vector<float>().swap( fTimeDigi );
    vector<float>().swap( fChargeDigi );
}
