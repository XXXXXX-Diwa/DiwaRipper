#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <cstdio>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <io.h>

extern FILE* g_ipf;
extern FILE* g_opf;

extern std::string g_ipfName;//主rom名字
extern std::string SWpath;//程序目录
extern uint32_t tabOffset;//选定的table地址
//extern uint32_t possibleTab[20];//可能的table地址
extern std::string g_subRom;//副rom的名字
extern uint32_t subTabOft;//副table地址


#endif // MAIN_H_INCLUDED
