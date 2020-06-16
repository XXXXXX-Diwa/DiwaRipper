#include "main.h"
#include "RomToRom.h"
#include "subTabSrh.h"
#include "getDat.h"
#include "subTrackInfoGet.h"
#include "importOft.h"
#include "copyDatSize.h"
#include "writeToRom.h"
#include "tabSearch.h"
#include "showDat.h"
#include <cstdint>
#include <vector>
#include <io.h>
//#include <fstream>
//
//using namespace std;

std::vector<uint32_t>subUseSong(0,0);//定义数组存储要被复制曲占用的歌曲号
std::vector<uint32_t>cpySong(0,0);//定义主rom要被复制的歌曲号组

void damageSet(uint32_t &startNum,uint32_t &endNum,bool flag){
	while(1)
        {
            system("cls");
            printf("请输入歌曲开始号: ");
            if(!(scanf("%d",&startNum)))
            {
                fflush(stdin);
                printf("\n请输入数字!");
                getchar();
            }

            else if(startNum==0)
            {
                fflush(stdin);
                printf("\n歌曲号最低从1开始!");
                getchar();
            }
            else if(startNum>tabtotal)
            {
                fflush(stdin);
                printf("\n起始号超过了要复制rom的歌曲总数!");
                getchar();
            }
            else if(flag) break;
            else if(startNum>subTabTol)
            {
                fflush(stdin);
                printf("\n起始号超过了要写入rom的歌曲总数!");
                getchar();
            }
            else break;
        }
    fflush(stdin);
    while(1)
    {
        system("cls");
        printf("请输入歌曲结束号: ");
        if(!(scanf("%d",&endNum)))
        {
            fflush(stdin);
            printf("\n请输入数字!");
            getchar();
        }
        else if(endNum<startNum)
        {
            fflush(stdin);
            printf("\n结束号不能小于开始号!");
            getchar();
        }
        else if(endNum>tabtotal)
        {
            fflush(stdin);
            printf("\n结束号超过了要复制rom的歌曲总数!");
            getchar();
        }
        else if(flag) break;
        else if(endNum>subTabTol)
        {
            fflush(stdin);
            printf("\n结束号超过了要写入rom的歌曲总数!");
            getchar();
        }
        else break;
    }
}

void VolSet(std::vector<uint32_t>volSetSong)
{
    //前两个记录地址坐标,第三记录多少个超过,第四记录最大超过数,以及记录设置的音量数,
    uint32_t bitoft,bitraoft,overcount,overmax=0,vol,cha;
    uint8_t bitranum,bitemp;//记录音轨数,和读取字节数据

    while(1)
    {
        system("cls");
        printf("注:8 - 32\n\n如果当前音量为16,请输入你想要的音量: ");
        if(scanf("%u",&vol)==0){
			fputs("请输入数字!",stderr);
		}
        else if(vol>32)
        {
            printf("\n\n请不要设置两倍以上!");
        }
        else if(vol<8)
        {
            printf("\n\n请不要设置一半以下!");
        }
        else break;
		fflush(stdin);
		getchar();
    }

	fflush(stdin);
    g_ipf=fopen(g_subRom.c_str(),"rb+");//打开副rom
    std::vector<uint32_t>voloft(0,0);//用于记录要修改的音量地址
    overcount=0;
    for(uint32_t i=0;i<volSetSong.size();i++)
    {
        fseek(g_ipf,subTabOft+(volSetSong[i]-1)*8,0);//定位到table地址
        fread(&bitoft,4,1,g_ipf);//读取head地址
        bitoft=bitoft<<5>>5;//去掉多余的数据
        fseek(g_ipf,bitoft,0);//跳到head数据位置
        fread(&bitranum,1,1,g_ipf);//读取歌曲数
        fseek(g_ipf,7,1);//跳到音轨数据位置
        bitoft=ftell(g_ipf);//保存音轨的数据起始坐标
        for(uint32_t j=0;j<bitranum;j++)
        {
            fseek(g_ipf,bitoft+4*j,0);//跳到每个音轨的地址
            fread(&bitraoft,4,1,g_ipf);//读取track坐标
            bitraoft=bitraoft<<5>>5;//去掉多余的数据
            fseek(g_ipf,bitraoft,0);//跳到每个音轨的数据位置
            do
            {
                bitemp=fgetc(g_ipf);

                if(bitemp==0xBE)
                {
                    voloft.push_back(ftell(g_ipf));//记录当前的坐标
                    while(1)
                    {
                        bitemp=fgetc(g_ipf);//获取音量值
                        if(bitemp>0xB0) break;
                        else if(bitemp<0x80)
                        {
                            cha=bitemp*vol/16;
                            if(cha>127)
                            {
                                overcount++;
                                if(cha>overmax)
                                {
                                    overmax=cha;
                                }
                            }
                        }

                    }

                }
                else if(bitemp==0xB2||bitemp==0xB3)
                {
                    fseek(g_ipf,4,1);
                }
                else if(bitemp==0xB1)
                {
                    break;
                }
            }while(!feof(g_ipf));
        }
    }
    if(overcount!=0)
    {
        printf("\n共有%d个音量在这个设置下会超过最大值127,最大值为: %d 确认改写?--Y\\N",overcount,overmax);
        if(toupper(getch())!='Y')
        {
            return;
        }
    }
    for(uint32_t i=0;i<voloft.size();i++)
    {
        fseek(g_ipf,voloft[i],0);//定位到每个0xBE后
        bitemp=fgetc(g_ipf);//获取音量值
        fseek(g_ipf,-1,1);
        cha=bitemp*vol/16;
        if(cha>127)
        {
            fputc(127,g_ipf);
        }
        else if(cha==0)
        {
            fputc(1,g_ipf);
        }
        else fputc(cha,g_ipf);

        while(1)
        {
            bitemp=fgetc(g_ipf);
            if(bitemp>0xB0) break;//命令的话则跳出否则继续检查
            else if(bitemp<0x80)//是音量参数的话
            {
                fseek(g_ipf,-1,1);
                cha=bitemp*vol/16;
                if(cha>127)
                {
                    fputc(127,g_ipf);
                }
                else if(cha==0)
                {
                    fputc(1,g_ipf);
                }
                else fputc(cha,g_ipf);
                fseek(g_ipf,0,1);//只有这样定位一下才不会出错...
            }
        }
    }
    printf("\n音量改写完毕!");
    getchar();
    fclose(g_ipf);
    return;
}

