#include <iostream>
#include "main.h"
#include "tabSearch.h"
#include "crc32.h"
#include "showDat.h"
#include "getDat.h"

using namespace std;

string g_ipfName="";
string g_subRom="";
string SWpath="";

FILE* g_ipf=nullptr;
FILE* g_opf=nullptr;
uint32_t tabOffset=0;

int main(int argc,char** argv)
{
    SWpath=argv[0];
    if(argc>=2) g_ipfName=argv[1];
    if(argc>2) g_subRom=argv[2];

    tabGet(g_ipfName);

    getData(tabOffset,g_ipfName);

    while(importinterface());

    return 0;
}
