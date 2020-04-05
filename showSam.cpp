#include "main.h"
#include "showDat.h"
#include "getDat.h"
#include "showTra.h"
#include "showSam.h"
#include<windows.h>
#include<mmsystem.h>
#include "MultAndDrum.h"
#include "playSample.h"
#include <cstring>


int originalFre(int Fre,int key)
{
    double Ratio = 1.05946309436;//1.0594630943593
    double Fren=Fre/=4.0;
    if(key==60)
        return Fren;
    else if(key>60)
    {
        for(int i=0;i<key-60;i++)
            Fren*=Ratio;
        return Fren;
    }
    else
    {
        for(int i=0;i<60-key;i++)
            Fren/=Ratio;
        return Fren;
    }

}

int showSampleDat(int samNum,int vgNum,int curkey)
{
    while(1)
    {
        system("cls");
        fflush(stdin);
        uint32_t samHead[4]={};
        fseek(g_ipf,(voiceoft[vgNum]<<5>>5)+samNum*0xC+4,0);
        fread(samHead,4,1,g_ipf);
        fseek(g_ipf,samHead[0]<<5>>5,0);
        fread(samHead,0x10,1,g_ipf);
        int hoffset=ftell(g_ipf);
        int wavFren=originalFre(samHead[1]>>8,curkey);
        printf("VoiceGroup: %X\t乐器号: %d   [ESC: 返回]   [回车:播放]\n",voiceoft[vgNum],samNum);
        printf("\t\nHEAD数据地址: %d / %07X\n",(int)hoffset-0x10,(int)hoffset-0x10);
        printf("\t\n样本数据地址: %d / %07X\n",(int)hoffset,(int)hoffset);
        printf("\t\n未转化前频率: %d / %X\n",wavFren,wavFren);
        printf("\t\n转化后的频率: %06d x 4 = %06d / %X\n",(samHead[1]>>8)/4,samHead[1]>>8,samHead[1]>>8);
        printf("\t\n循环偏移尺寸: %d / %X\n",samHead[2],samHead[2]);
        printf("\t\n样本数据尺寸: %d / %X\n",samHead[3],samHead[3]);
        printf("\t\n全部数据尺寸: %d / %X\n",samHead[3]+0x13-(samHead[0]>>30),samHead[3]+0x13-(samHead[0]>>30));

        int bit=MakeWav(wavFren,samHead[3]+36,samHead[3]+3-(samHead[0]>>30));
        while((clean=getch())!=13&&clean!=27);

        if(clean==13)
        {
            if(bit)
                PlaySound("_tempwav.wav",NULL,SND_ASYNC);
            else
                PlaySound("C:\\Users\\Administrator\\AppData\\Local\\Temp\\_tempwav.wav",NULL,SND_ASYNC);
        }

        else if(clean==27)
        {
            return 0;
        }
        continue;

    }


}
int showWaveMemoryData(int dataOffset)
{
    system("cls");
    fflush(stdin);
    printf("\n");
    printf("%X\n",dataOffset);

    fseek(g_ipf,dataOffset,0);
    uint8_t tempDat[16]={};
    fread(tempDat,16,1,g_ipf);
    for(int i=0;i<16;i++)
    {
        if(i==8)printf("-");
        printf("%02X",tempDat[i]);
    }
    printf("\n");
    for(int i=0;i<64;i++)
    {
        printf("_");
    }
    printf("\n");
    uint8_t waveDat[32]={};
    for(int i=0;i<16;i++)
    {
        waveDat[i*2]=tempDat[i]>>4;
        waveDat[i*2+1]=tempDat[i]&0xF;
    }
    uint8_t paint[16][64];
    memset(paint,0xFF,16*64);
    for(int i=0;i<16;i++)
        for(int j=0;j<32;j++)
            if(waveDat[j]==i)
                paint[i][j*2]=0xFE;

    int high=0;
    for(int j=0;j<32;j++)
    {
        for(int i=0;i<16;i++)
        {
            if(paint[i][j*2]!=0xFF)
            {
               for(int l=0;l<16;l++)
               {
                   if(paint[l][j*2+2]==0xFE)
                   {
                       if(i==l) paint[l][j*2+1]=0xFE;
                       else if(i>l)
                       {
                           paint[i][j*2+1]=3;//3
                           for(int k=l+1;k<i;k++)
                           {
                               paint[k][j*2+1]=0xFD;
                           }
                           paint[l][j*2+1]=4;//4
                       }

                        else
                        {
                            paint[i][j*2+1]=5;//5
                            for(int k=i+1;k<l;k++)
                            {
                                paint[k][j*2+1]=0xFD;
                            }
                            paint[l][j*2+1]=6;//6


                        }


                   }
               }

            }
        }
//        for(int i=15;i>=0;i--)
//        {
//            for(int j=0;j<63;j++)
//            {
//                printf("%d",paint[i][j]);
//            }
//            printf("\n");
//        }
//
//        getchar();

    }
    for(int i=15;i>=0;i--)
    {
        for(int j=0;j<64;j++)
        {
            if((high=paint[i][j])==0xFF)
            {
                if(i==8) printf("_");
                else
                printf(" ");
            }
            else if(high==0xFE) printf("-");
            else if(high==0xFD) printf("|");
//            else if(high==3) printf("ˇ");
//            else if(high==4) printf("ˇ");
//            else if(high==5) printf("^");
//            else if(high==6) printf("^");
            else
                printf("+");
        }
        printf("\n");
    }

    for(int i=0;i<32;i++)
    {
        printf("￣");
    }
    printf("\n");
    getchar();
    return 1;
}

