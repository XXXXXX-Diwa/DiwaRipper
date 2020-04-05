#ifndef SUBTRACKINFOGET_H_INCLUDED
#define SUBTRACKINFOGET_H_INCLUDED
#include <cstdint>

int getSubRomTrackInfo(int fstTabOft,std::string subRomName);

struct subTrackInfo
{
    uint32_t subHeadOft;
    uint32_t subSongGop;
};

struct subHeadInfo
{
    uint16_t subTrackNum;
    uint16_t subUnknow;
    uint32_t subVoiceOft;
    uint32_t subTrackDataOft[16];
};


#endif // SUBTRACKINFOGET_H_INCLUDED
