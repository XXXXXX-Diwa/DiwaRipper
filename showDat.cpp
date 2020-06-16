#include "main.h"
#include "showDat.h"
#include "tabSearch.h"
#include "getDat.h"
#include "showTra.h"
#include "showSam.h"
#include "MultAndDrum.h"
#include "RomToRom.h"
#include "romSwap.h"
#include "romtoAsm.h"
std::vector<uint32_t>backUpSubSong(0,0);

int importinterface()
{
    std::vector<uint32_t>().swap(backUpSubSong);
    system("cls");
    printf("��ǰ��romΪ:%s\n",getRomName(g_ipfName).c_str());
    printf("������Ϊ: %d ��Ч������Ϊ: %d VoiceGroup����Ϊ: %d\n",tabtotal,efctabtal,voitotal);
    printf(
           "\n��ѡ��: \n"
           "\n\t\tS.�鿴��������ϸ��Ϣ(��ROM)\n"
           "\n\t\tA.�鿴��������ϸ��Ϣ(��ROM)\n"
           "\n\t\tB.��ROM����(���Ƹ���,�޸�����,��������)\n"
           "\n\t\tC.������������ASM�ļ�\n"
           "\n\t\tD.�����л�\n"
           "\n\t\tE.�˳�����[ESC]\n"
           );
    switch(toupper(getch()))
    {
    case 'S':
        while(tableInfo());
        return 1;
    case 'A':
        while(VoiGopInfo());
        return 1;
    case 'B':
        while(trackCopy());
        return 1;
    case 'C':
        romToArmips();
        return 1;
    case 'D':
       return mainsubswap();
    case 'E':
    case 27 :
        return 0;

    }
    return 1;

}

