#include "main.h"
#include "showDat.h"
#include "getDat.h"


using namespace std;

uint8_t samplefc[128]={};//��������flag
int tabtotal=0;//table����
int efctabtal=0;//��Ч����������
int voitotal=0;//voicegroup����
uint8_t tralimit=1;//��ǰԤ����������

uint32_t voiceoft[100]={};//voicegroup�ĵ�ַ�洢

uint8_t tablefc[2000]={};//tabel��Чflag
uint8_t tabloopefc[2000]={};//table�Ƿ�ѭ��
uint8_t tabvoinum[2000]={};//ÿ������ʹ�õ�voice���
uint32_t headoft[2000]={};
uint32_t songGop[2000]={};

uint16_t tranum[2000]={};//ÿ������������Ŀ
uint16_t unknow[2000]={};//δ֪����
uint32_t voigop[2000]={};//voice group
uint32_t trackoft[2000][16]={0};//������ַ����

int tabstart=0;//���ſ�ʼ
int tabend=0;//���Ž���
int tabgop[2000]={};//ѡ�е�����
int tabgoptotal=0;//ѡ�е���������
uint16_t voitabNum[100]={};//ÿ��voicegroup�ĸ�����
uint8_t voisplNum[100]={};//ÿ��voicegroup��������

char clean='0';//ר��������ɨ������

//int gopgop[30]={};//ѡ�е�voice group��



void getData(uint32_t taboft,string ifn)
{
//    system("cls");
    g_ipf=fopen(ifn.c_str(),"rb");
    fseek(g_ipf,taboft,0);//��λ����λtable��
    tab_t tabel;//��ʱʹ�õ�table����
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

    tab_t tab[tabtotal];//����ʹ�õ�tab����
    head_t head[tabtotal];//����head������

    fseek(g_ipf,taboft,0);//�ٶ�λ
    for(int i=0;i<tabtotal;i++) fread(&tab[i],8,1,g_ipf);//����head��ַ��
    for(int i=0;i<tabtotal;i++)
    {
        fseek(g_ipf,(tab[i].headOft<<8>>8),0);
        fread(&head[i],72,1,g_ipf);//��ȡȫ��������
        if(feof(g_ipf)) continue;//head���ݲ����ļ���
        if(head[i].traNum>16||head[i].traNum<1) continue;//��������

        if(head[i].voigop>>24!=8) continue;//voicegroup�ǵ�ַ
        int j;
        for(j=0;j<=head[i].traNum;j++)//����һ����֤�����һ�������������
        if(head[i].trackoft[j]>>24!=8) break;//�������зǵ�ַ������
        if(j!=head[i].traNum) continue;//����Ƿ����һ������
        tablefc[i]=1;//��Ч������
        efctabtal++;
        for(int k=1;k<100;k++)
        {
            if(voiceoft[k]!=0) continue; //��λ����û��д��Ĳ���
            int l;
            for(l=1;l<=k;l++)//���֮ǰ�Ѿ�д��Ĳ���
            if(voiceoft[l]==head[i].voigop)
            {
                tabvoinum[i]=l;//��¼�����õ�voice���
                break;//ֻҪ����ȵľ�����ѭ��
            }
//            printf("voiceoft[%d]: %X\n",l,voiceoft[l]);
//            printf("head[i].voigop: %X\n",head[i].voigop);
//            printf("%d,%d\n",l,k);
            if(l==k+1)
            {
                voiceoft[k]=head[i].voigop;//����û�������д���µ�voice��ַ
                tabvoinum[i]=l-1;
                voitotal++;//ÿ��д���µ�voice��ַ����¼������
            }
            break;
        }
    }
    //������ת�Ƹ������ı�������ʹ��
    for(int i=0;i<tabtotal;i++)
    {
        headoft[i] = tab[i].headOft<<8>>8;
        songGop[i] = tab[i].songGop;
        tranum[i] = head[i].traNum;
        unknow[i] = head[i].unknow;
        voigop[i] = head[i].voigop<<8>>8;
        for(int j=0;j<tranum[i];j++)
        trackoft[i][j]=head[i].trackoft[j]<<8>>8;
        if(tablefc[i]!=1) continue;//��Ч����������
        voitabNum[tabvoinum[i]-1]++;//ÿ��voice������
        //�鿴ѭ�����
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
//        fseek(g_ipf,voiceoft[i]<<8>>8,0);//����vg��ַ
//
//        printf("%X\n",ftell(g_ipf));
//        getchar();
//    }
    fclose(g_ipf);
    while(_kbhit()) getch();//��ջ�����
}
