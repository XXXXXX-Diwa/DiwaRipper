#include "main.h"
#include "getDat.h"
#include "subTrackInfoGet.h"
#include "subTabSrh.h"
#include "showSam.h"
#include "copyDatSize.h"
#include "RomToRom.h"
#include <vector>
#include <cstdint>
//#include <pthread.h>
//#include <windows.h>
/*!
��Ҫ�������������:
ÿ��vg�ĵ�ַ һά���� �������������drum������vg�ķ�Χ��
����4bit���,���������Ϊ1,drum���Ϊ2
ÿ��vg��128����Ϣ ��ά����
��Ϣ����:һά����vg���
	��ά���������� ���κ�
		������ ��λ����Ƿ���vg�� ��λ��vg�����ϵ�ƫ��
		drum�� ��λ����Ƿ�����vg�� ͬ��
		�����vg��,��Χ��¼��16λ,��VG��ƫ�ƴ�����¼��16-23bit
		�������vg��,������vg�������͵�ַ.���������ͨ��VG��ͬ
		���ö�ά�����¼����,��16λ��Ȼ�Ƿ�Χ,��VG�ϵĵ�ַ��ż�¼��16-27bit
��ά�ĵڶ�ά�ȵ��ĸ��ֽ���Ϣ
���ֽ�Ϊ 1���� 2���� 3���� 4����(��ƫ��) 5����(vg��) 6drum(��ƫ��) 7drum(vg��) 0����
��¼:4��6������VG�ڵ����ͼ���̫����������,5�������λΪ��Χ���,16bitΪVG���.

ÿ�����ظ������� һά����
ÿ�����ظ��Ĳ��� һά����
ÿһ�����ظ���Χ һά����
�����ĸ�����	һά����
����ĸ�����	һά����
��¼ÿ������ʹ�õ�vg��� һά����
*/
std::vector<uint32_t>useSampleoft(0,0);//����һ�����������¼�������ݵ�ַ
std::vector<uint32_t>useWaveMemoft(0,0);//ÿһ�����ظ��Ĳ���
std::vector<uint32_t>useDamageoft(0,0);//ÿһ�����ظ��ķ�Χ
std::vector<uint32_t>useVGoft(0,0);//ÿһ�����ظ���VoiceGroup
std::vector<uint8_t>trackUseVG_No(0,0);//ÿһ������ʹ�õ�vg���
std::vector<std::vector<uint32_t> >VGinfo(0,std::vector<uint32_t>(0,0));//��¼vg��ÿ��������Ϣ�Ķ�ά����
std::vector<std::vector<uint8_t> >keyEfc(0,std::vector<uint8_t>(0,0));//��¼ÿ���ټ��Ƿ���ı�ʹ�õ�

void epyVector()
{
    std::vector<uint32_t>().swap(useSampleoft);//����ڴ������
    std::vector<uint32_t>().swap(useWaveMemoft);//ÿһ�����ظ��Ĳ���
    std::vector<uint32_t>().swap(useDamageoft);//ÿһ�����ظ��ķ�Χ
    std::vector<uint32_t>().swap(useVGoft);//ÿһ�����ظ���VoiceGroup
    std::vector<uint8_t>().swap(trackUseVG_No);//ÿһ������ʹ�õ�vg���
    std::vector<std::vector<uint32_t> >b(0,std::vector<uint32_t>(0,0));
    std::vector<std::vector<uint8_t> >a(0,std::vector<uint8_t>(0,0));
    b.swap(VGinfo);
    a.swap(keyEfc);

}

