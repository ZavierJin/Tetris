

/***************************************************************
	��Ϸ�浵 
***************************************************************/

//�����浵�ṹ
struct Record{ 
	int num;//�û�id 
	struct WIN window2[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];//��Ϸ���浥Ԫ�� 
	SHAPE sh;//��ǰ�ƶ���״ 
	int nextid;//��һ����״id 
	int score;//��ǰ�÷� 
	struct Record* next;//ָ����һ���浵 
};

/*�Ҷ�Ӧ�����ҵĵ�һ���浵���ҵ�����һ��ָ�룬�Ҳ�������NULL
����modeΪ��Ϸģʽ�ţ�numΪ���id*/ 
struct Record* SearchRecord(int mode, int num); 

/*����浵����ɾ������ҵ����д浵���ٴ��� 
����modeΪ��Ϸģʽ�ţ�pָ��ǰ�����浵*/ 
void AddRecord(int mode, struct Record* p);

/*ɾ���浵,�������Ϊ�û���ţ�ɾ�����д˱�ŵĴ浵*/ 
void DeleteRecord(int mode, int num);






