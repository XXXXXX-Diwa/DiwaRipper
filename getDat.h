#ifndef GETDAT_H_INCLUDED
#define GETDAT_H_INCLUDED

void getData(uint32_t taboft,std::string ifn);

/**���ȼ��������£�
*-��PSGƵ����(square 1,square 2,programmable waveform,noise)���������ȼ���ߵ�����
*-��ֱ������(direct sound channels)�ϣ���������п������������ȼ���ߵ��������������ţ������ȼ��ϵ͵������������Ի������Ա�Ϊ���ȼ��ϸߵ����������ռ䡣
*�����ȼ���ȵ�����£�����(track)�Ž������á��ϵ͵��������������ڽϸߵ����졣
*/

/**����������Ӱ������ֱ��������
*�������bit-7����ÿ��������ʼ����ʱ���������ö�����Ϊ��ֵ�������κ���ǰ��ֵ��
*���bit-7��������ڿ�ʼ���Ÿ���ʱ��������κ����ݣ����ұ�����ǰʹ�õĻ���ֵ��
*/

typedef struct header
{
    uint16_t traNum;//������Ŀ lowΪ��Ŀ high unknow
    uint16_t unknow;//δ֪���� lowΪ���ȼ� high Ϊ: ��������
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
extern uint32_t tabtotal;//table����
extern uint32_t efctabtal;//��Ч����������
extern uint32_t voitotal;//voicegroup����
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
