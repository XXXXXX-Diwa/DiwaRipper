
#include <Windows.h>
#include "crc32.h"
#include "main.h"
//#include <string>

//#pragma warning(disable:4996 )//防止警告

uint32_t Crc32Table[256];


//0x04 C1 1D B7
uint32_t BitReverse(uint32_t poly)//进行位颠倒
{
	uint32_t dwRet=0;
	for (int i = 0; i < 32; i++) {
		dwRet |= poly & 1;//每次获得最低位
//		printf("%d", dwRet & 1);
		if (i+1 == 32)//这里多移位了一次 所以出错 一定要小心
			break;
		dwRet <<= 1;
		poly >>= 1;
	}

	return dwRet;
}

//以这个IEEE802.3标准CRC32多项式为例：x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1 相应的bit为上对应1
void CRC32Init(uint32_t poly)//码表生成
{
	int i;

	poly = BitReverse(poly);

//	printf("bitReverse的结果为 : %08x\n",poly);
	for (i = 0; i < 256; i++) {//生成为256的码表
		ULONG eachGen = i;
		for (size_t j = 0; j < 8; j++)//移动一个字节的长度 每次检验最低位是否为1 若不是继续移位 不然多项式异或每轮的表值 且因为每次eachGen获得的为index 每次必定会不一样 故产生的表其散列性非常好
		{
			if (eachGen & 1)
				eachGen = poly ^ (eachGen >> 1);
			else
				eachGen >>= 1;
		}
		Crc32Table[i] = eachGen;
	}

}

uint32_t CRC32Encrypt(uint32_t crc32, char *data, ULONG dataLen)
{
	ULONG i;

	for (i = 0; i < dataLen; i++) {
		unsigned char index = (unsigned char)(crc32 ^ data[i]);//每一轮的crc32 和 data[i]生成index
		crc32 = (crc32 >> 8) ^ Crc32Table[index];//每一轮的原先的crc32的高24位与 index查表的值进行异或产生新的crc32 循环dataLen次数
	}

	return crc32;
}

uint32_t crc32Result(std::string s )
{
	FILE * fp = nullptr;
	ULONG fileLen = 0;
	char * buffer = NULL;
	uint32_t crc32 = 0;

	//生成码表
	CRC32Init(0x04C11DB7);

	fp = fopen(s.c_str(), "rb");
	if (nullptr == fp) {
		printf("无法打开文件%s\n",s.c_str());
	}
	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp);
	buffer = (char *)malloc(fileLen * sizeof(char));
	memset(buffer, 0, fileLen * sizeof(char));
//	printf("文件长度 : %d\n", fileLen);
	fseek(fp, 0, SEEK_SET);//定位到文件开头 开始读取数据
	if (fread(buffer, sizeof(char), fileLen, fp)<1) {
			printf("无法读取文件%s数据\n",s.c_str());
	}

	crc32 = 0xFFFFFFFF;//作为起始值 运算后分布更好点
	crc32 = CRC32Encrypt(crc32,buffer,fileLen);
	crc32 ^= 0xFFFFFFFF;
//	printf("CRC32的结果为  : %8x\n",crc32);

//	if (fp)
		fclose(fp);
		return crc32;
}
