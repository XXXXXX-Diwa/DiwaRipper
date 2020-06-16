#include "main.h"
#include "RomToRom.h"
#include "importOft.h"
#include "copyDatSize.h"

uint32_t addRomSize(uint32_t wSiz,uint32_t writeOft)
{
    fseek(g_ipf,0,2);//跳到文件尾
    uint32_t romSiz=ftell(g_ipf);
    if(writeOft%4!=0)
        writeOft=writeOft+4-writeOft%4;
    fseek(g_ipf,1,1);//跳到文件尾后一格
    for(uint32_t i=0;i<writeOft+wSiz-romSiz;i++)
    {
        fputc(0xFF,g_ipf);
    }
    return writeOft;
}

uint32_t getimportOft(uint32_t datSiz)
{
    system("cls");
    printf("\n是否自己输入要写入的rom位置(否则自动搜索)?--(Y\\N)  ");
    g_ipf=fopen(g_subRom.c_str(),"ab+");//二进制打开副rom文件流允许末尾添加数据
    char c;
    uint32_t oft;
    scanf("%c",&c);
    fflush(stdin);

    std::string temoft="";
    temoft.resize(10);
    if(toupper(c)=='Y')
    {
        while(1)
        {
            system("cls");
            printf("注: 0x开头的16进制地址!输入 >= (0x)1FFFFFF的数可以重新选择!\n");
            printf("\n请输入歌曲数据的写入地址: ");
            scanf("%s",&temoft[0]);
            fflush(stdin);

            if(temoft[0]!='0'||temoft[1]!='x'||temoft[2]=='\0')
            {
                printf("格式或数字错误!请重新输入!");
                getchar();
                continue;
            }
            temoft=&temoft[2];//去掉前两位
            sscanf(&temoft[0],"%x",&oft);
            if(oft%4!=0)
                oft=oft+4-oft%4;//对齐为4的整数
            if(oft+datSiz>0x1FFFFFF)
            {
                printf("写入数据将会超过最大容量!是否重新选择--(Y\\N)  ");
                scanf("%c",&c);
                fflush(stdin);
                if(toupper(c)=='Y')
                {
                    fclose(g_ipf);
                    return 0;
                }
                continue;
            }
            fseek(g_ipf,0,2);//定位到文件尾
            uint32_t subromsize=ftell(g_ipf);//得到副rom的尺寸
            if(oft>subromsize)
            {
                printf("会超过当前rom尺寸!是否允许扩容?--(Y\\N)  ");
                scanf("%c",&c);
                fflush(stdin);
                if(toupper(c)=='Y')
                {
                    oft=addRomSize(datSiz,oft);//扩容
                }
                else continue;
            }
            break;
        }
    }
    else
    {
        uint32_t blankFlag =0xCC000000;//记录空白数据是0还是0xFF
        uint32_t temcout =0;//需要检查的次数
        samplehead usearch;//借用这个数据结构
        rewind(g_ipf);//重新定位到文件头writeOft
        if(datSiz%16!=0)
            temcout=datSiz/16+1;
        else temcout=datSiz/16;

        while(blankFlag<<8>>8<temcout&&feof(g_ipf)==0)
        {
            fread(&usearch,16,1,g_ipf);
            if(usearch.loopTag==0xFFFFFFFF&&usearch.freq==0xFFFFFFFF
                &&usearch.loop==0xFFFFFFFF&&usearch.datsiz==0xFFFFFFFF)
            {
                if(blankFlag>>24!=0xFF)
                {
                    blankFlag=0xFF000001;//首次检查符合就会赋值
                }
                else blankFlag++;
            }
            else if(usearch.loopTag==0x0&&usearch.freq==0x0
                    &&usearch.loop==0x0&&usearch.datsiz==0x0)

            {
                if(blankFlag>>24!=0)
                {
                    blankFlag=1;//首次检查符合就是否重新选择--(Y\\N)  ");会赋值
                }
                else blankFlag++;
            }
            else blankFlag=0xCC000000;
        }
        if(temcout>blankFlag<<8>>8)
        {
            fseek(g_ipf,0,2);
            oft=ftell(g_ipf);
            if(oft+datSiz>0x1FFFFFF)
            {
                printf("既没有找到足够的空白数据!rom尺寸也不支持继续增加了!");
                printf("\n写入坐标为: %X 写入尺寸为: %X 需要ROM大小为: %X",oft,datSiz,oft+datSiz);
                getchar();
                exit(1);
            }
            printf("未找到足够的空白数据!会进行扩容!");
            getchar();
            oft=addRomSize(datSiz,oft);
        }
        else
        {
            oft=ftell(g_ipf);//获取导入的数据尾坐标
            oft=oft-temcout*16;
            fseek(g_ipf,oft-16,0);//检查是否有浪费的空白数据
            blankFlag=(blankFlag>>24)+(blankFlag>>24<<8)+(blankFlag>>24<<16)+(blankFlag>>24<<24);
//            printf("blankFlag的值为: %X",blankFlag);
//            getchar();
            fread(&usearch,16,1,g_ipf);
            if(usearch.loop==blankFlag)
                oft-=12;
            else if(usearch.freq==blankFlag)
                oft-=8;
            else if(usearch.datsiz==blankFlag)
                oft-=4;
            if(oft+temcout*16>0x1FFFFFF)
            {
                printf("该rom有超过32mb的无用数据!!!!!");
                getchar();
                exit(1);
            }
        }
    }
    fclose(g_ipf);
    return oft;

}
