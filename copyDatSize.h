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

extern std::vector<uint32_t>useSampleoft;//每一个不重复的样本
extern std::vector<uint32_t>useWaveMemoft;//每一个不重复的波形
extern std::vector<uint32_t>useDamageoft;//每一个不重复的范围
extern std::vector<uint32_t>useVGoft;//每一个不重复的VoiceGroup
extern std::vector<uint8_t>trackUseVG_No;//每一个歌曲使用的vg序号
extern std::vector<std::vector<uint32_t> >VGinfo;//记录vg的每个样本的信息
extern std::vector<std::vector<uint8_t> >keyEfc;//记录每个琴键是否真的被使用到

//extern std::vector<uint32_t>mulitSampleoft;//定义一个向量数组记录组合型的数据地址防止重复
//extern std::vector<uint32_t>drumPartoft;//定义一个向量数组记录dp类型的数据地址防止重复
//extern std::vector<uint32_t>waveMemoryoft;//定义一个向量数组记录wm类型的数据地址防止重复

//static uint8_t waitFlag=1;
//
//void *printProg(void *arg)
//{
//    const char *label="|/—\\";
//    uint8_t accum=1;
//    while(accum)
//    {
//        system("cls");
//        printf("请稍后,正在计算复制的数据所需的总容量..%c",label[accum%4]);
//        fflush(stdout);
//        if(!waitFlag) break;
//        accum++;
//        Sleep(100);//static uint8_t waitFlag=1;
//
//void *printProg(void *arg)
//{
//    const char *label="|/—\\";
//    uint8_t accum=1;
//    while(accum)
//    {
//        system("cls");
//        printf("请稍后,正在计算复制的数据所需的总容量..%c",label[accum%4]);
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


//    pthread_t printid;//创建线程的id号
//    int Prog;
//    //创建一条线程
//    Prog=pthread_create(&printid,NULL,printProg,NULL);
//    if(Prog!=0)
//    {
//        fprintf(stderr,"创建新线程失败!\n请等待计算完毕...");
//    }
//    //当线程结束时自动释放ID号
//    Prog=pthread_detach(printid);
        //多进程会导致程序问题,暂时不用了

#endif // COPYDATSIZE_H_INCLUDED
