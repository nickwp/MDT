#pragma once

class TrueHit
{
    public:
        TrueHit(float t, int id=999999999);
        TrueHit();
        virtual ~TrueHit();

        float GetTime() const { return fTime; }
        float GetParentId() const { return fParentId; }
        float GetPosition(int i) const { return fPosition[i]; }
        int GetPosBin(int i) const { return fBin[i]; }

        void SetPosition(int i, float f) { fPosition[i] = f; }
        void SetPosBin(int i, int b){ fBin[i] = b; }

    private:
        float fTime;
        float fPosition[3]; // Hit position on photocade
        int fParentId;
        int fBin[3];
};