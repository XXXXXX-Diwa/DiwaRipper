#ifndef SHOWSAM_H_INCLUDED
#define SHOWSAM_H_INCLUDED

int VoiGopInfo();
int showSampleDat(int samNum,int vgNum,int curkey);
int showWaveMemoryData(int dataOffset);
int originalFre(int Fre,int key);

typedef struct vtableDat
{
    uint32_t tdat1;
    uint32_t tdat2;
    uint32_t tdat3;
}vtda_t;


//typedef struct Wav_Riff
//{
//    char riffChunkID[4];//riff标识
//    uint32_t chunkSize;//总数据长度-8bit
//    char Wave[4];//wave标识
//}Riff_t;
//
//typedef struct Wav_Fmt
//{
//    char fmtChunkID[4];//标识 "fmt "
//    uint32_t subChunkSize;//PCM数据长度
//    uint16_t audioFomat;//PCM = 1
//    uint16_t numChannels;//通道 1单 2双
//    uint32_t Frequency;//频率
//    uint32_t byteRate;//= SampleRate * NumChannels * BitsPerSample/8
//    uint16_t blockAlign;//= NumChannels * BitsPerSample/8
//    uint16_t bitsPerSample;//8bit,16bit,etc.
//}Fmt_t;
//
//typedef struct Wav_Data
//{
//    char dataChunkID[4];//标识 "data"
//    uint32_t subChunk2Size;//数据长度 总长度-44
//}Data_t;
//
//typedef struct Wav_Fotmat
//{
//    Riff_t riff;
//    Fmt_t fmt;
//    Data_t data;
//}Wav;


#endif // SHOWSAM_H_INCLUDED
