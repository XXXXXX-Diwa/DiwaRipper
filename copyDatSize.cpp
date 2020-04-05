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
//std::vector<uint32_t>mulitSampleoft(0,0);//����һ�����������¼����͵����ݵ�ַ��ֹ�ظ�
//std::vector<uint32_t>drumPartoft(0,0);//����һ�����������¼dp���͵����ݵ�ַ��ֹ�ظ�
//std::vector<uint32_t>waveMemoryoft(0,0);//����һ�����������¼wm���͵����ݵ�ַ��ֹ�ظ�

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
uint32_t calculateCopySize(int samFlag,int cpyTol)
{

    uint32_t bitot=0;//��ʱ��

    epyVector();//ÿ�ζ����Ҫ�õ��Ѿ�����������
    system("cls");
    printf("���ڼ����ܹ���Ҫ���ƶ�������,���Ե�..");

    g_ipf=fopen(g_ipfName.c_str(),"rb");
    if(g_ipf==nullptr)
    {
        printf("����rom�ļ�ʧ��!");
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
    for(int i=0;i<cpyTol;i++)
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

    //����vg���ټ��Ƿ�ʹ��,ͬʱ��¼ÿ��head+����ĳߴ�
    keyEfc.resize(useVGoft.size());//����VG����������
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        keyEfc[i].resize(128,0);//ͬʱ����ÿ��VG��128���ټ�efc ��ȫ��Ϊ0
    }
    for(int i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,headoft[cpySong[i]-1]<<5>>5,0);//��λ��ÿ��������head��ַ
        fread(&bit16,2,1,g_ipf);//��������
        dataSize+=(bit16*4+8);//head���ݳ��ȼ�¼ ������*4+8
        fseek(g_ipf,6,1);//��λ��ÿ������ĵ�ַ
        uint32_t fstTrack=ftell(g_ipf);//�����������ַ��ַ
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
                    int c=fgetc(g_ipf);
                    //���ݸ�����ŵõ�����ʹ��vg����ٵ����ڵļ�λ�ŵ�flag
                    keyEfc[trackUseVG_No[i]][(uint8_t)c]=1;
                    dataSize++;//ÿ��ȡ�������ݵ�һ���ֽڶ�+1
                }
                else if(c==0xb2||c==0xb3)//��ת�����ñ�־,�����ǵ�ַ
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

    printf("head���ݺ��������ݳ���Ϊ: %X",dataSize);
    getchar();
    vtda_t vtdsiz;//���ݽṹ������ȡ12���ֽ�
    VGinfo.resize(useVGoft.size());//VG��Ϣ��ά�������VG��������
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        VGinfo[i].resize(128,0);//�����VG����չ��128����,������
    }

    for(uint32_t i=0;i<useVGoft.size();i++)//ѭ��vg��������
    {
        for(uint32_t j=0;j<VGinfo[i].size();j++)//�������δ��128����
        {
            if(i<keyEfc.size()&&samFlag==1&&keyEfc[i][j]==0)//����Ǿ�ȷ��λ���Ҷ�Ӧvoice���������ڸ�����ʹ����
            {
                dataSize+=12;//������Ч��ҲҪ����0,0,0
                continue;//������һ�����
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+j*12,0);//��λ��vg��ÿ��vg��ÿ���ټ���λ��
            fread(&vtdsiz,12,1,g_ipf);//��ȡÿ���ټ�����Ϣ
            dataSize+=12;//ÿ���ټ�����ϢҲ��Ҫ���븴�Ƶĳߴ�

            if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound����
            {
                bit32=vtdsiz.tdat2<<5>>5;
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                    {
                        VGinfo[i][j]=0x20000000|k;//�ؼ���Ϣ
                        goto passWriteSampleoft;
                    }
                }
                useSampleoft.push_back(bit32);//�����������ݵ�ַ
                //��i��vg�ĵ�j����λ�����ϸ��Ϣ
                VGinfo[i][j]=0x20000000|(useSampleoft.size()-1);//�ؼ���Ϣд��
                passWriteSampleoft: continue;
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

                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������

                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//���mulit�ĵ�ַ�Ƿ��ڵ�ǰvg�ķ�Χ��
                {   //��ַ��VG�е���ż�¼�ڵ�16-23bit ����-VG��ʼ�������12Ȼ������16λ
                    VGinfo[i][j]=0x40000000|(((vtdsiz.tdat2<<5>>5)-bit32)/12<<16)|VGinfo[i][j];//��VG��Χ�����¼�ؼ���Ϣ
                    continue;//����,��VG��ƫ��,��Χȫ����¼��,���Խ�����һ����ֵ��
                }
                for(uint32_t k=0;k<useVGoft.size();k++)//ָ��ĵ�ַ���ڵ�ǰVG��ַ��
                {
                    if(useVGoft[k]<<4>>4==vtdsiz.tdat2<<5>>5)
                    {
                        //����,��VG���е����,��Χȫ����¼,���Խ�����һ����ֵ��
                        VGinfo[i][j]=0x50000000|(k<<16)|VGinfo[i][j];
                        goto passWriteSampleoft;//���ն���������ǰѭ��Ȼ���ٴ�continue;
                    }

                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x50000000);//�����VG��ַ��,����ѭ�����
                VGinfo.resize(useVGoft.size());//VGinfoҲ��������
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//��λ���ټ���Χ����λ��
                int newtol=0;//��¼�µ�ַ������ʹ����Ŀ
                for(int k=0;k<128;k++)
                {
                    int c=fgetc(g_ipf);
                    if(c==0xFF) continue;
                    if(c>newtol) newtol=c;//���յó��ж��ٸ�����
                }
                VGinfo[useVGoft.size()-1].resize(newtol+1);//�������ӵ�VG�����µļ�ֵ��
                //����,��VG���е����,��Χȫ������¼,���Խ�����һ����ֵ��
                VGinfo[i][j]=0x50000000|(useVGoft.size()-1)<<16|VGinfo[i][j];

            }
            else if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
            {
                printf("��ֵ%d��dp",j);
                getchar();

                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������
                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//���drum�Ƿ��ڵ�ǰvg�ķ�Χ��
                {
                    VGinfo[i][j]=0x60000000|((vtdsiz.tdat2<<5>>5)-bit32)/12;//������ƫ��ֵ��Ϣ��¼
                    //����ƫ�Ƶĳ���/12���ǽ�����
                    continue;//�����������VG�ļ��,��һ��
                }

                for(uint32_t k=0;k<useVGoft.size();k++)
                {
                    if(useVGoft[k]<<4>>4==vtdsiz.tdat2<<5>>5)
                    {
                        VGinfo[i][j]=0x70000000|k;//��������VG�е������Ϣ��¼
                        goto passWriteSampleoft;//���ն���������ǰѭ��Ȼ���ٴ�continue;
                    }
                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x70000000);//������ظ�Ҳ���ڵ�ǰvg��Χ�ڵ�dp
                VGinfo.resize(useVGoft.size());//VGinfoҲ��������
                int ki=0;
                for(;ki<128;ki++)
                {
                    fseek(g_ipf,vtdsiz.tdat2<<5>>5,0);//��λ��drumpart�ĵ�ַ
                    fread(&vtdsiz,12,1,g_ipf);

                    if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
                    {
                        VGinfo[useVGoft.size()-1].resize(ki);//���ݵ��������dp
                        break;//���������������
                    }
                    else if(vtdsiz.tdat1<<24>>24==0x40&&vtdsiz.tdat2>>25<<1==8//mulitSample����
                    &&vtdsiz.tdat3>>25<<1==8)
                    {
                        VGinfo[useVGoft.size()-1].resize(ki);//���ݵ��������dp
                        break;//���������������,һ��������������
                    }
//                    else
//                        dataSize+=12;//���պϵ����ͼ�����  //����������Ϊ���ڴ�ѭ���м�
                }
                printf("ki��ֵΪ: %X",ki);
                getchar();

                VGinfo[useVGoft.size()-1].resize(ki);//��ֵҲ��������
                VGinfo[i][j]=0x70000000|(useVGoft.size()-1);//��Ϣ��¼

            }
            else
            {
                VGinfo[i][j]=0x10000000;
            }
        }

    }//�������еļ�λ���ݳߴ綼��¼,���Ҽ�¼��keyinfo�Լ�����VG��,��������,����,��Χ���ݶ���¼

    printf("�ټ��ϼ�ֵ��Ϣ����Ϊ: %X",dataSize);
    bitot=dataSize;
    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        dataSize+=32;//ÿһ���������ݶ�ռ��32�ֽ�
    }

    printf("�ټ��ϲ������ݳ���Ϊ: %X",dataSize);
    printf("�������ݳ���Ϊ: %X",dataSize-bitot);
    bitot=dataSize;
    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        dataSize+=128;//ÿһ����Χ��ռ��128�ֽ�
    }

    printf("�ټ��Ϸ�Χ���ݳ���Ϊ: %X",dataSize);
    printf("��Χ���ݳ���Ϊ: %X",dataSize-bitot);
    bitot=dataSize;
    getchar();

    samplehead shead;//���ݽṹ��ȡ����ͷ������
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//��λ��ÿ���������ݵ���ʼ
        fread(&shead,16,1,g_ipf);//��ȡ��ǰ������ͷ����
        dataSize+=shead.datsiz+13-(shead.loopTag>>30);//ÿ�����������ݳ��ȼ���
        if(dataSize%4!=0)
            dataSize=dataSize+4-dataSize%4;//ÿ���������ս�������4����
    }
    //����head����,��������,��������,��λ����,��Χ����,��������ȫ����ͳ������.
    printf("�ټ����������ݳ���Ϊ: %X",dataSize);
    printf("�������ݳ���Ϊ: %X",dataSize-bitot);

    getchar();
    /*
    uint8_t sampleUse[useVoiceTol][128]={};//�����ά��������������е�voiceGroup��������ʹ�����
    uint8_t curTrackVoiceNum=0;//������¼��ǰvoiceGroup��vg���е�λ��

    for(int i=0;i<cpyTol;i++)
    {
        curTrackVoiceNum=0;//��ǰ������voice�����е���Ź���
        fseek(g_ipf,headoft[cpySong[i]]<<5>>5,0);//��λ��ÿ��������head��ַ

        fread(&bit16,2,1,g_ipf);//��������
        dataSize+=(bit16*4+8);//head���ݳ���
        fseek(g_ipf,2,1);//ƫ�Ƶ�voicegroup�ĵ�ַ����λ��
        fread(&bit32,4,1,g_ipf);

        while(expVoiceoft[curTrackVoiceNum]!=bit32<<5>>5)
            curTrackVoiceNum++;//����voicegroup��ַ��Ӧ����ѭ��

        uint32_t curTabTrackFrtOft=ftell(g_ipf);//��¼����������ݵ�ַ
        for(int j=0;j<bit16;j++)
        {
            fseek(g_ipf,curTabTrackFrtOft+j*4,0);//��λ��ÿ����������ݵ�ַ
            fread(&bit32,4,1,g_ipf);

            fseek(g_ipf,bit32<<5>>5,0);//��λ��track����λ��
            do
            {
                int c=fgetc(g_ipf);

                dataSize++;//ÿ��ȡһ���ֽڶ�+1
                if(c==0xb1) break;//������Ϣ������־

                else if(c==0xbd)
                {
                    int c=fgetc(g_ipf);
                    //��Ӧ��sampleʹ�������¼
                    sampleUse[curTrackVoiceNum][(unsigned)c]=1;
                    dataSize++;
                }
                else if(c==0xb2||c==0xb3)
                {
                    fseek(g_ipf,4,1);//������ַ���ĸ��ֽ�
                    dataSize+=4;
                }

            }while(1);//(!feof());//�����ٶȵ�����..

        }//���˵�ǰ��track�����ݳ��ȼ������

    }//����ȫ����track�����ݺ�head���ݼ������

    vtda_t vtdsiz;//ʹ��showsamͷ�ļ������ݽṹ

    for(int i=0;i<useVoiceTol;i++)
    {
        for(int j=0;j<128;j++)//�ܹ�128����
        {
            if(samFlag==1&&sampleUse[i][j]==0)//����Ǿ�ȷ��λ���Ҷ�Ӧvoice���������ڸ�����ʹ����
                continue;//������һ�����
            fseek(g_ipf,expVoiceoft[i]+j*12,0);//��λ�����ʵ�ƫ��voice��ŵ�ַ

            fread(&vtdsiz,12,1,g_ipf);//�����Ч���ȡ����
            dataSize+=12;
            if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound����
            {
                bit32=vtdsiz.tdat2<<5>>5;
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                        goto passWriteSampleoft;
                }
                useSampleoft.push_back(bit32);//�����������ݵ�ַ
                passWriteSampleoft: continue;
            }
            else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample����
            {
                dataSize+=128;//�ټ��˺���Χ��Ҫ128�ֽ�
                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������
                if(vtdsiz.tdat2<<5>>5>bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//���mulit�Ƿ��ڵ�ǰvg�ķ�Χ��
                    continue;//�ڷ�Χ���������һ�����,��Ϊ�ڷ�Χ����������ַ����Ҫ��¼
                        //(��Ȼ���亱���������������..ֻ���п���,���ǻ���������)
                for(uint32_t k=0;k<mulitSampleoft.size();k++)
                {
                    if(mulitSampleoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;//���ն���������ǰѭ��Ȼ���ٴ�continue;
                }
                mulitSampleoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ�Ҳ���ٵ�ǰvg��Χ�ڵ�ms
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//��λ���ټ���Χ����λ��
                int newtol=0;//��¼�µ�ַ������ʹ����Ŀ
                for(int k=0;k<128;k++)
                {
                    int c=fgetc(g_ipf);
                    if(c==0xFF) continue;
                    if(c>newtol) newtol=c;//���յó��ж��ٸ�����
                }
                for(int k=0;k<=newtol;k++)
                {
                    fseek(g_ipf,(vtdsiz.tdat2<<5>>5)+k*12,0);//��λ���µ�voice��ַ
                    fread(&vtdsiz,12,1,g_ipf);//��ȡ������
                    dataSize+=12;
                    if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound����
                    {
                        bit32=vtdsiz.tdat2<<5>>5;
                        for(uint32_t k=0;k<useSampleoft.size();k++)
                        {
                            if(useSampleoft[k]==bit32)
                                goto passWriteSampleoft1;
                        }
                        useSampleoft.push_back(bit32);//�����������ݵ�ַ
                        passWriteSampleoft1: continue;
                    }
                    else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy����
                    {
                        for(uint32_t k=0;k<waveMemoryoft.size();k++)
                        {
                            if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                                goto passWriteSampleoft1;
                        }
                        waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ��Ĳ������ݵ�ַ
                        dataSize+=32;//�������ݳ���
                    }

                    else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
                        break;//���������������

                    else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample����
                        break;//���������������,һ��������������

                }
            }
            else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
            {
                bit32=ftell(g_ipf)-12*(j+1);//�õ�voiceGroup��ʼ������
                if(vtdsiz.tdat2<<5>>5>bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//���drum�Ƿ��ڵ�ǰvg�ķ�Χ��
                    continue;//�ڷ�Χ���������һ�����,��Ϊ�ڷ�Χ����������ַ����Ҫ��¼
                        //(��Ȼ���亱���������������..ֻ���п���,���ǻ���������)
                for(uint32_t k=0;k<drumPartoft.size();k++)
                {
                    if(drumPartoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;//���ն���������ǰѭ��Ȼ���ٴ�continue;
                }
                drumPartoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ�Ҳ���ٵ�ǰvg��Χ�ڵ�dp
                for(int k=0;k<128;k++)
                {
                    fseek(g_ipf,vtdsiz.tdat2<<5>>5,0);//��λ��drumpart�ĵ�ַ
                    fread(&vtdsiz,12,1,g_ipf);
                    dataSize+=12;
                    if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound����
                    {
                        bit32=vtdsiz.tdat2<<5>>5;
                        for(uint32_t k=0;k<useSampleoft.size();k++)
                        {
                            if(useSampleoft[k]==bit32)
                                goto passWriteSampleoft2;
                        }
                        useSampleoft.push_back(bit32);//�����������ݵ�ַ
                        passWriteSampleoft2: continue;
                    }
                    else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy����
                    {
                        for(uint32_t k=0;k<waveMemoryoft.size();k++)
                        {
                            if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                                goto passWriteSampleoft2;
                        }
                        waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ��Ĳ������ݵ�ַ
                        dataSize+=32;//�������ݳ���
                    }

                    else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part����
                        break;//���������������

                    else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample����
                        break;//���������������,һ��������������


                }
            }
            else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy����
            {
                for(uint32_t k=0;k<waveMemoryoft.size();k++)
                {
                    if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;
                }
                waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//������ظ��Ĳ������ݵ�ַ
                dataSize+=32;//�������ݳ���
            }
        }
    }//���˳�������������û�м�����,��������ȫ���������
    samplehead samh;
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//��λ��ÿ���������ݵ�λ��
        fread(&samh,16,1,g_ipf);//��ȡ����ͷ
        if(samh.loopTag!=0&&samh.loopTag>>24!=0x40)
        {
            printf("�������ݴ���!");
            getchar();
        }
        dataSize+=samh.datsiz+13-(samh.loopTag>>30);
    }
    */
    fclose(g_ipf);
    system("cls");
    return dataSize;
}
