
/***************************************************************
	通用函数及定义类型 
***************************************************************/

#define SCORE_MAX_CAP 5//单词游戏分数最高位数 

#define L 0.328//单元格长度 
#define FRAME_SIZE (0.15*L)//边框宽度 
#define TOTAL_SHAPE 7//初始形状总数 

struct WIN{//游戏总窗口单元格结构 
	bool isoccu;//单元格是否被占用（墙壁一种被占用）
	int type;//方块类型，0为普通，3为生长方块 
	char* color;//颜色 
};

typedef struct{//图形结构 
	int id;//图形编号（0~6）
	char* color;//图形颜色
	int point[4][2];//图形各方块坐标，第一个参数是第几个方块，第二个参数0代表x，1代表y 
	int No;//特殊方块在该形状中的相对位置   -1表示无特殊方块  0-3表示形状的四个位置 
    int  type;
	/*单元格被占用的情况
	0为普通方块未被遍历时的情况 
	1为炸弹方块未被遍历的情况 
	2为引爆的炸弹方块 
	3为生长方块 
	4为冰冻方块 
	5 为）
	*/
}SHAPE;
typedef SHAPE* SHAPEPTR1;//图形结构指针 
typedef SHAPE* SHAPEPTR2;
typedef SHAPE* SHAPEPTR;


//通用函数

int RandomNum(int total);//获得0-total的随机整数 
int RandomId();//获得0~6的随机id
void ScoreTrans(char* ch_score, int score);//得分由整型化为字符，前面补0 
bool InButton(double x,double y, double w, double h, double mx, double my);//判断坐标是否在按钮内


