#include "main.h"
#include "tabSearch.h"

using namespace std;

FILE* searchF=nullptr;
uint32_t offset32=0;
uint32_t srhFst=0;//�ĵ���¼�ĵ�ַ
uint32_t iptFst=0;//�ֶ�����ĵ�ַ
//uint8_t bit8=0;
fpos_t curOffset=0;
char tableData[16]={};
char headData[72]={};

string getRomName(string s)
{
    size_t pos=s.find_last_of('\\');
    if(pos>0&&pos!=string::npos)
        return s.substr(pos+1);
    return s;
}

string onlyPath(string s)
{
    size_t pos = s.find_last_of('\\');
    if(pos>0&&pos!=string::npos)
        return s.substr(0,pos+1);
    return s;
}

string getExtension(string s)
{
    size_t pos=s.find_last_of('.');
    if(pos>0&&pos!=string::npos)
        return s.substr(pos+1);
    return "";
}

uint32_t offsetGet(char* tabDat)
{
    uint32_t offset=0;
    for(int g=3;g>=0;g--)
    {
        offset+=(uint8_t)tabDat[g];
        if(g!=0) offset=offset<<8;
    }
    return offset<<5>>5;
}

int oldTabChk(uint32_t toft)//���ɵ�table��¼�Ƿ�����ȷ��table
{
    fseek(g_ipf,toft,0);//��λ��table
    fread(&offset32,4,1,g_ipf);//��ȡtable��head��ַ
    offset32=offset32<<5>>5;//ȥ��08
    fseek(g_ipf,offset32,0);//��λ��head

    int c;
    fread(&c,4,1,g_ipf);//��ȡ������Ϣ
    if(feof(g_ipf)!=0) return 0;//����Ѿ����ļ�ĩλ
    c=c<<24>>24;        //������������

    if(c>0x10||c<=0) return 0;//���������Է���0
    fread(headData,1,(c+1)*4,g_ipf);//��ȡvoicegb��track�ĵ�ַ

    for(int h=3;h<(c+1)*4;h+=4)
    {
        if(headData[h]>>1<<1!=8) return 0;//����Ƿ��ǵ�ַ
    }

    for(int j=1;j<=c;j++)
    {
        int z=0;

        fseek(g_ipf,offsetGet(headData+j*4),0);//���ÿ�������ͷ����
        z =fgetc(g_ipf);

        if(z!=0xBC&&z!=0xBE) return 0;//BE�Ǻ��������
    }
    return 1;
}

