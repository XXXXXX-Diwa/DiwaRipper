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
附录:4和6这种在VG内的类型计算太繁琐故舍弃,5类型最低位为范围序号,16bit为VG序号.

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
uint32_t calculateCopySize(int samFlag,uint32_t cpyTol)
{

    uint32_t bitot=0;//临时用
    std::vector<uint32_t>backUpSubSong(subUseSong);//拷贝
    epyVector();//每次都清空要用的已经声明的向量
    system("cls");
    printf("看到这条信息,没错,程序未知原因卡了,你可以关闭了!");
    g_ipf=fopen(g_ipfName.c_str(),"rb");
    if(g_ipf==nullptr)
    {
        fprintf(stderr,"打开主rom文件失败!");
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
    for(uint32_t i=0;i<cpyTol;i++)
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
//    for(uint16_t i=0;i<cpySong.size();i++){
//        printf("%d ",cpySong[i]);
//    }
//    fflush(stdin);
//    getchar();
    //计算vg中琴键是否被使用,同时记录每个head+音轨的尺寸
    keyEfc.resize(useVGoft.size());//根据VG数调整数量
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        keyEfc[i].resize(128,0);//同时调整每个普通VG有128个琴键efc 并全部为0
    }

    for(uint32_t i=0;i<cpyTol;i++)
    {
        fseek(g_ipf,headoft[cpySong[i]-1]<<5>>5,0);//定位到每个歌曲的head地址
        fread(&bit16,2,1,g_ipf);//歌曲数量
        bit16=bit16<<24>>24;
        dataSize+=(bit16*4+8);//head数据长度记录 音轨数*4+8
        fseek(g_ipf,6,1);//定位到每个音轨的地址
        uint32_t fstTrack=ftell(g_ipf);//保存首音轨地址
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
//                    printf("键位标志经过");
//                    fflush(stdin);
//                    getchar();

                    int c=fgetc(g_ipf);
                    //根据歌曲序号得到歌曲使用vg序号再到所在的键位号的flag
                    keyEfc[trackUseVG_No[i]][(uint8_t)c]=1;
//                    printf("keyEfc[%d][%d]",trackUseVG_No[i],(uint8_t)c);
//                    getchar();
                    dataSize++;//每读取音轨数据的一个字节都+1
                }
                else if(c==0xb3||c==0xb2)//跳转和引用标志,后面是地址
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

    printf("head数据和音轨数据长度为: %X\n",dataSize);
//    getchar();
    vtda_t vtdsiz;//数据结构用来读取12个字节
    VGinfo.resize(useVGoft.size());//VG信息二维数组根据VG数量扩容
    for(uint32_t i=0;i<useVGoft.size();i++)
    {
        VGinfo[i].resize(128,0);//正规的VG都扩展到128个键,并归零
    }
    samplehead shead;//数据结构获取样本头部数据
    for(uint32_t i=0;i<useVGoft.size();i++)//循环vg数量次数
    {
        for(uint32_t j=0;j<VGinfo[i].size();j++)//非正规的未必128个键
        {
//            if(useVGoft[i]>>28==7&&keyEfc[i][j]==1)
//            {
//                printf("samFlag的值为: %d 当前为Dp: keyEfc[%d][%d]有效",samFlag,i,j);
//                getchar();
//            }
//            printf("keyEfc[%d][%d]=%d",i,j,keyEfc[i][j]);
//            getchar();
            if(samFlag==1&&keyEfc[i][j]==0)//如果是精确定位并且对应voice的样本不在歌曲的使用中
            {
                dataSize+=12;//就算无效的也要留空0,0,0
                continue;//进行下一个检查
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+j*12,0);//定位到vg组每个vg的每个琴键的位置

//            printf("定位的位置为: %X",ftell(g_ipf));
//            getchar();

            fread(&vtdsiz,12,1,g_ipf);//读取每个琴键的信息
            dataSize+=12;//每个琴键的信息也是要算入复制的尺寸

            if(((vtdsiz.tdat1<<24>>24)|0x38)==0x38&&vtdsiz.tdat2>>25<<1==8)//directSound类型
            {
//                printf("经过了这里<\n");
                bit32=vtdsiz.tdat2<<5>>5;//当前指向的样本地址
//                printf("ds当前指向的样本地址: %X\n",bit32);
                for(uint32_t k=0;k<useSampleoft.size();k++)
                {
                    if(useSampleoft[k]==bit32)
                    {
                        VGinfo[i][j]=0x20000000|k;//关键信息
//                        printf("VG[%d][%d]= %X",i,j,VGinfo[i][j]);
                        goto passWriteSampleoft;
                    }
                }
                fseek(g_ipf,bit32,0);//定位到样本数据头
                fread(&shead,16,1,g_ipf);//读取头数据
//                printf("%X",shead.loopTag>>1<<3);
//                getchar();
/**
*某hack不标准的样本数据,开头为01,才设置>>1去掉这个1
*/
                if(shead.loopTag>>1<<3!=0||shead.datsiz>0xA00000)
                {
                    keyEfc[i][j]=0;//样本不符合则直接改为无效并下一样本检查
                    printf("有不符合标准的样本地址在: %X",(uint32_t)ftell(g_ipf)-16);
                    fflush(stdin);
                    getchar();
                    continue;
                }
                useSampleoft.push_back(bit32);//保存样本数据地址
//                printf("当前样本的数据地址为: %X VG为: %X",bit32,useVGoft[i]);
//                getchar();
                //第i个vg的第j个键位标记详细信息
                VGinfo[i][j]=0x20000000|(useSampleoft.size()-1);//关键信息写入
//                printf("VG[%d][%d]= %X",i,j,VGinfo[i][j]);
                passWriteSampleoft: continue;
//                if(useVGoft[i]>>28==5)
//                {
//                    printf("当前为ms=%X有效键位为%d",useVGoft[i],j);
//                    getchar();
//                }
//                if(useVGoft[i]>>28==7)
//                {
//                    printf("当前为dp=%X有效键位为%d",useVGoft[i],j);
//                    getchar();
//                }
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
//                printf("第%d个VGoft的第%d个键值为ms: \n",i,j);

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

//                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标
//
//                if(vtdsiz.tdat2<<5>>5>=bit32&&vtdsiz.tdat2<<5>>5<bit32+12*128)//检查mulit的地址是否在当前vg的范围内
//                {   //地址在VG中的序号记录在第16-23bit 坐标-VG起始坐标除以12然后左移16位
//                    VGinfo[i][j]=0x40000000|(((vtdsiz.tdat2<<5>>5)-bit32)/12<<16)|VGinfo[i][j];//在VG范围内则记录关键信息
//                    continue;//种类,与VG的偏移,范围全都记录了,可以进行下一个键值了
//                }
//              不管在不在范围都选择另写multi的VG
                for(uint32_t k=0;k<useVGoft.size();k++)//指向的地址不在当前VG地址中的情况(原)一律视为不在VG地址中
                {
//                    printf("当前为: %X useVGoft[k]= %X vtdsiz.tdat2= %X",useVGoft[i],useVGoft[k],vtdsiz.tdat2);
//                    getchar();
                    if(useVGoft[k]==(0x50000000|(vtdsiz.tdat2<<5>>5)))//只要当前的multi新VG是过去记录的,则记录在内则结束
                    {
                        //种类,在VG组中的序号,范围全都记录,可以进行下一个键值了

                        VGinfo[i][j]=0x50000000|(k<<16)|VGinfo[i][j];
                        goto passWriteuseVGoft;//不重复的跳过写入新的;
                    }

                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x50000000);//非重复的加入进VG地址中,进行循环检查
//                printf("vtdsiz.tdat2== %X",vtdsiz.tdat2);
//                getchar();
                VGinfo.resize(useVGoft.size());//VGinfo也跟着扩容
                keyEfc.resize(useVGoft.size());//KeyEfc也跟着扩容

                passWriteuseVGoft:
//                    printf("经过这里!:");
//                        getchar();
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//定位到琴键范围数据位置
                uint32_t newtol=0;//记录新地址的样本使用数目
                for(int k=0;k<128;k++)
                {
                    uint32_t c=fgetc(g_ipf);
                    if(c>0x7F) continue;//如果键值大于127则不考虑

                    if(c>newtol) newtol=c;//最终得出最大用到多少个样本
                }
                uint32_t curNum=0;//用来记录当前的vtdsiz.tdat2在useVGoft中的位置
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
                if(VGinfo[curNum].size()-temp<newtol)//如果当前尺寸小于新的尺寸则扩容
                {
                    VGinfo[curNum].resize(newtol+1,0);//给予增加的VG定义新的键值数
                    keyEfc[curNum].resize(newtol+1,0);//同上并把扩容的部分归零
                }

//                printf("VGinfo[%d].size()= %d\n",useVGoft.size()-1,VGinfo[useVGoft.size()-1].size());
//                getchar();
                fseek(g_ipf,vtdsiz.tdat3<<5>>5,0);//再次定位到琴键范围数据位置

//                printf("curNum当前的值为: %d\n",curNum);
//                getchar();
                for(uint32_t k=0;k<128;k++)
                {
                    uint32_t c=fgetc(g_ipf);
                    if(c>0x7F) continue;
//                    printf("当前位置为: %X vtdsiz.tdat2=%X的第%d个键位是有效的\n",ftell(g_ipf),vtdsiz.tdat2,c);
//                    printf("keyEfc[%d].size()= %d VG= %X",curNum,keyEfc[curNum].size(),useVGoft[i]);
//                    getchar();
                    keyEfc[curNum][c]=1;//保证了在范围数据中有效性的为1

                }
                //种类,在VG组中的序号,范围全部都记录,可以进行下一个键值了
                VGinfo[i][j]=0x50000000|curNum<<16|VGinfo[i][j];

            }
            else if(vtdsiz.tdat1<<24>>24==0x80&&vtdsiz.tdat2>>25<<1==8)//drum part类型
            {
//                printf("第%d个VGoft的第%d个键值为dp: ",i,j);
//                bit32=ftell(g_ipf)-12*(j+1);//得到voiceGroup开始的坐标
                for(uint32_t k=0;k<useVGoft.size();k++)
                {
                    if(useVGoft[k]==(0x70000000|(vtdsiz.tdat2<<5>>5)))
                    {
                        VGinfo[i][j]=0x70000000|k;//类型与在VG中的序号信息记录
                        goto passWriteSampleoft;//最终都是跳出当前循环然后再次continue;
                    }
                }
                useVGoft.push_back((vtdsiz.tdat2<<5>>5)|0x70000000);//保存非重复dp
                VGinfo.resize(useVGoft.size());//VGinfo也跟着扩容
                keyEfc.resize(useVGoft.size());//同上
                VGinfo[useVGoft.size()-1].resize(128,0);
                keyEfc[useVGoft.size()-1].resize(128,0);//先临时默认有128个键位
/*!BC KEYSH
BB TEMPO
BD VOICE 键值
BE VOL 音量
BF pan 最小为0x40
C0 BEND 最小为0x40
N07 D6  估计从d0开始
琴键
v音量   可无但是必须在琴键后 应该不超过0x7f
w +128 时间??

b4 peng

b3 patt

b2 goto

b1 fine*/

/**
*0x80-0xB0之间的字节是delta-time值，0xB1-0xFF之间的字节是命令
*字节0x00-0x7F是命令的参数。“负”参数0x80-0xff是可能的，但通常是“禁止的”。
*/

/**
*drum part使用的键值在track数据中有记录,D0-FF是note标记,后面跟着的就是键值;CE是note关,CF是开,CF也包括在内
*同时CF note开启标记(后面跟着就是键值) CE note关闭标记(关闭开启的键值 目前在音乐的hack一整首歌曲的时候用到.
*/
                for(uint32_t k=0;k<(unsigned)cpyTol;k++)//歌曲数次数循环
                {
                    for(uint32_t l=0;l<tranum[cpySong[k]-1];l++)//音轨次数循环
                    {
                        fseek(g_ipf,trackoft[cpySong[k]-1][l],0);//跳到音轨数据处
                        do
                        {
                           uint32_t w=fgetc(g_ipf);
//                           printf("当前读取地址为: %X w= %X",ftell(g_ipf)-1,w);
//                           getchar();
                           if(w==0xBD)//键值设置标志
                           {
                                w=fgetc(g_ipf);//读取设置的键值
                                //鼓点基本都是固定为一个轨道
                                if(w!=j) break;//如果当前音轨用的键值不是该VG中该drum part键值 检查下个音轨
                                //fseek(g_ipf,6,1);//跳过6个字节 BD后六个音轨基本不会遇到dp轨道设置 但不允许例外

                                do//相等的话检查有效的键值
                                {
                                    w=fgetc(g_ipf);//继续读取

                                    if(w>=0xCF)//(CF)D0-FF设置dp键值标记
                                    {
                                        do
                                        {
                                            w=fgetc(g_ipf);//读取键值
                                            if(w<0x80)//不能大于127 小于127则是键值 虽然这个判断很多余,但是怕例外
                                            {
                                                keyEfc[useVGoft.size()-1][w]=1;//此鼓点有效
//                                                printf("keyEfc[%d][%d]有效 当前读取地址为: %X w= %X",useVGoft.size()-1,w,ftell(g_ipf)-1,w);
//                                                getchar();
                                                w=fgetc(g_ipf);//读取可能是音量
                                                if(w<0x80)//小于128则是音量
                                                {
                                                    fseek(g_ipf,1,1);//上面是音量这里则是time 否则上面就是time
                                                }
                                            }
                                            else if(w>=0xCF) continue;//第二次循环才会有的判定 dp键指令继续
                                            else if(w>0xB0) break;//第二次循环才会有的判定 非dp键指令的话就跳出
                                            //else if(w<0x80) fseek(g_ipf,-1,1);//都不是的话,有可能是延续第一次标识的dp键
                                            //else //只可能是time了...
                                        }while(1);

                                    }
                                    if(w==0xB2||w==0xB3)//跳转标志
                                        fseek(g_ipf,4,1);//跳过地址
                                    if(w==0xB1) break;

                                }while(1);
                           }
                           if(w==0xB1) break;
                           if(w==0xB2||w==0xB3)
                                fseek(g_ipf,4,1);//跳过四个字节
                        }while(1);
                    }
                }//这样的话,所有歌曲的所有音轨的drumpart相关都检查过了
                for(int k=127;k>=0;k--)//检查dp使用的最大键值位
                {
                    if(keyEfc[useVGoft.size()-1][k]==1)//从后向前检查最后一个有效的键值
                    {
                        keyEfc[useVGoft.size()-1].resize(k+1);
                        VGinfo[useVGoft.size()-1].resize(k+1);//重新设置两者的长度
//                        printf("重新设置的长度为: %d",k+1);
//                        getchar();
                        break;//首次次有效就跳出
                    }
                }
                VGinfo[i][j]=0x70000000|(useVGoft.size()-1);//信息记录使用的VGoft序号

            }
            else
            {
                VGinfo[i][j]=0x10000000;
            }
        }

    }//至此所有的键位数据尺寸都记录,并且记录了keyinfo以及增加VG数,所有样本,波形,范围数据都记录

    printf("再加上键值信息长度为: %X\n",dataSize);
    bitot=dataSize;
//    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        dataSize+=32;//每一个波形数据都占用32字节
    }

    printf("波形数据长度为: %X\n",dataSize-bitot);
    bitot=dataSize;
//    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        dataSize+=128;//每一个范围都占用128字节
    }
    printf("范围数据长度为: %X\n",dataSize-bitot);
    bitot=dataSize;
//    getchar();
    for(uint32_t i=0;i<useSampleoft.size();i++)
    {
        fseek(g_ipf,useSampleoft[i],0);//定位到每个样本数据的起始
        fread(&shead,16,1,g_ipf);//读取当前样本的头数据
        dataSize+=shead.datsiz+0x13-(shead.loopTag>>30);//每个样本的数据长度加上
        // printf("样本%d的地址为: %X 尺寸为: %X",i,useSampleoft[i],shead.datsiz);
        if(dataSize%4!=0)
            dataSize=dataSize+4-dataSize%4;//每个样本最终结束都与4对齐
    }
    //至此head数据,样本数据,波形数据,键位数据,范围数据,音轨数据全部都统计完了.
    printf("样本数据长度为: %X\n",dataSize-bitot);
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
