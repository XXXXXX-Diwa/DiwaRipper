#include "main.h"
#include "tabSearch.h"
#include "subTabSrh.h"
#include "getDat.h"

using namespace std;

uint32_t subTabOft=0;

int subSrhChoose()
{
    string tabset="";
    string offset="";
    int tras=1;//默认要搜索的音轨数
    printf(
           "当前载入的副ROM为: %s\n",getRomName(g_subRom).c_str());
    printf(
           "\n请选择:\n"
           "\n\tA.正常的Table地址搜索\n"
           "\n\tB.手动输入Table地址\n"
           "\n\tC.搜索合适音轨数的Table地址\n"
           "\n\tD.从ROM中间开始搜索\n"
           "\n\tE.深度搜索(适合非标准ROM)\n"
           "\n\t回车:记录的Table地址"
           );
    if(srhFst==0) printf("(无记录)\n");
    else printf("(0x%X)\n",srhFst);

    switch (toupper(getch()))
    {
        case 'A':
            printf("\n正在检索中..\n");
            if(!subSearchTab(-4,1))
            {
                system("cls");
                printf("未搜寻到Table地址!\n");
                return 0;
            }
            break;
        case 'B':
            printf("\n请输入Table的十六进制地址!(0x开头): \n");

            tabset.resize(10);
            scanf("%s",&tabset[0]);
            if(tabset[0]!='0'||tabset[1]!='x'||tabset[2]=='\0')
            {
                system("cls");
                printf("输入的Table地址格式错误!\n");
                return 0;
            }
            tabset=&tabset[2];//去掉0x
            sscanf(&tabset[0],"%X",&iptFst);
            if(!oldTabChk(iptFst))
            {
                system("cls");
                printf("错误的Table地址!\n");
                return 0;
            }

            subTabOft=iptFst;//转交
            break;
        case 'C':
            printf("\n请输入要搜寻的曲子的音轨数(1-10): \n");
            scanf("%d",&tras);
            if(tras<1||tras>10)
            {
                system("cls");
                printf("输入的校验音轨数目错误!\n");
                return 0;
            }
            if(!subSearchTab(-4,tras))
            {
                system("cls");
                printf("未搜寻到Table地址!\n");
                return 0;
            }
            break;
        case 'D':
            printf("\n请输入检索的起始十六进制坐标!(0x开头):\n");

            offset.resize(10);
            scanf("%s",&offset[0]);
            if(offset[0]!='0'||offset[1]!='x'||offset[2]=='\0')
            {
                system("cls");
                printf("输入的Table地址格式错误!\n");
                return 0;
            }
            offset=&offset[2];//去掉0x
            uint32_t temp;
            sscanf(&offset[0],"%X",&temp);
            temp=temp/4*4;
//            printf("%X\n",temp);
            fseek(g_ipf,temp,0);
            if(!subSearchTab(-4,tras))
            {
                system("cls");
                printf("未搜寻到Table地址!\n");
                return 0;
            }
            break;
        case 'E':
            printf("\n请稍等,可能需要较长时间....\n");
            if(!subSearchTab(-7,tras))
            {
                system("cls");
                printf("未检索到Table地址!\n");
                return 0;
            }
            break;
        case 13:
            if(srhFst==0)
            {
                system("cls");
                printf("没有当前ROM的Table地址记录!\n");
                return 0;
            }
            subTabOft=srhFst;
            return 1;
        default:
            system("cls");
            return 0;
    }
    return 1;
}
int subheadDataCmp()
{
    while(!feof(searchF))
    {
        for(int ten=0;ten<0x10;ten++)
        {
            if(headData[ten]==tableData[ten])
            {
                if(ten==9) return 1;
            }
        }
        while(!feof(searchF))
        {
            char c =fgetc(searchF);
            if(c=='\n') break;
        }
        fread(tableData,0x10,1,searchF);
    }
    return 0;
}

int subSearchTab(int Lv,int traNum)
{
    while(1)
    {
        fread(tableData,8,1,g_ipf);
        if(feof(g_ipf)) return 0;
        fseek(g_ipf,Lv,1);
        if(tableData[3]==8&&tableData[5]==0&&tableData[7]==0)
        {
            offset32=offsetGet(tableData);//head地址
            fseek(searchF,offset32,0);
            int c;
            fread(&c,4,1,searchF);
            if(feof(searchF)!=0) continue;
            c=c<<16>>16;
            if(c>0x10||c<traNum) continue;

            fread(headData,1,(c+1)*4,searchF);
            for(int h=3;h<(c+1)*4;h+=4)
            {
                if(headData[h]!=8) break;
                if(h==((c+1)*4)-1)
                {
                    int z=0;
                    for(int j=1;j<=c;j++)
                    {
                        fseek(searchF,offsetGet(headData+j*4),0);
                        z =fgetc(searchF);
//                        printf("%X\n",z);
                        if(z!=0xBC&&z!=0xBE) break;
                    }
                    if(z!=0xBC&&z!=0xBE) break;
                    fgetpos(g_ipf,&curOffset);
//                    printf("%X\n",curOffset);
                    subTabOft = curOffset-(8+Lv);
                    return 1;
                }
            }
        }
    }
}

