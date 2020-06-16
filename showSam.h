#ifndef SHOWSAM_H_INCLUDED
#define SHOWSAM_H_INCLUDED

int VoiGopInfo();
int showSampleDat(int samNum,int vgNum,int curkey);
int showWaveMemoryData(int dataOffset);
int originalFre(int Fre,int key);

typedef struct vtableDat
{
    uint32_t tdat1;
    uint32_t tdat2;
    uint32_t tdat3;
}vtda_t;

#endif // SHOWSAM_H_INCLUDED
