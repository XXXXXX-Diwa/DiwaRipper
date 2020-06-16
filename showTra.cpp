#include "main.h"
#include "showDat.h"
#include "getDat.h"
#include "showTra.h"

int page=0;//页数默认为1
int pagetotal=0;//总页数
int wide=0;


using namespace std;

int comp(const void*a,const void*b)
{
    return *(int*)a-*(int*)b;
}

void delRepeatab(int &n)
{
    //当前序号对应的歌曲无效,或大于歌曲总数或音轨数小于限制
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

void unRepeatNumGop()//接收并去除数组中的重复元素
{
    system("cls");
    printf("注: 会自动去掉重复-无效-越界-音轨数小于%d的曲号!\n请输入曲号(空格隔开): \n",tabtotal);
    char c;
    char temp[5]={};
    int tn=0;
    memset(tabgop,0,sizeof(int)*2000);
    int tabgopn=0;
    while(1)
    {
        c=getch();
        if(c==' '||c==13)//空格或者回车
        {
            if(tn!=0||c==13)//回车直通车
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

                delRepeatab(tabgopn);//去除重复,无效,越界
                if(c==13||tabgopn==499) break;    //回车直接结束
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
            printf("注: 会自动去掉重复-无效-越界-音轨数小于%d的曲号!\n请输入曲号(空格隔开): \n",tralimit);
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
    qsort(tabgop,tabgoptotal,sizeof(int),comp);//排序

//    while(clean=getchar()!='\n'&&clean!=EOF);
//    printf("选中的歌曲数为: %d",tabgoptotal);
//    getchar();
}

int trackShow(int num)
{
        system("cls");
        int c;
        int n=tabgop[num-1]-1;
//        int trasize;
        printf("歌曲%d的详细信息:                     ESC [返回]\n",n+1);
        printf("\nHEAD地址: %X  VoiceGroup: %X  轨道数: %d\n",headoft[n],voigop[n],tranum[n]);
        printf("\n轨号   起始地址    乐器\t\t\t\t\t [SIZE]\n");
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
    printf("注: [D->] [A<-] [W-^] [S-v] [ESC:返回] \t页: %d\t总页数%d\n",page+1,pagetotal);
    printf("序号    曲号      HEAD地址   轨数     VoiceGroup     SIZE    循环\n");

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
    printf("\n请输入要查看的序号: ");
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
        else if(clean>48&&clean<58)//是数字的话
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
                if(temp[j]==13)//换行
                {
                    temp[j]='\0';
                    break;
                }
                else if(temp[j]==8)//退格
                    return 1;
            }

            sscanf(&temp[0],"%d",&nu);
            fflush(stdin);
            if(nu>0&&nu<=tabgoptotal)
                return trackShow(nu);
            else
            {
                printf("\n请输入1-%d的数!",tabgoptotal);
//                while((clean=getchar())!='\n'&&clean!=EOF);
                getchar();
                return 1;
            }
        }
        else if(clean==27) //esc返回
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
        biggest/=10;//计算曲数的宽度
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
    printf("\n请选择:\t\t\t\t\t\tESC [返回]\n");
    printf("\n\tS.设置查看的曲子音轨数限制(1-9)--<%d>\n",tralimit);
    printf(
           "\n\tA.按曲号?至曲号?查看曲子详细信\n"
           "\n\tB.按VoiceGroup查看曲子详细信息\n"
           "\n\tC.自行输入曲号查看曲子详细信息\n"
           );
    switch(toupper(getch()))
    {
    case 'S':

        while(1)
        {
            system("cls");
            printf("\n请输入音轨限制数(1-9): ");
            clean=getch();
            if(clean>48&&clean<58)
            {
                tralimit=clean-48;//获取输入
                memset(voitabNum,0,sizeof(uint16_t)*voitotal);//清空vg歌曲数量组
                for(int i=0;i<tabtotal;i++)
                {
                    if(tablefc[i]!=1) continue;//无效歌曲跳过
                    if(tranum[i]>=tralimit)//当前歌曲音轨数如果大于限制的
                    //由于是从0存储,而序号从1算起,所以-1
                        voitabNum[tabvoinum[i]-1]++;//当前歌曲使用的vg的数量++
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
            printf("注:会自动忽略无效和音轨数小于%d的曲号\n请输入起始和结束序号(空格隔开): \n",tralimit);
            if(scanf("%d %d",&tabstart,&tabend)==2&&
               tabstart<=tabend&&tabstart>0&&tabend<=tabtotal)
            {
                tabgoptotal=0;
                for(int i=tabstart;i<=tabend;i++)
                {
                    //只要歌曲是无效的或者音轨数小于设定
                    if(tablefc[i-1]==0||tranum[i-1]<tralimit) continue;

                    tabgop[tabgoptotal]=i;
                    tabgoptotal++;
                }
                if(tabgoptotal==0)
                {
                    printf("范围中没有有效或符合音轨数的歌曲!");
                    fflush(stdin);
                    getchar();
                    continue;
                }
                showHead(tabgop[tabgoptotal-1]);
                return 1;
            }
            else
            {
                printf("\n输入错误!");
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
            printf("\t\t   VoiceGroup列表(序号-地址-曲数)--<%d>\n",tralimit);
            for(int v=0;v<voitotal;v++)
            {
                if(v%4==0) printf("\n");
                printf("   %02d %X  %04d",v+1,voiceoft[v+1],voitabNum[v]);

            }
            printf("\n\n注: 会自动去除音轨数小于%d的曲号(任意非数返回上一界面)\n请输入VoiceGroup序号: ",tralimit);
            int a,b=0;
            if((b=scanf("%d",&a))==1&&a>0&&
                a<=voitotal&&voitabNum[a-1]!=0)
            {
                tabgoptotal=0;
                for(int i=0;i<tabtotal;i++)
                {
                    if(tabvoinum[i]!=a||tranum[i]<tralimit) continue;
                    //顺序,重复,无效,越界的可能全都去除了
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
                printf("输入的序号不在VoiceGroup号范围内!\n");
                fflush(stdin);
                getchar();
            }
            else if(voitabNum[a-1]==0)
            {
                printf("你选择的VoiceGroup号歌曲数为0!\n");
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