int srhChoose()
{
    string tabset="";
    string offset="";
    int tras=1;//Ĭ��Ҫ������������
    printf(
           "��ǰ��ROMΪ: %s\n",getRomName(g_ipfName).c_str());
    printf(
           "\n��ѡ��:\n"
           "\n\t\tA.������Table��ַ����\n"
           "\n\t\tB.�ֶ�����Table��ַ\n"
           "\n\t\tC.����������������Table��ַ\n"
           "\n\t\tD.��ROM�м俪ʼ����\n"
           "\n\t\tE.�������(�ʺϷǱ�׼ROM)\n"
           "\n\t\t�س�:��¼��Table��ַ"
           );
    if(srhFst==0) printf("(�޼�¼)\n");
    else printf("(0x%X)\n",srhFst);

    switch (toupper(getch()))
    {
        case 'A':
            printf("\n���ڼ�����..\n");
            if(!searchTab(-4,1))
            {
                system("cls");
                printf("δ��Ѱ��Table��ַ!\n");
                return 0;
            }
            break;
        case 'B':
            printf("\n������Table��ʮ�����Ƶ�ַ!(0x��ͷ): \n");

            tabset.resize(10);
            scanf("%s",&tabset[0]);
            fflush(stdin);
            if(tabset[0]!='0'||tabset[1]!='x'||tabset[2]=='\0')
            {
                system("cls");
                printf("�����Table��ַ��ʽ����!\n");
                return 0;
            }
            tabset=&tabset[2];//ȥ��0x
            sscanf(&tabset[0],"%X",&iptFst);
            if(!oldTabChk(iptFst))
            {
                system("cls");
                printf("�����Table��ַ!\n");
                return 0;
            }

            tabOffset=iptFst;//ת��
            break;
        case 'C':
            printf("\n������Ҫ��Ѱ�����ӵ�������(1-10): \n");
            scanf("%d",&tras);
            if(tras<1||tras>10)
            {
                system("cls");
                printf("�����У��������Ŀ����!\n");
                return 0;
            }
            if(!searchTab(-4,tras))
            {
                system("cls");
                printf("δ��Ѱ��Table��ַ!\n");
                return 0;
            }
            break;
        case 'D':
            printf("\n�������������ʼʮ����������!(0x��ͷ):\n");

            offset.resize(10);
            scanf("%s",&offset[0]);
            if(offset[0]!='0'||offset[1]!='x'||offset[2]=='\0')
            {
                system("cls");
                printf("�����Table��ַ��ʽ����!\n");
                return 0;
            }
            offset=&offset[2];//ȥ��0x
            uint32_t temp;
            sscanf(&offset[0],"%X",&temp);
            temp=temp/4*4;
//            printf("%X\n",temp);
            fseek(g_ipf,temp,0);
            if(!searchTab(-4,tras))
            {
                system("cls");
                printf("δ��Ѱ��Table��ַ!\n");
                return 0;
            }
            break;
        case 'E':
            printf("\n���Ե�,������Ҫ�ϳ�ʱ��....\n");
            if(!searchTab(-7,tras))
            {
                system("cls");
                printf("δ������Table��ַ!\n");
                return 0;
            }
            break;
            case 13:
            if(srhFst==0)
            {
                system("cls");
                printf("û�е�ǰROM��Table��ַ��¼!\n");
                return 0;
            }
            tabOffset=srhFst;
            return 1;
        default:
            system("cls");
            return 0;
    }
    return 1;
}
int headDataCmp()
{
    while(!feof(searchF))
    {
        for(int ten=0;ten<0x10;ten++)
        {
//            printf("headData[%d]= %c\n",ten,headData[ten]);
//            printf("tableData[%d]= %c\n",ten,tableData[ten]);
//            getchar();
            if(headData[ten]==tableData[ten])
            {
                if(ten==15) return 1;
            }

            else break;
        }
        while(!feof(searchF))
        {
            char c =fgetc(searchF);
            if(c=='\n') break;
        }
        fread(tableData,0x10,1,searchF);
        for(int i=0;i<0x10;i++)
            if(tableData[i]==0) tableData[i]=32;
//        puts(tableData);
//        puts(headData);
    }
    return 0;
}

int searchTab(int Lv,int traNum)
{
    while(1)
    {
        fread(tableData,8,1,g_ipf);
        if(feof(g_ipf)) return 0;
        fseek(g_ipf,Lv,1);
        if(tableData[3]>>1<<1==8&&tableData[5]==0&&tableData[7]==0)
        {
            offset32=offsetGet(tableData);//���ܵ�head��ַ
            fseek(searchF,offset32,0);//ת��head��ַ��
            int c;
            fread(&c,4,1,searchF);//��ȡ��������ֵ
            if(feof(searchF)!=0) continue;
            c=c<<24>>24;
            if(c>0x10||c<traNum) continue;

            fread(headData,1,(c+1)*4,searchF);
            for(int h=3;h<(c+1)*4;h+=4)
            {
                if(headData[h]!=8) break;
                if(h==((c+1)*4)-1)
                {
                    int z=0;
                    for(int j=1;j<=c;j++)
                    {
                        fseek(searchF,offsetGet(headData+j*4),0);
                        z =fgetc(searchF);
//                        printf("%X\n",z);
                        if(z!=0xBC&&z!=0xBE) break;
                    }
                    if(z!=0xBC&&z!=0xBE) break;
                    fgetpos(g_ipf,&curOffset);
//                    printf("%X\n",curOffset);
                    tabOffset = curOffset-(8+Lv);
                    return 1;
                }
            }
        }
    }
}

