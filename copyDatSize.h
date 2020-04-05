#ifndef COPYDATSIZE_H_INCLUDED
#define COPYDATSIZE_H_INCLUDED
#include <vector>
#include <cstdint>

uint32_t calculateCopySize(int samFlag,int cpyTol);

struct samplehead
{
    uint32_t loopTag;//>>30
    uint32_t freq;//>>8
    uint32_t loop;
    uint32_t datsiz;
};

extern std::vector<uint32_t>useSampleoft;//ÿһ�����ظ�������
extern std::vector<uint32_t>useWaveMemoft;//ÿһ�����ظ��Ĳ���
extern std::vector<uint32_t>useDamageoft;//ÿһ�����ظ��ķ�Χ
extern std::vector<uint32_t>useVGoft;//ÿһ�����ظ���VoiceGroup
extern std::vector<uint8_t>trackUseVG_No;//ÿһ������ʹ�õ�vg���
extern std::vector<std::vector<uint32_t> >VGinfo;//��¼vg��ÿ����������Ϣ
extern std::vector<std::vector<uint8_t> >keyEfc;//��¼ÿ���ټ��Ƿ���ı�ʹ�õ�

//extern std::vector<uint32_t>mulitSampleoft;//����һ�����������¼����͵����ݵ�ַ��ֹ�ظ�
//extern std::vector<uint32_t>drumPartoft;//����һ�����������¼dp���͵����ݵ�ַ��ֹ�ظ�
//extern std::vector<uint32_t>waveMemoryoft;//����һ�����������¼wm���͵����ݵ�ַ��ֹ�ظ�

//static uint8_t waitFlag=1;
//
//void *printProg(void *arg)
//{
//    const char *label="|/��\\";
//    uint8_t accum=1;
//    while(accum)
//    {
//        system("cls");
//        printf("���Ժ�,���ڼ��㸴�Ƶ����������������..%c",label[accum%4]);
//        fflush(stdout);
//        if(!waitFlag) break;
//        accum++;
//        Sleep(100);//static uint8_t waitFlag=1;
//
//void *printProg(void *arg)
//{
//    const char *label="|/��\\";
//    uint8_t accum=1;
//    while(accum)
//    {
//        system("cls");
//        printf("���Ժ�,���ڼ��㸴�Ƶ����������������..%c",label[accum%4]);
//        fflush(stdout);
//        if(!waitFlag) break;
//        accum++;
//        Sleep(100);
//    }
//    system("cls");
//    return 0;
//}
//    }
//    system("cls");
//    return 0;
//}


//    pthread_t printid;//�����̵߳�id��
//    int Prog;
//    //����һ���߳�
//    Prog=pthread_create(&printid,NULL,printProg,NULL);
//    if(Prog!=0)
//    {
//        fprintf(stderr,"�������߳�ʧ��!\n��ȴ��������...");
//    }
//    //���߳̽���ʱ�Զ��ͷ�ID��
//    Prog=pthread_detach(printid);
        //����̻ᵼ�³�������,��ʱ������

#endif // COPYDATSIZE_H_INCLUDED
