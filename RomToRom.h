#ifndef ROMTOROM_H_INCLUDED
#define ROMTOROM_H_INCLUDED
#include <cstdint>
#include <vector>

int trackCopy();

extern int subTabTol;//��RomTab����
extern uint8_t subTabefc[2000];//��RomTab��Ч ���
extern int subEfcTabTol;//��Rom��ЧTab����

extern std::vector<int>subUseSong;//��������洢Ҫ��������ռ�õĸ�����
extern std::vector<int>cpySong;//������romҪ�����Ƶĸ�������


#endif // ROMTOROM_H_INCLUDED
