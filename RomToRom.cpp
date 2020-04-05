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

std::vector<int>subUseSong(0,0);//��������洢Ҫ��������ռ�õĸ�����
std::vector<int>cpySong(0,0);//������romҪ�����Ƶĸ�������

void printUnEfcTable(int UnEfcTrack[],int cpyTol,std::vector<int>subUseSong)//��ӡ��Ч�ĸ�����
{
    printf("��rom��Ч�ĸ�������б�����: \n");
    for(int i=0,j=1;i<subTabTol-subEfcTabTol;i++,j++)
    {
        for(int k=0;k<cpyTol;k++)
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
        printf("��⵽��û�����븱ROM!\n\n");
        while(1)
        {
            fflush(stdin);
            printf("ע:�����ͬĿ¼������ROM���������ּ���,������϶�ROM�������ϻ��·��!\n");
            printf("�����븱ROM��·��: ");
            g_subRom="";
            char *tempName=new char[300];//��ֹ������
//            scanf("%s",tempName);

            gets(tempName);
            g_subRom=tempName;
            if(g_subRom[0]==34) g_subRom=removeQuotes(g_subRom);
            delete []tempName;
            if(access(g_subRom.c_str(),0)!=0) //����ļ��Ƿ����
            {
                system("cls");
                printf("��δ�ҵ��ļ�: %s\n",g_subRom.c_str());
                continue;
            }

            break;
        }
    }
    system("cls");
    if(!subTabGet(g_subRom)) return 0;

    getSubRomTrackInfo(subTabOft,g_subRom);//��ø�������,��Ч������,������Чflag

    printf("��Rom����������Ϊ: %d  ��Ч������Ϊ: %d\n",subTabTol,subEfcTabTol);

    int UnEfcTrack[subTabTol-subEfcTabTol]={};//����һ����¼��Ч����������
    int cpyTol=0;//���Ƶĸ�������

    int useFlag=0;//�Ƿ������Ч����flag
    int sampleFlag=0;//�Ƿ�ȷ��λʹ�õ�samples
    uint32_t cpyDatSize=0;//Ҫ���Ƶ������ܳ���
    uint32_t woffset=0;//����Ҫд�����ݵ�����


    for(int i=0,j=0;i<subTabTol;i++)
    {
        if(subTabefc[i]==1) continue;
            UnEfcTrack[j]=i+1;
        j++;
    }

    printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);//��ӡ��Ч�ĸ�����


    printf("\n�Ƿ��ʹ����Ч�ĸ�����ַ?--(Y\\N)  ");
    scanf("%c",&clean);
    if(toupper(clean)=='Y')
        useFlag=1;

    fflush(stdin);
    while(1)
    {
        system("cls");
        printUnEfcTable(UnEfcTrack,cpyTol,subUseSong);
        printf("\n������Ҫ����ROM�и��Ƶĸ�������: ");
        if(scanf("%d",&cpyTol)!=1)
            printf("����������!\n");
        else if(cpyTol>efctabtal)
            printf("������������rom��Ч����������!\n");
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

    for(int i=0;i<cpyTol;i++)
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
                    for(int k=0;k<i;k++)
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
                            for(int k=0;k<i;k++)
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
                    for(int k=0;k<i;k++)
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
    for(int i=0;i<cpyTol;i++)
        printf("%d ",subUseSong[i]);
    getchar();


    for(int i=0;i<cpyTol;i++)
    {

        fflush(stdin);
        while(1)
        {
            system("cls");
            printf("\n��������Ҫ���Ƶ���rom�еĵ�%d��������:\n",i+1);
            printf("%d <- ",subUseSong[i]);
            uint8_t t=scanf("%d",&cpySong[i]);
            if(t!=1)
                printf("����������!");
            else if(tablefc[cpySong[i]]!=1)
                printf("��������Ŷ�Ӧ�ĸ�����Ч!");
            else
            {
                for(int j=0;j<i;j++)
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
    for(int i=0;i<cpyTol;i++)
        printf("%d\t->\t%d\n",cpySong[i],subUseSong[i]);
    fflush(stdin);
//    printf("�Ƿ�ȷ��λ��ʹ�õ��������ݶ���ȫ�׵���(��ʱ)?--(Y\\N)  ");
//    scanf("%c",&clean);
//    fflush(stdin);
//    if(toupper(clean)=='Y')
    sampleFlag=0;//ǿ�ƾ�ȷ
    cpyDatSize=calculateCopySize(sampleFlag,cpyTol);
    printf("Ҫ���Ƶ����ݳ���Ϊ: %d / %X �ֽ�\n",cpyDatSize,cpyDatSize);
    getchar();
    while(!(woffset=getimportOft(cpyDatSize)));
    printf("Ҫд������λ��Ϊ: %X - %X\n",woffset,woffset+cpyDatSize);
    copytoRom(woffset,cpyDatSize,sampleFlag);
    getchar();
    return 0;
}
