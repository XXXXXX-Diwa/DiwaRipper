#include "main.h"
#include "showDat.h"
#include "getDat.h"
#include "showTra.h"

int page=0;//ҳ��Ĭ��Ϊ1
int pagetotal=0;//��ҳ��
int wide=0;


using namespace std;

int comp(const void*a,const void*b)
{
    return *(int*)a-*(int*)b;
}

void delRepeatab(int &n)
{
    //��ǰ��Ŷ�Ӧ�ĸ�����Ч,����ڸ���������������С������
    if(tablefc[tabgop[n]-1]!=1||tabgop[n]>tabtotal||tranum[tabgop[n]-1]<tralimit)
    {
        tabgop[n]=0;
        n--;
        return;
    }

    for(int i=0;i<n;i++)
    {
        if(tabgop[n]==tabgop[i])
        {
            tabgop[n]=0;
            n--;
            return;
        }

    }
}

void unRepeatNumGop()//���ղ�ȥ�������е��ظ�Ԫ��
{
    system("cls");
    printf("ע: ���Զ�ȥ���ظ�-��Ч-Խ��-������С��%d������!\n����������(�ո����): \n",tabtotal);
    char c;
    char temp[5]={};
    int tn=0;
    memset(tabgop,0,sizeof(int)*2000);
    int tabgopn=0;
    while(1)
    {
        c=getch();
        if(c==' '||c==13)//�ո���߻س�
        {
            if(tn!=0||c==13)//�س�ֱͨ��
            {
                if(temp[3]!='\0')
                {
                    tabgop[tabgopn]+=temp[3]-48;
                    tabgop[tabgopn]+=(temp[2]-48)*10;
                    tabgop[tabgopn]+=(temp[1]-48)*100;
                    tabgop[tabgopn]+=(temp[0]-48)*1000;
                }
                else if(temp[2]!='\0')
                {
                    tabgop[tabgopn]+=temp[2]-48;
                    tabgop[tabgopn]+=(temp[1]-48)*10;
                    tabgop[tabgopn]+=(temp[0]-48)*100;
                }
                else if(temp[1]!='\0')
                {
                    tabgop[tabgopn]+=temp[1]-48;
                    tabgop[tabgopn]+=(temp[0]-48)*10;
                }
                else if(temp[0]!='\0')
                {
                    tabgop[tabgopn]=temp[0]-48;
                }

                delRepeatab(tabgopn);//ȥ���ظ�,��Ч,Խ��
                if(c==13||tabgopn==499) break;    //�س�ֱ�ӽ���
                tabgopn++;
                memset(temp,0,5);
                tn=0;
            }
        }
        else if(c>='0'&&c<='9')
        {
                if(tn==0&&c=='0') continue;
                else if(tn<=3)
                {
                    temp[tn]=c;
                    tn++;
                }
                else continue;
        }
        else if(c==8&&temp[0]!='\0')
        {
            system("cls");
            printf("ע: ���Զ�ȥ���ظ�-��Ч-Խ��-������С��%d������!\n����������(�ո����): \n",tralimit);
            tn=0;
            for(int i=0;i<2000;i++)
            {
                if(tabgop[i]==0) break;
                printf("%d ",tabgop[i]);
            }
            continue;
        }
        else continue;
        printf("%c",c);

    }
    for(tabgoptotal=0;tabgoptotal<2000;tabgoptotal++)
        if(tabgop[tabgoptotal]==0) break;
    qsort(tabgop,tabgoptotal,sizeof(int),comp);//����

//    while(clean=getchar()!='\n'&&clean!=EOF);
//    printf("ѡ�еĸ�����Ϊ: %d",tabgoptotal);
//    getchar();
}

