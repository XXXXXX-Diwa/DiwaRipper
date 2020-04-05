#include<stdio.h>
#include<cstdint>
#include "main.h"
#include "playSample.h"
//#pragma comment(lib, "WinMM.Lib")//C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64
using namespace std;

int MakeWav(int fren,int chunk,int datsize)
{

        Wav *wav=new Wav();
        wav->chunkSize=chunk;
        wav->subChunkSize=0x10;
        wav->audioFomat=1;
        wav->numChannels=1;
        wav->Frequency=fren;
        wav->byteRate=fren;
        wav->blockAlign=1;
        wav->bitsPerSample=8;
        wav->subChunk2Size=datsize;
        int bit;

        bit=0;
        FILE *WavFile=fopen("C:\\Users\\Administrator\\AppData\\Local\\Temp\\_tempwav.wav","wb");

        if(WavFile==nullptr)
        {
            WavFile=fopen("_tempwav.wav","wb");
            bit=1;
        }
        fwrite(wav->riffChunkID,44,1,WavFile);

        int temp;
        uint8_t bit8;
        if(datsize>0x40000)
        {
            printf("样本数据尺寸异常!");
            delete(wav);
            return 0;
        }
        for(int i=0;i<datsize;i++)
        {
            temp=fgetc(g_ipf);
            bit8=temp+128;
            fputc(bit8,WavFile);
        }
        fclose(WavFile);
        delete(wav);
        return bit;
}
