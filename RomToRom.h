#ifndef ROMTOROM_H_INCLUDED
#define ROMTOROM_H_INCLUDED
#include <cstdint>
#include <vector>

int trackCopy();

extern int subTabTol;//副RomTab总数
extern uint8_t subTabefc[2000];//副RomTab有效 标记
extern int subEfcTabTol;//副Rom有效Tab总数

extern std::vector<int>subUseSong;//定义数组存储要被复制曲占用的歌曲号
extern std::vector<int>cpySong;//定义主rom要被复制的歌曲号组


#endif // ROMTOROM_H_INCLUDED