int tabGet(std::string ipfn)
{
    string oftPath=onlyPath(SWpath)+"search.oft";//ʹoft�ļ��̶������Ŀ¼
//    printf("%s\n",oftPath.c_str());
    g_ipf=fopen(ipfn.c_str(),"rb");//�����Ʒ�ʽ���ļ���
    if(g_ipf==nullptr) exit(1);
    if(getExtension(ipfn)!="gba"&&getExtension(ipfn)!="GBA")
    {
        printf("ROM����չ������!(%s)\n",getExtension(ipfn).c_str());
        fclose(g_ipf);
        getchar();
        exit(1);
    }
    if(access(oftPath.c_str(),F_OK)==-1)//����ļ��Ƿ����
    {
        searchF=fopen(oftPath.c_str(),"w+");//�����ļ�
        fclose(searchF);
    }
    else
    {
        fseek(g_ipf,0xA0,0);//��λ��ͷ�ؼ�����
        fread(headData,0x10,1,g_ipf);
        searchF=fopen(oftPath.c_str(),"rt+");//���ı������д
        fread(tableData,0x10,1,searchF);
        for(int i=0;i<0x10;i++)
        {
            if(headData[i]==0) headData[i]=' ';
            if(tableData[i]==0) tableData[i]=32;
        }

        if(headDataCmp())
        {
            fseek(searchF,1,1);//����һ���ո�
            memset(headData,0,0x10);//����ַ�����
            fread(headData,8,1,searchF);//��ȡ�ı��м�¼��table��ַ
            sscanf(headData,"%X",&srhFst);//תΪ����
//            printf("��ROM��ȥTable��¼Ϊ: %X\n",srhFst);
            if(!oldTabChk(srhFst)) srhFst=0;//���ϸ�������
        }
        fclose(searchF);
    }
    searchF=fopen(ipfn.c_str(),"rb");//ר����Ϊ���head���ļ���
    fseek(g_ipf,0xB0,0);//�ų�ͷ����Ϊtable�Ŀ�����
    while(!srhChoose())
    {
        fseek(g_ipf,0xB0,0);//�ų�ͷ����Ϊtable�Ŀ�����
        fflush(stdin);
    }
    system("cls");
    fclose(searchF);//;�ر��ļ���
    printf("\nTable��ַΪ: %X\n",tabOffset);
    if(tabOffset!=srhFst)
    {
        printf("�Ƿ񱣴�Table��ַ?--(Y\\N)\n");
        fflush(stdin);
        char c=getch();
        if(toupper(c)=='Y')
        {
            memset(headData,0,0x20);
            memset(tableData,0,0x10);
            searchF=fopen(oftPath.c_str(),"rt+");//��table��¼�ļ���
            fseek(g_ipf,0xA0,0);//��λ��ͷ�ؼ�����
            fread(headData,0x10,1,g_ipf);//��ȡRomhead����
            fread(tableData,0x10,1,searchF);//��ȡoft�ļ��е�����
            for(int i=0;i<0x10;i++)
            {
                if(headData[i]==0) headData[i]=' ';
                if(tableData[i]==0) tableData[i]=32;
            }

            if(headDataCmp())//���֮ǰ�м�¼�Ļ�
            {
                fseek(searchF,1,1);//���^1���ֹ�
                string temp;
                temp.resize(8);
                sprintf(&temp[0],"%X",tabOffset);

                if(strlen(&temp[0])<7)
                {
                    int q=strlen(&temp[0]);
                    for(int z=0;z<7-q;z++)
                    {
                        temp='0'+temp;
                    }

                }
                fprintf(searchF,"%s\n",temp.c_str());//��ӡ�µ�table����
            }
            else//���֮ǰû�м�¼�Ļ�
            {
                fseek(searchF,0,2);//��λ��ĩβ
                string temp;
                temp.resize(8);
                sprintf(&temp[0],"%X",tabOffset);

                if(strlen(&temp[0])<7)
                {
                    int q=strlen(&temp[0]);
                    for(int z=0;z<7-q;z++)
                    {
                        temp='0'+temp;
                    }

                }
                for(int m=0;m<16;m++)
                fprintf(searchF,"%c",headData[m]);
                fprintf(searchF," %s\n",temp.c_str());
            }
            fclose(searchF);
        }
    }
    fclose(g_ipf);
    return 1;
}