/*!

*/
uint32_t calculateCopySize(int samFlag,uint32_t cpyTol)
{

    uint32_t bitot=0;//��ʱ��
    std::vector<uint32_t>backUpSubSong(subUseSong);//����
    epyVector();//ÿ�ζ����Ҫ�õ��Ѿ�����������
    system("cls");
    printf("����������Ϣ,û��,����δ֪ԭ����,����Թر���!");
    g_ipf=fopen(g_ipfName.c_str(),"rb");
    if(g_ipf==nullptr)
    {
        fprintf(stderr,"����rom�ļ�ʧ��!");
        getchar();
        exit(1);
    }

    uint16_t bit16=0;
    uint32_t bit32=0;
    uint32_t dataSize=0;//�ۼ������ܳ���
//    uint32_t expVoiceoft[cpyTol];//�����ĸ���voice��ַ������ֹ�ظ�
//    uint8_t useVoiceTol=0;//������¼��ǰʹ���˶��ٸ�voicegroup

    //���ȼ����ж��ٸ�voice
    trackUseVG_No.resize(cpyTol,0);//���ݸ�������������
    for(uint32_t i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,(headoft[cpySong[i]-1]<<5>>5)+4,0);//��λ������������head����VG��
        fread(&bit32,4,1,g_ipf);//��ȡvg��ַ
        for(uint32_t j=0;j<useVGoft.size();j++)
        {
            if(bit32<<5>>5==useVGoft[j])
            {//iΪ�������,jΪvg���,����ÿ���������ظ�vg���
                trackUseVG_No[i]=j;
                goto passVGWrite;
            }
        }
        useVGoft.push_back(bit32<<5>>5);//������ظ������
        //�����ظ���vg�����ӵ�ʱ��,��ǰ������¼������ظ���vg���
        trackUseVG_No[i]=useVGoft.size()-1;
        passVGWrite: continue;

    }//�����Ѿ���¼��Ҫ�õ���voiceGroup�������Լ�ÿ��������VG���
//    for(uint16_t i=0;i<cpySong.size();i++){
//        printf("%d ",cpySong[i]);
//    }
//    fflush(stdin);
//    getchar();
    //����vg���ټ��Ƿ�ʹ��,ͬʱ��¼ÿ��head+����ĳߴ�
    keyEfc.resize(useVGoft.size());//����VG����������
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        keyEfc[i].resize(128,0);//ͬʱ����ÿ����ͨVG��128���ټ�efc ��ȫ��Ϊ0
    }

    for(uint32_t i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,headoft[cpySong[i]-1]<<5>>5,0);//��λ��ÿ��������head��ַ
        fread(&bit16,2,1,g_ipf);//��������
        bit16=bit16<<24>>24;
        dataSize+=(bit16*4+8);//head���ݳ��ȼ�¼ ������*4+8
        fseek(g_ipf,6,1);//��λ��ÿ������ĵ�ַ
        uint32_t fstTrack=ftell(g_ipf);//�����������ַ
//        printf("����%d ���������ַΪ: %X\n",cpySong[i],fstTrack);
        for(int j=0;j<bit16;j++)
        {
            fseek(g_ipf,fstTrack+j*4,0);//��λ��ÿ�������ַ
            fread(&bit32,4,1,g_ipf);//��ȡÿ������ĵ�ַ
            fseek(g_ipf,bit32<<5>>5,0);//��λ��ÿ����������λ��
            do
            {
                int c=fgetc(g_ipf);

                dataSize++;//ÿ��ȡ�������ݵ�һ���ֽڶ�+1
                if(c==0xb1) break;//������Ϣ������־

                else if(c==0xbd)//��λ��־
                {
//                    printf("��λ��־����");
//                    fflush(stdin);
//                    getchar();

                    int c=fgetc(g_ipf);
                    //���ݸ�����ŵõ�����ʹ��vg����ٵ����ڵļ�λ�ŵ�flag
                    keyEfc[trackUseVG_No[i]][(uint8_t)c]=1;
//                    printf("keyEfc[%d][%d]",trackUseVG_No[i],(uint8_t)c);
//                    getchar();
                    dataSize++;//ÿ��ȡ�������ݵ�һ���ֽڶ�+1
                }
                else if(c==0xb3||c==0xb2)//��ת�����ñ�־,�����ǵ�ַ
                {
                    fseek(g_ipf,4,1);//������ַ���ĸ��ֽ�
                    dataSize+=4;
                }

            }while(1);//(!feof());//�����ٶȵ�����..

//            printf("��ǰ����Ľ���λ��Ϊ: %X\n",(unsigned)ftell(g_ipf));
        }
        if(dataSize%4!=0)
            dataSize+=4-dataSize%4;//ÿ��������ȫ��������������ս���Ҫ��4����

    }//����,ÿ������͸�����head���ݼ�¼���.ͬʱvg��λ����Ч��Ҳ�����.

    printf("head���ݺ��������ݳ���Ϊ: %X\n",dataSize);
