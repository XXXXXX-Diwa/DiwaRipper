#include "main.h"
#include "showDat.h"
#include "getDat.h"
#include "showTra.h"
#include "showSam.h"
#include<windows.h>
#include<mmsystem.h>
#include "MultAndDrum.h"
#include "playSample.h"
//#include <cstring>

int nshowSampleDat(int samNum,int ntab,int curkey,int damageflag,int damageNum,int damageoffset)
{
    while(1)
    {
        system("cls");
        fflush(stdin);
        uint32_t samHead[4]={};
        fseek(g_ipf,ntab+samNum*0xC+4,0);
        fread(samHead,4,1,g_ipf);
        fseek(g_ipf,samHead[0]<<5>>5,0);
        fread(samHead,0x10,1,g_ipf);
        int hoffset=ftell(g_ipf);
        int wavFren=originalFre(samHead[1]>>8,curkey);
        printf("VoiceGroup: %X\t乐器号: %d   [ESC: 返回]   [回车:播放]\n",ntab,samNum);
        printf("\t\nHEAD数据地址: %d / %07X\n",(int)hoffset-0x10,(int)hoffset-0x10);
        printf("\t\n样本数据地址: %d / %07X\n",(int)hoffset,(int)hoffset);
        printf("\t\n未转化前频率: %d / %X\n",wavFren,wavFren);
        printf("\t\n转化后的频率: %06d x 4 = %06d / %X\n",(samHead[1]>>8)/4,samHead[1]>>8,samHead[1]>>8);
        printf("\t\n循环偏移尺寸: %d / %X\n",samHead[2],samHead[2]);
        printf("\t\n样本数据尺寸: %d / %X\n",samHead[3],samHead[3]);
        printf("\t\n全部数据尺寸: %d / %X\n",samHead[3]+0x13-(samHead[0]>>30),samHead[3]+0x13-(samHead[0]>>30));
        if(damageflag)
        {
            uint8_t damagedata[128];
            fseek(g_ipf,damageoffset,0);
            fread(damagedata,128,1,g_ipf);
            uint8_t se[2]={};
            for(int i=0;i<128;i++)
            {
                if(se[0]==0&&damagedata[i]==damageNum)
                    se[0]=i;
                else if(damagedata[i]==damageNum)
                    se[1]=i;
            }
            if(se[1])
            printf("\n琴键范围为: %d ~ %d\n",se[0],se[1]);
            else
            printf("\n琴键为: %d\n",se[0]);
        }
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

int showNewVoiTab(int flag,int ncount[],vtda_t vtd[],int ntable,int damageoff)
{
    system("cls");
    fflush(stdin);
    printf("\n请选择:\t\t\t\t\tVoiceGroup[%X]\n",ntable);
    printf("\n\t\tS.Multi Sample类型查看(%d) [禁止]\n",ncount[5]);
    printf("\n\t\tA.Direct Sound类型查看(%d)\n",ncount[0]);
    printf("\n\t\tB.SquareWave 1类型查看(%d)\n",ncount[1]);
    printf("\n\t\tC.SquareWave 2类型查看(%d)\n",ncount[2]);
    printf("\n\t\tD.Wave Memory 类型查看(%d)\n",ncount[3]);
    printf("\n\t\tE.Noise 类型查看(%d)\n",ncount[4]);
    printf("\n\t\tF.Drum Part 类型查看(%d) [禁止]\n",ncount[6]);
//    printf("\n\t\tG.VoiceTable纯数据查看(128)\n");
    printf("\n\t\t[ESC返回]\n");
    page=0;
    uint8_t nsamplefc[128];
    memset(nsamplefc,0xFF,128);
    //while(clean=getchar()!='\n'&&clean!=EOF);
//    vtda_t vtabgop[128]={0};
    switch(toupper(getch()))
    {
    case 'A':
        while(1)
        {
            system("cls");
            //if(ncount[0]==0) return 1;
            pagetotal=ncount[0]/32+1;

            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号   数据地址  键值\t样本地址  Pan  修频  倒放  Atk Dec Sus Rel\n",ntable,page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
                if(vtd[i].tdat1<<24>>24<0x40&&
                   vtd[i].tdat1<<29>>29==0&&
                    vtd[i].tdat1<<8>>24==0&&
                    vtd[i].tdat2>>25<<1==8)
                    {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                        nsamplefc[tabgoptotal]=i;
                        tabgoptotal++;
                    }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(nsamplefc[k]==0xFF) break;
                printf(" %d\t %07X    %d  %07X\t",nsamplefc[k],(ntable+nsamplefc[k]*0xC)<<8>>8,vtd[nsamplefc[k]].tdat1<<16>>24,vtd[nsamplefc[k]].tdat2<<5>>5);
                if(vtd[nsamplefc[k]].tdat1>>24==0)
                    printf("  无  ");
                else
                    printf("  %X  ",vtd[nsamplefc[k]].tdat1>>24);
                if((vtd[nsamplefc[k]].tdat1<<24&8)==8)
                    printf("  Yes   ");
                else printf("  No   ");
                if((vtd[nsamplefc[k]].tdat1<<24&0x10)==0x10)
                    printf(" Yes");
                else printf(" No");
                printf("   %03d %03d %03d %03d\n",vtd[nsamplefc[k]].tdat3<<24>>24,vtd[nsamplefc[k]].tdat3<<16>>24,vtd[nsamplefc[k]].tdat3<<8>>24,vtd[nsamplefc[k]].tdat3>>24);
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
                if(ncount[0]==1)
                    nshowSampleDat(nsamplefc[0],ntable,vtd[nsamplefc[0]].tdat1<<16>>24,flag,0,damageoff);
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
                            if(instr!=nsamplefc[i]) continue;
//                            printf("jingguo");
//                            getchar();
//                            getchar();
                            nshowSampleDat(instr,ntable,vtd[instr].tdat1<<16>>24,flag,i,damageoff);

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
            if(ncount[1]==0) return 1;
            pagetotal=ncount[1]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 SweepTime SweepShift SquarePattern Sus [Note Off With Noise]\n",ntable,page+1,pagetotal);

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
                nsamplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(nsamplefc[k]==0xFF) break;
                printf(" %03d   %07X",nsamplefc[k],(ntable+nsamplefc[k]*0xC)<<8>>8);
                int temp;
                if((temp=(vtd[nsamplefc[k]].tdat1<<8>>28))==0)
                    printf("  disable ");
                else if(temp==7)
                    printf("54.7ms    ");
                else
                    printf("%.2fms    ",(double)temp*7.8);

                if((temp=(vtd[nsamplefc[k]].tdat1<<12>>28))==0||temp==8)
                    printf("  disable    ");
                else if(temp<8)
                    printf("  add %dbit   ",1+temp);
                else
                    printf("  sub %dbit   ",temp-7);

                if((temp=vtd[nsamplefc[k]].tdat2)==0)
                    printf("12.5%% ");
                else if(temp==1)
                    printf("25.0%% ");
                else if(temp==2)
                    printf("50.0%% ");
                else
                    printf("75.0%% ");

                printf("        %03d ",vtd[nsamplefc[k]].tdat3<<8>>24);
                if((vtd[nsamplefc[k]].tdat1&8)==8)
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
            if(ncount[2]==0) return 1;
            pagetotal=ncount[2]/32+1;
            printf("%d\n",ncount[2]);

            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 SquarePattern Sus [Note Off With Noise]\n",ntable,page+1,pagetotal);

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
                nsamplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(nsamplefc[k]==0xFF) break;
                printf(" %03d   %07X",nsamplefc[k],(ntable+nsamplefc[k]*0xC)<<5>>5);
                int temp;

                if((temp=vtd[nsamplefc[k]].tdat2)==0)
                    printf("   12.5%% ");
                else if(temp==1)
                    printf("   25.0%% ");
                else if(temp==2)
                    printf("   50.0%% ");
                else
                    printf("   75.0%% ");

                printf("       %03d ",vtd[nsamplefc[k]].tdat3<<8>>24);
                if((vtd[nsamplefc[k]].tdat1&8)==8)
                    printf("\tYes\n");
                else printf("\tNo\n");

            }
//            for(int i=0;i<7;i++)
//            {
//                printf("%d\n",ncount[i]);
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
            if(ncount[3]==0) return 1;

            pagetotal=ncount[3]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
 波数据地址 Atk Dec Sus Rel   [Note Off With Noise]\n",ntable,page+1,pagetotal);

            tabgoptotal=0;

            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==3&&
            vtd[i].tdat1>>16==0&&
            vtd[i].tdat2>>25<<1==8)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                nsamplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(nsamplefc[k]==0xFF) break;
                printf(" %03d   %07X",nsamplefc[k],(ntable+nsamplefc[k]*0xC)<<5>>5);


                printf("  %07X ",vtd[nsamplefc[k]].tdat2<<5>>5);

                printf("   %03d %03d %03d %03d",vtd[nsamplefc[k]].tdat3<<24>>24,vtd[nsamplefc[k]].tdat3<<16>>24,vtd[nsamplefc[k]].tdat3<<8>>24,vtd[nsamplefc[k]].tdat3>>24);
                if((vtd[nsamplefc[k]].tdat1&8)==8)
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
                if(ncount[3]==1)
                    showWaveMemoryData(vtd[nsamplefc[0]].tdat2<<5>>5);
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
                            if(instr!=nsamplefc[i]) continue;
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
            if(ncount[4]==0) return 1;
            pagetotal=ncount[4]/32+1;
            printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] VoiceGroup[%X] 页: %d 总页: %d\n乐器号 数据地址\
  NoisePattern     Sus\t[Note Off With Noise]\n",ntable,page+1,pagetotal);

            tabgoptotal=0;
            for(int i=0;i<128;i++)
            if(vtd[i].tdat1<<29>>29==4&&
            vtd[i].tdat1>>16==0&&
            vtd[i].tdat2<<24>>24<2)
            {
//                        vtabgop[tabgoptotal]=vtd[i];
//                        tabgoptotal++;
                nsamplefc[tabgoptotal]=i;
                tabgoptotal++;
            }
            for(int k=page*32;k<page*32+32;k++)
            {
                if(nsamplefc[k]==0xFF) break;
                printf(" %03d   %07X",nsamplefc[k],(ntable+nsamplefc[k]*0xC)<<8>>8);
                int temp;

                if((temp=vtd[nsamplefc[k]].tdat2)==0)
                    printf("   32768 Samples ");
                else if(temp==1)
                    printf("   256 Samples   ");


                printf("   %03d ",vtd[nsamplefc[k]].tdat3<<8>>24);
                if((vtd[nsamplefc[k]].tdat1&8)==8)
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

    case 27:
        return 0;
    }
           return 1;
}


int MultAndDrum(int dama,int newTable,int damaoffset)
{
    while(1)
    {
        system("cls");


//            printf("%08X\n",voiceoft[num]);
            fseek(g_ipf,newTable,0);//跳到voicetabledata处

            vtda_t newtda[128];
            int typecount[7]={0};

            for(int i=0;i<128;i++)
            {
//                printf("%08X\n",(int)ftell(g_ipf));
                fread(&newtda[i],12,1,g_ipf);

                if(newtda[i].tdat1<<24>>24==0x40&&
                   newtda[i].tdat1>>16==0&&
                   newtda[i].tdat2>>25<<1==8&&newtda[i].tdat3>>25<<1==8)
                {
                    typecount[5]++;
                    continue;
//                    printf("%X\n",newtda[i].tdat1<<24>>24&0x40);

                }
                else if(newtda[i].tdat1<<24>>24==0x80&&
                    newtda[i].tdat1>>16==0&&
                    newtda[i].tdat2>>25<<1==8&&newtda[i].tdat3==0)
                {
                    typecount[6]++;
//                    printf("%X\n",newtda[i].tdat1<<24>>24&0x80);
                    continue;

                }
                else
                {
//                    if(newtda[i].tdat1<<29>>29>4) break;
                    if(newtda[i].tdat1<<24>>24<0x40&&
                            newtda[i].tdat1<<29>>29==0&&
                            newtda[i].tdat1<<8>>24==0&&
                            newtda[i].tdat2>>25<<1==8)
                                typecount[0]++;
                    else if(newtda[i].tdat1<<29>>29<3&&
                            newtda[i].tdat1<<8>>24<0x80&&
                            newtda[i].tdat1>>24==0&&
                            newtda[i].tdat2<<24>>24<4&&
                            newtda[i].tdat2>>8==0)
                                typecount[newtda[i].tdat1<<29>>29]++;
                    else if(newtda[i].tdat1<<29>>29==3&&
                            newtda[i].tdat1>>16==0&&
                            newtda[i].tdat2>>25<<1==8)
                                typecount[3]++;
                    else if(newtda[i].tdat1<<29>>29==4&&
                            newtda[i].tdat1>>16==0&&
                            newtda[i].tdat2<<24>>24<2)
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
            while(showNewVoiTab(dama,typecount,newtda,newTable,damaoffset));
            return 1;

        }
}
