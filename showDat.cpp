#include "main.h"
#include "showDat.h"
#include "getDat.h"
#include "showTra.h"
#include "showSam.h"
#include "MultAndDrum.h"
#include "RomToRom.h"
#include "romSwap.h"

int importinterface()
{
    system("cls");
    printf("\n������Ϊ: %d ��Ч������Ϊ: %d VoiceGroup����Ϊ: %d\n",tabtotal,efctabtal,voitotal);
    printf(
           "\n��ѡ��: \n"
           "\n\tS.�鿴��������ϸ��Ϣ(��ROM)\n"
           "\n\tA.�鿴��������ϸ��Ϣ(��ROM)\n"
           "\n\tB.���Ƹ�������ROM\n"
           "\n\tC.������������ASM�ļ�\n"
           "\n\tD.��(����)��(����)ROM�л�\n"
           "\n\tE.�˳�����(ESC)\n"
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
    case 'D':
       return mainsubswap();

//    case 'C':
    case 'E':
    case 27 :
        return 0;

    }
    return 1;

}

