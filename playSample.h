#ifndef PLAYSAMPLE_H_INCLUDED
#define PLAYSAMPLE_H_INCLUDED

int MakeWav(int fren,int chunk,int datsize);
class Wav
{
    public:
    const char riffChunkID[4]={'R','I','F','F'};//riff标识
    uint32_t chunkSize;//总数据长度-8bit
    const char Wave[4]={'W','A','V','E'};//wave标识
    const char fmtChunkID[4]={'f','m','t',' '};//标识 "fmt "
    uint32_t subChunkSize;//PCM数据长度
    uint16_t audioFomat;//PCM = 1
    uint16_t numChannels;//通道 1单 2双
    uint32_t Frequency;//频率
    uint32_t byteRate;//= SampleRate * NumChannels * BitsPerSample/8
    uint16_t blockAlign;//= NumChannels * BitsPerSample/8
    uint16_t bitsPerSample;//8bit,16bit,etc.
    const char dataChunkID[4]={'d','a','t','a'};//标识 "data"
    uint32_t subChunk2Size;//数据长度 总长度-44
    //std::vector <uint8_t>wavDat;//声音数据
};


#endif // PLAYSAMPLE_H_INCLUDED
