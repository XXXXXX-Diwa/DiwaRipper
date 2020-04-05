#include "main.h"
#include "RomToRom.h"
#include "subTabSrh.h"
#include "getDat.h"
#include "subTrackInfoGet.h"
#include "importOft.h"
#include "copyDatSize.h"
#include "writeToRom.h"
#include <cstdint>
#include <vector>
#include <io.h>

std::vector<int>subUseSong(0,0);//定义数组存储要被复制曲占用的歌曲号
std::vector<int>cpySong(0,0);//定义主rom要被复制的歌曲号组

void printUnEfcTable(int UnEfcTrack[],int cpyTol,std::vector<int>subUseSong)//打印无效的歌曲号
{
    printf("副rom无效的歌曲序号列表如下: \n");
    for(int i=0,j=1;i<subTabTol-subEfcTabTol;i++,j++)
    {
        for(int k=0;k<cpyTol;k++)
        {
            if(UnEfcTrack[i]==subUseSong[k])
               goto unprint;//如果无效的歌曲已经被占用则不打印出来
        }
        printf("%03d ",UnEfcTrack[i]);

        if(j>=16)
        {
            printf("\n");
            j=0;
        }
        unprint:
            continue;
    }
}

std::string removeQuotes(std::string s)
{
    std::size_t pos=s.find_last_of('"');
    if(pos>1&&pos!=std::string::npos)
        return s.substr(1,pos-1);
    return s.substr(1);
}
int trackCopy()
{
    std::vector<int>().swap(subUseSong);
    std::vector<int>().swap(cpySong);
    system("cls");
    if(g_subRom=="")
    {
        printf("检测到还没有载入副ROM!\n\n");
        while(1)
        {
            fflush(stdin);
            printf("注:与程序同目录仅输入ROM的完整名字即可,否则可拖动ROM到程序上获得路径!\n");
            printf("请输入副ROM的路径: ");
            g_subRom="";
            char *tempName=new char[300];//防止不够用
//            scanf("%s",tempName);

            gets(tempName);
            g_subRom=tempName;
            if(g_subRom[0]==34) g_subRom=removeQuotes(g_subRom);
            delete []tempName;
            if(access(g_subRom.c_str(),0)!=0) //检查文件是否存在
            {
                system("cls");
                printf("并未找到文件: %s\n",g_subRom.c_str());
                continue;
            }

            break;
        }
    }
    system("cls");
    if(!subTabGet(g_subRom)) return 0;

    getSubRomTrackInfo(subTabOft,g_subRom);//获得歌曲总数,有效歌曲数,歌曲有效flag

    printf("副Rom歌曲的总数为: %d  有效歌曲数为: %d\n",subTabTol,subEfcTabTol);

    int UnEfcTrack[subTabTol-subEfcTabTol]={};//定义一个记录无效歌曲的数组
    int cpyTol=0;//复制的歌曲数量

    int useFlag=0;//是否禁用有效歌曲flag
    int sampleFlag=0;//是否精确定位使用的samples
    uint32_t cpyDatSize=0;//要复制的数据总长度
    uint32_t woffset=0;//最终要写入数据的坐标


    for(int i=0,j=0;i<subTabTol;i++)
    {
        if(subTabefc[i]==1) continue;
            UnEfcTrack[j]=i+1;
        j++;
    }

    printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);//打印无效的歌曲号


    printf("\n是否仅使用无效的歌曲地址?--(Y\\N)  ");
    scanf("%c",&clean);
    if(toupper(clean)=='Y')
        useFlag=1;

    fflush(stdin);
    while(1)
    {
        system("cls");
        printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);
        printf("\n请输入要从主ROM中复制的歌曲数量: ");
        if(scanf("%d",&cpyTol)!=1)
            printf("请输入数字!\n");
        else if(cpyTol>efctabtal)
            printf("数量超过了主rom有效歌曲的数量!\n");
        else if(useFlag==1&&cpyTol>subTabTol-subEfcTabTol)
            printf("数量超过了副rom无效歌曲的数量!\n");
        else if(useFlag==0&&cpyTol>subTabTol)
            printf("数量超过了副rom全部歌曲的数量!\n");
        else
            break;
        fflush(stdin);
        getchar();
    }
    subUseSong.resize(cpyTol);
    cpySong.resize(cpyTol);

    for(int i=0;i<cpyTol;i++)
    {

        int reUse=0;//标记输错次数
        fflush(stdin);
        while(1)
        {
            system("cls");
            printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);

            printf("\n请输入副rom中要被复制占用的第%d个歌曲号!\n",i+1);
            if(scanf("%d",&subUseSong[i])!=1)
                printf("请输入数字!");
            else if(useFlag==0)
            {
                if(subUseSong[i]>subTabTol)
                    printf("歌曲号超过了副rom的最大歌曲号!");
                else
                {
                    for(int k=0;k<i;k++)
                    {
                        if(subUseSong[i]==subUseSong[k])
                        {
                            printf("当前歌曲号之前已经输入过!");
                            goto reinput;
                        }
                    }

                    break;//只要跳出循环就是正确的数
                }

            }
            else if(useFlag==1)
            {
                int mark=0;
                for(int j=0;j<subTabTol-subEfcTabTol;j++)
                {
                    if(subUseSong[i]==UnEfcTrack[j])
                    {
                        mark=1;
                        break;
                    }
                }
                if(mark==0)
                {
                    printf("数字不是无效的副rom歌曲号!\n");
                    reUse++;
                    if(reUse==2)
                    {
                        printf("是否要使用有效的歌曲号?--(Y\\N)  ");
                        scanf("%c",&clean);
                        fflush(stdin);
                        if(toupper(clean)=='Y')
                        {
                            useFlag=0;
                            for(int k=0;k<i;k++)
                            {
                                if(subUseSong[i]==subUseSong[k])
                                {
                                    printf("当前歌曲号之前已经输入过!");
                                    goto reinput;
                                }
                            }
                            break;
                        }
                        else
                            reUse=0;
                    }
                }
                else
                {
                    for(int k=0;k<i;k++)
                    {
                        if(subUseSong[i]==subUseSong[k])
                        {
                            printf("当前歌曲号之前已经输入过!");
                            goto reinput;
                        }
                    }

                    break;//只要跳出循环就是正确的数
                }

            }
            reinput:
            fflush(stdin);
            getchar();
        }
    }
    system("cls");
    printf("当前要被占用的副rom曲号如下: \n");
    for(int i=0;i<cpyTol;i++)
        printf("%d ",subUseSong[i]);
    getchar();


    for(int i=0;i<cpyTol;i++)
    {

        fflush(stdin);
        while(1)
        {
            system("cls");
            printf("\n请输入你要复制的主rom中的第%d个歌曲号:\n",i+1);
            printf("%d <- ",subUseSong[i]);
            uint8_t t=scanf("%d",&cpySong[i]);
            if(t!=1)
                printf("请输入数字!");
            else if(tablefc[cpySong[i]]!=1)
                printf("这个歌曲号对应的歌曲无效!");
            else
            {
                for(int j=0;j<i;j++)
                {
                    if(cpySong[i]==cpySong[j])
                    {
                        printf("这个歌曲号已经输入过了!");
                        goto reinputcpySong;
                    }

                }
                break;
            }
            reinputcpySong:
            fflush(stdin);
            getchar();
        }
    }
    system("cls");
    printf("主rom导入副rom的歌曲号如下:\n");
    printf("导出\t->\t导入\n");
    for(int i=0;i<cpyTol;i++)
        printf("%d\t->\t%d\n",cpySong[i],subUseSong[i]);
    fflush(stdin);
//    printf("是否精确定位仅使用的样本数据而不全套导入(费时)?--(Y\\N)  ");
//    scanf("%c",&clean);
//    fflush(stdin);
//    if(toupper(clean)=='Y')
    sampleFlag=0;//强制精确
    cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
    printf("要复制的数据长度为: %d / %X 字节\n",cpyDatSize,cpyDatSize);
    getchar();
    while(!(woffset=getimportOft(cpyDatSize)));
    printf("要写入数据位置为: %X - %X\n",woffset,woffset+cpyDatSize);
    copytoRom(woffset,cpyDatSize,sampleFlag);
    getchar();
    return 0;
}
