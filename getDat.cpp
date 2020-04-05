#include "main.h"
#include "showDat.h"
#include "getDat.h"


using namespace std;

uint8_t samplefc[128]={};//样本导出flag
int tabtotal=0;//table总数
int efctabtal=0;//有效的曲子总数
int voitotal=0;//voicegroup总数
uint8_t tralimit=1;//当前预览音轨限制

uint32_t voiceoft[100]={};//voicegroup的地址存储

uint8_t tablefc[2000]={};//tabel有效flag
uint8_t tabloopefc[2000]={};//table是否循环
uint8_t tabvoinum[2000]={};//每个曲子使用的voice序号
uint32_t headoft[2000]={};
uint32_t songGop[2000]={};

uint16_t tranum[2000]={};//每个歌曲音轨数目
uint16_t unknow[2000]={};//未知数据
uint32_t voigop[2000]={};//voice group
uint32_t trackoft[2000][16]={0};//歌曲地址数组

int tabstart=0;//曲号开始
int tabend=0;//曲号结束
int tabgop[2000]={};//选中的曲号
int tabgoptotal=0;//选中的曲号总数
uint16_t voitabNum[100]={};//每个voicegroup的歌曲数
uint8_t voisplNum[100]={};//每个voicegroup的样本数

char clean='0';//专门用来清扫缓冲区

//int gopgop[30]={};//选中的voice group号



void getData(uint32_t taboft,string ifn)
{
//    system("cls");
    g_ipf=fopen(ifn.c_str(),"rb");
    fseek(g_ipf,taboft,0);//定位到首位table处
    tab_t tabel;//临时使用的table数据
    while(!feof(g_ipf))
    {
        fread(&tabel,8,1,g_ipf);
        char cmp[3];
        cmp[0]=tabel.headOft>>25<<1;
        cmp[1]=tabel.songGop<<16>>24;// 20 01 10 01
        cmp[2]=tabel.songGop>>24;
        if(cmp[0]!=8||cmp[1]!=0||cmp[2]!=0) break;
        ++tabtotal;
    }

    tab_t tab[tabtotal];//真正使用的tab数据
    head_t head[tabtotal];//定义head数据组

    fseek(g_ipf,taboft,0);//再定位
    for(int i=0;i<tabtotal;i++) fread(&tab[i],8,1,g_ipf);//读入head地址等
    for(int i=0;i<tabtotal;i++)
    {
        fseek(g_ipf,(tab[i].headOft<<8>>8),0);
        fread(&head[i],72,1,g_ipf);//读取全部的数据
        if(feof(g_ipf)) continue;//head数据不在文件内
        if(head[i].traNum>16||head[i].traNum<1) continue;//曲数不对

        if(head[i].voigop>>24!=8) continue;//voicegroup非地址
        int j;
        for(j=0;j<=head[i].traNum;j++)//多检查一个保证非最后一个音轨错误跳出
        if(head[i].trackoft[j]>>24!=8) break;//音轨中有非地址则跳出
        if(j!=head[i].traNum) continue;//检查是否到最后一个音轨
        tablefc[i]=1;//有效的曲子
        efctabtal++;
        for(int k=1;k<100;k++)
        {
            if(voiceoft[k]!=0) continue; //定位到还没有写入的部分
            int l;
            for(l=1;l<=k;l++)//检查之前已经写入的部分
            if(voiceoft[l]==head[i].voigop)
            {
                tabvoinum[i]=l;//记录曲子用的voice序号
                break;//只要有相等的就跳出循环
            }
//            printf("voiceoft[%d]: %X\n",l,voiceoft[l]);
//            printf("head[i].voigop: %X\n",head[i].voigop);
//            printf("%d,%d\n",l,k);
            if(l==k+1)
            {
                voiceoft[k]=head[i].voigop;//遍历没有相等则写入新的voice地址
                tabvoinum[i]=l-1;
                voitotal++;//每次写入新的voice地址都记录新数量
            }
            break;
        }
    }
    //把数据转移给声明的变量方便使用
    for(int i=0;i<tabtotal;i++)
    {
        headoft[i] = tab[i].headOft<<8>>8;
        songGop[i] = tab[i].songGop;
        tranum[i] = head[i].traNum;
        unknow[i] = head[i].unknow;
        voigop[i] = head[i].voigop<<8>>8;
        for(int j=0;j<tranum[i];j++)
        trackoft[i][j]=head[i].trackoft[j]<<8>>8;
        if(tablefc[i]!=1) continue;//无效的曲子跳过
        voitabNum[tabvoinum[i]-1]++;//每个voice的曲数
        //查看循环情况
        uint32_t c=0;
        if(tranum[i]<2)
        {
            for(int j=0;j<tranum[i]-1;j++)
            {
                fseek(g_ipf,trackoft[i][j+1]-1,0);
                c=fgetc(g_ipf);
                if(c==0xB1)
                {
                    fseek(g_ipf,-9,1);
                    fread(&c,4,1,g_ipf);

                    if(c>>24==0xB2||c<<8>>24==0xB2
                       ||c<<24>>24==0xB2||c<<16>>24==0xB2)
                    {
                        tabloopefc[i]=1;
                        break;
                    }
                    else
                    {
                        fseek(g_ipf,-155,1);
                        int k=150;
                        while(k)
                        {
                            c=fgetc(g_ipf);
                            if(c==0xB2)
                            {
                                tabloopefc[i]=1;
                                break;
                            }
                        }

                    }
                }
                else
                {
                    tabloopefc[i]=2;
                    break;
                }
                tabloopefc[i]=0;
            }

        }
        else
        {
            fseek(g_ipf,trackoft[i][0]+4,0);
            while(c==0xB1)
            {
                c=fgetc(g_ipf);
                if(c==0xB2)
                {
                    tabloopefc[i]=1;
                    break;
                }

            }


        }

    }
//    for(int i=0;i<voitotal;i++)
//    {
//        fseek(g_ipf,voiceoft[i]<<8>>8,0);//跳到vg地址
//
//        printf("%X\n",ftell(g_ipf));
//        getchar();
//    }
    fclose(g_ipf);
    while(_kbhit()) getch();//清空缓冲区
}
