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
*�����ļ��з�Ϊ���ļ���(��romĿ¼��)
*���ļ�������Ϊrom A0-AF����,���ļ����������ļ����Դ�����ʼ+'_'
*���ļ�������track�ļ���,voice_group�ļ���,�Լ�main asm�ļ�.
*track�ļ��б�����ÿ�������������ļ�.
*voice_group�ļ��б�����VG,MS,DP,SWD�ļ���samples,wave_memorys,damages�ļ���
*/


int romToArmips(){
    /**
    *���ø�����Ų���
    */
    std::vector<uint32_t>().swap(cpySong);
    system("cls");
    printf("����ĸ�����ŵ���?-Y/N\n");

    uint32_t song_tatol;
    if(toupper(getch())=='Y'){
        uint32_t start_num,end_num;
        bool invalid=false;
        damageSet(start_num,end_num,true);
        for(;start_num<=end_num;start_num++){
            if(tablefc[start_num-1]){
                cpySong.push_back(start_num);
            }else{
                printf("\n���еĸ���%d��Ч!",start_num);
                invalid=true;
            }
        }
        if(invalid){
            fflush(stdin);
            getchar();
        }
        if(!cpySong.size()){
            fputs("\n�����Χ��û��һ����Ч�ĸ���!\n",stderr);
            getchar();
            return 0;
        }
        fflush(stdin);
        puts("�Ƿ��޳����е�ĳЩ����?-Y/N\n");
        if(toupper(getch())=='Y'){
                uint32_t te,si;
                printf("������Ҫ�޳��ĸ�����%d-%d(0����): ",cpySong.front(),cpySong.back());
                si=cpySong.size();
            for(uint16_t k=0;k<si;k++){
                fflush(stdin);
                if(scanf("%u",&te)!=1){
                    fputs("�������!\n",stderr);
                }else if(!te){
                    break;
                }else if(te>cpySong.back()){
                    fputs("��ų����˷�Χ!\n",stderr);
                }else if(te<cpySong.front()){
                    fputs("���С�ڷ�Χ!\n",stderr);
                }else{
                    std::vector<uint32_t>::iterator it=std::find(cpySong.begin(),cpySong.end(),te);
                    cpySong.erase(it);//ɾ�������
                }
            }
            if(cpySong.empty()){
                fflush(stdin);
                fputs("ɾ����������ʣ��!",stdout);
                getchar();
                return 0;
            }
        }
    }else{
        while(true){
            fflush(stdin);
            printf("������Ҫ�����ĸ�������: ");
            if(scanf("%u",&song_tatol)!=1){
                fputs("�������!\n",stderr);
            }else if(song_tatol>efctabtal){
                fputs("��Ŀ��������Ч����������!\n",stderr);
            }else if(song_tatol<1){
                fputs("��Ŀ����С��1!\n",stderr);
            }else break;
        }
        uint32_t te_num=1;
        while(te_num<=song_tatol){
            fflush(stdin);
//            system("cls");
            printf("�������%d��Ҫ�����ĸ������(7777��ǰ����): ",te_num);
            uint32_t song_num=0;
            if(scanf("%d",&song_num)!=1){
                fputs("�������!\n",stderr);
            }else if(song_num==7777){
                if(!cpySong.size()){
                    fputs("��û��һ����Ч�ĸ�������!",stderr);
                }else break;
            }
            else if(song_num>1999){
                fputs("����̫��!\n",stderr);
            }else if(!tablefc[song_num]){
                fputs("��Ŷ�Ӧ�ĸ�����Ч!\n",stderr);
            }else{
                for(uint16_t j=0;j<cpySong.size();j++){
                    if(song_num==cpySong[j]){
                        fputs("������֮ǰ�Ѿ��������!\n",stderr);
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
//    *���õ�����������
//    */
//    bool vol_flag=false;
//    fflush(stdin);
//    printf("�Ƿ�Ե����ĸ����������е���?-Y/N");
//    if(toupper(getch())=='Y') vol_flag=true;
    fflush(stdin);
    calculateCopySize(1,cpySong.size());   //����������㺯���ó���Ҫ��һЩ����
    /**
    *�������� ���� ��Χ�����ļ�
    */
    std::string export_file_name;
//    export_file_name.resize(16);
    if((g_ipf=fopen(g_ipfName.c_str(),"rb"))==nullptr){//����rom�����
        fprintf(stderr,"���ļ�: %sʧ��!",g_ipfName.c_str());
        getchar();
        exit(1);
    }
    fseek(g_ipf,0xA0,0);    //��λͷlabel
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
    mkdir(export_folder_path.c_str());//��rom�����������ļ���

    std::string current_oft_str;
    std::string current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    mkdir(current_path.c_str());//�ڵ����ļ����ڴ���VG�ļ���
    char c2str[8];
    /**
    *�������������ļ�����
    */
    if(useSampleoft.size()){
        //���������ļ���
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_samples";
        mkdir(current_path.c_str());
        char* buffer=new char[40000];
        if(buffer==NULL){
            fputs("ָ��buffer����ѿռ�ʧ��!",stderr);
            getchar();
            exit(1);
        }
        std::vector<uint32_t>off_set(useSampleoft);//����
        sort(off_set.begin(),off_set.end());//����
        off_set.erase(unique(off_set.begin(),off_set.end()),off_set.end());//ȥ���ظ�
        for(uint16_t l=0;l<off_set.size();l++){//����ÿ�������ļ�
            sprintf(c2str,"%X",off_set[l]);
            current_oft_str=c2str;
            //����ÿ�����������ļ�
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".sample").c_str(),"wb"))==nullptr){
                fprintf(stderr,"�����ļ�: %sʧ��!",(export_file_name+"_0x"+current_oft_str+".sample").c_str());
                getchar();
                exit(1);
            }
            fseek(g_ipf,off_set[l],0);//��λ��ÿ���������ݵ�λ��
            samplehead sample_head;
            fread(&sample_head,16,1,g_ipf);
            fwrite(&sample_head,16,1,g_opf);//д��ͷ����
            int siz=sample_head.datsiz+(sample_head.loopTag>>30);
            if(siz%4!=0){
                siz=siz+4-siz%4;
            }
            if(siz>40000){
                char* buff=new char[siz];
                if(buff==NULL){
                fputs("ָ��buff����ѿռ�ʧ��!",stderr);
                getchar();
                exit(1);
            }
                fread(buff,siz,1,g_ipf);//��������ȫ����ȡ
                fwrite(buff,siz,1,g_opf);//д�����������ļ�
                delete[] buff;
                buff=NULL;
            }else{
                fread(buffer,siz,1,g_ipf);
                fwrite(buffer,siz,1,g_opf);
            }
            fclose(g_opf);//�ر����������ļ���
        }
        delete[] buffer;
        buffer=NULL;
    }
    /**
    *�������������ļ�����
    */
    if(useWaveMemoft.size()){
        //���������ļ���
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_wave_memorys";
        mkdir(current_path.c_str());
        char buff[16];
        for(uint16_t l=0;l<useWaveMemoft.size();l++){
            sprintf(c2str,"%X",useWaveMemoft[l]);
            current_oft_str=c2str;
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".wave_memory").c_str(),"wb"))==nullptr){
                fprintf(stderr,"�����ļ�: \"%s\"ʧ��",(export_file_name+"_0x"+current_oft_str+".wave_memory").c_str());
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
    *������Χ�����ļ�����
    */
    if(useDamageoft.size()){
        //������Χ�ļ���
        current_path=export_folder_path+"\\"+export_file_name+"_voice_group\\"+export_file_name+"_damages";
        mkdir(current_path.c_str());
        char buff[128];
        for(uint16_t l=0;l<useDamageoft.size();l++){
            sprintf(c2str,"%X",useDamageoft[l]);
            current_oft_str=c2str;
            if((g_opf=fopen((current_path+"\\"+export_file_name+"_0x"+current_oft_str+".damage").c_str(),"wb"))==nullptr){
                fprintf(stderr,"�����ļ�: \"%s\"ʧ��!",(export_file_name+"_0x"+current_oft_str+".damage").c_str());
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
    *����-����-����-��Χ-���� SWD.asm�����ļ�
    */
    current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    if(useSampleoft.size()||useWaveMemoft.size()||useDamageoft.size()){
        if((g_opf=fopen((current_path+"\\"+export_file_name+"_SWD.asm").c_str(),"w"))==nullptr){
            fprintf(stderr,"�����ļ�: \"%s\"ʧ��!",(export_file_name+"_SWD.asm").c_str());
            getchar();
            exit(1);
        }
        /**д����������*/
        if(useSampleoft.size()){
            bool un_ique=true;
            std::vector<uint32_t>off_set(0,0);
//            sort(off_set.begin(),off_set.end());//����
//            off_set.erase(unique(off_set.begin(),off_set.end()),off_set.end());//ȥ���ظ�
            fputs(";sample����-------------------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X ��������:++++++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
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
        /**д�벨�β���*/
        if(useWaveMemoft.size()){
            std::vector<uint32_t>off_set(0,0);
            bool un_ique=true;
            fputs(";wave memory����-----------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X ��������:++++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
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
        /**д�뷶Χ����*/
        if(useDamageoft.size()){
            std::vector<uint32_t>off_set(0,0);
            bool un_ique=true;
            fputs(";damage����---------------------------------------------------\n",g_opf);
            for(uint16_t i=0;i<VGinfo.size();i++){
                if((useVGoft[i]&0xF0000000)>0x50000000){
                    fprintf(g_opf,"\n.align\t\t;Drum Part 0x%X\n",useVGoft[i]&0xFFFFFFF);
                }else if(!(useVGoft[i]&0xF0000000)){
                    fprintf(g_opf,"\n.align\t\t;Voice Group 0x%X ��������:+++++++++++\n\n",useVGoft[i]&0xFFFFFFF);
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
    *����Voice_Group asm�ļ�
    */
    std::string temp_name;
    current_path=export_folder_path+"\\"+export_file_name+"_voice_group";
    std::vector<std::string>include_str(0);
    for(uint16_t i=0;i<VGinfo.size();i++){//VG��ַ����ѭ��
        vtda_t instr;
        uint8_t bit_8;
        if(!(bit_8=useVGoft[i]>>28)){//��ͨ��VG
            sprintf(c2str,"%X",useVGoft[i]);
            current_oft_str=c2str;
            temp_name=export_file_name+"_VG_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"�����ļ�: \"%s\"ʧ��!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_VG_0x%X:\n",export_file_name.c_str(),useVGoft[i]);
            fseek(g_ipf,useVGoft[i],0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//��������
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            (useVGoft[(VGinfo[i][j]>>16)&0xFFF])<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_DP_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[VGinfo[i][j]&0xFFFF]<<4>>4,instr.tdat3,j);
                    }
                }else{
                    fputs("\t.dw 0,0,0\n",g_opf);
                }
            }
            fclose(g_opf);
        }else if(bit_8==5){//mutil sample����VG
            sprintf(c2str,"%X",useVGoft[i]&0xFFFFFFF);
            current_oft_str=c2str;
            temp_name=export_file_name+"_MS_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"�����ļ�: \"%s\"ʧ��!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_MS_0x%s:\n",export_file_name.c_str(),current_oft_str.c_str());
            fseek(g_ipf,useVGoft[i]<<5>>5,0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//��������
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[(VGinfo[i][j]>>16)&0xFFF]<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_DP_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[VGinfo[i][j]&0xFFFF]<<4>>4,instr.tdat3,j);
                    }
                }else{
                    fputs("\t.dw 0,0,0\n",g_opf);
                }
            }
            fclose(g_opf);
        }else if(bit_8==7){//drum part����VG
            sprintf(c2str,"%X",useVGoft[i]&0xFFFFFFF);
            current_oft_str=c2str;
            temp_name=export_file_name+"_DP_0x"+current_oft_str+".asm";
            if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
                fprintf(stderr,"�����ļ�: \"%s\"ʧ��!",temp_name.c_str());
                getchar();
                exit(1);
            }
            include_str.push_back(temp_name.c_str());
            fprintf(g_opf,".align\n%s_DP_0x%s:\n",export_file_name.c_str(),current_oft_str.c_str());
            fseek(g_ipf,useVGoft[i]<<5>>5,0);
            for(uint16_t j=0;j<VGinfo[i].size();j++){
                fread(&instr,12,1,g_ipf);
                if(keyEfc[i][j]){
                    if((bit_8=VGinfo[i][j]>>28)==1){//��������
                        fprintf(g_opf,"\t.dw 0x%08X,0x%08X,0x%08X\t;%d instrument\n",instr.tdat1,instr.tdat2,instr.tdat3,j);
                    }else if(bit_8==2){//sample����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_sample_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useSampleoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==3){//wave_memory����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_wave_memory_0x%X,0x%08X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useWaveMemoft[VGinfo[i][j]&0xFFFF],instr.tdat3,j);
                    }else if(bit_8==5){//voice_group����
                        fprintf(g_opf,"\t.dw 0x%08X,%s_MS_0x%X,%s_damage_0x%X\t;%d instrument\n",instr.tdat1,export_file_name.c_str(),
                            useVGoft[(VGinfo[i][j]>>16)&0xFFF]<<4>>4,export_file_name.c_str(),useDamageoft[VGinfo[i][j]&0xFFFF],j);
                    }else if(bit_8==7){//drum part����
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
    *����VG�����ļ�
    */
    temp_name=export_file_name+"_voice_include_main.asm";
    if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
        fprintf(stderr,"�����ļ�: \"%s\" ʧ��!",temp_name.c_str());
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
    *Song����
    */
    include_str.clear();//���
    current_path=export_folder_path+"\\"+export_file_name+"_Song";
    mkdir(current_path.c_str());//����Song�ļ���
    for(uint16_t i=0;i<cpySong.size();i++){//�������� ��ѭ��
        sprintf(c2str,"%d",cpySong[i]);//�������ַ�����
        current_oft_str=c2str;
        temp_name=export_file_name+"_Song_"+current_oft_str+".asm";
        if((g_opf=fopen((current_path+"\\"+temp_name).c_str(),"w"))==nullptr){
            fprintf(stderr,"�����ļ�: \"%s\" ʧ��!",temp_name.c_str());
            getchar();
            exit(1);
        }
        include_str.push_back(temp_name.c_str());//����Song�ļ���
        /**��ӡÿ�����������*/
        for(uint16_t j=0;j<tranum[cpySong[i]-1];j++){//�������� ��ѭ��
            uint32_t track_begin=trackoft[cpySong[i]-1][j];//ÿ��track��ʼ��ַ
            fseek(g_ipf,track_begin,0);//��λ����ǰ������ÿ��trackλ��

            std::vector<uint32_t>jump(0,0);//������¼ÿ����ת�ĵ�ַ
            std::vector<uint32_t>b2b3(0,0);//������¼b2b3��Ծ��
            int c;
            uint32_t jump_oft;
            uint32_t track_end;//ÿ��track�Ľ�����ַ
            uint32_t techk;//���ڼ��
            do{
                c=fgetc(g_ipf);
                if(c>0xB0&&c<0xB4){
                    if(c==0xB1){
                        track_end=ftell(g_ipf);
                        break;
                    }else if(c==0xB3||c==0xB2){
                        techk=ftell(g_ipf);//��¼��ַ
                        fread(&jump_oft,4,1,g_ipf);//��ȡ��ַ
                        jump_oft^=0x8000000;
                        //ĳhack������,��ת��ַ��Ȼ�����˵�ǰ����
                        if(jump_oft>>24!=0||track_begin>jump_oft||techk<jump_oft){
                            fprintf(stderr,"��ַ: %X������ת��ַΪ: %X�ǲ��ϸ����ת��ַ!",techk,jump_oft^0x8000000);
                            getchar();
                            exit(1);
                        }
                        jump.push_back(jump_oft);//��¼��ַ
                    }
                }
            }while(!feof(g_ipf));
            fseek(g_ipf,track_begin,0);//�ض�λ�ڵ�ǰtrack�״�

            sort(jump.begin(),jump.end());//����
            jump.erase(unique(jump.begin(),jump.end()),jump.end());//ȥ���ظ��ĵ�ַ
            std::vector<uint32_t>label_oft(0,0);//labelƫ��
            if(jump.size()!=0&&jump[0]!=track_begin){
                jump.insert(jump.begin(),track_begin);
            }
            jump.push_back(track_end);
            if(jump.size()==1){
                label_oft.push_back(track_end-track_begin);
            }else{
                for(uint16_t k=0;k<jump.size()-1;k++){
                    label_oft.push_back(jump[k+1]-jump[k]);//�õ�ÿ��label����Զ�ȡ���ֽ���
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
        /**��ӡheader����*/
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
    *����armips���ļ�
    */
    if((g_opf=fopen((export_folder_path+"\\"+export_file_name+"_MAIN.asm").c_str(),"w"))==nullptr){
        fprintf(stderr,"�����ļ�: \"%s\" ʧ��!",(export_file_name+"_MAIN.asm").c_str());
        getchar();
        exit(1);
    }
    /**��ӡ�ļ�ͷ*/
    fprintf(g_opf,".gba\n.open \"zm.gba\",\"%s.gba\",0x8000000\n\n;by: JumZhu.Diwa\n.definelabel %s_TABLE,0x808f2c8\n",
            export_file_name.c_str(),export_file_name.c_str());
//            getRomName(g_ipfName).c_str(),export_file_name.c_str(),export_file_name.c_str(),tabOffset|0x8000000);
    /**��ӡdefinelabel*/
    fputs(";�������޸Ĵ�(�����TABLE��ַҲҪ��Ҫ�����rom��table��ַ���)\n",g_opf);
    fputs(".definelabel unuseoffset,0x8800000\t;�������ݴ� (����-����ROM�м�ȷ�������㹻)\n",g_opf);
    for(uint16_t i=0;i<cpySong.size();i++){
        fprintf(g_opf,".definelabel Song_NO_%d,%d\n",i+1,cpySong[i]);
    }
    /**��ӡTABEL*/
    for(uint16_t i=0;i<cpySong.size();i++){
        uint32_t tn=cpySong[i]-1;
        fprintf(g_opf,"\n\n.org %s_TABLE + ( Song_NO_%d - 1 ) * 8\n\t.dw %s_Song_%d_header_0x%X,0x%08X\n",
                export_file_name.c_str(),i+1,export_file_name.c_str(),cpySong[i],headoft[tn],songGop[tn]);
    }
    /**��ӡ�����ļ���*/
    fprintf(g_opf,"\n\n.org unuseoffset\n.include \"%s_voice_group\\%s_voice_include_main.asm\"\n",
            export_file_name.c_str(),export_file_name.c_str());
    for(uint16_t i=0;i<include_str.size();i++){
        fprintf(g_opf,".include \"%s_Song\\%s\"\n",export_file_name.c_str(),include_str[i].c_str());
    }
    /**��ӡ�ļ�β*/
    fputs("\n\n.close\n",g_opf);
    fclose(g_opf);
    fflush(stdin);
    fputs("armips�ļ�������!\n",stdout);
    getchar();
    return 0;
}