int trackShow(int num)
{
        system("cls");
        int c;
        int n=tabgop[num-1]-1;
//        int trasize;
        printf("����%d����ϸ��Ϣ:                     ESC [����]\n",n+1);
        printf("\nHEAD��ַ: %X  VoiceGroup: %X  �����: %d\n",headoft[n],voigop[n],tranum[n]);
        printf("\n���   ��ʼ��ַ    ����\t\t\t\t\t [SIZE]\n");
        g_ipf=fopen(g_ipfName.c_str(),"rb");
        for(int i=0;i<tranum[n];i++)
        {
            c=0;
            printf(" %02d    %07X      ",i+1,trackoft[n][i]);
            fseek(g_ipf,trackoft[n][i],0);

            while(c!=0xB1)
            {
                c=fgetc(g_ipf);
                if(c==0xB3||c==0xB2)
                {
                    fseek(g_ipf,4,1);
                    continue;
                }
                if(c==0xBD)
                {
                    c=fgetc(g_ipf);
                    printf("%d ",c);

                }
            }
            printf("\n--------------------------------[%07X] ~ [%07X] == [%04X]\n",
                   trackoft[n][i],(unsigned int)ftell(g_ipf)-1,(unsigned int)ftell(g_ipf)-trackoft[n][i]);


        }
        fclose(g_ipf);
//        while(clean=getch()!='\n'&&clean!=EOF);
        while(getch()!=27);

    return 1;
}

int headate()
{
    system("cls");
    printf("ע: [D->] [A<-] [W-^] [S-v] [ESC:����] \tҳ: %d\t��ҳ��%d\n",page+1,pagetotal);
    printf("���    ����      HEAD��ַ   ����     VoiceGroup     SIZE    ѭ��\n");

    for(int i=page*20;i<(page+1)*20&&i<tabgoptotal;i++)
    {
        if(tabgoptotal<100) printf(" %02d     ",i+1);
        else printf("%03d     ",i+1);

        switch(wide)
        {
        case 1:
            printf(" %d  ",tabgop[i]);
            break;
        case 2:
            printf(" %02d ",tabgop[i]);
            break;
        case 3:
            printf("%03d ",tabgop[i]);
            break;
        case 4:
            printf("%04d",tabgop[i]);
            break;
        }

        printf("      %07X     %02d       %07X        %02X",
                    headoft[tabgop[i]-1],
                    tranum[tabgop[i]-1],
                    voigop[tabgop[i]-1],
                    (8+tranum[tabgop[i]-1]*4));
        if(tabloopefc[tabgop[i]-1]==0)
            printf("     No\n");
        else if(tabloopefc[tabgop[i]-1]==1)
            printf("     Yes\n");
        else
            printf("     ?\n");
    }
    printf("\n������Ҫ�鿴�����: ");
    while(1)
    {
        clean=toupper(getch());


        if(clean=='D'&&page!=pagetotal-1)
        {
            page++;
            return 1;
        }
        else if(clean=='A'&&page!=0)
        {
            page--;
            return 1;
        }
        else if(clean=='W'&&page!=0)
        {
            page=0;
            return 1;
        }
        else if(clean=='S'&&page!=pagetotal-1)
        {
            page=pagetotal-1;
            return 1;
        }
        else if(clean>48&&clean<58)//�����ֵĻ�
        {
            if(tabgoptotal==1) return trackShow(1);
            printf("%c",clean);
            string temp;
            temp.resize(5);
            int nu;
            temp[0]=clean;
            for(int j=1;j<4;j++)
            {
                temp[j]=getche();
                if(temp[j]==13)//����
                {
                    temp[j]='\0';
                    break;
                }
                else if(temp[j]==8)//�˸�
                    return 1;
            }

            sscanf(&temp[0],"%d",&nu);
            fflush(stdin);
            if(nu>0&&nu<=tabgoptotal)
                return trackShow(nu);
            else
            {
                printf("\n������1-%d����!",tabgoptotal);
//                while((clean=getchar())!='\n'&&clean!=EOF);
                getchar();
                return 1;
            }
        }
        else if(clean==27) //esc����
        return 0;
        else continue;
    }

}

int showHead(int biggest)
{
    page=0;
    wide=0;
    pagetotal=0;
    int temp=tabgoptotal;
    while(temp>0)
    {
        pagetotal++;
        temp-=20;

    }

    while(biggest)
    {
        wide++;
        biggest/=10;//���������Ŀ��
    }
    while(headate());

    return 0;
}