//    getchar();
    vtda_t vtdsiz;//���ݽṹ������ȡ12���ֽ�
    VGinfo.resize(useVGoft.size());//VG��Ϣ��ά�������VG��������
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        VGinfo[i].resize(128,0);//�����VG����չ��128����,������
    }
    samplehead shead;//���ݽṹ��ȡ����ͷ������
    for(uint32_t i=0;i<useVGoft.size();i++)//ѭ��vg��������
    {
        for(uint32_t j=0;j<VGinfo[i].size();j++)//�������δ��128����
        {
//            if(useVGoft[i]>>28==7&&keyEfc[i][j]==1)
//            {
//                printf("samFlag��ֵΪ: %d ��ǰΪDp: keyEfc[%d][%d]��Ч",samFlag,i,j);
//                getchar();
//            }
//            printf("keyEfc[%d][%d]=%d",i,j,keyEfc[i][j]);
//            getchar();
            if(samFlag==1&&keyEfc[i][j]==0)//����Ǿ�ȷ��λ���Ҷ�Ӧvoice���������ڸ�����ʹ����
            {
                dataSize+=12;//������Ч��ҲҪ����0,0,0
                continue;//������һ�����
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+j*12,0);//��λ��vg��ÿ��vg��ÿ���ټ���λ��

//            printf("��λ��λ��Ϊ: %X",ftell(g_ipf));
//            getchar();

            fread(&vtdsiz,12,1,g_ipf);//��ȡÿ���ټ�����Ϣ
            dataSize+=12;//ÿ���ټ�����ϢҲ��Ҫ���븴�Ƶĳߴ�

            if(((vtdsiz.tdat1<<24>>24)|0x38)==0x38&&vtdsiz.tdat2>>25<<1==8)//directSound����
            {
//                printf("����������<\n");
                bit32=vtdsiz.tdat2<<5>>5;//��ǰָ���������ַ
//                printf("ds��ǰָ���������ַ: %X\n",bit32);
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                    {
                        VGinfo[i][j]=0x20000000|k;//�ؼ���Ϣ
//                        printf("VG[%d][%d]= %X",i,j,VGinfo[i][j]);
                        goto passWriteSampleoft;
                    }
                }
                fseek(g_ipf,bit32,0);//��λ����������ͷ
                fread(&shead,16,1,g_ipf);//��ȡͷ����
//                printf("%X",shead.loopTag>>1<<3);
//                getchar();
/**
*ĳhack����׼����������,��ͷΪ01,������>>1ȥ�����1
*/
                if(shead.loopTag>>1<<3!=0||shead.datsiz>0xA00000)
                {
                    keyEfc[i][j]=0;//������������ֱ�Ӹ�Ϊ��Ч����һ�������
                    printf("�в����ϱ�׼��������ַ��: %X",(uint32_t)ftell(g_ipf)-16);
                    fflush(stdin);
                    getchar();
                    continue;
                }
                useSampleoft.push_back(bit32);//�����������ݵ�ַ
//                printf("��ǰ���������ݵ�ַΪ: %X VGΪ: %X",bit32,useVGoft[i]);
//                getchar();
                //��i��vg�ĵ�j����λ�����ϸ��Ϣ
                VGinfo[i][j]=0x20000000|(useSampleoft.size()-1);//�ؼ���Ϣд��
//                printf("VG[%d][%d]= %X",i,j,VGinfo[i][j]);
                passWriteSampleoft: continue;
//                if(useVGoft[i]>>28==5)
//                {
//                    printf("��ǰΪms=%X��Ч��λΪ%d",useVGoft[i],j);
//                    getchar();
//                }
//                if(useVGoft[i]>>28==7)
//                {
//                    printf("��ǰΪdp=%X��Ч��λΪ%d",useVGoft[i],j);
//                    getchar();
//                }
            }

            else if(((vtdsiz.tdat1<<24>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy����
            {
                for(uint32_t k=0;k<useWaveMemoft.size();k++)
                {
                    if(useWaveMemoft[k]==vtdsiz.tdat2<<5>>5)
                    {
                        VGinfo[i][j]=0x30000000|k;//�ؼ���Ϣ��¼
                        goto passWriteSampleoft;//������һ��ѭ��
                    }

                }
                useWaveMemoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ��Ĳ������ݵ�ַ
                VGinfo[i][j]=0x30000000|(useWaveMemoft.size()-1);//��¼�ؼ�����Ϣ �����벨���������
            }

            else if(vtdsiz.tdat1<<24>>24==0x40&&vtdsiz.tdat2>>25<<1==8//mulitSample����
                    &&vtdsiz.tdat3>>25<<1==8)
            {
//                printf("��%d��VGoft�ĵ�%d����ֵΪms: \n",i,j);

                bit32=vtdsiz.tdat3<<5>>5;//�õ���Χ��ַ

                for(uint32_t k=0;k<useDamageoft.size();k++)
                {
                    if(useDamageoft[k]==bit32)
                    {
                        VGinfo[i][j]=k;//��Χ�����Ϣ�ڵ�λ ǰ16bitȫ�����˷�Χ���
                        goto passWriteDamageoft;
                    }
                }
                useDamageoft.push_back(bit32);//�����µķ�Χ���ݵ�ַ
                VGinfo[i][j]=useDamageoft.size()-1;//��Χ��ż�¼
                passWriteDamageoft://��Χ��¼���

//                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������
//
//                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//���mulit�ĵ�ַ�Ƿ��ڵ�ǰvg�ķ�Χ��
//                {   //��ַ��VG�е���ż�¼�ڵ�16-23bit ����-VG��ʼ�������12Ȼ������16λ
//                    VGinfo[i][j]=0x40000000|(((vtdsiz.tdat2<<5>>5)-bit32)/12<<16)|VGinfo[i][j];//��VG��Χ�����¼�ؼ���Ϣ
//                    continue;//����,��VG��ƫ��,��Χȫ����¼��,���Խ�����һ����ֵ��
//                }
//              �����ڲ��ڷ�Χ��ѡ����дmulti��VG
                for(uint32_t k=0;k<useVGoft.size();k++)//ָ��ĵ�ַ���ڵ�ǰVG��ַ�е����(ԭ)һ����Ϊ����VG��ַ��
                {
//                    printf("��ǰΪ: %X useVGoft[k]= %X vtdsiz.tdat2= %X",useVGoft[i],useVGoft[k],vtdsiz.tdat2);
//                    getchar();
                    if(useVGoft[k]==(0x50000000|(vtdsiz.tdat2<<5>>5)))//ֻҪ��ǰ��multi��VG�ǹ�ȥ��¼��,���¼���������
                    {
                        //����,��VG���е����,��Χȫ����¼,���Խ�����һ����ֵ��

                        VGinfo[i][j]=0x50000000|(k<<16)|VGinfo[i][j];
                        goto passWriteuseVGoft;//���ظ�������д���µ�;
                    }

                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x50000000);//���ظ��ļ����VG��ַ��,����ѭ�����
//                printf("vtdsiz.tdat2== %X",vtdsiz.tdat2);
//                getchar();
                VGinfo.resize(useVGoft.size());//VGinfoҲ��������
                keyEfc.resize(useVGoft.size());//KeyEfcҲ��������

                passWriteuseVGoft:
//                    printf("��������!:");
//                        getchar();
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//��λ���ټ���Χ����λ��
                uint32_t newtol=0;//��¼�µ�ַ������ʹ����Ŀ
                for(int k=0;k<128;k++)
                {
                    uint32_t c=fgetc(g_ipf);
                    if(c>0x7F) continue;//�����ֵ����127�򲻿���

                    if(c>newtol) newtol=c;//���յó�����õ����ٸ�����
                }
                uint32_t curNum=0;//������¼��ǰ��vtdsiz.tdat2��useVGoft�е�λ��
                while(1)
                {
                    if(useVGoft[curNum]==(0x50000000|(vtdsiz.tdat2<<5>>5))) break;
                    curNum++;
                }
//                printf("newtol= %d\n",newtol);
//                getchar();
//                printf("VGinfo[curNum].size()-1= %d",VGinfo[curNum].size()-1);
//                getchar();
                uint8_t temp=0;
                if(VGinfo[curNum].size()!=0) temp=1;
                if(VGinfo[curNum].size()-temp<newtol)//�����ǰ�ߴ�С���µĳߴ�������
                {
                    VGinfo[curNum].resize(newtol+1,0);//�������ӵ�VG�����µļ�ֵ��
                    keyEfc[curNum].resize(newtol+1,0);//ͬ�ϲ������ݵĲ��ֹ���
                }

//                printf("VGinfo[%d].size()= %d\n",useVGoft.size()-1,VGinfo[useVGoft.size()-1].size());
//                getchar();
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//�ٴζ�λ���ټ���Χ����λ��

//                printf("curNum��ǰ��ֵΪ: %d\n",curNum);
//                getchar();
                for(uint32_t k=0;k<128;k++)
                {
                    uint32_t c=fgetc(g_ipf);
                    if(c>0x7F) continue;
//                    printf("��ǰλ��Ϊ: %X vtdsiz.tdat2=%X�ĵ�%d����λ����Ч��\n",ftell(g_ipf),vtdsiz.tdat2,c);
//                    printf("keyEfc[%d].size()= %d VG= %X",curNum,keyEfc[curNum].size(),useVGoft[i]);
//                    getchar();
                    keyEfc[curNum][c]=1;//��֤���ڷ�Χ��������Ч�Ե�Ϊ1

                }
                //����,��VG���е����,��Χȫ������¼,���Խ�����һ����ֵ��
                VGinfo[i][j]=0x50000000|curNum<<16|VGinfo[i][j];

            }
            else if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
            {
//                printf("��%d��VGoft�ĵ�%d����ֵΪdp: ",i,j);
//                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������
                for(uint32_t k=0;k<useVGoft.size();k++)
                {
                    if(useVGoft[k]==(0x70000000|(vtdsiz.tdat2<<5>>5)))
                    {
                        VGinfo[i][j]=0x70000000|k;//��������VG�е������Ϣ��¼
                        goto passWriteSampleoft;//���ն���������ǰѭ��Ȼ���ٴ�continue;
                    }
                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x70000000);//������ظ�dp
                VGinfo.resize(useVGoft.size());//VGinfoҲ��������
                keyEfc.resize(useVGoft.size());//ͬ��
                VGinfo[useVGoft.size()-1].resize(128,0);
                keyEfc[useVGoft.size()-1].resize(128,0);//����ʱĬ����128����λ
/*!BC KEYSH
BB TEMPO
BD VOICE ��ֵ
BE VOL ����
BF pan ��СΪ0x40
C0 BEND ��СΪ0x40
N07 D6  ���ƴ�d0��ʼ
�ټ�
v����   ���޵��Ǳ������ټ��� Ӧ�ò�����0x7f
w +128 ʱ��??

b4 peng

b3 patt

b2 goto

b1 fine*/

/**
*0x80-0xB0֮����ֽ���delta-timeֵ��0xB1-0xFF֮����ֽ�������
*�ֽ�0x00-0x7F������Ĳ���������������0x80-0xff�ǿ��ܵģ���ͨ���ǡ���ֹ�ġ���
*/

/**
*drum partʹ�õļ�ֵ��track�������м�¼,D0-FF��note���,������ŵľ��Ǽ�ֵ;CE��note��,CF�ǿ�,CFҲ��������
*ͬʱCF note�������(������ž��Ǽ�ֵ) CE note�رձ��(�رտ����ļ�ֵ Ŀǰ�����ֵ�hackһ���׸�����ʱ���õ�.
*/
                for(uint32_t k=0;k<(unsigned)cpyTol;k++)//����������ѭ��
                {
                    for(uint32_t l=0;l<tranum[cpySong[k]-1];l++)//�������ѭ��
                    {
                        fseek(g_ipf,trackoft[cpySong[k]-1][l],0);//�����������ݴ�
                        do
                        {
                           uint32_t w=fgetc(g_ipf);
//                           printf("��ǰ��ȡ��ַΪ: %X w= %X",ftell(g_ipf)-1,w);
//                           getchar();
                           if(w==0xBD)//��ֵ���ñ�־
                           {
                                w=fgetc(g_ipf);//��ȡ���õļ�ֵ
                                //�ĵ�������ǹ̶�Ϊһ�����
                                if(w!=j) break;//�����ǰ�����õļ�ֵ���Ǹ�VG�и�drum part��ֵ ����¸�����
                                //fseek(g_ipf,6,1);//����6���ֽ� BD���������������������dp������� ������������

                                do//��ȵĻ������Ч�ļ�ֵ
                                {
                                    w=fgetc(g_ipf);//������ȡ

                                    if(w>=0xCF)//(CF)D0-FF����dp��ֵ���
                                    {
                                        do
                                        {
                                            w=fgetc(g_ipf);//��ȡ��ֵ
                                            if(w<0x80)//���ܴ���127 С��127���Ǽ�ֵ ��Ȼ����жϺܶ���,����������
                                            {
                                                keyEfc[useVGoft.size()-1][w]=1;//�˹ĵ���Ч
//                                                printf("keyEfc[%d][%d]��Ч ��ǰ��ȡ��ַΪ: %X w= %X",useVGoft.size()-1,w,ftell(g_ipf)-1,w);
//                                                getchar();
                                                w=fgetc(g_ipf);//��ȡ����������
                                                if(w<0x80)//С��128��������
                                                {
                                                    fseek(g_ipf,1,1);//������������������time �����������time
                                                }
                                            }
                                            else if(w>=0xCF) continue;//�ڶ���ѭ���Ż��е��ж� dp��ָ�����
                                            else if(w>0xB0) break;//�ڶ���ѭ���Ż��е��ж� ��dp��ָ��Ļ�������
                                            //else if(w<0x80) fseek(g_ipf,-1,1);//�����ǵĻ�,�п�����������һ�α�ʶ��dp��
                                            //else //ֻ������time��...
                                        }while(1);

                                    }
                                    if(w==0xB2||w==0xB3)//��ת��־
                                        fseek(g_ipf,4,1);//������ַ
                                    if(w==0xB1) break;

                                }while(1);
                           }
                           if(w==0xB1) break;
                           if(w==0xB2||w==0xB3)
                                fseek(g_ipf,4,1);//�����ĸ��ֽ�
                        }while(1);
                    }
                }//�����Ļ�,���и��������������drumpart��ض�������
                for(int k=127;k>=0;k--)//���dpʹ�õ�����ֵλ
                {
                    if(keyEfc[useVGoft.size()-1][k]==1)//�Ӻ���ǰ������һ����Ч�ļ�ֵ
                    {
                        keyEfc[useVGoft.size()-1].resize(k+1);
                        VGinfo[useVGoft.size()-1].resize(k+1);//�����������ߵĳ���
//                        printf("�������õĳ���Ϊ: %d",k+1);
//                        getchar();
                        break;//�״δ���Ч������
                    }
                }
                VGinfo[i][j]=0x70000000|(useVGoft.size()-1);//��Ϣ��¼ʹ�õ�VGoft���

            }
            else
            {
                VGinfo[i][j]=0x10000000;
            }
        }

    }//�������еļ�λ���ݳߴ綼��¼,���Ҽ�¼��keyinfo�Լ�����VG��,��������,����,��Χ���ݶ���¼

    printf("�ټ��ϼ�ֵ��Ϣ����Ϊ: %X\n",dataSize);
    bitot=dataSize;
//    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        dataSize+=32;//ÿһ���������ݶ�ռ��32�ֽ�
    }

    printf("�������ݳ���Ϊ: %X\n",dataSize-bitot);
    bitot=dataSize;
//    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        dataSize+=128;//ÿһ����Χ��ռ��128�ֽ�
    }
    printf("��Χ���ݳ���Ϊ: %X\n",dataSize-bitot);
    bitot=dataSize;
