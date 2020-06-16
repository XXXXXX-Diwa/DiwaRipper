#include "RomToRom.h"
#include "writeToRom.h"
#include "main.h"
#include "copyDatSize.h"
#include "showSam.h"
#include "getDat.h"
#include "showDat.h"
#include <vector>
/*!
写入过程
1.根据记录的数组写入样本,波形和范围数据,并顺序他们的新地址

2.对于二维信息为样本集和drum集并不在vg范围内的,优先以vg的方式写它们的vg.
它们的vg记录在正常vb组的后目前.只需要考虑样本,波形其他并在在相同的序号上记录vg的新地址

3.根据vg的地址配合二维信息,写入128个数据,样本波形 样本集 drum 范围新地址  并记录vg的新地址

以下为按歌曲号写入
4.根据歌曲的信息,对正跳转偏移,并记录每个音轨的首地址 一维数组

5.根据head信息,写新head 根据歌曲使用的vg序号写入新vg和音轨地址

6.修改table的地址为上面新head的地址 重复每个歌曲4 5 6操作
*/

uint32_t copytoRom(uint32_t writeOft,uint32_t writeSiz,int writeTag)
{
//    uint32_t bitot=writeSiz;//废弃
    uint8_t bit8=0;//临时用
    uint16_t bit16=0;//临时用
    uint32_t bit32=0;//临时用
    uint32_t movDat=0;//搬运数据用
    g_ipf=fopen(g_ipfName.c_str(),"rb");//打开主rom流允许读
    g_opf=fopen(g_subRom.c_str(),"rb+");//打开副rom流允许读写
    fseek(g_opf,writeOft,0);//副文件定位到写入坐标处

    samplehead samh;//数据结构读取样本的头数据
    for(uint32_t i=0;i<useSampleoft.size();i++)//先把样本写入
    {
        fseek(g_ipf,useSampleoft[i],0);//主rom定位到每个样本的数据处
        fread(&samh,16,1,g_ipf);//读取每个样本的头数据
        fseek(g_ipf,useSampleoft[i],0);//再次定位到主rom每个样本的数据处
        useSampleoft[i]=writeOft;//复制的地点替换为写入的地点

        bit32=samh.datsiz+0x13-(samh.loopTag>>30);//得到样本的总长度
//        if(bit32%4!=0)
//            bit32=bit32+4-bit32%4;//使数据与四对齐

        for(uint32_t j=0;j<bit32/4;j++)
        {
            fread(&movDat,4,1,g_ipf);//四个字节读取
            fwrite(&movDat,4,1,g_opf);//写入四个字节
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
        bit32=ftell(g_opf);//得到当前写入后的坐标
        bit32=ftell(g_opf);
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=bit32;//得到新的起点
        system("cls");
        printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//样本全部写入完毕,并顺序记录了每个写入处的地址
//    printf("写入的样本数据长度为: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    for(uint32_t i=0;i<useWaveMemoft.size();i++)
    {
        fseek(g_ipf,useWaveMemoft[i],0);//主rom定位到每个波形的数据处
        useWaveMemoft[i]=writeOft;//复制的地点替换为写入的地点
        for(uint32_t j=0;j<8;j++)
        {
            fread(&movDat,4,1,g_ipf);
            fwrite(&movDat,4,1,g_opf);
        }
        bit32=ftell(g_opf);//得到当前的坐标
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=bit32;//得到新的起点
        system("cls");
        printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//波形数据全部写入完毕,并顺序记录了每个写入处的地址
//    printf("写入的波形数据长度为: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    for(uint32_t i=0;i<useDamageoft.size();i++)
    {
        fseek(g_ipf,useDamageoft[i],0);//主rom定位到每个范围的数据处
        useDamageoft[i]=writeOft;//复制的地点替换为写入的地点
        for(uint32_t j=0;j<32;j++)
        {
            fread(&movDat,4,1,g_ipf);
            fwrite(&movDat,4,1,g_opf);
        }
        bit32=ftell(g_opf);//得到当前的坐标
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=bit32;//得到新的起点
        system("cls");
        printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }//范围数据全部写入完毕,并顺序的记录了每个写入处的地址
//    printf("写入的范围数据长度为: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    vtda_t vd;//数据结构用来读取琴键信息
    for(int i=(signed)useVGoft.size()-1;i>=0;i--)//倒过来循环以便先得到ms和dp的新VG地址
    {
        uint32_t j=0;
//        printf(" i当前的值为: %d ",i);
//        printf("useVGoft[%d]的值为: %X 长度为: %d ",i,useVGoft[i],useVGoft.size());
//        printf("VGinfo的长度为: %d ",VGinfo[i].size());
//        printf("keyEfc的长度为: %d ",keyEfc[i].size());
//        getchar();
        for(;j<VGinfo[i].size();j++)//键位不能倒...
        {
            if(writeTag==1&&keyEfc[i][j]==0)//无效的键位
            {
//                printf("经过无效");
                memset(&vd,0,12);//vd内存清零
                fwrite(&vd,12,1,g_opf);//写入无效的键位都是0,0,0
                continue;
            }
            fseek(g_ipf,(useVGoft[i]<<4>>4)+12*j,0);//定位到每个VG的键位的位置
//            printf("当前位置为: %X 键值为: %d",ftell(g_ipf),j);
            fread(&vd,12,1,g_ipf);//读取琴键信息
            if(VGinfo[i][j]>>28<2)//其它的类型不改变数据
            {
//                printf("经过其他");
            }
            else if(VGinfo[i][j]>>28==2)//常规样本
            {
//                printf("经过常规");
                vd.tdat2=0x8000000|useSampleoft[VGinfo[i][j]<<4>>4];//更改地址为写入的样本地址
            }
            else if(VGinfo[i][j]>>28==3)//波形
            {
//                printf("经过波形");
                vd.tdat2=0x8000000|useWaveMemoft[VGinfo[i][j]<<4>>4];//更改地址为写入的波形地址
            }
            else if(VGinfo[i][j]>>28==5)//multi VG范围外
            {
//                printf("经过ms");
                if(useVGoft[i]>>28==5)//当前multi的VG遇到了multi的键值
                {
//                    fprintf(stderr,"0x8000000|writeOft=%X",0x8000000|writeOft);
                    if(useVGoft[i]<<4>>4!=vd.tdat2<<5>>5)
                    {
                        fprintf(stderr,"MS类型VG: %X中的第%d个键值也是MS,但是这MS的地址不是该MS类型VG本身"
                                "请使用ASM的方式导出,因为在获取正常的VG的MS时,只检查了一级,没有对MS类型VG的MS检查"
                                "MS类型的VG默认设置是调用MS类型VG,除非是改版,正常游戏不会有这种情况!",useVGoft[i],
                                j);
                        fflush(stdin);
                        getchar();
                        exit(1);
                    }
                    vd.tdat2=0x8000000|writeOft;//就是当前VG的写入的地址
                    vd.tdat3=0x8000000|(useDamageoft[VGinfo[i][j]]<<16>>16);//得到范围地址
                }
                else
                {
//                    printf("经过这里!:");
//                    getchar();
//                    printf("0x8000000|(useVGoft[VGinfo[i][j]]<<4>>20)<<4>>4的值为: %X",0x8000000|(useVGoft[VGinfo[i][j]<<4>>20])<<4>>4);
                    vd.tdat2=0x8000000|(useVGoft[VGinfo[i][j]<<4>>20])<<4>>4;//得到样本集备份到VG组的地址
//                    printf("0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16])的值为: %X",0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16]));
                    vd.tdat3=0x8000000|(useDamageoft[VGinfo[i][j]<<16>>16]);//得到范围地址

                }
            }
            else if(VGinfo[i][j]>>28==7)//drum VG范围外
            {
//                printf("经过dp useVGoft[%d]=%X ",i,useVGoft[i]);
//                getchar();
                if(useVGoft[i]>>28==7)//当前drumpart的VG遇到了Drum的键值
                    vd.tdat2=0x8000000|writeOft;//就是当前VG的写入地址
                else
                    vd.tdat2=0x8000000|(useVGoft[VGinfo[i][j]<<4>>4])<<4>>4;//得到drum集备份在VG组的地址
            }
            fwrite(&vd,12,1,g_opf);//写入琴键信息
        }
        useVGoft[i]=0x8000000|writeOft;//当前复制处的VG地址改成写入处的VG地址
        bit32=ftell(g_opf);//得到当前的坐标
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=bit32;//得到新的起点
        system("cls");
        printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
//        printf("i= %d j= %d useVGoft.size()-1= %d",i,j,useVGoft.size()-1);
    }
//    printf("写入键值数据长度为: %X",bitot-writeSiz);
//    bitot=writeSiz;
//    getchar();

    std::vector<uint32_t>usetrackoft(0,0);//定义向量临时存储所有音轨的数据

    for(uint32_t i=0;i<cpySong.size();i++)//根据歌曲数循环
    {
        bit32=(tabOffset<<5>>5)+(cpySong[i]-1)*8;//得到每个复制歌曲的table地址 从1开始所以减1
//        printf("主rom的tab起始地址为: %X\n",tabOffset);
//        printf("要复制的歌曲的TABLE地址为: %X",bit32);
//        getchar();
        fseek(g_ipf,bit32,0);//偏移到歌曲的table
        fread(&bit32,4,1,g_ipf);//读取table的head
        movDat=bit32<<5>>5;//保存head的地址
        fseek(g_ipf,movDat,0);//定位到歌曲的head
        fread(&bit16,2,1,g_ipf);//读取歌曲数
        bit16=bit16<<24>>24;

        usetrackoft.resize(bit16,0);//扩容用来储存音轨地址
        fseek(g_ipf,6,1);//定位到音轨地址
        for(uint32_t j=0;j<bit16;j++)
        {
            fread(&bit32,4,1,g_ipf);
            usetrackoft[j]=bit32<<5>>5;//得到每个音轨的地址
        }
        for(uint32_t j=0;j<bit16;j++)
        {
            fseek(g_ipf,usetrackoft[j],0);//定位到每个音轨的数据处
            do
            {
                bit8=fgetc(g_ipf);//读取字节
                fputc(bit8,g_opf);//写入字节
                if(bit8==0xB1) break;
                else if(bit8==0xB2||bit8==0xB3)
                {
                    fread(&bit32,4,1,g_ipf);//读取地址
                    //跳转的地址减去音轨起始地址得到偏移差值
                    bit32=(bit32<<5>>5)-usetrackoft[j];
                    bit32=0x8000000|(bit32+writeOft);//算出新的坐标
                    fwrite(&bit32,4,1,g_opf);//写入新地址
                }
            }while(!feof(g_ipf));


            bit32=ftell(g_opf);
            usetrackoft[j]=0x8000000|writeOft;//音轨地址改成写入的地址
            writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
            writeOft=bit32;//得到新的起点
            system("cls");
            printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
        }//所有音轨都写入,并且音轨的坐标更新
        bit32=ftell(g_opf);//得到当前的坐标
        if(bit32%4!=0)
                for(uint32_t k=0;k<4-(bit32%4);k++)
                    fputc(0,g_opf);//补齐的字符
        bit32=ftell(g_opf);
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=ftell(g_opf);//head起点

        fseek(g_ipf,movDat,0);//定位到每个复制歌曲的head
        fread(&bit32,4,1,g_ipf);//歌曲数和未知数据读取



//        bit32=(tranum[cpySong[i]-1]<<16)+unknow[cpySong[i]-1];

        fwrite(&bit32,4,1,g_opf);//写入歌曲数和未知数据
        bit32=useVGoft[trackUseVG_No[i]];//写入新的vg地址
        fwrite(&bit32,4,1,g_opf);
        for(uint32_t j=0;j<bit16;j++)
        {
            bit32=usetrackoft[j];
            fwrite(&bit32,4,1,g_opf);//写入新的每个音轨
        }
        movDat=ftell(g_opf);//保存当前的写入地址
        bit32=(subTabOft<<5>>5)+(subUseSong[i]-1)*8;//得到写入rom的每个歌曲的table地址
//        printf("副rom的table起始地址为: %X",subTabOft);
//        printf("写入的table地址为: %X",bit32);
//        getchar();
        fseek(g_opf,bit32,0);//定位到写入rom每个歌曲的table处
        bit32=0x8000000|writeOft;//相当于新的head地址
//        printf("新head地址为: %X",bit32);
//        getchar();
        fwrite(&bit32,4,1,g_opf);//table写入新的head地址
        bit32=(tabOffset<<5>>5)+(cpySong[i]-1)*8;//得到每个复制歌曲的table地址 从1开始所以减1
        fseek(g_ipf,bit32+4,0);//定位到Songgroup数据

        fread(&bit32,4,1,g_ipf);//读取sg数据
//        printf("songGroup的数据为: %X",bit32);
//        getchar();
//        fwrite(&songGop[cpySong[i]-1],4,1,g_opf);//直接读取过去的sg备份更省事
        fwrite(&bit32,4,1,g_opf);//写入sg数据
        fseek(g_opf,movDat,0);//返回之前写入的地址

        bit32=movDat;//得到当前的坐标
        writeSiz-=bit32-writeOft;//得到写入后减少的尺寸
        writeOft=bit32;//得到新的起点
        system("cls");
        printf("当前写入地址为: %X\n\n还有%X字节需要写入\n\n终点为: %X\n",writeOft,writeSiz,writeOft+writeSiz);
    }
//    printf("写入的head和歌曲长度为: %X",bitot-writeSiz);
//    bitot=writeSiz;
    printf("\n数据已经写入完毕!\n");
    subUseSong.swap(backUpSubSong);
    fclose(g_ipf);//关闭文件流
    fclose(g_opf);
    return writeSiz;
}