int tableInfo()
{
    system("cls");
    tabstart=0;
    tabend=0;
    memset(tabgop,0,sizeof(int)*2000);
    fflush(stdin);
    printf("\n��ѡ��:\t\t\t\t\t\tESC [����]\n");
    printf("\n\tS.���ò鿴����������������(1-9)--<%d>\n",tralimit);
    printf(
           "\n\tA.������?������?�鿴������ϸ��\n"
           "\n\tB.��VoiceGroup�鿴������ϸ��Ϣ\n"
           "\n\tC.�����������Ų鿴������ϸ��Ϣ\n"
           );
    switch(toupper(getch()))
    {
    case 'S':

        while(1)
        {
            system("cls");
            printf("\n����������������(1-9): ");
            clean=getch();
            if(clean>48&&clean<58)
            {
                tralimit=clean-48;//��ȡ����
                memset(voitabNum,0,sizeof(uint16_t)*voitotal);//���vg����������
                for(int i=0;i<tabtotal;i++)
                {
                    if(tablefc[i]!=1) continue;//��Ч��������
                    if(tranum[i]>=tralimit)//��ǰ��������������������Ƶ�
                    //�����Ǵ�0�洢,����Ŵ�1����,����-1
                        voitabNum[tabvoinum[i]-1]++;//��ǰ����ʹ�õ�vg������++
                }

                return 1;
            }
        }
    case 'A':
        while(1)
        {
            system("cls");
            tabstart=0;
            tabend=0;
            printf("ע:���Զ�������Ч��������С��%d������\n��������ʼ�ͽ������(�ո����): \n",tralimit);
            if(scanf("%d %d",&tabstart,&tabend)==2&&
               tabstart<=tabend&&tabstart>0&&tabend<=tabtotal)
            {
                tabgoptotal=0;
                for(int i=tabstart;i<=tabend;i++)
                {
                    //ֻҪ��������Ч�Ļ���������С���趨
                    if(tablefc[i-1]==0||tranum[i-1]<tralimit) continue;

                    tabgop[tabgoptotal]=i;
                    tabgoptotal++;
                }
                if(tabgoptotal==0)
                {
                    printf("��Χ��û����Ч������������ĸ���!");
                    fflush(stdin);
                    getchar();
                    continue;
                }
                showHead(tabgop[tabgoptotal-1]);
                return 1;
            }
            else
            {
                printf("\n�������!");
                fflush(stdin);
                getchar();
            }

        }
        break;
    case 'B':
        while(1)
        {
            system("cls");
//            char clean;
//            while(clean=getchar()!='\n'&&clean!=EOF);
            printf("\t\t   VoiceGroup�б�(���-��ַ-����)--<%d>\n",tralimit);
            for(int v=0;v<voitotal;v++)
            {
                if(v%4==0) printf("\n");
                printf("   %02d %X  %04d",v+1,voiceoft[v+1],voitabNum[v]);

            }
            printf("\n\nע: ���Զ�ȥ��������С��%d������(�������������һ����)\n������VoiceGroup���: ",tralimit);
            int a,b=0;
            if((b=scanf("%d",&a))==1&&a>0&&
                a<=voitotal&&voitabNum[a-1]!=0)
            {
                tabgoptotal=0;
                for(int i=0;i<tabtotal;i++)
                {
                    if(tabvoinum[i]!=a||tranum[i]<tralimit) continue;
                    //˳��,�ظ�,��Ч,Խ��Ŀ���ȫ��ȥ����
                    tabgop[tabgoptotal]=i+1;
                    tabgoptotal++;
                }
                showHead(tabgop[tabgoptotal-1]);
                return 1;

            }
            else if(b==0)
            {
                while((clean=getchar())!='\n'&&clean!=EOF);
                return 1;
            }

            else if(a<1||a>voitotal)
            {
                printf("�������Ų���VoiceGroup�ŷ�Χ��!\n");
                fflush(stdin);
                getchar();
            }
            else if(voitabNum[a-1]==0)
            {
                printf("��ѡ���VoiceGroup�Ÿ�����Ϊ0!\n");
                fflush(stdin);
                getchar();
            }


        }

        break;
    case 'C':
        unRepeatNumGop();
        showHead(tabgop[tabgoptotal-1]);
        return 1;

        break;
    case 27:
        return 0;
    }
    return 1;
}