//    getchar();
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//��λ��ÿ���������ݵ���ʼ
        fread(&shead,16,1,g_ipf);//��ȡ��ǰ������ͷ����
        dataSize+=shead.datsiz+0x13-(shead.loopTag>>30);//ÿ�����������ݳ��ȼ���
        // printf("����%d�ĵ�ַΪ: %X �ߴ�Ϊ: %X",i,useSampleoft[i],shead.datsiz);
        if(dataSize%4!=0)
            dataSize=dataSize+4-dataSize%4;//ÿ���������ս�������4����
    }
    //����head����,��������,��������,��λ����,��Χ����,��������ȫ����ͳ������.
    printf("�������ݳ���Ϊ: %X\n",dataSize-bitot);
	// getchar();
//	for(uint16_t i=0;i<useVGoft.size();i++)
//        printf("useVGoft[%d]=%X",i,useVGoft[i]);
//    std::vector<std::vector<uint8_t> >::iterator it=keyEfc.begin();
//	for(;it!=keyEfc.end();it++){
//        printf("%d ",*it);
//	}
//	getchar();
//    printf("useSampleoft.size()=%d",useSampleoft.size());
//    getchar();
//    std::vector<uint32_t>::iterator it=useSampleoft.begin();
//    for(;it!=useSampleoft.end();++it){
//        printf("%X\n",*it);
//        getchar();
//    }
    fclose(g_ipf);
    system("cls");
    return dataSize;
}