int showVoiTab(int vnum,int tcount[],vtda_t vtd[])
{
    system("cls");
    fflush(stdin);
    printf("\n请选择:\t\t\t\t\tVoiceGroup[%X]\n",voiceoft[vnum]);
    printf("\n\t\tS.Multi Sample类型查看(%d)\n",tcount[5]);
    printf("\n\t\tA.Direct Sound类型查看(%d)\n",tcount[0]);
    printf("\n\t\tB.SquareWave 1类型查看(%d)\n",tcount[1]);
    printf("\n\t\tC.SquareWave 2类型查看(%d)\n",tcount[2]);
    printf("\n\t\tD.Wave Memory 类型查看(%d)\n",tcount[3]);
    printf("\n\t\tE.Noise 类型查看(%d)\n",tcount[4]);
    printf("\n\t\tF.Drum Part 类型查看(%d)\n",tcount[6]);
//    printf("\n\t\tG.VoiceTable纯数据查看(128)\n");
    printf("\n\t\t[ESC返回]\n");
    page=0;

    memset(samplefc,0xFF,128);
    //while(clean=getchar()!='\n'&&clean!=EOF);
//    vtda_t vtabgop[128]={0};
    switch(toupper(getch()))
    {
    case 'S':
        while(1)
        {
            system("cls");
            if(tcount[5]==0) return 1;
            pagetotal=tcount[5]/32+1;


            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n",voiceoft[vnum],page+1,pagetotal);
            printf("乐器组号  数据地址\tNew地址\t范围地址\n");

            tabgoptotal=0;
            for(int i=0;i<128;i++)
                if(vtd[i].tdat1<<24>>24==0x40&&
                   vtd[i].tdat1>>16==0&&
                    vtd[i].tdat2>>25<<1==8&&vtd[i].tdat3>>25<<1==8)
                    {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                        samplefc[tabgoptotal]=i;
                        tabgoptotal++;
                    }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %02d\t %07X  %07X\t%07X\t\n",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5,vtd[samplefc[k]].tdat2<<5>>5,vtd[samplefc[k]].tdat3<<5>>5);

            }

            printf("请输入要查看的乐器组号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 13:
                if(tcount[5]==1)
                    MultAndDrum(1,vtd[samplefc[0]].tdat2<<5>>5,vtd[samplefc[0]].tdat3<<5>>5);
                else continue;
            default:
                if(clean>47&&clean<58)
                {
                    printf("%c",clean);
                    std::string temp;
                    temp.resize(4);
                    int instr=0;
                    for(int j=1;j<4;j++)
                    {
                        temp[j]=getche();
                        if(temp[j]==13) break;
                        else if(temp[j]==8)
                        {
                            instr=1;
                            break;
                        }
                    }
                    printf("\n");
                    if(instr) continue;

                    temp[0]=clean;
                    sscanf(&temp[0],"%d",&instr);
                    if(instr>=0&&instr<128)
                    {
                        for(int i=0;i<tabgoptotal;i++)
                        {
                            if(instr!=samplefc[i]) continue;
//                            printf("jingguo");
//                            getchar();
//                            getchar();
                            MultAndDrum(1,vtd[instr].tdat2<<5>>5,vtd[instr].tdat3<<5>>5);
                        }
                        continue;

//                        getchar();
                    }
                    else
                    {
                        printf("歌曲号错误!");

                        getchar();
                        continue;
                    }

                }
                else if(clean==27)
                    return 1;
                else continue;
            }


    }

    case 'A':
        while(1)
        {
            system("cls");
            if(tcount[0]==0) return 1;
            pagetotal=tcount[0]/32+1;


            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号   数据地址  键值\t样本地址  Pan  修频  倒放  Atk Dec Sus Rel\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
                if(vtd[i].tdat1<<24>>24<0x40&&
                   vtd[i].tdat1<<29>>29==0&&
                    vtd[i].tdat1<<8>>24==0&&
                    vtd[i].tdat2>>25<<1==8)
                    {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                        samplefc[tabgoptotal]=i;
                        tabgoptotal++;
                    }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %d\t %07X    %d  %07X\t",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5,vtd[samplefc[k]].tdat1<<16>>24,vtd[samplefc[k]].tdat2<<5>>5);
                if(vtd[samplefc[k]].tdat1>>24==0)
                    printf("  无  ");
                else if((vtd[samplefc[k]].tdat1>>24)-0xC0)
                    printf("  %X  ",vtd[samplefc[k]].tdat1>>24);
                if((vtd[samplefc[k]].tdat1<<24&8)==8)
                    printf("  Yes   ");
                else printf("  No   ");
                if((vtd[samplefc[k]].tdat1<<24&0x10)==0x10)
                    printf(" Yes");
                else printf(" No");
                printf("   %03d %03d %03d %03d\n",vtd[samplefc[k]].tdat3<<24>>24,vtd[samplefc[k]].tdat3<<16>>24,vtd[samplefc[k]].tdat3<<8>>24,vtd[samplefc[k]].tdat3>>24);
            }

            printf("请输入要查看的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 13:
                if(tcount[0]==1)
                    showSampleDat(samplefc[0],vnum,vtd[samplefc[0]].tdat1<<16>>24);
                else continue;
            default:
                if(clean>47&&clean<58)
                {
                    printf("%c",clean);
                    std::string temp;
                    temp.resize(4);
                    int instr=0;
                    for(int j=1;j<4;j++)
                    {
                        temp[j]=getche();
                        if(temp[j]==13) break;
                        else if(temp[j]==8)
                        {
                            instr=1;
                            break;
                        }
                    }
                    printf("\n");
                    if(instr) continue;

                    temp[0]=clean;
                    sscanf(&temp[0],"%d",&instr);
                    if(instr>=0&&instr<128)
                    {
                        for(int i=0;i<tabgoptotal;i++)
                        {
                            if(instr!=samplefc[i]) continue;
//                            printf("jingguo");
//                            getchar();
//                            getchar();
                            showSampleDat(instr,vnum,vtd[instr].tdat1<<16>>24);
                        }
                        continue;

//                        getchar();
                    }
                    else
                    {
                        printf("歌曲号错误!");

                        getchar();
                        continue;
                    }

                }
                else if(clean==27)
                    return 1;
                else continue;
            }


    }

        break;
    case 'B':
        while(1)
        {
            system("cls");
            if(tcount[1]==0) return 1;
            pagetotal=tcount[1]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 SweepTime SweepShift SquarePattern Sus [Note Off With Noise]\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;

            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==1&&
            vtd[i].tdat1<<8>>24<0x80&&
            vtd[i].tdat1>>24==0&&
            vtd[i].tdat2<<24>>24<4&&
            vtd[i].tdat2>>8==0)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                samplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %03d   %07X",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5);
                int temp;
                if((temp=(vtd[samplefc[k]].tdat1<<8>>28))==0)
                    printf("  disable ");
                else if(temp==7)
                    printf("  54.7ms  ");
                else
                    printf("  %.2fms  ",(double)temp*7.8);

                if((temp=(vtd[samplefc[k]].tdat1<<12>>28))==0||temp==8)
                    printf("  disable    ");
                else if(temp<8)
                    printf("  add %dbit   ",1+temp);
                else
                    printf("  sub %dbit   ",temp-7);

                if((temp=vtd[samplefc[k]].tdat2)==0)
                    printf(" 12.5%%");
                else if(temp==1)
                    printf(" 25.0%%");
                else if(temp==2)
                    printf(" 50.0%%");
                else
                    printf(" 75.0%%");

                printf("        %03d ",vtd[samplefc[k]].tdat3<<8>>24);
                if((vtd[samplefc[k]].tdat1&8)==8)
                    printf("\t\tYes\n");
                else printf("\t\tNo\n");

            }

