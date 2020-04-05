#ifndef GETDAT_H_INCLUDED
#define GETDAT_H_INCLUDED

void getData(uint32_t taboft,std::string ifn);

typedef struct header
{
    uint16_t traNum;//������Ŀ
    uint16_t unknow;//δ֪����
    uint32_t voigop;//voice group
    uint32_t trackoft[16];//������ַ����
}head_t;


typedef struct table
{
    uint32_t headOft;//head��ַ
    uint32_t songGop;//song group
}tab_t;


extern uint8_t samplefc[128];//��������flag
extern uint8_t tralimit;//��ǰԤ����������
extern int tabtotal;//table����
extern int efctabtal;//��Ч����������
extern int voitotal;//voicegroup����
extern uint32_t voiceoft[100];//voicegroup�ĵ�ַ�洢

extern uint32_t headoft[2000];
extern uint32_t songGop[2000];

extern uint16_t tranum[2000];//������Ŀ
extern uint16_t unknow[2000];//δ֪����
extern uint32_t voigop[2000];//voice group
extern uint32_t trackoft[2000][16];//������ַ����

extern uint8_t tablefc[2000];//tabel��Чflag
extern uint8_t tabvoinum[2000];//ÿ������ʹ�õ�voice���
extern uint8_t tabloopefc[2000];//ÿ�����ӵ�ѭ�����
extern int tabstart;//���ſ�ʼ
extern int tabend;//���Ž���
extern int tabgop[2000];//ѡ�е�����
extern int tabgoptotal;//ѡ�е���������
extern uint16_t voitabNum[100];//ÿ��voicegroup�ĸ�����
extern uint8_t voisplNum[100];//ÿ��voicegroup��������

extern char clean;
//extern int gopgop[30];//ѡ�е�voice group��

#endif // GETDAT_H_INCLUDED
