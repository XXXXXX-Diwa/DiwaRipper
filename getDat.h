#ifndef GETDAT_H_INCLUDED
#define GETDAT_H_INCLUDED

void getData(uint32_t taboft,std::string ifn);

/**优先级处理如下：
*-在PSG频道上(square 1,square 2,programmable waveform,noise)将播放优先级最高的音符
*-在直接声道(direct sound channels)上，如果不再有空闲声道，优先级最高的音符将继续播放，而优先级较低的音符将被忽略或静音，以便为优先级较高的音符留出空间。
*在优先级相等的情况下，音轨(track)号将起作用。较低的音轨总是优先于较高的音轨。
*/

/**回音反馈（影响所有直接声道）
*如果设置bit-7，则每当歌曲开始播放时，混响设置都设置为该值，覆盖任何先前的值。
*如果bit-7清除，则在开始播放歌曲时不会更改任何内容，并且保留以前使用的混响值。
*/

typedef struct header
{
    uint16_t traNum;//歌曲数目 low为数目 high unknow
    uint16_t unknow;//未知数据 low为优先级 high 为: 回音反馈
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
extern uint32_t tabtotal;//table总数
extern uint32_t efctabtal;//有效的曲子总数
extern uint32_t voitotal;//voicegroup总数
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