int subTabGet(std::string ipfn)
{
    searchF=nullptr;
    offset32=0;
    srhFst=0;//文档记录的地址
    iptFst=0;//手动输入的地址
//uint8_t bit8=0;
    curOffset=0;
    tableData[16]={};
    headData[72]={};
    string oftPath=onlyPath(SWpath)+"search.oft";//使oft文件固定在软件目录
//    printf("%s\n",oftPath.c_str());
    g_ipf=fopen(ipfn.c_str(),"rb");//二进制方式打开文件流
    if(g_ipf==nullptr) exit(1);
    string exten =getExtension(ipfn);
    if(toupper(exten[0])!='G'||toupper(exten[1])!='B'||toupper(exten[2])!='A')
    {
        printf("ROM的扩展名不符!%s",getExtension(ipfn).c_str());
        fclose(g_ipf);
        while(clean=getchar()!='\n'&&clean!=EOF);
        getchar();
        exit(1);
    }

    if(access(oftPath.c_str(),F_OK)==-1)//检查文件是否存在
    {
        searchF=fopen(oftPath.c_str(),"w+");//创建文件
        fclose(searchF);
    }
    else
    {
        fseek(g_ipf,0xA0,0);//定位到头关键数据
        fread(headData,0x10,1,g_ipf);
        searchF=fopen(oftPath.c_str(),"rt+");//打开文本允许读写
        fread(tableData,0x10,1,searchF);
        if(subheadDataCmp())
        {
            fseek(searchF,1,1);//跳过一个空格
            memset(headData,0,0x10);//清空字符数组
            fread(headData,8,1,searchF);//读取文本中记录的table地址
            sscanf(headData,"%X",&srhFst);//转为整型
//            printf("该ROM过去Table记录为: %X\n",srhFst);
            if(!oldTabChk(srhFst)) srhFst=0;//不合格则清零
        }
        fclose(searchF);
    }
    searchF=fopen(ipfn.c_str(),"rb");//专门作为检查head的文件流
    fseek(g_ipf,0xB0,0);//排除头数据为table的可能性
    while(!subSrhChoose())
    {
        fseek(g_ipf,0xB0,0);//排除头数据为table的可能性
        while (_kbhit())//清空缓冲区
        {
            getch();
        }
    }
    if(subTabOft==0)
    {
        fclose(g_ipf);
        return 0;
    }
    system("cls");
    fclose(searchF);//;关闭文件流
    printf("\nTable地址为: %X\n",subTabOft);
    if(subTabOft!=srhFst)
    {
        printf("是否保存Table地址?--(Y\\N)\n");
        while (_kbhit())//清空缓冲区
        {
            getch();
        }
        char c=getch();
        if(toupper(c)=='Y')
        {
            memset(headData,0,0x20);
            memset(tableData,0,0x10);
            searchF=fopen(oftPath.c_str(),"rt+");
            fseek(g_ipf,0xA0,0);//定位到头关键数据
            fread(headData,0x10,1,g_ipf);//读取Rom中的数据
            fread(tableData,0x10,1,searchF);//读取oft文件中的数据
            if(subheadDataCmp())//如果有的话
            {
                fseek(searchF,1,1);//跳過1個字節
                string temp;
                temp.resize(8);
                sprintf(&temp[0],"%X",subTabOft);

                if(strlen(&temp[0])<7)
                {
                    int q=strlen(&temp[0]);
                    for(int z=0;z<7-q;z++)
                    {
                        temp='0'+temp;
                    }

                }
                fprintf(searchF,"%s\n",temp.c_str());//打印新的table数据
            }
            else
            {
                fseek(searchF,0,2);//定位到末尾
                string temp;
                temp.resize(8);
                sprintf(&temp[0],"%X",subTabOft);

                if(strlen(&temp[0])<7)
                {
                    int q=strlen(&temp[0]);
                    for(int z=0;z<7-q;z++)
                    {
                        temp='0'+temp;
                    }

                }
                for(int m=0;m<16;m++)
                fprintf(searchF,"%c",headData[m]);
                fprintf(searchF," %s\n",temp.c_str());
            }
            fclose(searchF);
        }
    }
    fclose(g_ipf);
    return 1;
}
