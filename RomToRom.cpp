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

std::vector<uint32_t>subUseSong(0,0);//��������洢Ҫ��������ռ�õĸ�����
std::vector<uint32_t>cpySong(0,0);//������romҪ�����Ƶĸ�������

void damageSet(uint32_t &startNum,uint32_t &endNum,bool flag){
	while(1)
        {
            system("cls");
            printf("�����������ʼ��: ");
            if(!(scanf("%d",&startNum)))
            {
                fflush(stdin);
                printf("\n����������!");
                getchar();
            }

            else if(startNum==0)
            {
                fflush(stdin);
                printf("\n��������ʹ�1��ʼ!");
                getchar();
            }
            else if(startNum>tabtotal)
            {
                fflush(stdin);
                printf("\n��ʼ�ų�����Ҫ����rom�ĸ�������!");
                getchar();
            }
            else if(flag) break;
            else if(startNum>subTabTol)
            {
                fflush(stdin);
                printf("\n��ʼ�ų�����Ҫд��rom�ĸ�������!");
                getchar();
            }
            else break;
        }
    fflush(stdin);
    while(1)
    {
        system("cls");
        printf("���������������: ");
        if(!(scanf("%d",&endNum)))
        {
            fflush(stdin);
            printf("\n����������!");
            getchar();
        }
        else if(endNum<startNum)
        {
            fflush(stdin);
            printf("\n�����Ų���С�ڿ�ʼ��!");
            getchar();
        }
        else if(endNum>tabtotal)
        {
            fflush(stdin);
            printf("\n�����ų�����Ҫ����rom�ĸ�������!");
            getchar();
        }
        else if(flag) break;
        else if(endNum>subTabTol)
        {
            fflush(stdin);
            printf("\n�����ų�����Ҫд��rom�ĸ�������!");
            getchar();
        }
        else break;
    }
}

