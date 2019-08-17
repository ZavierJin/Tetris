
/***************************************************************
	�û������а� 
***************************************************************/

#define MODE_NUM 5//��Ϸģʽ����
 
//�û��ṹ 
struct user{ 
	int num;//�û�id 
	char name[16];//�û��� 
    int score[MODE_NUM];//�û���ģʽ��ߵ÷� 
	struct user *next;//ָ����һ���û� 
};

//�û���¼�����������¼�ɹ���ָ���޸ĵ�ǰ�����Ϣ��ʧ�ܲ��޸Ĳ�����FALSE 
bool UserLogin(struct user *p,const char* name,struct user *cur_u);
//�û�ע�ắ�������ע��ɹ�������ĩβ׷�����û�������ָ���޸ĵ�ǰ�����Ϣ��ʧ�ܲ��޸Ĳ�����FALSE 
bool UserRegister(struct user *p,const char* name,struct user *cur_u);
//����ַ����Ƿ���ϱ�׼����ֻ����ĸ��15�ֽ����ڣ� 
bool Inputcheck(const char *s); 

//Ѱ���û����������������ҵ�idΪnum���û�������ָ���û���ָ�� 
struct user* SearchUser(struct user* head, const int num);
//���û��ļ������ݴ�����������ͷָ�롣
struct user* OpenUserFile(); 
//���������ݴ����û��ļ� 
void ModifyUserFile( struct user* head);
//ɾ�������� 
void DeleteList(struct user* head);

/*���а��ļ���������
����stateΪ�������ͣ�0�����ȡ���а�1�����޸����а�
mode_idΪ��Ϸģʽ���ͣ�nameָ��10���û�����scoreָ��ʮ���÷�*/ 
void RankingFile(int mode_id, char name[][16], int score[], short state);



