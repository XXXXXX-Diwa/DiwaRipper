#include "main.h"
#include "tabSearch.h"
#include "getDat.h"
#include "subTrackInfoGet.h"
#include "subTabSrh.h"
#include "showSam.h"
#include "copyDatSize.h"
#include "RomToRom.h"
#include <cstring>
#include <algorithm>
/**
*导出文件夹分为主文件夹(在rom目录下)
*主文件夹命名为rom A0-AF数据,该文件夹内所有文件均以此名起始+'_'
*主文件夹内有track文件夹,voice_group文件夹,以及main asm文件.
*track文件夹保存了每个歌曲的数据文件.
*voice_group文件夹保存了VG,MS,DP,SWD文件和samples,wave_memorys,damages文件夹
*/


int romToArmips(){
    /**
    *设置歌曲序号部分
    */
    std::vector<uint32_t>().swap(cpySong);
    system("cls");
    printf("连贯的歌曲序号导出?-Y/N\n");

    uint32_t song_tatol;
    if(toupper(getch())=='Y'){
        uint32_t start_num,end_num;
        bool invalid=false;
        damageSet(start_num,end_num,true);
        for(;start_num<=end_num;start_num++){
            if(tablefc[start_num-1]){
                cpySong.push_back(start_num);
            }else{
                printf("\n其中的歌曲%d无效!",start_num);
                invalid=true;
            }
        }
        if(invalid){
            fflush(stdin);
            getchar();
        }
        if(!cpySong.size()){
            fputs("\n这个范围内没有一个有效的歌曲!\n",stderr);
            getchar();
            return 0;
        }
        fflush(stdin);
        puts("是否剔除其中的某些歌曲?-Y/N\n");
        if(toupper(getch())=='Y'){
                uint32_t te,si;
                printf("请输入要剔除的歌曲号%d-%d(0结束): ",cpySong.front(),cpySong.back());
                si=cpySong.size();
            for(uint16_t k=0;k<si;k++){
                fflush(stdin);
                if(scanf("%u",&te)!=1){
                    fputs("输入错误!\n",stderr);
                }else if(!te){
                    break;
                }else if(te>cpySong.back()){
                    fputs("序号超过了范围!\n",stderr);
                }else if(te<cpySong.front()){
                    fputs("序号小于范围!\n",stderr);
                }else{
                    std::vector<uint32_t>::iterator it=std::find(cpySong.begin(),cpySong.end(),te);
                    cpySong.erase(it);//删除这个数
                }
            }
            if(cpySong.empty()){
                fflush(stdin);
                fputs("删除的渣都不剩了!",stdout);
                getchar();
                return 0;
            }
        }
    }else{
        while(true){
            fflush(stdin);
            printf("请输入要导出的歌曲数量: ");
            if(scanf("%u",&song_tatol)!=1){
                fputs("输入错误!\n",stderr);
            }else if(song_tatol>efctabtal){
                fputs("数目超过了有效歌曲的数量!\n",stderr);
            }else if(song_tatol<1){
                fputs("数目不能小于1!\n",stderr);
            }else break;
        }
        uint32_t te_num=1;
        while(te_num<=song_tatol){
            fflush(stdin);
//            system("cls");
            printf("请输入第%d个要导出的歌曲序号(7777提前结束): ",te_num);
            uint32_t song_num=0;
            if(scanf("%d",&song_num)!=1){
                fputs("输入错误!\n",stderr);
            }else if(song_num==7777){
                if(!cpySong.size()){
                    fputs("还没有一个有效的歌曲号呢!",stderr);
                }else break;
            }
            else if(song_num>1999){
                fputs("数字太大!\n",stderr);
            }else if(!tablefc[song_num]){
                fputs("序号对应的歌曲无效!\n",stderr);
            }else{
                for(uint16_t j=0;j<cpySong.size();j++){
                    if(song_num==cpySong[j]){
                        fputs("这个序号之前已经输入过了!\n",stderr);
                        goto next_to;
                    }
                }
                cpySong.push_back(song_num);
                te_num++;
                next_to:continue;
            }
        }
    }
//    /**
//    *设置导出参数部分
//    */
//    bool vol_flag=false;
//    fflush(stdin);
//    printf("是否对导出的歌曲音量进行调整?-Y/N");
//    if(toupper(getch())=='Y') vol_flag=true;
    fflush(stdin);
    calculateCopySize(1,cpySong.size());   //借助这个计算函数得出想要的一些数据
    /**
    *导出样本 波形 范围数据文件
    */
    std::string export_file_name;
//    export_file_name.resize(16);
    if((g_ipf=fopen(g_ipfName.c_str(),"rb"))==nullptr){//打开主rom允许读
        fprintf(stderr,"打开文件: %s失败!",g_ipfName.c_str());
        getchar();
        exit(1);
    }
    fseek(g_ipf,0xA0,0);    //定位头label
    char rom_head_str[16];
    fread(rom_head_str,16,1,g_ipf);
    bool bl=true;
    for(int k=0;k<16;k++){
        if(rom_head_str[k]=='\0'||rom_head_str[k]==' '){
            if(bl){
                export_file_name.append("_");
                bl=false;
            }
        }else{
            bl=true;
            export_file_name.append(1,rom_head_str[k]);
        }
    }
    std::string export_folder_path=onlyPath(g_ipfName)+export_file_name;
    mkdir(export_folder_path.c_str());//在rom处建立导出文件夹

    std::string current_oft_str;
    std::string current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    mkdir(current_path.c_str());//在导出文件夹内创建VG文件夹
    char c2str[8];
    /**
    *导出样本数据文件部分
    */
    if(useSampleoft.size()){
        //创建样本文件夹
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_samples";
        mkdir(current_path.c_str());
        char* buffer=new char[40000];
        if(buffer==NULL){
            fputs("指针buffer申请堆空间失败!",stderr);
            getchar();
            exit(1);
        }
        std::vector<uint32_t>off_set(useSampleoft);//复制
        sort(off_set.begin(),off_set.end());//排序
        off_set.erase(unique(off_set.begin(),off_set.end()),off_set.end());//去除重复
        for(uint16_t l=0;l<off_set.size();l++){//导出每个样本文件
            sprintf(c2str,"%X",off_set[l]);
            current_oft_str=c2str;
            //创建每个样本数据文件
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".sample").c_str(),"wb"))==nullptr){
                fprintf(stderr,"创建文件: %s失败!",(export_file_name+"_0x"+current_oft_str+".sample").c_str());
                getchar();
                exit(1);
            }
            fseek(g_ipf,off_set[l],0);//定位到每个样本数据的位置
            samplehead sample_head;
            fread(&sample_head,16,1,g_ipf);
            fwrite(&sample_head,16,1,g_opf);//写入头数据
            int siz=sample_head.datsiz+(sample_head.loopTag>>30);
            if(siz%4!=0){
                siz=siz+4-siz%4;
            }
            if(siz>40000){
                char* buff=new char[siz];
                if(buff==NULL){
                fputs("指针buff申请堆空间失败!",stderr);
                getchar();
                exit(1);
            }
                fread(buff,siz,1,g_ipf);//样本数据全部读取
                fwrite(buff,siz,1,g_opf);//写入样本数据文件
                delete[] buff;
                buff=NULL;
            }else{
                fread(buffer,siz,1,g_ipf);
                fwrite(buffer,siz,1,g_opf);
            }
            fclose(g_opf);//关闭样本数据文件流
        }
        delete[] buffer;
        buffer=NULL;
    }
    /**
    *导出波形数据文件部分
    */
    if(useWaveMemoft.size()){
        //创建波形文件夹
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_wave_memorys";
        mkdir(current_path.c_str());
        char buff[16];
        for(uint16_t l=0;l<useWaveMemoft.size();l++){
            sprintf(c2str,"%X",useWaveMemoft[l]);
            current_oft_str=c2str;
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".wave_memory").c_str(),"wb"))==nullptr){
                fprintf(stderr,"创建文件: \"%s\"失败",(export_file_name+"_0x"+current_oft_str+".wave_memory").c_str());
                getchar();
                exit(1);
            }
            fseek(g_ipf,useWaveMemoft[l],0);
            fread(buff,16,1,g_ipf);
            fwrite(buff,16,1,g_opf);
            fclose(g_opf);
        }
    }
    /**
    *导出范围数据文件部分
    */
    if(useDamageoft.size()){
        //创建范围文件夹
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_damages";
        mkdir(current_path.c_str());
        char buff[128];
        for(uint16_t l=0;l<useDamageoft.size();l++){
            sprintf(c2str,"%X",useDamageoft[l]);
            current_oft_str=c2str;
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".damage").c_str(),"wb"))==nullptr){
                fprintf(stderr,"创建文件: \"%s\"失败!",(export_file_name+"_0x"+current_oft_str+".damage").c_str());
                getchar();
                exit(1);
            }
            fseek(g_ipf,useDamageoft[l],0);
            fread(buff,128,1,g_ipf);
            fwrite(buff,128,1,g_opf);
            fclose(g_opf);
        }
    }
    /**
    *绘制-样本-波形-范围-数据 SWD.asm导入文件
    */
    current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    if(useSampleoft.size()||useWaveMemoft.size()||useDamageoft.size()){
        if((g_opf=fopen((current_path+"\\"+export_file_name+"_SWD.asm").c_str(),"w"))==nullptr){
            fprintf(stderr,"创建文件: \"%s\"失败!",(export_file_name+"_SWD.asm").c_str());
            getchar();
            exit(1);
        }
        /**写入样本部分*/
        if(useSampleoft.size()){
            bool un_ique=true;
            std::vector<uint32_t>off_set(0,0);
//            sort(off_set.begin(),off_set.end());//排序
//            off_set.erase(unique(off_set.begin(),off_set.end()),off_set.end());//去除重复
            fputs(";sample部分-------------------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X 包含以下:++++++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
                }else{
                    fprintf(g_opf,"\n.align\t\t;Multi Sample 0x%X\n\n",useVGoft[i]&0xFFFFFFF);
                }
                for(uint16_t j=0;j<VGinfo[i].size();j++){
                    if((VGinfo[i][j]&0xF0000000)==0x20000000&&keyEfc[i][j]){
                        un_ique=true;
                        uint32_t temp=useSampleoft[VGinfo[i][j]&0xFFFF];
                        uint16_t tesize=off_set.size();
                        uint16_t k=0;
                        if(tesize==0){
                            off_set.push_back(temp);
                        }else{
                            for(;k<tesize;k++){
                                if(off_set[k]==temp){
                                    break;
                                }
                            }
                            if(k==tesize){
                                off_set.push_back(temp);
                            }else{
                                un_ique=false;
                            }
                        }


                        if(un_ique){
                            fprintf(g_opf,"%s_sample_0x%X:\n\t.import \"%s_voice_group\\%s_samples\\%s_0x%X.sample\"\n",
                                export_file_name.c_str(),temp,export_file_name.c_str(),
                                export_file_name.c_str(),export_file_name.c_str(),temp);
                        }
                    }
                }
            }
            fputs("\n\n",g_opf);
        }
        /**写入波形部分*/
        if(useWaveMemoft.size()){
            std::vector<uint32_t>off_set(0,0);
            bool un_ique=true;
            fputs(";wave memory部分-----------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X 包含以下:++++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
                }else{
                    fprintf(g_opf,"\n.align\t\t;Multi Sample 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }
                for(uint16_t j=0;j<VGinfo[i].size();j++){
                    if((VGinfo[i][j]&0xF0000000)==0x30000000&&keyEfc[i][j]){
                        un_ique=true;
                        uint32_t temp=useWaveMemoft[VGinfo[i][j]&0xFFFF];
                        uint16_t tesize=off_set.size();
                        uint16_t k=0;
                        if(tesize==0){
                            off_set.push_back(temp);
                        }else{
                            for(;k<tesize;k++){
                                if(off_set[k]==temp){
                                    break;
                                }
                            }
                            if(k==tesize){
                                off_set.push_back(temp);
                            }else{
                                un_ique=false;
                            }
                        }
                        if(un_ique){
                            fprintf(g_opf,"%s_wave_memory_0x%X:\n\t.import \"%s_voice_group\\%s_wave_memorys\\%s_0x%X.wave_memory\"\n",
                                export_file_name.c_str(),temp,export_file_name.c_str(),
                                export_file_name.c_str(),export_file_name.c_str(),temp);
                        }
                    }
                }
            }
            fputs("\n\n",g_opf);

        }
        /**写入范围部分*/
        if(useDamageoft.size()){
            std::vector<uint32_t>off_set(0,0);
            bool un_ique=true;
            fputs(";damage部分---------------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X 包含以下:+++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
                }else{
                    fprintf(g_opf,"\n.align\t\t;Multi Sample 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }
                for(uint16_t j=0;j<VGinfo[i].size();j++){
                    if((VGinfo[i][j]&0xF0000000)==0x50000000&&keyEfc[i][j]){
                        un_ique=true;
                        uint32_t temp=useDamageoft[VGinfo[i][j]&0xFFFF];
                        uint16_t tesize=off_set.size();
                        uint16_t k=0;
                        if(tesize==0){
                            off_set.push_back(temp);
                        }else{
                            for(;k<tesize;k++){
                                if(off_set[k]==temp){
                                    break;
                                }
                            }
                            if(k==tesize){
                                off_set.push_back(temp);
                            }else{
                                un_ique=false;
                            }
                        }
                        if(un_ique){
                            fprintf(g_opf,"%s_damage_0x%X:\n\t.import \"%s_voice_group\\%s_damages\\%s_0x%X.damage\"\n",
                                export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],export_file_name.c_str(),
                                export_file_name.c_str(),export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF]);
                        }
                    }
                }
            }
        }
        fclose(g_opf);
    }
    /**
    *绘制Voice_Group asm文件
    */
    std::string temp_name;
    current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    std::vector<std::string>include_str(0);
    for(uint16_t i=0;i<VGinfo.size();i++){//VG地址数次循环
        vtda_t instr;
        uint8_t bit_8;
        if(!(bit_8=useVGoft[i]>>28)){//普通的VG
            sprintf(c2str,"%X",useVGoft[i]);
            current_oft_str=c2str;
            temp_name=export_file_name+"_VG_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"创建文件: \"%s\"失败!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_VG_0x%X:\n",export_file_name.c_str(),useVGoft[i]);
            fseek(g_ipf,useVGoft[i],0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//其它类型
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            (useVGoft[(VGinfo[i][j]>>16)&0xFFF])<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_DP_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[VGinfo[i][j]&0xFFFF]<<4>>4,instr.tdat3,j);
                    }
                }else{
                    fputs("\t.dw 0,0,0\n",g_opf);
                }
            }
            fclose(g_opf);
        }else if(bit_8==5){//mutil sample类型VG
            sprintf(c2str,"%X",useVGoft[i]&0xFFFFFFF);
            current_oft_str=c2str;
            temp_name=export_file_name+"_MS_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"创建文件: \"%s\"失败!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_MS_0x%s:\n",export_file_name.c_str(),current_oft_str.c_str());
            fseek(g_ipf,useVGoft[i]<<5>>5,0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//其它类型
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[(VGinfo[i][j]>>16)&0xFFF]<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_DP_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[VGinfo[i][j]&0xFFFF]<<4>>4,instr.tdat3,j);
                    }
                }else{
                    fputs("\t.dw 0,0,0\n",g_opf);
                }
            }
            fclose(g_opf);
        }else if(bit_8==7){//drum part类型VG
            sprintf(c2str,"%X",useVGoft[i]&0xFFFFFFF);
            current_oft_str=c2str;
            temp_name=export_file_name+"_DP_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"创建文件: \"%s\"失败!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_DP_0x%s:\n",export_file_name.c_str(),current_oft_str.c_str());
            fseek(g_ipf,useVGoft[i]<<5>>5,0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//其它类型
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[(VGinfo[i][j]>>16)&0xFFF]<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part类型
                        fprintf(g_opf,"\t.dw 0x%08X,%s_DP_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[VGinfo[i][j]&0xFFFF]<<4>>4,instr.tdat3,j);
                    }
                }else{
                    fputs("\t.dw 0,0,0\n",g_opf);
                }
            }
            fclose(g_opf);
        }
    }
    /**
    *绘制VG编译文件
    */
    temp_name=export_file_name+"_voice_include_main.asm";
    if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
        fprintf(stderr,"创建文件: \"%s\" 失败!",temp_name.c_str());
        getchar();
        exit(1);
    }
    fprintf(g_opf,".include \"%s_voice_group\\%s_SWD.asm\"\n",
            export_file_name.c_str(),export_file_name.c_str());
    for(int i=(int)include_str.size()-1;i>=0;i--){
        fprintf(g_opf,".include \"%s_voice_group\\%s\"\n",
                export_file_name.c_str(),include_str[i].c_str());
    }
    fclose(g_opf);
    /**
    *Song数据
    */
    include_str.clear();//清空
    current_path=export_folder_path+"\\"+export_file_name+"_Song";
    mkdir(current_path.c_str());//创建Song文件夹
    for(uint16_t i=0;i<cpySong.size();i++){//歌曲数量 次循环
        sprintf(c2str,"%d",cpySong[i]);//歌曲号字符串化
        current_oft_str=c2str;
        temp_name=export_file_name+"_Song_"+current_oft_str+".asm";
        if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
            fprintf(stderr,"创建文件: \"%s\" 失败!",temp_name.c_str());
            getchar();
            exit(1);
        }
        include_str.push_back(temp_name.c_str());//保存Song文件名
        /**打印每个音轨的数据*/
        for(uint16_t j=0;j<tranum[cpySong[i]-1];j++){//音轨数量 次循环
            uint32_t track_begin=trackoft[cpySong[i]-1][j];//每个track起始地址
            fseek(g_ipf,track_begin,0);//定位到当前歌曲的每个track位置

            std::vector<uint32_t>jump(0,0);//用来记录每个跳转的地址
            std::vector<uint32_t>b2b3(0,0);//用来记录b2b3跳跃处
            int c;
            uint32_t jump_oft;
            uint32_t track_end;//每个track的结束地址
            uint32_t techk;//用于检查
            do{
                c=fgetc(g_ipf);
                if(c>0xB0&&c<0xB4){
                    if(c==0xB1){
                        track_end=ftell(g_ipf);
                        break;
                    }else if(c==0xB3||c==0xB2){
                        techk=ftell(g_ipf);//记录地址
                        fread(&jump_oft,4,1,g_ipf);//读取地址
                        jump_oft^=0x8000000;
                        //某hack的问题,跳转地址竟然跳出了当前音轨
                        if(jump_oft>>24!=0||track_begin>jump_oft||techk<jump_oft){
                            fprintf(stderr,"地址: %X处的跳转地址为: %X是不合格的跳转地址!",techk,jump_oft^0x8000000);
                            getchar();
                            exit(1);
                        }
                        jump.push_back(jump_oft);//记录地址
                    }
                }
            }while(!feof(g_ipf));
            fseek(g_ipf,track_begin,0);//重定位在当前track首处

            sort(jump.begin(),jump.end());//排序
            jump.erase(unique(jump.begin(),jump.end()),jump.end());//去除重复的地址
            std::vector<uint32_t>label_oft(0,0);//label偏移
            if(jump.size()!=0&&jump[0]!=track_begin){
                jump.insert(jump.begin(),track_begin);
            }
            jump.push_back(track_end);
            if(jump.size()==1){
                label_oft.push_back(track_end-track_begin);
            }else{
                for(uint16_t k=0;k<jump.size()-1;k++){
                    label_oft.push_back(jump[k+1]-jump[k]);//得到每个label后可以读取的字节数
                }
            }

            for(uint16_t k=0;k<label_oft.size();k++){
                fprintf(g_opf,"\n%s_Song_%d_track_0x%X:\n\t.db ",
                        export_file_name.c_str(),cpySong[i],(uint32_t)ftell(g_ipf));
                uint8_t reline=1;

                for(uint16_t len=0;len<label_oft[k];len++){
                    c=fgetc(g_ipf);
                    reline++;
                    fprintf(g_opf,"0x%02X",c);
                    if(c==0xB3||c==0xB2){
                        uint32_t te;
                        len+=4;
                        fread(&te,4,1,g_ipf);
                        fprintf(g_opf,"\n\t.word %s_Song_%d_track_0x%X\n",
                                export_file_name.c_str(),cpySong[i],te<<5>>5);
                        reline=1;
                        if(len!=label_oft[k]-1){
                            fputs("\t.db ",g_opf);
                        }
                    }else if(len==label_oft[k]-1){
                        fputs("\n",g_opf);
                        reline=1;
                    }else if(reline%10==1){
                        fputs("\n\t.db ",g_opf);
                        reline=1;
                    }else{
                        fputs(",",g_opf);
                    }
                }
            }
        }
        /**打印header数据*/
        int sn=cpySong[i]-1;
        fprintf(g_opf,"\n\n\n.align\n%s_Song_%d_header_0x%X:\n\t.dh 0x%04X,0x%04X\n\t.dw %s_VG_0x%X\n",
                export_file_name.c_str(),sn+1,headoft[sn],tranum[sn],unknow[sn],export_file_name.c_str(),voigop[sn]);
        for(int k=0;k<tranum[sn];k++){
            fprintf(g_opf,"\t.dw %s_Song_%d_track_0x%X\n",
                    export_file_name.c_str(),sn+1,trackoft[sn][k]);
        }
        fclose(g_opf);
    }
    /**
    *创建armips主文件
    */
    if((g_opf=fopen((export_folder_path+"\\"+export_file_name+"_MAIN.asm").c_str(),"w"))==nullptr){
        fprintf(stderr,"创建文件: \"%s\" 失败!",(export_file_name+"_MAIN.asm").c_str());
        getchar();
        exit(1);
    }
    /**打印文件头*/
    fprintf(g_opf,".gba\n.open \"zm.gba\",\"%s.gba\",0x8000000\n\n;by: JumZhu.Diwa\n.definelabel %s_TABLE,0x808f2c8\n",
            export_file_name.c_str(),export_file_name.c_str());
