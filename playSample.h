#ifndef PLAYSAMPLE_H_INCLUDED
#define PLAYSAMPLE_H_INCLUDED

int MakeWav(int fren,int chunk,int datsize);
class Wav
{
    public:
    const char riffChunkID[4]={'R','I','F','F'};//riff��ʶ
    uint32_t chunkSize;//�����ݳ���-8bit
    const char Wave[4]={'W','A','V','E'};//wave��ʶ
    const char fmtChunkID[4]={'f','m','t',' '};//��ʶ "fmt "
    uint32_t subChunkSize;//PCM���ݳ���
    uint16_t audioFomat;//PCM = 1
    uint16_t numChannels;//ͨ�� 1�� 2˫
    uint32_t Frequency;//Ƶ��
    uint32_t byteRate;//= SampleRate * NumChannels * BitsPerSample/8
    uint16_t blockAlign;//= NumChannels * BitsPerSample/8
    uint16_t bitsPerSample;//8bit,16bit,etc.
    const char dataChunkID[4]={'d','a','t','a'};//��ʶ "data"
    uint32_t subChunk2Size;//���ݳ��� �ܳ���-44
    //std::vector <uint8_t>wavDat;//��������
};


#endif // PLAYSAMPLE_H_INCLUDED
