
/***************************************************************
	������״���� 
***************************************************************/

#define WINDOW_WIDTH 9 //��Ϸ���ں���Ԫ�����  
#define WINDOW_HEIGHT 21//��Ϸ��������Ԫ����� 


struct WIN window[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];

SHAPE prishape[TOTAL_SHAPE];//Ԥ��7����״ 



void InitShape(double gcx, double gcy);//��ʼ��ͼ�κ��� 

void DrawFrame();//���ƿ�ܺ���  

void DrawShape(const SHAPEPTR shapeptr);//����ͼ�κ���
void DrawBlock(double x, double y, const char* color);//���Ʒ��麯��,�����Ǵ��ڽ������� 
 
void ShapeDrop(SHAPEPTR shapeptr);//ͼ������1���� 
void ShapeMove(SHAPEPTR shapeptr, int dir);//ͼ�������ƶ�������dir��1���� ��-1���� 
void ShapeSpin(SHAPEPTR shapeptr, int dir);//ͼ����ת������dir��1˳ʱ�� ��-1��ʱ��
void StraDrop(SHAPEPTR shapeptr);//ͼ�ε��䵽�׺��� 
 
int XtransMyX(double x);//����ת����������������ת��Ϊ��Ϸ�������� 
int YtransMyY(double y);
double MyXtransX(int mx);//��Ϸ��������ת��Ϊ�������� 
double MyYtransY(int my);

bool IsBottom(SHAPEPTR shapeptr);//�ж�ͼ���Ƿ񵽵׺��� 

void CleanLine(int k);//���һ�к�����k������Ϸ����y���� 