//            printf("请输入要查看的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 27:
                return 1;
            }
            continue;
        }
        break;
    case 'C':
        while(1)
        {
            system("cls");
//            while(clean=getchar()!='\n'&&clean!=EOF);
            if(tcount[2]==0) return 1;
            pagetotal=tcount[2]/32+1;
            printf("%d\n",tcount[2]);

            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 SquarePattern Sus [Note Off With Noise]\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==2&&
            vtd[i].tdat1<<8>>24==0&&
            vtd[i].tdat1>>24==0&&
            vtd[i].tdat2<<24>>24<4&&
            vtd[i].tdat2>>8==0)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                samplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %03d   %07X",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5);
                int temp;

                if((temp=vtd[samplefc[k]].tdat2)==0)
                    printf("   12.5%% ");
                else if(temp==1)
                    printf("   25.0%% ");
                else if(temp==2)
                    printf("   50.0%% ");
                else
                    printf("   75.0%% ");

                printf("       %03d ",vtd[samplefc[k]].tdat3<<8>>24);
                if((vtd[samplefc[k]].tdat1&8)==8)
                    printf("\tYes\n");
                else printf("\tNo\n");

            }
//            for(int i=0;i<7;i++)
//            {
//                printf("%d\n",tcount[i]);
//                getchar();
//            }

