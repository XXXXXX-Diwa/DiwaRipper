#ifndef ROMTOROM_H_INCLUDED
#define ROMTOROM_H_INCLUDED
#include <cstdint>
#include <vector>
void damageSet(uint32_t& startNum,uint32_t& endNum,bool flag);
int trackCopy();

extern uint32_t subTabTol;//��RomTab����
extern uint8_t subTabefc[2000];//��RomTab��Ч ���
extern uint32_t subEfcTabTol;//��Rom��ЧTab����

extern std::vector<uint32_t>subUseSong;//��������洢Ҫ��������ռ�õĸ�����
extern std::vector<uint32_t>cpySong;//������romҪ�����Ƶĸ�������


#endif // ROMTOROM_H_INCLUDED
