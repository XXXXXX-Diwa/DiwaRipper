#ifndef TABSEARCH_H_INCLUDED
#define TABSEARCH_H_INCLUDED

#include <string>
#include <cstring>
#include <cstdint>


int tabGet(std::string ipfn);
int searchTab(int Lv,int traNum);
std::string getRomName(std::string s);
std::string onlyPath(std::string s);
std::string getExtension(std::string s);
uint32_t offsetGet(char* tabDat);
int oldTabChk(uint32_t toft);

extern FILE* searchF;
extern uint32_t offset32;
extern uint32_t srhFst;
extern uint32_t iptFst;
//uint8_t bit8=0;
extern fpos_t curOffset;
extern char tableData[16];
extern char headData[72];

#endif // TABSEARCH_H_INCLUDED