//            printf("请输入要查看的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 27:
                return 1;
            }
            continue;
        }
        break;
    case 'D':
        while(1)
        {
            system("cls");
            if(tcount[3]==0) return 1;

            pagetotal=tcount[3]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 波数据地址 Atk Dec Sus Rel   [Note Off With Noise]\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;

            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==3&&
            vtd[i].tdat1>>16==0&&
            vtd[i].tdat2>>25<<1==8)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                samplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %03d   %07X",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5);


                printf("  %07X ",vtd[samplefc[k]].tdat2<<5>>5);

                printf("   %03d %03d %03d %03d",vtd[samplefc[k]].tdat3<<24>>24,vtd[samplefc[k]].tdat3<<16>>24,vtd[samplefc[k]].tdat3<<8>>24,vtd[samplefc[k]].tdat3>>24);
                if((vtd[samplefc[k]].tdat1&8)==8)
                    printf("\t\tYes\n");
                else printf("\t\tNo\n");

            }

            printf("请输入要查看的波形数据的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 13:
                if(tcount[3]==1)
                    showWaveMemoryData(vtd[samplefc[0]].tdat2<<5>>5);
                else
                continue;
            default:
                if(clean>47&&clean<58)
                {
                    printf("%c",clean);
                    std::string temp;
                    temp.resize(4);
                    int instr=0;
                    for(int j=1;j<4;j++)
                    {
                        temp[j]=getche();
                        if(temp[j]==13) break;
                        else if(temp[j]==8)
                        {
                            instr=1;
                            break;
                        }
                    }
                    printf("\n");
                    if(instr) continue;

                    temp[0]=clean;
                    sscanf(&temp[0],"%d",&instr);
                    if(instr>=0&&instr<128)
                    {
                        for(int i=0;i<tabgoptotal;i++)
                        {
                            if(instr!=samplefc[i]) continue;
//                            printf("jingguo");
//                            getchar();
//                            getchar();
                            showWaveMemoryData(vtd[instr].tdat2<<5>>5);
                        }
                        continue;

//                        getchar();
                    }
                    else
                    {
                        printf("歌曲号错误!");

                        getchar();
                        continue;
                    }

                }
                else if(clean==27)
                    return 1;
                else continue;
            }
    }
        break;
    case 'E':
        while(1)
        {
            system("cls");
            if(tcount[4]==0) return 1;
            pagetotal=tcount[4]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
  NoisePattern     Sus\t[Note Off With Noise]\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==4&&
            vtd[i].tdat1>>16==0&&
            vtd[i].tdat2<<24>>24<2)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                samplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %03d   %07X",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5);
                int temp;

                if((temp=vtd[samplefc[k]].tdat2)==0)
                    printf("   32768 Samples ");
                else if(temp==1)
                    printf("   256 Samples   ");


                printf("   %03d ",vtd[samplefc[k]].tdat3<<8>>24);
                if((vtd[samplefc[k]].tdat1&8)==8)
                    printf("\t\tYes\n");
                else printf("\t\tNo\n");

            }

