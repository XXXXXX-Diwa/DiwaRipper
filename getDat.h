#ifndef GETDAT_H_INCLUDED
#define GETDAT_H_INCLUDED

void getData(uint32_t taboft,std::string ifn);

typedef struct header
{
    uint16_t traNum;//歌曲数目
    uint16_t unknow;//未知数据
    uint32_t voigop;//voice group
    uint32_t trackoft[16];//歌曲地址数组
}head_t;


typedef struct table
{
    uint32_t headOft;//head地址
    uint32_t songGop;//song group
}tab_t;


extern uint8_t samplefc[128];//样本导出flag
extern uint8_t tralimit;//当前预览音轨限制
extern int tabtotal;//table总数
extern int efctabtal;//有效的曲子总数
extern int voitotal;//voicegroup总数
extern uint32_t voiceoft[100];//voicegroup的地址存储

extern uint32_t headoft[2000];
extern uint32_t songGop[2000];

extern uint16_t tranum[2000];//歌曲数目
extern uint16_t unknow[2000];//未知数据
extern uint32_t voigop[2000];//voice group
extern uint32_t trackoft[2000][16];//歌曲地址数组

extern uint8_t tablefc[2000];//tabel有效flag
extern uint8_t tabvoinum[2000];//每个曲子使用的voice序号
extern uint8_t tabloopefc[2000];//每个曲子的循环标记
extern int tabstart;//曲号开始
extern int tabend;//曲号结束
extern int tabgop[2000];//选中的曲号
extern int tabgoptotal;//选中的曲号总数
extern uint16_t voitabNum[100];//每个voicegroup的歌曲数
extern uint8_t voisplNum[100];//每个voicegroup的样本数

extern char clean;
//extern int gopgop[30];//选中的voice group号

#endif // GETDAT_H_INCLUDED
