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

extern std::string g_ipfName;//��rom����
extern std::string SWpath;//����Ŀ¼
extern uint32_t tabOffset;//ѡ����table��ַ
//extern uint32_t possibleTab[20];//���ܵ�table��ַ
extern std::string g_subRom;//��rom������
extern uint32_t subTabOft;//��table��ַ


#endif // MAIN_H_INCLUDED
