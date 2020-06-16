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
    printf("当前主rom为:%s\n",getRomName(g_ipfName).c_str());
    printf("总曲数为: %d 有效的曲数为: %d VoiceGroup总数为: %d\n",tabtotal,efctabtal,voitotal);
    printf(
           "\n请选择: \n"
           "\n\t\tS.查看歌曲的详细信息(主ROM)\n"
           "\n\t\tA.查看样本的详细信息(主ROM)\n"
           "\n\t\tB.副ROM操作(复制歌曲,修改音量,增加曲数)\n"
           "\n\t\tC.导出歌曲数据ASM文件\n"
           "\n\t\tD.主副切换\n"
           "\n\t\tE.退出程序[ESC]\n"
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

