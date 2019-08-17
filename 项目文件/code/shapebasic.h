
/***************************************************************
	基本形状函数 
***************************************************************/

#define WINDOW_WIDTH 9 //游戏窗口横向单元格个数  
#define WINDOW_HEIGHT 21//游戏窗口纵向单元格个数 


struct WIN window[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];

SHAPE prishape[TOTAL_SHAPE];//预设7种形状 



void InitShape(double gcx, double gcy);//初始化图形函数 

void DrawFrame();//绘制框架函数  

void DrawShape(const SHAPEPTR shapeptr);//绘制图形函数
void DrawBlock(double x, double y, const char* color);//绘制方块函数,参数是窗口界面坐标 
 
void ShapeDrop(SHAPEPTR shapeptr);//图形下落1格函数 
void ShapeMove(SHAPEPTR shapeptr, int dir);//图形左右移动函数，dir：1向右 ，-1向左 
void ShapeSpin(SHAPEPTR shapeptr, int dir);//图形旋转函数，dir：1顺时针 ，-1逆时针
void StraDrop(SHAPEPTR shapeptr);//图形掉落到底函数 
 
int XtransMyX(double x);//坐标转换函数，窗口坐标转换为游戏界面坐标 
int YtransMyY(double y);
double MyXtransX(int mx);//游戏界面坐标转换为窗口坐标 
double MyYtransY(int my);

bool IsBottom(SHAPEPTR shapeptr);//判断图形是否到底函数 

void CleanLine(int k);//清除一行函数，k代表游戏界面y坐标 



