#ifndef ROMTOROM_H_INCLUDED
#define ROMTOROM_H_INCLUDED
#include <cstdint>
#include <vector>
void damageSet(uint32_t& startNum,uint32_t& endNum,bool flag);
int trackCopy();

extern uint32_t subTabTol;//副RomTab总数
extern uint8_t subTabefc[2000];//副RomTab有效 标记
extern uint32_t subEfcTabTol;//副Rom有效Tab总数

extern std::vector<uint32_t>subUseSong;//定义数组存储要被复制曲占用的歌曲号
extern std::vector<uint32_t>cpySong;//定义主rom要被复制的歌曲号组


#endif // ROMTOROM_H_INCLUDED
