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
//    char riffChunkID[4];//riff��ʶ
//    uint32_t chunkSize;//�����ݳ���-8bit
//    char Wave[4];//wave��ʶ
//}Riff_t;
//
//typedef struct Wav_Fmt
//{
//    char fmtChunkID[4];//��ʶ "fmt "
//    uint32_t subChunkSize;//PCM���ݳ���
//    uint16_t audioFomat;//PCM = 1
//    uint16_t numChannels;//ͨ�� 1�� 2˫
//    uint32_t Frequency;//Ƶ��
//    uint32_t byteRate;//= SampleRate * NumChannels * BitsPerSample/8
//    uint16_t blockAlign;//= NumChannels * BitsPerSample/8
//    uint16_t bitsPerSample;//8bit,16bit,etc.
//}Fmt_t;
//
//typedef struct Wav_Data
//{
//    char dataChunkID[4];//��ʶ "data"
//    uint32_t subChunk2Size;//���ݳ��� �ܳ���-44
//}Data_t;
//
//typedef struct Wav_Fotmat
//{
//    Riff_t riff;
//    Fmt_t fmt;
//    Data_t data;
//}Wav;


#endif // SHOWSAM_H_INCLUDED
