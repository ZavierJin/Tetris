
/***************************************************************
	ͨ�ú������������� 
***************************************************************/

#define SCORE_MAX_CAP 5//������Ϸ�������λ�� 

#define L 0.328//��Ԫ�񳤶� 
#define FRAME_SIZE (0.15*L)//�߿��� 
#define TOTAL_SHAPE 7//��ʼ��״���� 

struct WIN{//��Ϸ�ܴ��ڵ�Ԫ��ṹ 
	bool isoccu;//��Ԫ���Ƿ�ռ�ã�ǽ��һ�ֱ�ռ�ã�
	int type;//�������ͣ�0Ϊ��ͨ��3Ϊ�������� 
	char* color;//��ɫ 
};

typedef struct{//ͼ�νṹ 
	int id;//ͼ�α�ţ�0~6��
	char* color;//ͼ����ɫ
	int point[4][2];//ͼ�θ��������꣬��һ�������ǵڼ������飬�ڶ�������0����x��1����y 
	int No;//���ⷽ���ڸ���״�е����λ��   -1��ʾ�����ⷽ��  0-3��ʾ��״���ĸ�λ�� 
    int  type;
	/*��Ԫ��ռ�õ����
	0Ϊ��ͨ����δ������ʱ����� 
	1Ϊը������δ����������� 
	2Ϊ������ը������ 
	3Ϊ�������� 
	4Ϊ�������� 
	5 Ϊ��
	*/
}SHAPE;
typedef SHAPE* SHAPEPTR1;//ͼ�νṹָ�� 
typedef SHAPE* SHAPEPTR2;
typedef SHAPE* SHAPEPTR;


//ͨ�ú���

int RandomNum(int total);//���0-total��������� 
int RandomId();//���0~6�����id
void ScoreTrans(char* ch_score, int score);//�÷������ͻ�Ϊ�ַ���ǰ�油0 
bool InButton(double x,double y, double w, double h, double mx, double my);//�ж������Ƿ��ڰ�ť��


