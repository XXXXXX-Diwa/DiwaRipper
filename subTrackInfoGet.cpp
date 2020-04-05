#include "main.h"
#include "RomToRom.h"
#include "subTrackInfoGet.h"
#include <string>
#include <cstdint>

int subTabTol=0;//副RomTab总数
uint8_t subTabefc[2000]={};//副RomTab有效 标记
int subEfcTabTol=0;//副Rom有效Tab总数


int getSubRomTrackInfo(int fstTabOft,std::string subRomName)
{
    g_ipf=fopen(subRomName.c_str(),"rb");

    subTrackInfo sTInfo;
    subHeadInfo sHInfo;
    char chtemp[8];
    while(!feof(g_ipf))
    {
        fseek(g_ipf,fstTabOft+8*subTabTol,0);
        fread(&sTInfo,8,1,g_ipf);
        memcpy(chtemp,&sTInfo,8);
        if((uint8_t)chtemp[3]>>1!=4||chtemp[5]!=0||chtemp[7]!=0)
            break;//只要不再是Table就跳出循环
        subTabTol++;
        fseek(g_ipf,sTInfo.subHeadOft<<5>>5,0);
        fread(&sHInfo,8,1,g_ipf);//读取曲数,未知和voice的部分
        if(sHInfo.subTrackNum>16||sHInfo.subTrackNum==0||sHInfo.subVoiceOft>>25<<1!=8)
            continue;//无效的歌曲跳过
        subTabefc[subTabTol-1]=1;
        subEfcTabTol++;
    }
    fclose(g_ipf);

    return 0;
}
