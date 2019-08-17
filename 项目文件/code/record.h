

/***************************************************************
	游戏存档 
***************************************************************/

//单个存档结构
struct Record{ 
	int num;//用户id 
	struct WIN window2[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];//游戏界面单元格 
	SHAPE sh;//当前移动形状 
	int nextid;//下一个形状id 
	int score;//当前得分 
	struct Record* next;//指向下一个存档 
};

/*找对应编号玩家的第一个存档，找到返回一个指针，找不到返回NULL
参数mode为游戏模式号，num为玩家id*/ 
struct Record* SearchRecord(int mode, int num); 

/*存入存档，先删除此玩家的所有存档，再存入 
参数mode为游戏模式号，p指向当前需存入存档*/ 
void AddRecord(int mode, struct Record* p);

/*删除存档,传入参数为用户编号，删除所有此编号的存档*/ 
void DeleteRecord(int mode, int num);






