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
需要声明定义的数组:
每个vg的地址 一维数组 如果是样本集和drum并不在vg的范围内
则首4bit标记,样本集标记为1,drum标记为2
每个vg的128个信息 二维数组
信息内容:一维代表vg序号
	二维代表样本号 波形号
		样本集 高位标记是否是vg内 地位在vg基础上的偏移
		drum集 高位标记是否是在vg内 同上
		如果在vg内,则范围记录低16位,与VG的偏移次数记录在16-23bit
		如果不在vg内,则增加vg的数量和地址.并标记与普通的VG不同
		而该二维数组记录类型,低16位依然是范围,在VG上的地址序号记录在16-27bit
二维的第二维度的四个字节信息
首字节为 1其它 2样本 3波形 4集合(仅偏移) 5集合(vg外) 6drum(仅偏移) 7drum(vg外) 0无用

每个不重复的样本 一维数组
每个不重复的波形 一维数组
每一个不重复范围 一维数组
导出的歌曲号	一维数组
导入的歌曲号	一维数组
记录每个歌曲使用的vg序号 一维数组
*/
std::vector<uint32_t>useSampleoft(0,0);//定义一个向量数组记录样本数据地址
std::vector<uint32_t>useWaveMemoft(0,0);//每一个不重复的波形
std::vector<uint32_t>useDamageoft(0,0);//每一个不重复的范围
std::vector<uint32_t>useVGoft(0,0);//每一个不重复的VoiceGroup
std::vector<uint8_t>trackUseVG_No(0,0);//每一个歌曲使用的vg序号
std::vector<std::vector<uint32_t> >VGinfo(0,std::vector<uint32_t>(0,0));//记录vg的每个样本信息的二维向量
std::vector<std::vector<uint8_t> >keyEfc(0,std::vector<uint8_t>(0,0));//记录每个琴键是否真的被使用到
//std::vector<uint32_t>mulitSampleoft(0,0);//定义一个向量数组记录组合型的数据地址防止重复
//std::vector<uint32_t>drumPartoft(0,0);//定义一个向量数组记录dp类型的数据地址防止重复
//std::vector<uint32_t>waveMemoryoft(0,0);//定义一个向量数组记录wm类型的数据地址防止重复

void epyVector()
{
    std::vector<uint32_t>().swap(useSampleoft);//清空内存和数量
    std::vector<uint32_t>().swap(useWaveMemoft);//每一个不重复的波形
    std::vector<uint32_t>().swap(useDamageoft);//每一个不重复的范围
    std::vector<uint32_t>().swap(useVGoft);//每一个不重复的VoiceGroup
    std::vector<uint8_t>().swap(trackUseVG_No);//每一个歌曲使用的vg序号
    std::vector<std::vector<uint32_t> >b(0,std::vector<uint32_t>(0,0));
    std::vector<std::vector<uint8_t> >a(0,std::vector<uint8_t>(0,0));
    b.swap(VGinfo);
    a.swap(keyEfc);

}

/*!

*/
uint32_t calculateCopySize(int samFlag,int cpyTol)
{

    uint32_t bitot=0;//临时用

    epyVector();//每次都清空要用的已经声明的向量
    system("cls");
    printf("正在计算总共需要复制多少数据,请稍等..");

    g_ipf=fopen(g_ipfName.c_str(),"rb");
    if(g_ipf==nullptr)
    {
        printf("打开主rom文件失败!");
        getchar();
        exit(1);
    }

    uint16_t bit16=0;
    uint32_t bit32=0;
    uint32_t dataSize=0;//累计数据总长度
//    uint32_t expVoiceoft[cpyTol];//导出的歌曲voice地址用来防止重复
//    uint8_t useVoiceTol=0;//用来记录当前使用了多少个voicegroup

    //首先计算有多少个voice
    trackUseVG_No.resize(cpyTol,0);//根据歌曲数调整数量
    for(int i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,(headoft[cpySong[i]-1]<<5>>5)+4,0);//定位到导出歌曲的head数据VG处
        fread(&bit32,4,1,g_ipf);//读取vg地址

        for(uint32_t j=0;j<useVGoft.size();j++)
        {
            if(bit32<<5>>5==useVGoft[j])
            {//i为歌曲序号,j为vg序号,保存每个歌曲的重复vg序号
                trackUseVG_No[i]=j;
                goto passVGWrite;
            }
        }
        useVGoft.push_back(bit32<<5>>5);//如果不重复则添加
        //当不重复的vg号增加的时候,当前歌曲记录这个不重复的vg序号
        trackUseVG_No[i]=useVGoft.size()-1;
        passVGWrite: continue;

    }//至此已经记录了要用到的voiceGroup和数量以及每个歌曲的VG序号

    //计算vg中琴键是否被使用,同时记录每个head+音轨的尺寸
    keyEfc.resize(useVGoft.size());//根据VG数调整数量
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        keyEfc[i].resize(128,0);//同时调整每个VG有128个琴键efc 并全部为0
    }
    for(int i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,headoft[cpySong[i]-1]<<5>>5,0);//定位到每个歌曲的head地址
        fread(&bit16,2,1,g_ipf);//歌曲数量
        dataSize+=(bit16*4+8);//head数据长度记录 音轨数*4+8
        fseek(g_ipf,6,1);//定位到每个音轨的地址
        uint32_t fstTrack=ftell(g_ipf);//保存首音轨地址地址
