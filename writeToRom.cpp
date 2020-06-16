#include "RomToRom.h"
#include "writeToRom.h"
#include "main.h"
#include "copyDatSize.h"
#include "showSam.h"
#include "getDat.h"
#include "showDat.h"
#include <vector>
/*!
д�����
1.���ݼ�¼������д������,���κͷ�Χ����,��˳�����ǵ��µ�ַ

2.���ڶ�ά��ϢΪ��������drum��������vg��Χ�ڵ�,������vg�ķ�ʽд���ǵ�vg.
���ǵ�vg��¼������vb��ĺ�Ŀǰ.ֻ��Ҫ��������,����������������ͬ������ϼ�¼vg���µ�ַ

3.����vg�ĵ�ַ��϶�ά��Ϣ,д��128������,�������� ������ drum ��Χ�µ�ַ  ����¼vg���µ�ַ

����Ϊ��������д��
4.���ݸ�������Ϣ,������תƫ��,����¼ÿ��������׵�ַ һά����

5.����head��Ϣ,д��head ���ݸ���ʹ�õ�vg���д����vg�������ַ

6.�޸�table�ĵ�ַΪ������head�ĵ�ַ �ظ�ÿ������4 5 6����
*/

uint32_t copytoRom(uint32_t writeOft,uint32_t writeSiz,int writeTag)
{
//    uint32_t bitot=writeSiz;//����
    uint8_t bit8=0;//��ʱ��
    uint16_t bit16=0;//��ʱ��
    uint32_t bit32=0;//��ʱ��
    uint32_t movDat=0;//����������
    g_ipf=fopen(g_ipfName.c_str(),"rb");//����rom�������
    g_opf=fopen(g_subRom.c_str(),"rb+");//�򿪸�rom�������д
    fseek(g_opf,writeOft,0);//���ļ���λ��д�����괦

    samplehead samh;//���ݽṹ��ȡ������ͷ����
    for(uint32_t i=0;i<useSampleoft.size();i++)//�Ȱ�����д��
    {
        fseek(g_ipf,useSampleoft[i],0);//��rom��λ��ÿ�����������ݴ�
        fread(&samh,16,1,g_ipf);//��ȡÿ��������ͷ����
        fseek(g_ipf,useSampleoft[i],0);//�ٴζ�λ����romÿ�����������ݴ�
        useSampleoft[i]=writeOft;//���Ƶĵص��滻Ϊд��ĵص�

        bit32=samh.datsiz+0x13-(samh.loopTag>>30);//�õ��������ܳ���
//        if(bit32%4!=0)
//            bit32=bit32+4-bit32%4;//ʹ�������Ķ���

        for(uint32_t j=0;j<bit32/4;j++)
        {
            fread(&movDat,4,1,g_ipf);//�ĸ��ֽڶ�ȡ
            fwrite(&movDat,4,1,g_opf);//д���ĸ��ֽ�
        }
        if(bit32%4!=0)
        {
            for(uint32_t k=0;k<bit32%4;k++)
            {
                int c=fgetc(g_ipf);
                fputc(c,g_opf);
            }
            for(uint32_t k=0;k<4-(bit32%4);k++)
                fputc(0,g_opf);
        }
        bit32=ftell(g_opf);//�õ���ǰд��������
        bit32=ftell(g_opf);
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=bit32;//�õ��µ����
        system("cls");
        printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//����ȫ��д�����,��˳���¼��ÿ��д�봦�ĵ�ַ
//    printf("д����������ݳ���Ϊ: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        fseek(g_ipf,useWaveMemoft[i],0);//��rom��λ��ÿ�����ε����ݴ�
        useWaveMemoft[i]=writeOft;//���Ƶĵص��滻Ϊд��ĵص�
        for(uint32_t j=0;j<8;j++)
        {
            fread(&movDat,4,1,g_ipf);
            fwrite(&movDat,4,1,g_opf);
        }
        bit32=ftell(g_opf);//�õ���ǰ������
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=bit32;//�õ��µ����
        system("cls");
        printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//��������ȫ��д�����,��˳���¼��ÿ��д�봦�ĵ�ַ
//    printf("д��Ĳ������ݳ���Ϊ: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        fseek(g_ipf,useDamageoft[i],0);//��rom��λ��ÿ����Χ�����ݴ�
        useDamageoft[i]=writeOft;//���Ƶĵص��滻Ϊд��ĵص�
        for(uint32_t j=0;j<32;j++)
        {
            fread(&movDat,4,1,g_ipf);
            fwrite(&movDat,4,1,g_opf);
        }
        bit32=ftell(g_opf);//�õ���ǰ������
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=bit32;//�õ��µ����
        system("cls");
        printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//��Χ����ȫ��д�����,��˳��ļ�¼��ÿ��д�봦�ĵ�ַ
//    printf("д��ķ�Χ���ݳ���Ϊ: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    vtda_t vd;//���ݽṹ������ȡ�ټ���Ϣ
    for(int i=(signed)useVGoft.size()-1;i>=0;i--)//������ѭ���Ա��ȵõ�ms��dp����VG��ַ
    {
        uint32_t j=0;
//        printf(" i��ǰ��ֵΪ: %d ",i);
//        printf("useVGoft[%d]��ֵΪ: %X ����Ϊ: %d ",i,useVGoft[i],useVGoft.size());
//        printf("VGinfo�ĳ���Ϊ: %d ",VGinfo[i].size());
//        printf("keyEfc�ĳ���Ϊ: %d ",keyEfc[i].size());
//        getchar();
        for(;j<VGinfo[i].size();j++)//��λ���ܵ�...
        {
            if(writeTag==1&&keyEfc[i][j]==0)//��Ч�ļ�λ
            {
//                printf("������Ч");
                memset(&vd,0,12);//vd�ڴ�����
                fwrite(&vd,12,1,g_opf);//д����Ч�ļ�λ����0,0,0
                continue;
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+12*j,0);//��λ��ÿ��VG�ļ�λ��λ��
//            printf("��ǰλ��Ϊ: %X ��ֵΪ: %d",ftell(g_ipf),j);
            fread(&vd,12,1,g_ipf);//��ȡ�ټ���Ϣ
            if(VGinfo[i][j]>>28<2)//���������Ͳ��ı�����
            {
//                printf("��������");
            }
            else if(VGinfo[i][j]>>28==2)//��������
            {
//                printf("��������");
                vd.tdat2=0x8000000|useSampleoft[VGinfo[i][j]<<4>>4];//���ĵ�ַΪд���������ַ
            }
            else if(VGinfo[i][j]>>28==3)//����
            {
//                printf("��������");
                vd.tdat2=0x8000000|useWaveMemoft[VGinfo[i][j]<<4>>4];//���ĵ�ַΪд��Ĳ��ε�ַ
            }
            else if(VGinfo[i][j]>>28==5)//multi VG��Χ��
            {
//                printf("����ms");
                if(useVGoft[i]>>28==5)//��ǰmulti��VG������multi�ļ�ֵ
                {
//                    fprintf(stderr,"0x8000000|writeOft=%X",0x8000000|writeOft);
                    if(useVGoft[i]<<4>>4!=vd.tdat2<<5>>5)
                    {
                        fprintf(stderr,"MS����VG: %X�еĵ�%d����ֵҲ��MS,������MS�ĵ�ַ���Ǹ�MS����VG����"
                                "��ʹ��ASM�ķ�ʽ����,��Ϊ�ڻ�ȡ������VG��MSʱ,ֻ�����һ��,û�ж�MS����VG��MS���"
                                "MS���͵�VGĬ�������ǵ���MS����VG,�����Ǹİ�,������Ϸ�������������!",useVGoft[i],
                                j);
                        fflush(stdin);
                        getchar();
                        exit(1);
                    }
                    vd.tdat2=0x8000000|writeOft;//���ǵ�ǰVG��д��ĵ�ַ
                    vd.tdat3=0x8000000|(useDamageoft[VGinfo[i][j]]<<16>>16);//�õ���Χ��ַ
                }
                else
                {
//                    printf("��������!:");
//                    getchar();
//                    printf("0x8000000|(useVGoft[VGinfo[i][j]]<<4>>20)<<4>>4��ֵΪ: %X",0x8000000|(useVGoft[VGinfo[i][j]<<4>>20])<<4>>4);
                    vd.tdat2=0x8000000|(useVGoft[VGinfo[i][j]<<4>>20])<<4>>4;//�õ����������ݵ�VG��ĵ�ַ
//                    printf("0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16])��ֵΪ: %X",0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16]));
                    vd.tdat3=0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16]);//�õ���Χ��ַ

                }
            }
            else if(VGinfo[i][j]>>28==7)//drum VG��Χ��
            {
//                printf("����dp useVGoft[%d]=%X ",i,useVGoft[i]);
//                getchar();
                if(useVGoft[i]>>28==7)//��ǰdrumpart��VG������Drum�ļ�ֵ
                    vd.tdat2=0x8000000|writeOft;//���ǵ�ǰVG��д���ַ
                else
                    vd.tdat2=0x8000000|(useVGoft[VGinfo[i][j]<<4>>4])<<4>>4;//�õ�drum��������VG��ĵ�ַ
            }
            fwrite(&vd,12,1,g_opf);//д���ټ���Ϣ
        }
        useVGoft[i]=0x8000000|writeOft;//��ǰ���ƴ���VG��ַ�ĳ�д�봦��VG��ַ
        bit32=ftell(g_opf);//�õ���ǰ������
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=bit32;//�õ��µ����
        system("cls");
        printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
//        printf("i= %d j= %d useVGoft.size()-1= %d",i,j,useVGoft.size()-1);
    }
