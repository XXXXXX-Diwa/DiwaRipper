#include "main.h"
#include "RomToRom.h"
#include "subTrackInfoGet.h"
#include <string>
#include <cstdint>

int subTabTol=0;//��RomTab����
uint8_t subTabefc[2000]={};//��RomTab��Ч ���
int subEfcTabTol=0;//��Rom��ЧTab����


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
            break;//ֻҪ������Table������ѭ��
        subTabTol++;
        fseek(g_ipf,sTInfo.subHeadOft<<5>>5,0);
        fread(&sHInfo,8,1,g_ipf);//��ȡ����,δ֪��voice�Ĳ���
        if(sHInfo.subTrackNum>16||sHInfo.subTrackNum==0||sHInfo.subVoiceOft>>25<<1!=8)
            continue;//��Ч�ĸ�������
        subTabefc[subTabTol-1]=1;
        subEfcTabTol++;
    }
    fclose(g_ipf);

    return 0;
}