//        printf("歌曲%d 的首音轨地址为: %X\n",cpySong[i],fstTrack);
        for(int j=0;j<bit16;j++)
        {
            fseek(g_ipf,fstTrack+j*4,0);//定位到每个音轨地址
            fread(&bit32,4,1,g_ipf);//读取每个音轨的地址
            fseek(g_ipf,bit32<<5>>5,0);//定位到每个音轨数据位置
            do
            {
                int c=fgetc(g_ipf);

                dataSize++;//每读取音轨数据的一个字节都+1
                if(c==0xb1) break;//歌曲信息结束标志

                else if(c==0xbd)//键位标志
                {
                    int c=fgetc(g_ipf);
                    //根据歌曲序号得到歌曲使用vg序号再到所在的键位号的flag
                    keyEfc[trackUseVG_No[i]][(uint8_t)c]=1;
                    dataSize++;//每读取音轨数据的一个字节都+1
                }
                else if(c==0xb2||c==0xb3)//跳转和引用标志,后面是地址
                {
                    fseek(g_ipf,4,1);//跳过地址的四个字节
                    dataSize+=4;
                }

            }while(1);//(!feof());//提升速度的设置..

//            printf("当前音轨的结束位置为: %X\n",(unsigned)ftell(g_ipf));
        }
        if(dataSize%4!=0)
            dataSize+=4-dataSize%4;//每个歌曲的全部音轨的数据最终结束要与4对齐

    }//至此,每个音轨和歌曲的head数据记录完毕.同时vg键位的有效性也标记了.

    printf("head数据和音轨数据长度为: %X",dataSize);
    getchar();
    vtda_t vtdsiz;//数据结构用来读取12个字节
    VGinfo.resize(useVGoft.size());//VG信息二维数组根据VG数量扩容
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        VGinfo[i].resize(128,0);//正规的VG都扩展到128个键,并归零
    }

    for(uint32_t i=0;i<useVGoft.size();i++)//循环vg数量次数
    {
        for(uint32_t j=0;j<VGinfo[i].size();j++)//非正规的未必128个键
        {
            if(i<keyEfc.size()&&samFlag==1&&keyEfc[i][j]==0)//如果是精确定位并且对应voice的样本不在歌曲的使用中
            {
                dataSize+=12;//就算无效的也要留空0,0,0
                continue;//进行下一个检查
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+j*12,0);//定位到vg组每个vg的每个琴键的位置
            fread(&vtdsiz,12,1,g_ipf);//读取每个琴键的信息
            dataSize+=12;//每个琴键的信息也是要算入复制的尺寸

            if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound类型
            {
                bit32=vtdsiz.tdat2<<5>>5;
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                    {
                        VGinfo[i][j]=0x20000000|k;//关键信息
                        goto passWriteSampleoft;
                    }
                }
                useSampleoft.push_back(bit32);//保存样本数据地址
                //第i个vg的第j个键位标记详细信息
                VGinfo[i][j]=0x20000000|(useSampleoft.size()-1);//关键信息写入
                passWriteSampleoft: continue;
            }

            else if(((vtdsiz.tdat1<<24>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy类型
            {
                for(uint32_t k=0;k<useWaveMemoft.size();k++)
                {
                    if(useWaveMemoft[k]==vtdsiz.tdat2<<5>>5)
                    {
                        VGinfo[i][j]=0x30000000|k;//关键信息记录
                        goto passWriteSampleoft;//继续下一个循环
                    }

                }
                useWaveMemoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复的波形数据地址
                VGinfo[i][j]=0x30000000|(useWaveMemoft.size()-1);//记录关键的信息 类型与波形数据序号
            }

            else if(vtdsiz.tdat1<<24>>24==0x40&&vtdsiz.tdat2>>25<<1==8//mulitSample类型
                    &&vtdsiz.tdat3>>25<<1==8)
            {
                bit32=vtdsiz.tdat3<<5>>5;//得到范围地址

                for(uint32_t k=0;k<useDamageoft.size();k++)
                {
                    if(useDamageoft[k]==bit32)
                    {
                        VGinfo[i][j]=k;//范围序号信息在低位 前16bit全部给了范围序号
                        goto passWriteDamageoft;
                    }
                }
                useDamageoft.push_back(bit32);//保存新的范围数据地址
                VGinfo[i][j]=useDamageoft.size()-1;//范围序号记录
                passWriteDamageoft://范围记录完毕

                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标

                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//检查mulit的地址是否在当前vg的范围内
                {   //地址在VG中的序号记录在第16-23bit 坐标-VG起始坐标除以12然后左移16位
                    VGinfo[i][j]=0x40000000|(((vtdsiz.tdat2<<5>>5)-bit32)/12<<16)|VGinfo[i][j];//在VG范围内则记录关键信息
                    continue;//种类,与VG的偏移,范围全都记录了,可以进行下一个键值了
                }
                for(uint32_t k=0;k<useVGoft.size();k++)//指向的地址不在当前VG地址中
                {
                    if(useVGoft[k]<<4>>4==vtdsiz.tdat2<<5>>5)
                    {
                        //种类,在VG组中的序号,范围全都记录,可以进行下一个键值了
                        VGinfo[i][j]=0x50000000|(k<<16)|VGinfo[i][j];
                        goto passWriteSampleoft;//最终都是跳出当前循环然后再次continue;
                    }

                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x50000000);//加入进VG地址中,进行循环检查
                VGinfo.resize(useVGoft.size());//VGinfo也跟着扩容
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//定位到琴键范围数据位置
                int newtol=0;//记录新地址的样本使用数目
                for(int k=0;k<128;k++)
                {
                    int c=fgetc(g_ipf);
                    if(c==0xFF) continue;
                    if(c>newtol) newtol=c;//最终得出有多少个样本
                }
                VGinfo[useVGoft.size()-1].resize(newtol+1);//给予增加的VG定义新的键值数
                //种类,在VG组中的序号,范围全部都记录,可以进行下一个键值了
                VGinfo[i][j]=0x50000000|(useVGoft.size()-1)<<16|VGinfo[i][j];

            }
            else if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
            {
                printf("键值%d是dp",j);
                getchar();

                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标
                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//检查drum是否在当前vg的范围内
                {
                    VGinfo[i][j]=0x60000000|((vtdsiz.tdat2<<5>>5)-bit32)/12;//类型与偏移值信息记录
                    //最终偏移的长度/12就是结束点
                    continue;//反正不会进入VG的检查,下一个
                }

                for(uint32_t k=0;k<useVGoft.size();k++)
                {
                    if(useVGoft[k]<<4>>4==vtdsiz.tdat2<<5>>5)
                    {
                        VGinfo[i][j]=0x70000000|k;//类型与在VG中的序号信息记录
                        goto passWriteSampleoft;//最终都是跳出当前循环然后再次continue;
                    }
                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x70000000);//保存非重复也不在当前vg范围内的dp
                VGinfo.resize(useVGoft.size());//VGinfo也跟着扩容
                int ki=0;
                for(;ki<128;ki++)
                {
                    fseek(g_ipf,vtdsiz.tdat2<<5>>5,0);//定位到drumpart的地址
                    fread(&vtdsiz,12,1,g_ipf);

                    if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
                    {
                        VGinfo[useVGoft.size()-1].resize(ki);//扩容到不到这个dp
                        break;//不可能连续是组合
                    }
                    else if(vtdsiz.tdat1<<24>>24==0x40&&vtdsiz.tdat2>>25<<1==8//mulitSample类型
                    &&vtdsiz.tdat3>>25<<1==8)
                    {
                        VGinfo[useVGoft.size()-1].resize(ki);//扩容到不到这个dp
                        break;//不可能连续是组合,一旦遭遇立即结束
                    }
//                    else
//                        dataSize+=12;//还凑合的类型加数据  //不加数据因为会在大循环中加
                }
                printf("ki的值为: %X",ki);
                getchar();

                VGinfo[useVGoft.size()-1].resize(ki);//键值也跟着扩容
                VGinfo[i][j]=0x70000000|(useVGoft.size()-1);//信息记录

            }
            else
            {
                VGinfo[i][j]=0x10000000;
            }
        }

    }//至此所有的键位数据尺寸都记录,并且记录了keyinfo以及增加VG数,所有样本,波形,范围数据都记录

    printf("再加上键值信息长度为: %X",dataSize);
    bitot=dataSize;
    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        dataSize+=32;//每一个波形数据都占用32字节
    }

    printf("再加上波形数据长度为: %X",dataSize);
    printf("波形数据长度为: %X",dataSize-bitot);
    bitot=dataSize;
    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        dataSize+=128;//每一个范围都占用128字节
    }

    printf("再加上范围数据长度为: %X",dataSize);
    printf("范围数据长度为: %X",dataSize-bitot);
    bitot=dataSize;
    getchar();

    samplehead shead;//数据结构获取样本头部数据
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//定位到每个样本数据的起始
        fread(&shead,16,1,g_ipf);//读取当前样本的头数据
        dataSize+=shead.datsiz+13-(shead.loopTag>>30);//每个样本的数据长度加上
        if(dataSize%4!=0)
            dataSize=dataSize+4-dataSize%4;//每个样本最终结束都与4对齐
    }
    //至此head数据,样本数据,波形数据,键位数据,范围数据,音轨数据全部都统计完了.
    printf("再加上样本数据长度为: %X",dataSize);
    printf("样本数据长度为: %X",dataSize-bitot);

    getchar();
    /*
    uint8_t sampleUse[useVoiceTol][128]={};//定义二维数组用来存放所有的voiceGroup中样本的使用情况
    uint8_t curTrackVoiceNum=0;//用来记录当前voiceGroup在vg组中的位置

    for(int i=0;i<cpyTol;i++)
    {
        curTrackVoiceNum=0;//当前歌曲的voice在组中的序号归零
        fseek(g_ipf,headoft[cpySong[i]]<<5>>5,0);//定位到每个歌曲的head地址

        fread(&bit16,2,1,g_ipf);//歌曲数量
        dataSize+=(bit16*4+8);//head数据长度
        fseek(g_ipf,2,1);//偏移到voicegroup的地址数据位置
        fread(&bit32,4,1,g_ipf);

        while(expVoiceoft[curTrackVoiceNum]!=bit32<<5>>5)
            curTrackVoiceNum++;//仅当voicegroup地址对应跳出循环

        uint32_t curTabTrackFrtOft=ftell(g_ipf);//记录首音轨的数据地址
        for(int j=0;j<bit16;j++)
        {
            fseek(g_ipf,curTabTrackFrtOft+j*4,0);//定位到每个音轨的数据地址
            fread(&bit32,4,1,g_ipf);

            fseek(g_ipf,bit32<<5>>5,0);//定位到track数据位置
            do
            {
                int c=fgetc(g_ipf);

                dataSize++;//每读取一个字节都+1
                if(c==0xb1) break;//歌曲信息结束标志

                else if(c==0xbd)
                {
                    int c=fgetc(g_ipf);
                    //对应的sample使用情况记录
                    sampleUse[curTrackVoiceNum][(unsigned)c]=1;
                    dataSize++;
                }
                else if(c==0xb2||c==0xb3)
                {
                    fseek(g_ipf,4,1);//跳过地址的四个字节
                    dataSize+=4;
                }

            }while(1);//(!feof());//提升速度的设置..

        }//至此当前的track的数据长度计算完毕

    }//至此全部的track的数据和head数据计算完毕

    vtda_t vtdsiz;//使用showsam头文件的数据结构

    for(int i=0;i<useVoiceTol;i++)
    {
        for(int j=0;j<128;j++)//总共128个键
        {
            if(samFlag==1&&sampleUse[i][j]==0)//如果是精确定位并且对应voice的样本不在歌曲的使用中
                continue;//进行下一个检查
            fseek(g_ipf,expVoiceoft[i]+j*12,0);//定位到合适的偏移voice序号地址

            fread(&vtdsiz,12,1,g_ipf);//如果有效则读取数据
            dataSize+=12;
            if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound类型
            {
                bit32=vtdsiz.tdat2<<5>>5;
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                        goto passWriteSampleoft;
                }
                useSampleoft.push_back(bit32);//保存样本数据地址
                passWriteSampleoft: continue;
            }
            else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample类型
            {
                dataSize+=128;//琴键伤害范围需要128字节
                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标
                if(vtdsiz.tdat2<<5>>5>bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//检查mulit是否在当前vg的范围内
                    continue;//在范围内则继续下一个检查,因为在范围内则样本地址不需要记录
                        //(当然极其罕见的情况不考虑了..只是有可能,但是基本不可能)
                for(uint32_t k=0;k<mulitSampleoft.size();k++)
                {
                    if(mulitSampleoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;//最终都是跳出当前循环然后再次continue;
                }
                mulitSampleoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复也不再当前vg范围内的ms
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//定位到琴键范围数据位置
                int newtol=0;//记录新地址的样本使用数目
                for(int k=0;k<128;k++)
                {
                    int c=fgetc(g_ipf);
                    if(c==0xFF) continue;
                    if(c>newtol) newtol=c;//最终得出有多少个样本
                }
                for(int k=0;k<=newtol;k++)
                {
                    fseek(g_ipf,(vtdsiz.tdat2<<5>>5)+k*12,0);//定位到新的voice地址
                    fread(&vtdsiz,12,1,g_ipf);//读取新数据
                    dataSize+=12;
                    if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound类型
                    {
                        bit32=vtdsiz.tdat2<<5>>5;
                        for(uint32_t k=0;k<useSampleoft.size();k++)
                        {
                            if(useSampleoft[k]==bit32)
                                goto passWriteSampleoft1;
                        }
                        useSampleoft.push_back(bit32);//保存样本数据地址
                        passWriteSampleoft1: continue;
                    }
                    else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy类型
                    {
                        for(uint32_t k=0;k<waveMemoryoft.size();k++)
                        {
                            if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                                goto passWriteSampleoft1;
                        }
                        waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复的波形数据地址
                        dataSize+=32;//波形数据长度
                    }

                    else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
                        break;//不可能连续是组合

                    else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample类型
                        break;//不可能连续是组合,一旦遭遇立即结束

                }
            }
            else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
            {
                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标
                if(vtdsiz.tdat2<<5>>5>bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//检查drum是否在当前vg的范围内
                    continue;//在范围内则继续下一个检查,因为在范围内则样本地址不需要记录
                        //(当然极其罕见的情况不考虑了..只是有可能,但是基本不可能)
                for(uint32_t k=0;k<drumPartoft.size();k++)
                {
                    if(drumPartoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;//最终都是跳出当前循环然后再次continue;
                }
                drumPartoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复也不再当前vg范围内的dp
                for(int k=0;k<128;k++)
                {
                    fseek(g_ipf,vtdsiz.tdat2<<5>>5,0);//定位到drumpart的地址
                    fread(&vtdsiz,12,1,g_ipf);
                    dataSize+=12;
                    if(((vtdsiz.tdat1<<24)|0x18)==0x18&&vtdsiz.tdat2>>25<<1==8)//directSound类型
                    {
                        bit32=vtdsiz.tdat2<<5>>5;
                        for(uint32_t k=0;k<useSampleoft.size();k++)
                        {
                            if(useSampleoft[k]==bit32)
                                goto passWriteSampleoft2;
                        }
                        useSampleoft.push_back(bit32);//保存样本数据地址
                        passWriteSampleoft2: continue;
                    }
                    else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy类型
                    {
                        for(uint32_t k=0;k<waveMemoryoft.size();k++)
                        {
                            if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                                goto passWriteSampleoft2;
                        }
                        waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复的波形数据地址
                        dataSize+=32;//波形数据长度
                    }

                    else if(vtdsiz.tdat1<<24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
                        break;//不可能连续是组合

                    else if(vtdsiz.tdat1<<24==0x40&&vtdsiz.tdat2>>25<<1==8
                    &&vtdsiz.tdat3>>25<<1==8)//mulitSample类型
                        break;//不可能连续是组合,一旦遭遇立即结束


                }
            }
            else if(((vtdsiz.tdat1>>24)|0x8)==0xB&&vtdsiz.tdat2>>25<<1==8)//wave Memroy类型
            {
                for(uint32_t k=0;k<waveMemoryoft.size();k++)
                {
                    if(waveMemoryoft[k]==vtdsiz.tdat2<<5>>5)
                        goto passWriteSampleoft;
                }
                waveMemoryoft.push_back(vtdsiz.tdat2<<5>>5);//保存非重复的波形数据地址
                dataSize+=32;//波形数据长度
            }
        }
    }//至此除了样本的数据没有计算外,其它数据全部计算完毕
    samplehead samh;
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//定位到每个样本数据的位置
        fread(&samh,16,1,g_ipf);//读取数据头
        if(samh.loopTag!=0&&samh.loopTag>>24!=0x40)
        {
            printf("样本数据错误!");
            getchar();
        }
        dataSize+=samh.datsiz+13-(samh.loopTag>>30);
    }
    */
    fclose(g_ipf);
    system("cls");
    return dataSize;
}
