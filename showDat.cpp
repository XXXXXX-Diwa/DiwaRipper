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
    printf("\n总曲数为: %d 有效的曲数为: %d VoiceGroup总数为: %d\n",tabtotal,efctabtal,voitotal);
    printf(
           "\n请选择: \n"
           "\n\tS.查看歌曲的详细信息(主ROM)\n"
           "\n\tA.查看样本的详细信息(主ROM)\n"
           "\n\tB.复制歌曲到副ROM\n"
           "\n\tC.导出歌曲数据ASM文件\n"
           "\n\tD.主(导出)副(导入)ROM切换\n"
           "\n\tE.退出程序(ESC)\n"
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

