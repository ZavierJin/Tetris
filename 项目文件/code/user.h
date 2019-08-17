
/***************************************************************
	用户及排行榜 
***************************************************************/

#define MODE_NUM 5//游戏模式总数
 
//用户结构 
struct user{ 
	int num;//用户id 
	char name[16];//用户名 
    int score[MODE_NUM];//用户各模式最高得分 
	struct user *next;//指向下一个用户 
};

//用户登录函数，如果登录成功用指针修改当前玩家信息，失败不修改并返回FALSE 
bool UserLogin(struct user *p,const char* name,struct user *cur_u);
//用户注册函数，如果注册成功在链表末尾追加新用户，并用指针修改当前玩家信息，失败不修改并返回FALSE 
bool UserRegister(struct user *p,const char* name,struct user *cur_u);
//检测字符串是否符合标准，（只含字母，15字节以内） 
bool Inputcheck(const char *s); 

//寻找用户函数，从链表中找到id为num的用户，返回指向用户的指针 
struct user* SearchUser(struct user* head, const int num);
//打开用户文件，数据存入链表，返回头指针。
struct user* OpenUserFile(); 
//将链表数据存入用户文件 
void ModifyUserFile( struct user* head);
//删除链表函数 
void DeleteList(struct user* head);

/*排行榜文件操作函数
参数state为操作类型，0代表读取排行榜，1代表修改排行榜
mode_id为游戏模式类型，name指向10个用户名，score指向十个得分*/ 
void RankingFile(int mode_id, char name[][16], int score[], short state);