//            getRomName(g_ipfName).c_str(),export_file_name.c_str(),export_file_name.c_str(),tabOffset|0x8000000);
    /**打印definelabel*/
    fputs(";歌曲号修改处(上面的TABLE地址也要和要打入的rom的table地址相符)\n",g_opf);
    fputs(".definelabel unuseoffset,0x8800000\t;无用数据处 (自设-若在ROM中间确保长度足够)\n",g_opf);
    for(uint16_t i=0;i<cpySong.size();i++){
        fprintf(g_opf,".definelabel Song_NO_%d,%d\n",i+1,cpySong[i]);
    }
    /**打印TABEL*/
    for(uint16_t i=0;i<cpySong.size();i++){
        uint32_t tn=cpySong[i]-1;
        fprintf(g_opf,"\n\n.org %s_TABLE + ( Song_NO_%d - 1 ) * 8\n\t.dw %s_Song_%d_header_0x%X,0x%08X\n",
                export_file_name.c_str(),i+1,export_file_name.c_str(),cpySong[i],headoft[tn],songGop[tn]);
    }
    /**打印编译文件集*/
    fprintf(g_opf,"\n\n.org unuseoffset\n.include \"%s_voice_group\\%s_voice_include_main.asm\"\n",
            export_file_name.c_str(),export_file_name.c_str());
    for(uint16_t i=0;i<include_str.size();i++){
        fprintf(g_opf,".include \"%s_Song\\%s\"\n",export_file_name.c_str(),include_str[i].c_str());
    }
    /**打印文件尾*/
    fputs("\n\n.close\n",g_opf);
    fclose(g_opf);
    fflush(stdin);
    fputs("armips文件输出完毕!\n",stdout);
    getchar();
    return 0;
}