//            printf("请输入要查看的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 27:
                return 1;
            }
            continue;
        }
        break;
    case 'F':
        while(1)
        {
            system("cls");
            if(tcount[6]==0) return 1;
            pagetotal=tcount[6]/32+1;


            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号  数据地址  新乐器数据地址\n",voiceoft[vnum],page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
                if(vtd[i].tdat1<<24>>24==0x80&&
                    vtd[i].tdat1>>16==0&&
                    vtd[i].tdat2>>25<<1==8&&vtd[i].tdat3==0)
                    {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                        samplefc[tabgoptotal]=i;
                        tabgoptotal++;
                    }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(samplefc[k]==0xFF) break;
                printf(" %03d\t%07X\t\t%07X\n",samplefc[k],(voiceoft[vnum]+samplefc[k]*0xC)<<5>>5,vtd[samplefc[k]].tdat2<<5>>5);

            }

            printf("请输入要查看的乐器号: ");
            switch(clean=toupper(getch()))
            {
            case 'D':
                if(page!=pagetotal-1)
                    page++;
                continue;
            case 'A':
                if(page!=0)
                    page--;
                continue;
            case 'S':
                page=pagetotal-1;
                continue;
            case 'W':
                page=0;
                continue;
            case 13:
                if(tcount[6]==1)
                    MultAndDrum(0,vtd[samplefc[0]].tdat2<<5>>5,0);
                else continue;
            default:
                if(clean>47&&clean<58)
                {
                    printf("%c",clean);
                    std::string temp;
                    temp.resize(4);
                    int instr=0;
                    for(int j=1;j<4;j++)
                    {
                        temp[j]=getche();
                        if(temp[j]==13) break;
                        else if(temp[j]==8)
                        {
                            instr=1;
                            break;
                        }
                    }
                    printf("\n");
                    if(instr) continue;

                    temp[0]=clean;
                    sscanf(&temp[0],"%d",&instr);
                    if(instr>=0&&instr<128)
                    {
                        for(int i=0;i<tabgoptotal;i++)
                        {
                            if(instr!=samplefc[i]) continue;
//                            printf("jingguo");
//                            getchar();
//                            getchar();

                            MultAndDrum(0,vtd[instr].tdat2<<5>>5,0);
                        }
                        continue;

//                        getchar();
                    }
                    else
                    {
                        printf("歌曲号错误!");

                        getchar();
                        continue;
                    }

                }
                else if(clean==27)
                    return 1;
                else continue;
            }


        }

        break;
    case 27:
        return 0;
    }
           return 1;
}