void VolSet(std::vector<uint32_t>volSetSong)
{
    //ǰ������¼��ַ����,������¼���ٸ�����,���ļ�¼��󳬹���,�Լ���¼���õ�������,
    uint32_t bitoft,bitraoft,overcount,overmax=0,vol,cha;
    uint8_t bitranum,bitemp;//��¼������,�Ͷ�ȡ�ֽ�����

    while(1)
    {
        system("cls");
        printf("ע:8 - 32\n\n�����ǰ����Ϊ16,����������Ҫ������: ");
        if(scanf("%u",&vol)==0){
			fputs("����������!",stderr);
		}
        else if(vol>32)
        {
            printf("\n\n�벻Ҫ������������!");
        }
        else if(vol<8)
        {
            printf("\n\n�벻Ҫ����һ������!");
        }
        else break;
		fflush(stdin);
		getchar();
    }

	fflush(stdin);
    g_ipf=fopen(g_subRom.c_str(),"rb+");//�򿪸�rom
    std::vector<uint32_t>voloft(0,0);//���ڼ�¼Ҫ�޸ĵ�������ַ
    overcount=0;
    for(uint32_t i=0;i<volSetSong.size();i++)
    {
        fseek(g_ipf,subTabOft+(volSetSong[i]-1)*8,0);//��λ��table��ַ
        fread(&bitoft,4,1,g_ipf);//��ȡhead��ַ
        bitoft=bitoft<<5>>5;//ȥ�����������
        fseek(g_ipf,bitoft,0);//����head����λ��
        fread(&bitranum,1,1,g_ipf);//��ȡ������
        fseek(g_ipf,7,1);//������������λ��
        bitoft=ftell(g_ipf);//���������������ʼ����
        for(uint32_t j=0;j<bitranum;j++)
        {
            fseek(g_ipf,bitoft+4*j,0);//����ÿ������ĵ�ַ
            fread(&bitraoft,4,1,g_ipf);//��ȡtrack����
            bitraoft=bitraoft<<5>>5;//ȥ�����������
            fseek(g_ipf,bitraoft,0);//����ÿ�����������λ��
            do
            {
                bitemp=fgetc(g_ipf);

                if(bitemp==0xBE)
                {
                    voloft.push_back(ftell(g_ipf));//��¼��ǰ������
                    while(1)
                    {
                        bitemp=fgetc(g_ipf);//��ȡ����ֵ
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
        printf("\n����%d����������������»ᳬ�����ֵ127,���ֵΪ: %d ȷ�ϸ�д?--Y\\N",overcount,overmax);
        if(toupper(getch())!='Y')
        {
            return;
        }
    }
    for(uint32_t i=0;i<voloft.size();i++)
    {
        fseek(g_ipf,voloft[i],0);//��λ��ÿ��0xBE��
        bitemp=fgetc(g_ipf);//��ȡ����ֵ
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
            if(bitemp>0xB0) break;//����Ļ�����������������
            else if(bitemp<0x80)//�����������Ļ�
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
                fseek(g_ipf,0,1);//ֻ��������λһ�²Ų������...
            }
        }
    }
    printf("\n������д���!");
    getchar();
    fclose(g_ipf);
    return;
}

void printUnEfcTable(uint32_t UnEfcTrack[],uint32_t cpyTol,std::vector<uint32_t>subUseSong)//��ӡ��Ч�ĸ�����
{
    printf("��rom��Ч�ĸ�������б�����: \n");
    for(uint32_t i=0,j=1;i<subTabTol-subEfcTabTol;i++,j++)
    {
        for(uint32_t k=0;k<cpyTol;k++)
        {
            if(UnEfcTrack[i]==subUseSong[k])
               goto unprint;//�����Ч�ĸ����Ѿ���ռ���򲻴�ӡ����
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

int ckPath(std::string s)//����Ƿ���Ŀ¼
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
        printf("��⵽��û�����븱ROM!\n\n");
        while(1)
        {
            fflush(stdin);
            printf("ע:�����ͬĿ¼������ROM���������ּ���,������϶�ROM�������ϻ��·��!\n");
            printf("�����븱ROM��·��: ");
            g_subRom="";
            char *tempName=new char[300];//��ֹ������

            gets(tempName);
            fflush(stdin);
            g_subRom=tempName;
            if(g_subRom[0]==34) g_subRom=removeQuotes(g_subRom);
            delete []tempName;
            if(access(g_subRom.c_str(),0)!=0) //����ļ��Ƿ����
            {
                system("cls");
                printf("��δ�ҵ��ļ�: %s\n",g_subRom.c_str());
                continue;
            }
            printf("SWpath= %s",SWpath.c_str());
            if(ckPath(g_subRom))
                g_subRom=onlyPath(SWpath)+g_subRom;
            if(ckPath(g_ipfName))
                g_ipfName=onlyPath(SWpath)+g_ipfName;
            if(g_ipfName==g_subRom)
            {
                printf("��rom�͸�rom��ͬһ��λ�õ�rom!");
                getchar();
                continue;
            }
            break;
        }
    }
    system("cls");
    if(!subTabGet(g_subRom)) return 0;

    getSubRomTrackInfo(subTabOft,g_subRom);//��ø�������,��Ч������,������Чflag

    printf("��Rom����������Ϊ: %d  ��Ч������Ϊ: %d\n",subTabTol,subEfcTabTol);

    uint32_t UnEfcTrack[subTabTol-subEfcTabTol]={};//����һ����¼��Ч����������
    uint32_t cpyTol=0;//���Ƶĸ�������

    int useFlag=0;//�Ƿ������Ч����flag
    int sampleFlag=0;//�Ƿ�ȷ��λʹ�õ�samples
    uint32_t cpyDatSize=0;//Ҫ���Ƶ������ܳ���
    uint32_t woffset=0;//����Ҫд�����ݵ�����


    for(uint32_t i=0,j=0;i<subTabTol;i++)
    {
        if(subTabefc[i]==1) continue;
            UnEfcTrack[j]=i+1;
        j++;
    }

    //printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);//��ӡ��Ч�ĸ�����
    printf("\n\t\tA.��ʹ����Ч�ĸ�����ַ\n"
           "\n\t\tB.����ʹ�����и�����ַ\n"
           "\n\t\tC.��Χ��ֲ(���� ���������ͬ�޶�)\n"
           "\n\t\tD.���ø�rom����������\n"
           "\n\t\tE.���ݸ�rom��������\n"
           "\n\t\t    ESC [����]\n");
    while((clean=toupper(getch())))//��ʡ����,���÷���ˢ�����ϵĴ���
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
                    subUseSong.push_back(i);//��¼��Ч�ĸ������
                    cpySong.push_back(i);
                }
            }
            cpyTol=cpySong.size();
//            backUpSubSong(subUseSong);//����
//            printf("cpytol= %d subUseSong.size= %d",cpyTol,subUseSong.size());
//            getchar();
            for(uint32_t i=0;i<subUseSong.size();i++)
            {
                subTabefc[subUseSong[i]-1]=1;//���Ƹ�������Ч��ȫ���Ϊ1;
            }
            sampleFlag=1;//ǿ�ƾ�ȷ
            printf("cpySong.size= %d,cpyTol= %d",cpySong.size(),cpyTol);
            getchar();
            cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
            printf("Ҫ���Ƶ����ݳ���Ϊ: %d / %X �ֽ�\n",cpyDatSize,cpyDatSize);
            getchar();
            while(!(woffset=getimportOft(cpyDatSize)));
            printf("Ҫд������λ��Ϊ: %X ����Ϊ: %X\n",woffset,cpyDatSize);
            getchar();
            copytoRom(woffset,cpyDatSize,sampleFlag);
            getchar();
            return 1;
        case 'D':
            system("cls");
            if(backUpSubSong.size()!=0)
            {
                printf("�Ƿ�Ըոո��ƻ����õĸ���������������?--Y/N");
                if(toupper(getch())=='Y')
                {
                    VolSet(backUpSubSong);
                    return 1;
                }
            }
            uint32_t setSong=0;
            printf("\nע:���������ʱ����\n\n������Ҫ�޸������ĸ�rom����: ");
            while((scanf("%d",&setSong)))
            {
                fflush(stdin);
                if(setSong>subTabTol)
                    printf("�����ų����˸�rom����������");
                else if(setSong==0)
                    printf("��������СΪ1!");
                else if(subTabefc[setSong-1]==0)
                {
                    printf("���������Ч!");
                }
                else
                {
                    for(uint32_t i=0;i<subUseSong.size();i++)
                    {
                        if(setSong==subUseSong[i])
                            printf("���������֮ǰ�Ѿ��������!");
                    }
                    subUseSong.push_back(setSong);
                    continue;
                }
                getchar();
            }
            fflush(stdin);
            if(subUseSong.size()==0)
            {
                printf("��ǰû���κθ�����!");
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
        printf("\n������Ҫ����ROM�и��Ƶĸ�������: ");
        if(scanf("%d",&cpyTol)!=1)
            printf("����������!\n");
        else if(cpyTol>efctabtal)
            printf("������������rom��Ч����������!efctable= %d\n",efctabtal);
        else if(useFlag==1&&cpyTol>subTabTol-subEfcTabTol)
            printf("���������˸�rom��Ч����������!\n");
        else if(useFlag==0&&cpyTol>subTabTol)
            printf("���������˸�romȫ������������!\n");
        else
            break;
        fflush(stdin);
        getchar();
    }
    subUseSong.resize(cpyTol);
    cpySong.resize(cpyTol);

    for(uint32_t i=0;i<cpyTol;i++)
    {

        int reUse=0;//���������
        fflush(stdin);
        while(1)
        {
            system("cls");
            printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);

            printf("\n�����븱rom��Ҫ������ռ�õĵ�%d��������!\n",i+1);
            if(scanf("%d",&subUseSong[i])!=1)
                printf("����������!");
            else if(useFlag==0)
            {
                if(subUseSong[i]>subTabTol)
                    printf("�����ų����˸�rom����������!");
                else
                {
                    for(uint32_t k=0;k<i;k++)
                    {
                        if(subUseSong[i]==subUseSong[k])
                        {
                            printf("��ǰ������֮ǰ�Ѿ������!");
                            goto reinput;
                        }
                    }

                    break;//ֻҪ����ѭ��������ȷ����
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
                    printf("���ֲ�����Ч�ĸ�rom������!\n");
                    reUse++;
                    if(reUse==2)
                    {
                        printf("�Ƿ�Ҫʹ����Ч�ĸ�����?--(Y\\N)  ");
                        scanf("%c",&clean);
                        fflush(stdin);
                        if(toupper(clean)=='Y')
                        {
                            useFlag=0;
                            for(uint32_t k=0;k<i;k++)
                            {
                                if(subUseSong[i]==subUseSong[k])
                                {
                                    printf("��ǰ������֮ǰ�Ѿ������!");
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
                            printf("��ǰ������֮ǰ�Ѿ������!");
                            goto reinput;
                        }
                    }

                    break;//ֻҪ����ѭ��������ȷ����
                }

            }
            reinput:
            fflush(stdin);
            getchar();
        }
    }
    system("cls");
    printf("��ǰҪ��ռ�õĸ�rom��������: \n");
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
            printf("\n��������Ҫ���Ƶ���rom�еĵ�%d��������:\n",i+1);
            printf("%d <- ",subUseSong[i]);
            uint8_t t=scanf("%d",&cpySong[i]);
            if(t!=1)
                printf("����������!");
            else if(tablefc[cpySong[i]-1]!=1)
                printf("��������Ŷ�Ӧ�ĸ�����Ч!");
            else
            {
                for(uint32_t j=0;j<i;j++)
                {
                    if(cpySong[i]==cpySong[j])
                    {
                        printf("����������Ѿ��������!");
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
    printf("��rom���븱rom�ĸ���������:\n");
    printf("����\t->\t����\n");
    for(uint32_t i=0;i<cpyTol;i++)
        printf("%d\t->\t%d\n",cpySong[i],subUseSong[i]);

    fflush(stdin);
    getchar();
//    printf("�Ƿ�ȷ��λ��ʹ�õ��������ݶ���ȫ�׵���(��ʱ)?--(Y\\N)  ");
//    scanf("%c",&clean);
//    fflush(stdin);
//    if(toupper(clean)=='Y')
    for(uint32_t i=0;i<subUseSong.size();i++)
    {
        subTabefc[subUseSong[i]-1]=1;//���Ƹ�������Ч��ȫ���Ϊ1;
    }
    sampleFlag=1;//ǿ�ƾ�ȷ
    cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
    printf("Ҫ���Ƶ����ݳ���Ϊ: %d / %X �ֽ�\n",cpyDatSize,cpyDatSize);
    getchar();
    while(!(woffset=getimportOft(cpyDatSize)));
    printf("Ҫд������λ��Ϊ: %X ����Ϊ: %X\n",woffset,cpyDatSize);
    getchar();
    copytoRom(woffset,cpyDatSize,sampleFlag);
    getchar();
    return 1;
}