//    printf("д���ֵ���ݳ���Ϊ: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    std::vector<uint32_t>usetrackoft(0,0);//����������ʱ�洢�������������

    for(uint32_t i=0;i<cpySong.size();i++)//���ݸ�����ѭ��
    {
        bit32=(tabOffset<<5>>5)+(cpySong[i]-1)*8;//�õ�ÿ�����Ƹ�����table��ַ ��1��ʼ���Լ�1
//        printf("��rom��tab��ʼ��ַΪ: %X\n",tabOffset);
//        printf("Ҫ���Ƶĸ�����TABLE��ַΪ: %X",bit32);
//        getchar();
        fseek(g_ipf,bit32,0);//ƫ�Ƶ�������table
        fread(&bit32,4,1,g_ipf);//��ȡtable��head
        movDat=bit32<<5>>5;//����head�ĵ�ַ
        fseek(g_ipf,movDat,0);//��λ��������head
        fread(&bit16,2,1,g_ipf);//��ȡ������
        bit16=bit16<<24>>24;

        usetrackoft.resize(bit16,0);//�����������������ַ
        fseek(g_ipf,6,1);//��λ�������ַ
        for(uint32_t j=0;j<bit16;j++)
        {
            fread(&bit32,4,1,g_ipf);
            usetrackoft[j]=bit32<<5>>5;//�õ�ÿ������ĵ�ַ
        }
        for(uint32_t j=0;j<bit16;j++)
        {
            fseek(g_ipf,usetrackoft[j],0);//��λ��ÿ����������ݴ�
            do
            {
                bit8=fgetc(g_ipf);//��ȡ�ֽ�
                fputc(bit8,g_opf);//д���ֽ�
                if(bit8==0xB1) break;
                else if(bit8==0xB2||bit8==0xB3)
                {
                    fread(&bit32,4,1,g_ipf);//��ȡ��ַ
                    //��ת�ĵ�ַ��ȥ������ʼ��ַ�õ�ƫ�Ʋ�ֵ
                    bit32=(bit32<<5>>5)-usetrackoft[j];
                    bit32=0x8000000|(bit32+writeOft);//����µ�����
                    fwrite(&bit32,4,1,g_opf);//д���µ�ַ
                }
            }while(!feof(g_ipf));


            bit32=ftell(g_opf);
            usetrackoft[j]=0x8000000|writeOft;//�����ַ�ĳ�д��ĵ�ַ
            writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
            writeOft=bit32;//�õ��µ����
            system("cls");
            printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
        }//�������춼д��,����������������
        bit32=ftell(g_opf);//�õ���ǰ������
        if(bit32%4!=0)
                for(uint32_t k=0;k<4-(bit32%4);k++)
                    fputc(0,g_opf);//������ַ�
        bit32=ftell(g_opf);
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=ftell(g_opf);//head���

        fseek(g_ipf,movDat,0);//��λ��ÿ�����Ƹ�����head
        fread(&bit32,4,1,g_ipf);//��������δ֪���ݶ�ȡ



//        bit32=(tranum[cpySong[i]-1]<<16)+unknow[cpySong[i]-1];

        fwrite(&bit32,4,1,g_opf);//д���������δ֪����
        bit32=useVGoft[trackUseVG_No[i]];//д���µ�vg��ַ
        fwrite(&bit32,4,1,g_opf);
        for(uint32_t j=0;j<bit16;j++)
        {
            bit32=usetrackoft[j];
            fwrite(&bit32,4,1,g_opf);//д���µ�ÿ������
        }
        movDat=ftell(g_opf);//���浱ǰ��д���ַ
        bit32=(subTabOft<<5>>5)+(subUseSong[i]-1)*8;//�õ�д��rom��ÿ��������table��ַ
//        printf("��rom��table��ʼ��ַΪ: %X",subTabOft);
//        printf("д���table��ַΪ: %X",bit32);
//        getchar();
        fseek(g_opf,bit32,0);//��λ��д��romÿ��������table��
        bit32=0x8000000|writeOft;//�൱���µ�head��ַ
//        printf("��head��ַΪ: %X",bit32);
//        getchar();
        fwrite(&bit32,4,1,g_opf);//tableд���µ�head��ַ
        bit32=(tabOffset<<5>>5)+(cpySong[i]-1)*8;//�õ�ÿ�����Ƹ�����table��ַ ��1��ʼ���Լ�1
        fseek(g_ipf,bit32+4,0);//��λ��Songgroup����

        fread(&bit32,4,1,g_ipf);//��ȡsg����
//        printf("songGroup������Ϊ: %X",bit32);
//        getchar();
//        fwrite(&songGop[cpySong[i]-1],4,1,g_opf);//ֱ�Ӷ�ȡ��ȥ��sg���ݸ�ʡ��
        fwrite(&bit32,4,1,g_opf);//д��sg����
        fseek(g_opf,movDat,0);//����֮ǰд��ĵ�ַ

        bit32=movDat;//�õ���ǰ������
        writeSiz-=bit32-writeOft;//�õ�д�����ٵĳߴ�
        writeOft=bit32;//�õ��µ����
        system("cls");
        printf("��ǰд���ַΪ: %X\n\n����%X�ֽ���Ҫд��\n\n�յ�Ϊ: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }
//    printf("д���head�͸�������Ϊ: %X",bitot-writeSiz);
//    bitot=writeSiz;
    printf("\n�����Ѿ�д�����!\n");
    subUseSong.swap(backUpSubSong);
    fclose(g_ipf);//�ر��ļ���
    fclose(g_opf);
    return writeSiz;
}