int VoiGopInfo()
{
    while(1)
    {
        system("cls");
//            char clean;
//            while(clean=getchar()!='\n'&&clean!=EOF);
        printf("\t\t   VoiceGroup列表(序号-地址-曲数)\t\t(任意非数返回)\n");
        for(int v=0;v<voitotal;v++)
        {
            if(v%4==0) printf("\n");
            printf("   %02d %X  %04d",v+1,voiceoft[v+1],voitabNum[v]);

        }
        printf("\n\n请输入要查看样本的VoiceGroup序号: ");
        int num=0,b=0;
        if((b=scanf("%d",&num))==1&&num>0&&
            num<=voitotal&&voitabNum[num-1]!=0)
        {
            g_ipf=fopen(g_ipfName.c_str(),"rb");

//            printf("%08X\n",voiceoft[num]);
            fseek(g_ipf,voiceoft[num]<<5>>5,0);//跳到voicetabledata处

            vtda_t vtda[128];

            int typecount[7]={0};

            for(int i=0;i<128;i++)
            {
//                printf("%08X\n",(int)ftell(g_ipf));
                fread(&vtda[i],12,1,g_ipf);
//                printf("%X\n",vtda[i].tdat1);
//                printf("%X\n",vtda[i].tdat1<<24);
//                printf("%X\n",vtda[i].tdat1<<24>>24);
//                printf("%X\n",vtda[i].tdat1<<24>>24&0x40);
                if(vtda[i].tdat1<<24>>24==0x40&&
                   vtda[i].tdat1>>16==0&&
                   vtda[i].tdat2>>25<<1==8&&vtda[i].tdat3>>25<<1==8)
                {
                    typecount[5]++;
                    continue;
//                    printf("%X\n",vtda[i].tdat1<<24>>24&0x40);

                }
                else if(vtda[i].tdat1<<24>>24==0x80&&
                    vtda[i].tdat1>>16==0&&
                    vtda[i].tdat2>>25<<1==8&&vtda[i].tdat3==0)
                {
                    typecount[6]++;
//                    printf("%X\n",vtda[i].tdat1<<24>>24&0x80);
                    continue;

                }
                else
                {
//                    if(vtda[i].tdat1<<29>>29>4) break;
                    if(vtda[i].tdat1<<24>>24<0x40&&
                       vtda[i].tdat1<<29>>29==0&&
                            vtda[i].tdat1<<8>>24==0&&
                            vtda[i].tdat2>>25<<1==8)
                                typecount[0]++;
                    else if(vtda[i].tdat1<<29>>29<3&&
                            vtda[i].tdat1<<8>>24<0x80&&
                            vtda[i].tdat1>>24==0&&
                            vtda[i].tdat2<<24>>24<4&&
                            vtda[i].tdat2>>8==0)
                                typecount[vtda[i].tdat1<<29>>29]++;
                    else if(vtda[i].tdat1<<29>>29==3&&
                            vtda[i].tdat1>>16==0&&
                            vtda[i].tdat2>>25<<1==8)
                                typecount[3]++;
                    else if(vtda[i].tdat1<<29>>29==4&&
                            vtda[i].tdat1>>16==0&&
                            vtda[i].tdat2<<24>>24<2)
                                typecount[4]++;
                    else break;
                }

            }
////            tabgoptotal=0;
//            int total=0;
//            for(int i=0;i<7;i++) total+=typecount[i];
//            if(total!=128)
//            {
//                system("cls");
//                printf("\t\t\n不是标准的VoiceTable!\n\n\t\t类型统计可能有误!");
//                while(clean=getchar()!='\n'&&clean!=EOF);
//            }
//            while(clean=getchar()!='\n'&&clean!=EOF);
            while(showVoiTab(num,typecount,vtda));
            fclose(g_ipf);

            return 1;


        }
        else if(b==0)
        {
            while((clean=getchar())!='\n'&&clean!=EOF);
            return 0;
        }

        else if(num<1||num>voitotal)
        {
            printf("输入的序号不在VoiceGroup号范围内!\n");

            while((clean=getchar())!='\n'&&clean!=EOF);
            getchar();
        }

    }
}
