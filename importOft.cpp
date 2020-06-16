#include "main.h"
#include "RomToRom.h"
#include "importOft.h"
#include "copyDatSize.h"

uint32_t addRomSize(uint32_t wSiz,uint32_t writeOft)
{
    fseek(g_ipf,0,2);//�����ļ�β
    uint32_t romSiz=ftell(g_ipf);
    if(writeOft%4!=0)
        writeOft=writeOft+4-writeOft%4;
    fseek(g_ipf,1,1);//�����ļ�β��һ��
    for(uint32_t i=0;i<writeOft+wSiz-romSiz;i++)
    {
        fputc(0xFF,g_ipf);
    }
    return writeOft;
}

uint32_t getimportOft(uint32_t datSiz)
{
    system("cls");
    printf("\n�Ƿ��Լ�����Ҫд���romλ��(�����Զ�����)?--(Y\\N)  ");
    g_ipf=fopen(g_subRom.c_str(),"ab+");//�����ƴ򿪸�rom�ļ�������ĩβ�������
    char c;
    uint32_t oft;
    scanf("%c",&c);
    fflush(stdin);

    std::string temoft="";
    temoft.resize(10);
    if(toupper(c)=='Y')
    {
        while(1)
        {
            system("cls");
            printf("ע: 0x��ͷ��16���Ƶ�ַ!���� >= (0x)1FFFFFF������������ѡ��!\n");
            printf("\n������������ݵ�д���ַ: ");
            scanf("%s",&temoft[0]);
            fflush(stdin);

            if(temoft[0]!='0'||temoft[1]!='x'||temoft[2]=='\0')
            {
                printf("��ʽ�����ִ���!����������!");
                getchar();
                continue;
            }
            temoft=&temoft[2];//ȥ��ǰ��λ
            sscanf(&temoft[0],"%x",&oft);
            if(oft%4!=0)
                oft=oft+4-oft%4;//����Ϊ4������
            if(oft+datSiz>0x1FFFFFF)
            {
                printf("д�����ݽ��ᳬ���������!�Ƿ�����ѡ��--(Y\\N)  ");
                scanf("%c",&c);
                fflush(stdin);
                if(toupper(c)=='Y')
                {
                    fclose(g_ipf);
                    return 0;
                }
                continue;
            }
            fseek(g_ipf,0,2);//��λ���ļ�β
            uint32_t subromsize=ftell(g_ipf);//�õ���rom�ĳߴ�
            if(oft>subromsize)
            {
                printf("�ᳬ����ǰrom�ߴ�!�Ƿ���������?--(Y\\N)  ");
                scanf("%c",&c);
                fflush(stdin);
                if(toupper(c)=='Y')
                {
                    oft=addRomSize(datSiz,oft);//����
                }
                else continue;
            }
            break;
        }
    }
    else
    {
        uint32_t blankFlag =0xCC000000;//��¼�հ�������0����0xFF
        uint32_t temcout =0;//��Ҫ���Ĵ���
        samplehead usearch;//����������ݽṹ
        rewind(g_ipf);//���¶�λ���ļ�ͷwriteOft
        if(datSiz%16!=0)
            temcout=datSiz/16+1;
        else temcout=datSiz/16;

        while(blankFlag<<8>>8<temcout&&feof(g_ipf)==0)
        {
            fread(&usearch,16,1,g_ipf);
            if(usearch.loopTag==0xFFFFFFFF&&usearch.freq==0xFFFFFFFF
                &&usearch.loop==0xFFFFFFFF&&usearch.datsiz==0xFFFFFFFF)
            {
                if(blankFlag>>24!=0xFF)
                {
                    blankFlag=0xFF000001;//�״μ����Ͼͻḳֵ
                }
                else blankFlag++;
            }
            else if(usearch.loopTag==0x0&&usearch.freq==0x0
                    &&usearch.loop==0x0&&usearch.datsiz==0x0)

            {
                if(blankFlag>>24!=0)
                {
                    blankFlag=1;//�״μ����Ͼ��Ƿ�����ѡ��--(Y\\N)  ");�ḳֵ
                }
                else blankFlag++;
            }
            else blankFlag=0xCC000000;
        }
        if(temcout>blankFlag<<8>>8)
        {
            fseek(g_ipf,0,2);
            oft=ftell(g_ipf);
            if(oft+datSiz>0x1FFFFFF)
            {
                printf("��û���ҵ��㹻�Ŀհ�����!rom�ߴ�Ҳ��֧�ּ���������!");
                printf("\nд������Ϊ: %X д��ߴ�Ϊ: %X ��ҪROM��СΪ: %X",oft,datSiz,oft+datSiz);
                getchar();
                exit(1);
            }
            printf("δ�ҵ��㹻�Ŀհ�����!���������!");
            getchar();
            oft=addRomSize(datSiz,oft);
        }
        else
        {
            oft=ftell(g_ipf);//��ȡ���������β����
            oft=oft-temcout*16;
            fseek(g_ipf,oft-16,0);//����Ƿ����˷ѵĿհ�����
            blankFlag=(blankFlag>>24)+(blankFlag>>24<<8)+(blankFlag>>24<<16)+(blankFlag>>24<<24);
//            printf("blankFlag��ֵΪ: %X",blankFlag);
//            getchar();
            fread(&usearch,16,1,g_ipf);
            if(usearch.loop==blankFlag)
                oft-=12;
            else if(usearch.freq==blankFlag)
                oft-=8;
            else if(usearch.datsiz==blankFlag)
                oft-=4;
            if(oft+temcout*16>0x1FFFFFF)
            {
                printf("��rom�г���32mb����������!!!!!");
                getchar();
                exit(1);
            }
        }
    }
    fclose(g_ipf);
    return oft;

}