void printUnEfcTable(uint32_t UnEfcTrack[],uint32_t cpyTol,std::vector<uint32_t>subUseSong)//打印无效的歌曲号
{
    printf("副rom无效的歌曲序号列表如下: \n");
    for(uint32_t i=0,j=1;i<subTabTol-subEfcTabTol;i++,j++)
    {
        for(uint32_t k=0;k<cpyTol;k++)
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

int ckPath(std::string s)//检查是否有目录
{
    size_t pos=s.find_first_of('\\');
    if(pos>1&&pos!=std::string::npos)
        return 0;
    return 1;
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
    std::vector<uint32_t>().swap(subUseSong);
    std::vector<uint32_t>().swap(cpySong);
    system("cls");
    fflush(stdin);
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

            gets(tempName);
            fflush(stdin);
            g_subRom=tempName;
            if(g_subRom[0]==34) g_subRom=removeQuotes(g_subRom);
            delete []tempName;
            if(access(g_subRom.c_str(),0)!=0) //检查文件是否存在
            {
                system("cls");
                printf("并未找到文件: %s\n",g_subRom.c_str());
                continue;
            }
            printf("SWpath= %s",SWpath.c_str());
            if(ckPath(g_subRom))
                g_subRom=onlyPath(SWpath)+g_subRom;
            if(ckPath(g_ipfName))
                g_ipfName=onlyPath(SWpath)+g_ipfName;
            if(g_ipfName==g_subRom)
            {
                printf("主rom和副rom是同一个位置的rom!");
                getchar();
                continue;
            }
            break;
        }
    }
    system("cls");
    if(!subTabGet(g_subRom)) return 0;

    getSubRomTrackInfo(subTabOft,g_subRom);//获得歌曲总数,有效歌曲数,歌曲有效flag

    printf("副Rom歌曲的总数为: %d  有效歌曲数为: %d\n",subTabTol,subEfcTabTol);

    uint32_t UnEfcTrack[subTabTol-subEfcTabTol]={};//定义一个记录无效歌曲的数组
    uint32_t cpyTol=0;//复制的歌曲数量

    int useFlag=0;//是否禁用有效歌曲flag
    int sampleFlag=0;//是否精确定位使用的samples
    uint32_t cpyDatSize=0;//要复制的数据总长度
    uint32_t woffset=0;//最终要写入数据的坐标


    for(uint32_t i=0,j=0;i<subTabTol;i++)
    {
        if(subTabefc[i]==1) continue;
            UnEfcTrack[j]=i+1;
        j++;
    }

    //printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);//打印无效的歌曲号
    printf("\n\t\tA.仅使用无效的歌曲地址\n"
           "\n\t\tB.任意使用所有歌曲地址\n"
           "\n\t\tC.范围移植(特设 歌曲序号相同限定)\n"
           "\n\t\tD.设置副rom歌曲的音量\n"
           "\n\t\tE.扩容副rom歌曲数量\n"
           "\n\t\t    ESC [返回]\n");
    while((clean=toupper(getch())))//节省计算,不用反复刷新以上的代码
    {
        if(clean>='A'&&clean<='D') break;
        else if(clean==27) return 0;
    }
    switch(clean)
    {
        case 'A':
            useFlag=1;
            break;
        case 'B':
            useFlag=0;
            break;
        case 'C':
            uint32_t startNum,endNum;
            damageSet(startNum,endNum,false);
            fflush(stdin);
            cpyTol=endNum-startNum+1;
            for(uint32_t i=startNum;i<=cpyTol;i++)
            {
                if(tablefc[i-1]==1)
                {
                    subUseSong.push_back(i);//记录有效的歌曲序号
                    cpySong.push_back(i);
                }
            }
            cpyTol=cpySong.size();
//            backUpSubSong(subUseSong);//拷贝
//            printf("cpytol= %d subUseSong.size= %d",cpyTol,subUseSong.size());
//            getchar();
            for(uint32_t i=0;i<subUseSong.size();i++)
            {
                subTabefc[subUseSong[i]-1]=1;//复制歌曲的有效性全标记为1;
            }
            sampleFlag=1;//强制精确
            printf("cpySong.size= %d,cpyTol= %d",cpySong.size(),cpyTol);
            getchar();
            cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
            printf("要复制的数据长度为: %d / %X 字节\n",cpyDatSize,cpyDatSize);
            getchar();
            while(!(woffset=getimportOft(cpyDatSize)));
            printf("要写入数据位置为: %X 长度为: %X\n",woffset,cpyDatSize);
            getchar();
            copytoRom(woffset,cpyDatSize,sampleFlag);
            getchar();
            return 1;
        case 'D':
            system("cls");
            if(backUpSubSong.size()!=0)
            {
                printf("是否对刚刚复制或设置的歌曲进行音量设置?--Y/N");
                if(toupper(getch())=='Y')
                {
                    VolSet(backUpSubSong);
                    return 1;
                }
            }
            uint32_t setSong=0;
            printf("\n注:当输入非数时结束\n\n请输入要修改音量的副rom曲号: ");
            while((scanf("%d",&setSong)))
            {
                fflush(stdin);
                if(setSong>subTabTol)
                    printf("歌曲号超过了副rom的最大歌曲号");
                else if(setSong==0)
                    printf("歌曲号最小为1!");
                else if(subTabefc[setSong-1]==0)
                {
                    printf("这个歌曲无效!");
                }
                else
                {
                    for(uint32_t i=0;i<subUseSong.size();i++)
                    {
                        if(setSong==subUseSong[i])
                            printf("这个歌曲号之前已经输入过了!");
                    }
                    subUseSong.push_back(setSong);
                    continue;
                }
                getchar();
            }
            fflush(stdin);
            if(subUseSong.size()==0)
            {
                printf("当前没有任何歌曲号!");
                getchar();
                return 0;
            }
            VolSet(subUseSong);
            subUseSong.swap(backUpSubSong);
            return 1;
    }
    system("cls");
    fflush(stdin);
    while(1)
    {
        system("cls");
        printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);
        printf("\n请输入要从主ROM中复制的歌曲数量: ");
        if(scanf("%d",&cpyTol)!=1)
            printf("请输入数字!\n");
        else if(cpyTol>efctabtal)
            printf("数量超过了主rom有效歌曲的数量!efctable= %d\n",efctabtal);
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

    for(uint32_t i=0;i<cpyTol;i++)
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
                    for(uint32_t k=0;k<i;k++)
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
                for(uint32_t j=0;j<subTabTol-subEfcTabTol;j++)
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
                            for(uint32_t k=0;k<i;k++)
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
                    for(uint32_t k=0;k<i;k++)
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
    for(uint32_t i=0;i<cpyTol;i++)
        printf("%d ",subUseSong[i]);
    getchar();


    for(uint32_t i=0;i<cpyTol;i++)
    {

//        cpySong[i]=i+1;
//        subUseSong[i]=cpySong[i];
        fflush(stdin);
        while(1)
        {
            system("cls");
            printf("\n请输入你要复制的主rom中的第%d个歌曲号:\n",i+1);
            printf("%d <- ",subUseSong[i]);
            uint8_t t=scanf("%d",&cpySong[i]);
            if(t!=1)
                printf("请输入数字!");
            else if(tablefc[cpySong[i]-1]!=1)
                printf("这个歌曲号对应的歌曲无效!");
            else
            {
                for(uint32_t j=0;j<i;j++)
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
    for(uint32_t i=0;i<cpyTol;i++)
        printf("%d\t->\t%d\n",cpySong[i],subUseSong[i]);

    fflush(stdin);
    getchar();
//    printf("是否精确定位仅使用的样本数据而不全套导入(费时)?--(Y\\N)  ");
//    scanf("%c",&clean);
//    fflush(stdin);
//    if(toupper(clean)=='Y')
    for(uint32_t i=0;i<subUseSong.size();i++)
    {
        subTabefc[subUseSong[i]-1]=1;//复制歌曲的有效性全标记为1;
    }
    sampleFlag=1;//强制精确
    cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
    printf("要复制的数据长度为: %d / %X 字节\n",cpyDatSize,cpyDatSize);
    getchar();
    while(!(woffset=getimportOft(cpyDatSize)));
    printf("要写入数据位置为: %X 长度为: %X\n",woffset,cpyDatSize);
    getchar();
    copytoRom(woffset,cpyDatSize,sampleFlag);
    getchar();
    return 1;
}
