#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "graphics.h"
#include "genlib.h"
#include "conio.h"

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include <time.h> 
#include <string.h>

#include "imgui.h" //简单交互函数库 

#include "commonuse.h"
#include "user.h"
#include "doublemode.h"
//#include "record.h"

/***************************************************************
	双人模式 
***************************************************************/


/*****************************************/

#define WINDOW_WIDTH_2 18 //游戏窗口横向单元格个数  
#define WINDOW_HEIGHT_2 21//游戏窗口纵向单元格个数 


static struct WIN window[WINDOW_WIDTH_2+2][WINDOW_HEIGHT_2+1];


static SHAPE prishape[2][TOTAL_SHAPE];//预设7种形状


static void InitShape1(double gcx, double gcy);//初始化图形函数 
static void InitShape2(double gcx, double gcy);//初始化图形函数 
static void DrawFrame();//绘制框架函数 



static void DrawShape(const SHAPEPTR shapeptr);//绘制图形函数
static void DrawBlock(double x, double y, const char* color);//绘制方块函数,参数是窗口界面坐标 
 
static void ShapeDrop_2(SHAPEPTR shapeptr1,SHAPEPTR shapeptr2);//图形下落1格函数 
static void ShapeMove_2(SHAPEPTR shapeptr1, int dir,SHAPEPTR shapeptr2);//图形左右移动函数，dir1：1向右 ，-1向左，并判断图形是否与另一个相撞 
static void ShapeSpin_2(SHAPEPTR shapeptr1, int dir1,SHAPEPTR shapeptr2);//图形旋转函数，dir1：1顺时针 ，-1逆时针，并判断是否与另一个相撞 
static bool StraDrop_2(SHAPEPTR shapeptr1,SHAPEPTR shapeptr2);//图形掉落到底函数 
 
static int XtransMyX(double x);//坐标转换函数，窗口坐标转换为游戏界面坐标 
static int YtransMyY(double y);
static double MyXtransX(int mx);//游戏界面坐标转换为窗口坐标 
static double MyYtransY(int my);
static void IsKnock(SHAPEPTR shapeptr);
static bool IsBottom(SHAPEPTR shapeptr);//判断图形是否到底函数 

static void CleanLine(int k);//清除一行函数，k代表游戏界面y坐标 

static void AvoidFric(const SHAPEPTR shapeptr);//避免形状摩擦擦掉方块函数 


/******************************/


//#define VK_P 0x50 

#define DEFAULT_COLOR "Black" //默认颜色

#define TIMER_BLINK_1 1     //1000ms定时器时间标志号
#define TIMER_BLINK_2 2
#define TIMER_BLINK 3       //游戏定时器时间标志号

#define VK_W  0x57
#define VK_A  0x41
#define VK_S  0x53
#define VK_D  0x44 

#define BEGIN_BUTTON 0
#define PAUSE_BUTTON 1
#define RETURN_BUTTON 2 
#define RESTART_BUTTON 3
#define SAVE_BUTTON 4
#define OUT_PAUSE_BUTTON 5

#define POPUP_TOTAL 4 
#define RETURN_POPUP 1 
#define SAVE_TIPS_POPUP 2
#define SAVE_SUCC_POPUP 3
#define RESTART_POPUP 4


static double cx,cy;//窗口中心坐标 

static bool ispop[POPUP_TOTAL+1];//弹窗,0号代表是否有弹窗，1号开始代表1号弹窗 

static int Chap_id = 0;//当前关卡 

static SHAPE temshape1;//移动的形状
static SHAPE temshape2;
static int nextid1 = -1; //下一个形状的id，-1代表未初始化
static int nextid2 = -1; //下一个形状的id，-1代表未初始化

static int sco = 0;//单次分数 
static int self_sco[2]={0,0};//各自得分 

static int drop_id=-1;//当前掉落到底的玩家id，0代表第一个 

static int mseconds=1000; 
static int Chap_sco=5;//目标得分 
static int sum_seconds=120000; //倒计时
 
static const int s_second = 1000;  //1s定时器 


static bool isbegin = FALSE;//是否开始 
static bool ispause = FALSE;//是否暂停 
static bool islose = FALSE;//游戏是否结束 





//外部引用，用于界面跳转，用户存档等 
extern struct user cur_user;
extern int cur_page;
extern InitMyGui();//界面交互开启


static void CloseDoubleMode();//关闭双人模式 

extern double ScaleXInches(int x);//外部引用将鼠标坐标从像素转为英寸的函数 
extern double ScaleYInches(int y);

static void InitDoubleWindow();//初始化游戏窗口函数 
static void MouseEventProcess(int x, int y, int button, int event);//鼠标消息回调函数 
static void TimerEventProcess(int timerID);//定时器消息回调函数
static void KeyboardEventProcess(int key, int event);//键盘消息回调函数
static void RefreshWindow();//刷新游戏窗口函数 
static void DrawNextShape1();//提示下一个形状函数 
static void DrawNextShape2();
static bool IsLose1();//判断游戏是否结束函数 
static bool IsLose2();//时间到了是否到达分数 

static void RefreshChapter();//绘制刷新界面关卡函数 
static void PrintTime();//显示倒计时函数
static void PrintChap();//显示当前关卡 

static void PrintScore();//显示分数函数 
static void PrintHelp();//显示帮助函数
static void PrintTitle();//显示标题 

static void GameBegin(); //游戏开始函数
static void GameEnd(bool finish); //游戏结束函数,finish记录是否完成  

static void MyButton(int id);//绘制按钮函数

static void PopupWindow(int id);//弹窗 

static void UpdateScore();//更新成绩 
static void RankingList();//排行榜更改 
static void PrintRankingList(char name[][16], const int score[],int ra); //绘制排行榜 

static void GrowBlock();//生长方块 
static void BeforeGame();//游戏进入前提示 

static void BeforeGame()
{
	int i; 
	char* tips[7]={
		"紧张又刺激的双人模式",
		"时间、得分、高度等多重挑战", 
		"你们有信心闯到第几关呢",
		"拿起你们的武器\"方块\"，去战斗吧", 
		"操作仅需方向键和WASD键",
		"F1 键暂停游戏", 
		"P.S. 小心那些绿色的家伙",
	};
	PrintTitle();
	
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("Brown");
	for(i=0;i<4;i++){
		drawLabel(cx-1.5, cy+2.0-i*0.5, tips[i]);
	}
	SetPenColor("Blue");
	for(i=4;i<6;i++){
		drawLabel(cx-1.5, cy+2.0-i*0.5, tips[i]);
	}
	SetPenColor("Green");
	drawLabel(cx-1.5, cy+2.0-6*0.5, tips[6]);
	SetPenColor("Black");
	SetPointSize(13);
	SetStyle(0);
}


static void GrowBlock()
{
	int i,j,k;
	int state;
	int root=4;
	int cur_high=0;
	int px, py, x, y;
	int dir[4][2]={{0,1},{1,0},{-1,0},{0,-1}};//0上，1右，2左
	int tem[WINDOW_WIDTH_2+2][WINDOW_HEIGHT_2+1];//暂存之前生长方块位置 
	int vaca = 0;
	
	//侧面生长高度 
	if(Chap_id==0) cur_high=0;
	else if(Chap_id<=1) cur_high=2;
	else if(Chap_id<=2) cur_high=4;
	else if(Chap_id<=3) cur_high=6;
	else if(Chap_id<=5) cur_high=8;
	else cur_high=9;
	
	//暂存 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 3){//生长方块 
				tem[i][j]=1;
			}else
				tem[i][j]=0;
		}
		
	//地面生长 
	root = RandomNum(18)+1;//1-18随机数 
	while(window[root][1].isoccu){
		root = RandomNum(18)+1;
	}
	tem[root][1]=1;
	
	state = RandomNum(4);//4分之一概率长边上 
	//边墙生长
	if(state==0 && cur_high!=0){
		vaca=1;
		x = RandomNum(2);
		if(x==0) x = 18;
		for(j=1;j<=cur_high && window[x][j].isoccu;j++) continue;
		if(j==cur_high+1){//如果长不出 
			x = (x==1)? 18 : 1;//切换 
			for(j=1;j<=cur_high && window[x][j].isoccu;j++) continue;
			if(j==cur_high+1) vaca=0;
		}
		if(vaca){
			root = RandomNum(cur_high)+1;
			while(window[x][root].isoccu){
				root = RandomNum(cur_high)+1;	
			}
			tem[x][root]=1;
		}
	}
	
	//外面生长 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 3){//生长方块
				px = i; py = j;
				for(k=0;k<4;k++){//四个方向 
					
					x = px + dir[k][0];
					y = py + dir[k][1];
					if(!window[x][y].isoccu){//没被占用，生长 
						tem[x][y]=1; 
						break;
					}
				} 
			}
		}
	


	//储存 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(tem[i][j]){//生长方块 
				vaca = 0;//此行还有几个空缺 
				for(k=1;k<WINDOW_WIDTH_2+1;k++){
					if(!window[k][j].isoccu) vaca++;
				}
				if(vaca>1){//空缺多于一个，生长 
					window[i][j].isoccu = TRUE;
					window[i][j].color = "Green";
					window[i][j].type = 3;
				}
			}
		}
	
	RefreshWindow();
}

 
void InitDoubleMode()
{
	int i;
	InitGraphics();
	InitGUI();
	
	
	
	registerMouseEvent(MouseEventProcess);      // 鼠标
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//弹窗初始化 
	
	InitDoubleWindow();
	

	//InitConsole();
	//printf("%.4f,%.4f",cx,cy);
	
}

void CloseDoubleMode()
{
	int i,j;
	//初始化数据，防止遗留 
	nextid1 = -1;
	nextid2 = -1;
	sco = 0;
	isbegin = FALSE;
	ispause = FALSE;
	islose = FALSE;
	
	//初始化窗口，防止游戏遗留 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			if(i==0 || i==WINDOW_WIDTH_2+1 || j==0){//墙壁单元格 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
			window[i][j].type = 0;//普通方块 
		}
	
	GameEnd(FALSE);//关闭计时和键盘函数
	cancelMouseEvent(MouseEventProcess);
	 
}

static void InitDoubleWindow()
{
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape1(cx, cy);//初始化形状 
	InitShape2(cx, cy);
	
	MyButton(BEGIN_BUTTON);
	BeforeGame();
	
}

static void GameBegin()
{
	int i,j;
	
	DisplayClear();

	Chap_id = 0;
	Chap_sco=5;	
	sum_seconds=120000;//初始化倒计时2分钟 
	
	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//注册定时器消息回调函数
	
	startTimer(TIMER_BLINK_1, mseconds);//1000ms定时器触发
	startTimer(TIMER_BLINK, s_second);//1s定时器 
	
	//初始化窗口单元格 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			if(i==0 || i==WINDOW_WIDTH_2+1 || j==0){//墙壁单元格 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
			window[i][j].type = 0;//普通方块 
		}
		
	DrawFrame();
	
	temshape1 = prishape[0][RandomId()];
	DrawShape(&temshape1);
	DrawNextShape1();
	
	temshape2 = prishape[1][RandomId()];
	DrawShape(&temshape2);
	DrawNextShape2();
	
	ispause = FALSE;
	islose = FALSE;
	sco = 0;//单次分数
	self_sco[0] = self_sco[1] = 0;
	
	PrintScore();
	PrintTitle();
	PrintTime();
	PrintHelp();
	PrintChap();
	
	MyButton(OUT_PAUSE_BUTTON); 
	//MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //重新开始按钮
	//InitMyGui();
	//PopupWindow(SAVE_TIPS_POPUP);
}

static void GameEnd(bool finish)
{
	cancelTimerEvent(TimerEventProcess);
	cancelTimer(TIMER_BLINK_1);
	cancelTimer(TIMER_BLINK);
	
	cancelKeyboardEvent(KeyboardEventProcess);
	
	if(finish){
		DisplayClear();
		
		PrintScore();
		PrintTitle();
		PrintChap();
		PrintTime();
		PrintHelp();
		
		MyButton(RETURN_BUTTON);
		MyButton(RESTART_BUTTON); //重新开始按钮
		
		UpdateScore();
		RankingList();
		/*
		SetEraseMode(TRUE); //擦掉外面的暂停和存档按钮 
		drawRectangle(cx-4.5-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//防止擦不干净（w,h已经直接带入） 
		drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//防止擦不干净 
		SetEraseMode(FALSE);
		*/
		 
	}
} 

static void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;//当前鼠标坐标
	
	double h = 0.5;  // 控件高度
	double w = 1.0;

	
	mx = ScaleXInches(x);//像素转英寸，数据类型int转double 
	my = ScaleYInches(y);
	//uiGetMouse(x,y,button,event); //GUI获取鼠标
	switch(event){
		case BUTTON_DOWN://如果鼠标被按下 
			if(button==LEFT_BUTTON){
				//弹窗状态 
				if(ispop[0]){
					if(ispop[1]){//1号弹窗，返回 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//是 
							ispop[1] = FALSE;
							ispop[0] = FALSE;
							CloseDoubleMode();//退出游戏模式 
							InitMyGui();
							cur_page = 3;//跳转
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//否 
							ispop[1] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							 
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}/*
					else if(ispop[2]){//2号弹窗，存档提示 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//是 
							SaveRecord();//存档 
							ispop[2] = FALSE;
							
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							
							ispop[3] = TRUE; //存档成功弹窗 
							PopupWindow(SAVE_SUCC_POPUP);
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//否 
							ispop[2] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}
					}
					else if(ispop[3]){//3号弹窗，存档成功 
						if(InButton(cx-1.4, cy-0.4, 1.0, 0.4, mx, my)){//继续 
							ispop[3] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.61, 4.1, 1.62, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}else if(InButton(cx+0.4, cy-0.4, 1.0, 0.4, mx, my)){//退出 
							ispop[3] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();//退出游戏 
							InitMyGui();
							cur_page = 3;//跳转
						}
					}*/
					else if(ispop[4]){//4号弹窗，重新开始 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//是 
							ispop[4] = FALSE;
							ispop[0] = FALSE;
							GameEnd(FALSE); 
							GameBegin();
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//否 
							ispop[4] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							 
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}
				}
				//未弹窗状态 
				else{
					if(!isbegin){ //未开始状态
						if(!islose){//第一次进入 
							if(InButton(cx-w/2.0, cy-3.0, w, h, mx, my) ) {//开始按钮 
								GameBegin();
								isbegin = TRUE;//开始状态切换
							}
						}else{
							if(InButton(cx+3.4, cy-3.0, w, h, mx, my) ) {//重新开始按钮 
								GameBegin();
								isbegin = TRUE;//开始状态切换
							}
							if(InButton(cx-4.5, cy-3.0, w, h, mx, my)){//退出按钮，经典模式游戏
								CloseClassicMode();//退出游戏模式 
								InitMyGui();
								cur_page = 3;//跳转
							}
						}
					}else{//开始状态
						if(InButton(cx-4.5, cy-3.0, w, h, mx, my)){//退出按钮，经典模式游戏
							ispause = TRUE;
							PopupWindow(RETURN_POPUP);
							ispop[1] = TRUE;
							ispop[0] = TRUE;
						}
						if(InButton(cx+3.4, cy-2.3, w, h, mx, my) && !ispause){//暂停按钮
							ispause = TRUE; 
							MyButton(PAUSE_BUTTON);//继续按钮 
							SetEraseMode(TRUE); //擦掉外面的暂停按钮 
							drawRectangle(cx+3.4-0.01, cy-2.3-0.01, w+0.02, h+0.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
						}/*
						if(InButton(cx-4.5, cy-2.3, w, h, mx, my)){//存档按钮,会暂停 
							ispause = TRUE;
							PopupWindow(SAVE_TIPS_POPUP);
							ispop[2] = TRUE;
							ispop[0] = TRUE;	
						}*/
						if(InButton(cx+3.4, cy-3.0, w, h, mx, my) ) {//重新开始按钮 
							ispause = TRUE;
							PopupWindow(RESTART_POPUP);
							ispop[4] = TRUE;
							ispop[0] = TRUE;
						}
					}
					
					if(ispause) {//暂停状态 
						if(InButton(cx-w/2.0, cy, w, h, mx, my) ) {//继续按钮 
							RefreshWindow();
							ispause = FALSE;//暂停状态切换
							MyButton(OUT_PAUSE_BUTTON); 
						} 
					}	
				}
						
				
			}
			break; 
	}
}


static void KeyboardEventProcess(int key, int event)
{
	if(islose) return; 
	//响应暂停切换按键
	if(event==KEY_DOWN && key==VK_F1 && !ispop[0]) {
		ispause = !ispause;//暂停状态切换 
		if(ispause){
			MyButton(PAUSE_BUTTON);
			SetEraseMode(TRUE); //擦掉外面的暂停按钮 
			drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//防止擦不干净（w,h已经直接带入） 
			SetEraseMode(FALSE);
		}else{
			RefreshWindow();
			MyButton(OUT_PAUSE_BUTTON); 
		}
	} 
		
	if(!ispause){//如果未暂停和结束，响应游戏按键 
		switch(event){
			case KEY_DOWN:
				switch(key){
					case VK_UP:
						ShapeSpin_2(&temshape1,1,&temshape2);
						break;
					case VK_LEFT:
						ShapeMove_2(&temshape1,-1,&temshape2);
						break;
					case VK_RIGHT:
						ShapeMove_2(&temshape1,1,&temshape2);
						break;
					case VK_DOWN: 
						if(StraDrop_2(&temshape1,&temshape2)){
							temshape1 = prishape[0][nextid1];//重新获得随机图形 
							DrawNextShape1();
							drop_id = 0;
							RefreshWindow();
						}
						break;
						
					case VK_W:
						ShapeSpin_2(&temshape2,1,&temshape1);
						break;
					case VK_A:
						ShapeMove_2(&temshape2,-1,&temshape1);
						break;
					case VK_D:
						ShapeMove_2(&temshape2,1,&temshape1);
						break;
					case VK_S: 
						if(StraDrop_2(&temshape2,&temshape1)){
							temshape2 = prishape[1][nextid2];//重新获得随机图形 
							DrawNextShape2();
							drop_id = 1;
							RefreshWindow();
						}
						break;
				}
				break;
		}
	}
}

static void TimerEventProcess(int timerID)
{
	bool b1,b2;
	if(!isbegin || islose || ispause) return;
	
	switch(timerID){
		case TIMER_BLINK_1://下坠倒计时 
			if(!IsLose1()){//没到顶 
				b1 = IsBottom(&temshape1);
				b2 = IsBottom(&temshape2);
				
				if(!b1){
					ShapeDrop_2(&temshape1,&temshape2);
				}else{
					drop_id = 0;
					temshape1 = prishape[0][nextid1];
					DrawNextShape1();
					RefreshWindow();//?
				}
				
				if(!b2){
					ShapeDrop_2(&temshape2,&temshape1);
				}else{
					drop_id =1;
					temshape2 = prishape[1][nextid2];
					DrawNextShape2();
					RefreshWindow();//?
				}
			} 
			break;
			
		case TIMER_BLINK://1秒，倒计时刷新 
			if(sum_seconds>0){//如果时间没到 
				if((sum_seconds+2000)%5000 == 0) //5秒生长方块生长一次 
					GrowBlock();
				sum_seconds-=1000;	//减一秒 
				PrintTime();
			}else{//如果时间到了 
				if(!IsLose2()){//判断是否到达分数 
					RefreshChapter();
				}else{//输了 
					ispause = TRUE;
				}
			}
			break;
		
	}
}

static void RefreshWindow()
{
	int i,j;
	bool isclean;
	int conclean = 0;//连续清除几行 
	int c_sco=0;//单次刷新得分
	 
	//清除游戏界面 
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");	
		}
	PrintScore();
	//判断是否清除一行 
	for(j=1;j<WINDOW_HEIGHT_2+1;j++){
		isclean=TRUE;
		for(i=1;i<WINDOW_WIDTH_2+1;i++){
			if(!window[i][j].isoccu){//如果这一行有一个单元格为被占用，则此行不需要清除 
				isclean=FALSE;
				break;
			}
		}
		if(isclean){
			CleanLine(j);
			j--; //因为清除了之后上面一行下移了，所以此行要重新判断 
			conclean += 1;
		}else if(conclean!=0){
			c_sco += 2 * conclean - 1; 
			conclean = 0;
		}
	}
	sco+=c_sco;
	self_sco[drop_id]+=c_sco;
	drop_id=-1;//？
	
	//重新绘制游戏界面 
	SetEraseMode(FALSE);
	DrawFrame();
	PrintScore();
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].isoccu)
				DrawBlock(MyXtransX(i),MyYtransY(j),window[i][j].color);
		}
	DrawShape(&temshape1);
	DrawShape(&temshape2);
	
	PrintTime();
}

static void DrawNextShape1()
{
	SHAPEPTR shapeptr1;
	int i;
	if(nextid1!=-1){//如果之前有画过，先擦除上一个形状 
		SetEraseMode(TRUE);
		shapeptr1 = &prishape[0][nextid1];
		for(i=0;i<4;i++)
			DrawBlock(MyXtransX(shapeptr1->point[i][0])+3.4,MyYtransY(shapeptr1->point[i][1])-0.4,shapeptr1->color);
	}
	 
	SetEraseMode(FALSE);
	nextid1 = RandomId();
	shapeptr1 = &prishape[0][nextid1];
	for(i=0;i<4;i++)
		DrawBlock(MyXtransX(shapeptr1->point[i][0])+3.4,MyYtransY(shapeptr1->point[i][1])-0.4,shapeptr1->color);
		
	SetStyle(1);
	drawLabel(cx+3.2, cy+2.3, "下一个：");
	SetStyle(0);

}	

static void DrawNextShape2()
{
	SHAPEPTR shapeptr2;
	int i;
	if(nextid2!=-1){//如果之前有画过，先擦除上一个形状 
		SetEraseMode(TRUE);
		shapeptr2 = &prishape[1][nextid2];
		for(i=0;i<4;i++)
			DrawBlock(MyXtransX(shapeptr2->point[i][0])-2.7,MyYtransY(shapeptr2->point[i][1])-0.4,shapeptr2->color);
	}
	 
	SetEraseMode(FALSE);
	nextid2 = RandomId();
	shapeptr2 = &prishape[1][nextid2];
	for(i=0;i<4;i++)
		DrawBlock(MyXtransX(shapeptr2->point[i][0])-2.7,MyYtransY(shapeptr2->point[i][1])-0.4,shapeptr2->color);
	
	SetStyle(1);
	drawLabel(cx-4.8, cy+2.3, "下一个：");
	SetStyle(0);
}	


static bool IsLose1()
{
	int i;
	
	
	//判断窗口单元格是否超出所画线，超出则游戏结束 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		if(window[i][WINDOW_HEIGHT_2-4].isoccu) {
			islose = TRUE;
			break;
		}
	

	if(islose){
		GameEnd(TRUE); 
		
		SetPenColor("Red");
		drawLabel(cx-0.45, cy+3.1, "游 戏 结 束");
		
		isbegin = FALSE;
		
		return TRUE;
	}else 
		return FALSE;//返回是否结束 
}
static bool IsLose2()
{
	
	int i;
	
	if(sco< Chap_sco){//未达到目标分 
		islose = TRUE;
	}

	if(islose){
		GameEnd(TRUE); 
		
			
		SetPenColor("Red");
		drawLabel(cx-0.45, cy+3.1, "游 戏 结 束");
		
		isbegin = FALSE;
		
		
		return TRUE;
	}else 
		return FALSE;//返回是否结束 
}

static void PrintTitle()
{
	
	SetStyle(1);
	SetPointSize(26);
	drawLabel(cx-4.8, cy+3.0, "双人模式");
	SetPointSize(13);
	SetStyle(0);
}

static void PrintScore()
{
	
	char self[50];
	char total[50];
	static char score[SCORE_MAX_CAP+1];
	int temsco = sco;
	int i;
	
	SetStyle(1);
	if(sco>=Chap_sco) SetPenColor("Red"); 
	strcpy(total,"总得分：");
	ScoreTrans(score,temsco);
	strcat(total,score);
	drawLabel(cx+3.2, cy-0.8, total);
	if(sco>=Chap_sco) SetPenColor("Black"); 
	
	strcpy(self,"P1 得分："); 
	ScoreTrans(score,self_sco[0]);
	strcat(self,score);
	drawLabel(cx+3.2, cy-1.2, self);
	
	strcpy(self,"P2 得分："); 
	ScoreTrans(score,self_sco[1]);
	strcat(self,score);
	drawLabel(cx+3.2, cy-1.5, self);
	
	SetStyle(0);
	
	
}

static void PrintTime()
{
	char memo[38]="倒计时：";
	static char sum_sec[10];
	itoa(sum_seconds/1000,sum_sec,10);
	strcat(memo,sum_sec);
	strcat(memo," 秒");
	
	SetEraseMode(TRUE);
	drawRectangle(cx+3.2-0.01,cy+0.3-0.03,1.5,0.25+0.04,1);
	drawRectangle(cx-0.7-0.01,cy+3.2-0.03,1.5,0.22+0.04,1);
	SetEraseMode(FALSE); 
	
	if(sum_seconds<10000) SetPenColor("Red");//小于10秒变红 
	SetStyle(1);
	drawLabel(cx-0.7,cy+3.2,memo);//顶部提示 
	drawLabel(cx+3.2,cy+0.3,memo);
	SetStyle(0);
	if(sum_seconds<10000) SetPenColor("Black");
}

static void PrintHelp()
{
	int i;
	char *helptips[3] = {
		"P1：方向键控制",
		"P2：WASD控制",
		"| F1键 | 暂停" 
	};
	SetStyle(1);
	drawLabel(cx-4.8, cy+0.75, "操作指南：");
	SetStyle(0);
	for(i=0;i<3;i++){
		drawLabel(cx-4.7, cy+0.45-i*0.3, helptips[i]);
	}
}





//绘制或擦除按钮 
static void MyButton(int id)
{
	double h = 0.5;  // 控件高度
	double w = 1.0;
	char label_color[]= "White";
	char button_color[]="Dark Gray";
	char* button_label = NULL;//当前按钮标签 
	
	SetPenColor(button_color);
	switch(id){
		case BEGIN_BUTTON:
			drawBox( cx-w/2.0, cy-3.0, w, h, 1, "开 始", 'Z', label_color);
			break;
		case PAUSE_BUTTON:
			drawBox( cx-w/2.0, cy, w, h, 1, "继 续", 'Z', label_color);
			break;
		case OUT_PAUSE_BUTTON:
			drawBox( cx+3.4, cy-2.3, w, h, 1, "暂 停", 'Z', label_color);
			break;
		case SAVE_BUTTON:
			drawBox( cx-4.5, cy-2.3, w, h, 1, "存 档", 'Z', label_color);
			break;
		case RETURN_BUTTON:
			drawBox( cx-4.5, cy-3.0, w, h, 1, "返 回", 'Z', label_color);
			break;
		case RESTART_BUTTON:
			drawBox( cx+3.4, cy-3.0, w, h, 1, "重新开始", 'Z', label_color);
			break;
	}
	SetPenColor(DEFAULT_COLOR); //将画笔重新设为默认颜色 
}
//更新用户成绩 
static void UpdateScore()
{
	struct user *head, *p, *pnext;;
	bool needmo = FALSE;
	head = p = pnext = NULL;
	
	if(cur_user.score[3]<sco){ //比较是否超过当前模式最佳成绩 
		cur_user.score[3] = sco;
		needmo = TRUE;
		head = OpenUserFile();
		if((p=SearchUser(head,cur_user.num))!=NULL){
			pnext = p->next;//与之后节点相连 
			*p=cur_user;
			p->next = pnext;
		}
		ModifyUserFile(head);
		DeleteList(head);
	}
	
}

static void RankingList()
{
	bool ismod = FALSE;//是否需要修改排行榜 
	bool check = FALSE;//是否进入排行榜 
	char name[10][16];
	int score[10];
	int cur_sco;
	int rank=-1;//在排行榜中排名 
	int i;
	
	
	for(i=0;i<10;i++) score[i]=0;
	
	RankingFile(3,name,score,0);//读入排行榜
	cur_sco = cur_user.score[3]; 
	for(i=0;i<10;i++){
		if(strcmp(name[i],cur_user.name)==0){
			if(score[i]<cur_sco){
				ismod = TRUE;
			}
			rank = i;
			check = TRUE;//已找到名字 
			break;
		} 
	}
	if(!check){//如果排行榜没有此名字，从后向前比 
		for(i=9;i>=0 && score[i]<cur_sco;i--)
			continue;
		rank=i+1;
		if(rank!=10){
			check = TRUE;//进入排行榜 
			ismod = TRUE;
			for(i=8;i>=rank;i--){
				strcpy(name[i+1],name[i]);
				score[i+1]=score[i];
			}
			strcpy(name[rank],cur_user.name);
			score[rank]=cur_sco;
		}
	}else if(ismod){//排行榜有名字，并且比之前高，从当前位置继续向前比 
		for(i=rank-1;i>=0 && score[i]<cur_sco;i--){
			strcpy(name[i+1],name[i]);
			score[i+1]=score[i];
		}
		strcpy(name[i+1],cur_user.name);
		score[i+1]=cur_sco;
		rank = i+1;
	}
	
	if(!check) rank = -1;
	if(ismod) RankingFile(3,name,score,1);//修改排行榜 
	
	PrintRankingList(name,score,rank);//展示排行榜 
}

static void PrintRankingList(char name[][16], const int score[],int ra)
{
	char* title = "排 行 榜";
	char* note1 = "名 次"; 
	char* note2 = "用 户 名";
	char* note3 = "得 分";
	char rank[4];
	double cex, cey;//排行榜中心坐标 
	double x, y;
	double w, h;//宽，高 
	double th, tj;//行宽,间距, 
	double w1, w2 ,w3;
	int i,j; 
	bool full;//排行榜是否满 
	char ch_score[SCORE_MAX_CAP+1];
	int temsco;
	
	
	cex = cx; cey = cy;
	w = 4.0;  h = 5.8; 
	th = 0.35 ; tj = 0.2;
	w1 = 0.9; w2 = 1.8; w3 = w-2*tj-w1-w2;
	
	//擦除部分界面 
	SetPenColor("White");
	drawRectangle(cx-2.0,cy-3.5,w,7.0,1);

	//画排行榜底色 
	SetPenColor("Gray");
	drawRectangle(cex-w/2.0,cey-h/2.0,w,h,1);
	//画标题 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cex-w/2.0+tj,cey+h/2.0-2*(tj+th),w-tj*2,2*th,1,title,'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	//画小标题 
	SetPenColor("White");
	drawBox(cex-w/2.0+tj,cey+h/2.0-3*(tj+th),w1,th,1,note1,'Z',"Black");
	SetPenColor("White");
	drawBox(cex-w/2.0+tj+w1,cey+h/2.0-3*(tj+th),w2,th,1,note2,'Z',"Black");
	SetPenColor("White");
	drawBox(cex-w/2.0+tj+w1+w2,cey+h/2.0-3*(tj+th),w3,th,1,note3,'Z',"Black");
	//输出排名 
	x = cex-w/2.0+tj;
	y = cey+h/2.0-4*(tj+th);
	full = TRUE; 
	for(i=0;i<10;i++){
		if(strcmp(name[i],"0")==0){
			full = FALSE;//排行榜未满 
			break;	
		}
		
		if(i!=9){
			rank[0]=i+1+'0';
			rank[1]='\0';
		}else strcpy(rank,"10");
		//得分由整型化为字符，前面补0 
		temsco = score[i];
		for(j=SCORE_MAX_CAP-1;j>=0;j--){
			ch_score[j] = temsco % 10 + '0';
			temsco/=10;
		}
		ch_score[SCORE_MAX_CAP]='\0';
		//名次 
		SetPenColor("White");
		drawBox(x,y-i*th,w1,th,1,rank,'Z',"Black");
		//用户名、得分 
		SetPenColor("White");
		drawBox(x+w1,y-i*th,w2,th,1,name[i],'Z',"Black");
		SetPenColor("White");
		drawBox(x+w1+w2,y-i*th,w3,th,1,ch_score,'Z',"Black");	
		
		
		
	}
	if(!full){//如果排行榜未满，补空白 
		SetPenColor("White");
		drawRectangle(x,y-9*th,w-2*tj,(10-i)*th,1);
	}
	
	if(ra!=-1){//红框标记在排行榜中的位置 
		SetPenColor("Red");
		drawRectangle(x,y-ra*th,w-2*tj,th,0);
	}
	
	
	SetPenColor(DEFAULT_COLOR);
}
/*
static void SaveRecord()
{
	struct Record cur_rec;
	struct Record *head=NULL;
	int i,j;
	
	cur_rec.num = cur_user.num;//用户编号 
	cur_rec.score = sco;//当前得分
	
	//窗口存档 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			cur_rec.window2[i][j]=window[i][j];
		}
	
	cur_rec.sh = temshape;//当前形状
	cur_rec.nextid = nextid;//下一个形状编号
	
	cur_rec.next = NULL;
	
	AddRecord(cur_state-1, &cur_rec);//加入新存档

}
*/
static void PopupWindow(int id)
{
	double w, h;//宽，高 
	double th, tj;//行宽,间距
	double x, y;//输出起始位置 

	th = 0.4 ; tj = 0.2;

	switch(id){
		case RETURN_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//画文本//前打8个空格空两个汉字，18个汉字一行 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"警告！！！",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"当前游戏还未保存，确定要结束游戏吗？",'L',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," 是 ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," 否 ",'Z',"White");
			break;
		case SAVE_TIPS_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//画文本//前打8个空格空两个汉字，18个汉字一行 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"警告！！！",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"此操作将会覆盖原有存档，是否继续？",'L',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," 是 ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," 否 ",'Z',"White");
			break;
		case SAVE_SUCC_POPUP:
			w = 4.05; h = 1.6;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//画文本//前打8个空格空两个汉字，18个汉字一行 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"  ""存档成功！",'Z',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.4,y,1.0,th,1,"继续",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,1.0,th,1,"退出",'Z',"White");
			break;
		case RESTART_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//画文本//前打8个空格空两个汉字，18个汉字一行 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"警告！！！",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"当前游戏还未保存，确定要重新开始吗？",'L',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," 是 ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," 否 ",'Z',"White");
			break;
	}
	
	SetPenColor(DEFAULT_COLOR);
}

static void PrintChap()
{
	char chap1[35];
	char c_id[20];
	char c_sco[20];
	char memo[35]="目标分："; 

	
	strcpy(chap1,"关卡："); 
	itoa(Chap_id+1,c_id,10);
	itoa(Chap_sco,c_sco,10);
	strcat(chap1,c_id);
	strcat(memo,c_sco);
	
	
	SetEraseMode(TRUE);
	drawRectangle(cx+3.2-0.01,cy-0.01,1.0,0.26,1);
	SetEraseMode(FALSE); 
	
	SetPointSize(16);
	SetPenColor("Red");
	drawBox(cx+3.2,cy+0.6,1.0,0.4,1,chap1,'Z',"Black");
	SetPointSize(13);
	
	SetStyle(1);
	drawLabel(cx+3.2,cy,memo);
	SetStyle(0);
	
	
	if(!islose){//中间弹出第几关
		SetPointSize(16); 
		SetPenColor("Red");
		drawBox(cx-0.5,cy-0.1,1.0,0.4,1,chap1,'Z',"Black");
		Pause(1.0); 
		SetEraseMode(TRUE);
		drawRectangle(cx-0.5-0.01,cy-0.1-0.01,1.0+0.02,0.4+0.02,1);
		SetEraseMode(FALSE);
		SetPointSize(13);
	}
	
	
	SetPenColor(DEFAULT_COLOR); 
	//drawBox( cx+4, cy-2.5, 0.2, h, 1, c_id, 'L', label_color);
	
}

static void RefreshChapter()
{

	int i, j;
	
	cancelTimer(TIMER_BLINK_1);
	
	Chap_id++;//关卡 
		
	sum_seconds=120000;//倒计时重置 
	
	if(Chap_id<=5) //下落速度 
		mseconds=1000-Chap_id*100;
	else
		mseconds=2500/Chap_id;

	Chap_sco = Chap_id*(2*Chap_id+7) +5;//目标分二次增长 
	
	startTimer(TIMER_BLINK_1, mseconds);
	

	//初始化窗口单元格 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 0){//不是生长方块 
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}
		}
	
	
	temshape1 = prishape[0][RandomId()];
	temshape2 = prishape[1][RandomId()];
	RefreshWindow();
	
	PrintChap();

}




/**************************************
防止和经典模式函数名不兼容，直接粘入
**************************************/ 

static void InitShape1(double gcx, double gcy)
{
	int i;
	
	srand((unsigned int) time(0));//根据系统时间重置随机种子
	
	cx = gcx;
	cy = gcy;
	
	
	for(i=0;i<TOTAL_SHAPE;i++){
		prishape[0][i].id = i;
		switch(i){
			case 0:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-4;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].color="Blue";
				break;
			case 1:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-3;//旋转中心	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-2;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].color="Blue";
				break;
			case 2:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2 - 1; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].color="BLue";
				break;
			case 3:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-4;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2 - 1; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].color="Blue";
				break;
			case 4:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2 - 1; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2 + 1; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].color="Blue";
				break;
			case 5:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2 - 1; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-2;
				prishape[0][i].color="Blue";
				break;
			case 6:
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-2;	
				prishape[0][i].point[1][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[0][i].point[2][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[0][i].point[3][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[3][1]=WINDOW_HEIGHT_2-1;
				prishape[0][i].color="Blue";
				break;
		}
	}
}

static void InitShape2(double gcx, double gcy)
{
	int i;
	
	srand((unsigned int) time(0));//根据系统时间重置随机种子
	
	cx = gcx;
	cy = gcy;
	
	
	for(i=0;i<TOTAL_SHAPE;i++){
		prishape[1][i].id = i;
		switch(i){
			case 0:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-4;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].color="Yellow";
				break;
			case 1:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-3;//旋转中心	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-2;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].color="Yellow";
				break;
			case 2:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2 - 1; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].color="Yellow";
				break;
			case 3:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-4;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2 - 1; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].color="Yellow";
				break;
			case 4:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2 - 1; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2 + 1; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].color="Yellow";
				break;
			case 5:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-3;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2 - 1; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-2;
				prishape[1][i].color="Yellow";
				break;
			case 6:
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-2;	
				prishape[1][i].point[1][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[1][1]=WINDOW_HEIGHT_2-3;
				prishape[1][i].point[2][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[2][1]=WINDOW_HEIGHT_2-4;
				prishape[1][i].point[3][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[3][1]=WINDOW_HEIGHT_2-1;
				prishape[1][i].color="Yellow";
				break;
		}
	}
}
static int XtransMyX(double x)
{
	int i;
	if(x+0.001<cx-(WINDOW_WIDTH_2+1)/2.0*L) return -1;
	for(i=1;i<WINDOW_WIDTH_2+3;i++){
		if(x+0.01 < i*L + cx-(WINDOW_WIDTH_2+1)/2.0*L) return i-1;
	}
	return -1;//非正常返回 
}

static int YtransMyY(double y)
{
	int i;
	if(y+0.001 < cy-(WINDOW_HEIGHT_2-1)/2.0*L) return 0;
	for(i=1;i<(WINDOW_HEIGHT_2+1);i++){
		if(y+0.01 < i*L + (cy-(WINDOW_HEIGHT_2-1)/2.0*L)) return i;
	}
	return -1;
}

static double MyXtransX(int mx)
{
	if(mx>=0 && mx<=(WINDOW_WIDTH_2+1)) return cx-(WINDOW_WIDTH_2+1)/2.0*L+mx*L;
	else return -0.1;
}


static double MyYtransY(int my)
{
	if(my>=1 && my<=(WINDOW_HEIGHT_2)) return cy-(WINDOW_HEIGHT_2-1)/2.0*L+(my-1)*L;
	else return -0.1;
}

static void DrawFrame()
{
	SetPenColor("black");
	SetPenSize(4);//边框粗细0.1*L 
	
	MovePen(MyXtransX(1)-0.5*L-FRAME_SIZE/3.0, MyYtransY(1)-0.5*L-FRAME_SIZE/3.0*2.0);
	DrawLine(0.0, (WINDOW_HEIGHT_2)*1.0*L+FRAME_SIZE);
	DrawLine((WINDOW_WIDTH_2)*1.0*L+FRAME_SIZE, 0.0);
	DrawLine(0.0, -(WINDOW_HEIGHT_2)*1.0*L-FRAME_SIZE);
	DrawLine(-(WINDOW_WIDTH_2)*1.0*L-FRAME_SIZE, 0.0);
	
	SetPenSize(1);
	MovePen(MyXtransX(1)-0.5*L-FRAME_SIZE/3.0, MyYtransY(WINDOW_HEIGHT_2-4)-0.5*L);
	DrawLine((WINDOW_WIDTH_2)*1.0*L+FRAME_SIZE, 0.0);
} 

static void DrawBlock(double x, double y, const char* color)
{
	SetPenColor(color);
	SetPenSize(1);
	drawRectangle(x-0.5*L,y-0.5*L,L,L,1);
	
	SetPenColor("Black");
	SetPenSize(1);
	MovePen(x-0.5*L,y-0.5*L);
	DrawLine(0.0,L);
	DrawLine(L,0.0);
	DrawLine(0.0,-L);
	DrawLine(-L,0.0);
	
}


static void DrawShape(const SHAPEPTR shapeptr)
{
	int i;
	for(i=0;i<4;i++)
		DrawBlock(MyXtransX(shapeptr->point[i][0]),MyYtransY(shapeptr->point[i][1]),shapeptr->color);
}

static void ShapeDrop_2(SHAPEPTR shapeptr1,SHAPEPTR shapeptr2)
{
	SHAPE preshape = *shapeptr1;
	bool candrop = TRUE;
	
	SetEraseMode(TRUE);
	DrawShape(shapeptr1);
	DrawShape(shapeptr2);
		
	int i,j;
	for(i=0;i<4;i++)
		shapeptr1->point[i][1]--;
	//判断下落后是否符合要求  
	for(i=0;i<4;i++){
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) candrop = FALSE;//防止和原来的背景碰 
	}
	if(!candrop) *shapeptr1 = preshape;//如果不能下落，则重新读取图形之前的位置 
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//防止摩擦擦除背景方块边框
	DrawShape(shapeptr1);
	DrawShape(shapeptr2);
	
}

static void ShapeMove_2(SHAPEPTR shapeptr1, int dir1, SHAPEPTR shapeptr2)
{
	SHAPE preshape = *shapeptr1;
	bool canmove = TRUE;
	int i,j;
	 
	SetEraseMode(TRUE);
	DrawShape(shapeptr1);
	//DrawShape(shapeptr2);
	
	for(i=0;i<4;i++){
		shapeptr1->point[i][0]+=dir1;
		
	}
	//穿墙 
	IsKnock(shapeptr1);
	for(i=0;i<4;i++){
		if(!canmove) break;
		for(j=0;j<4;j++){//防止两个碰 
			if(shapeptr1->point[i][0]==shapeptr2->point[j][0]&&shapeptr1->point[i][1]==shapeptr2->point[j][1]){
				canmove=FALSE;
				break;
			}
		
				
		}
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) canmove = FALSE;//防止和原来的背景碰 
	}
	

	if(!canmove){
		*shapeptr1 = preshape;
	}
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//防止摩擦擦除背景方块边框 
	DrawShape(shapeptr1);
	DrawShape(shapeptr2);
}
static void IsKnock(SHAPEPTR shapeptr)
{
	int i=0,j;
	int Max_X,Min_X;
	int width;
	Max_X=Min_X=shapeptr->point[i][0];
	for(i=1;i<4;i++){
		if(shapeptr->point[i][0]>Max_X)
			Max_X=shapeptr->point[i][0];
		if(shapeptr->point[i][0]<Min_X)
			Min_X=shapeptr->point[i][0];
	}
	width=Max_X-Min_X;
	for(i=0;i<4;i++){
		if(shapeptr->point[i][0]==WINDOW_WIDTH_2+1) {
			for(j=0;j<4;j++){
				shapeptr->point[j][0]=shapeptr->point[j][0]-Min_X+1;
			}
		}
		else if(shapeptr->point[i][0]==0)
			for(j=0;j<4;j++){
				shapeptr->point[j][0]=WINDOW_WIDTH_2-Max_X+shapeptr->point[j][0];
			}
	}
}
static void ShapeSpin_2(SHAPEPTR shapeptr1, int dir1,SHAPEPTR shapeptr2)
{
	SHAPE preshape = *shapeptr1;
	bool canspin = TRUE;
	int rex,rey;
	int i,j;
	
	SetEraseMode(TRUE);
	DrawShape(shapeptr1);
	
	if(shapeptr1->id==0) canspin = FALSE;//编号为0的图形无法旋转 
	if(canspin){
		for(i=1;i<4;i++){
			rex = shapeptr1->point[i][0]-shapeptr1->point[0][0];//图形各单元格相对于旋转中心单元格（第0号）的坐标 
			rey = shapeptr1->point[i][1]-shapeptr1->point[0][1];
			switch(dir1){//判断顺时针还是逆时针 
				case 1://顺时针 
					shapeptr1->point[i][0]=shapeptr1->point[0][0] + rey;
					shapeptr1->point[i][1]=shapeptr1->point[0][1] - rex;
					break;
				case -1:
					shapeptr1->point[i][0]=shapeptr1->point[0][0] - rey;
					shapeptr1->point[i][1]=shapeptr1->point[0][1] + rex;
					break;
			}
		}
	}
	
	for(i=0;i<4;i++){
		if(!canspin) break;
		for(j=0;j<4;j++){//防止两个碰 
			if(shapeptr1->point[i][0]==shapeptr2->point[j][0]&&shapeptr1->point[i][1]==shapeptr2->point[j][1]){
				canspin=FALSE;
				break;
			}
		
				
		}
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) canspin = FALSE;//防止和原来的背景碰 
	}
	
	if(!canspin) *shapeptr1 = preshape;
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//防止摩擦擦除背景方块边框
	DrawShape(shapeptr1);
	DrawShape(shapeptr2);
} 

static bool StraDrop_2(SHAPEPTR shapeptr1, SHAPEPTR shapeptr2)
{
	SHAPE preshape = *shapeptr1;
	int dropdis;//下落到底距离 
	int px,py;//图形中一个单元格的坐标 
	int i,j;
	bool candrop = TRUE;
	dropdis=WINDOW_HEIGHT_2;
	for(i=0;i<4 && candrop;i++){
		px=shapeptr1->point[i][0];
		py=shapeptr1->point[i][1];
		for(j=0;j<4;j++){//防止两个碰 
			if(shapeptr2->point[j][0]==px && shapeptr2->point[j][1]<=py){
				candrop=FALSE;
				break;
			}
		}
		for(j=py-1;j>=0;j--){
			if(window[px][j].isoccu){
				if(py-j-1 < dropdis) dropdis = py - j - 1;
				break;
			}
		}
	}

	for(i=0;i<4;i++){
		shapeptr1->point[i][1]-=dropdis;
	}
	if(!candrop) *shapeptr1 = preshape;//如果不能下落，则重新读取图形之前的位置
	else{
		//到底之后更新窗口单元格 
		for(i=0;i<4;i++){
			window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu = TRUE;
			window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].color = shapeptr1->color;
		}
	}
	
	return candrop;
}

static bool IsBottom(SHAPEPTR shapeptr)
{
	bool isbot=FALSE;
	int i;
	for(i=0;i<4;i++){//如果图形下方的窗口单元格被占用，则已经到底 
		if(window[shapeptr->point[i][0]][shapeptr->point[i][1]-1].isoccu) isbot = TRUE;
	}
	if(isbot){
		for(i=0;i<4;i++){
			window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu = TRUE;
			window[shapeptr->point[i][0]][shapeptr->point[i][1]].color = shapeptr->color;
		}
		
	}
	return isbot;//返回是否到底 
}


static void CleanLine(int k)
{
	int i,j;
	//清除此行，即将上面的行都下移（最顶部的一行保证全是空白） 
	for(j=k;j<WINDOW_HEIGHT_2;j++){
		for(i=1;i<WINDOW_WIDTH_2+1;i++) 
			window[i][j]=window[i][j+1];
	}
}

//避免形状摩擦擦掉方块函数
static void AvoidFric(const SHAPEPTR shapeptr)
{
	int i;
	int px,py;
	for(i=0;i<4;i++){
		px = shapeptr->point[i][0];//图形其中一个方块的原坐标 
		py = shapeptr->point[i][1];
		if(px>=2 && window[px-1][py].isoccu == TRUE)//左边 
			DrawBlock(MyXtransX(px-1),MyYtransY(py),window[px-1][py].color);
		if(px<=WINDOW_WIDTH_2-1 && window[px+1][py].isoccu == TRUE)//右边 
			DrawBlock(MyXtransX(px+1),MyYtransY(py),window[px+1][py].color);
		if(py<=WINDOW_HEIGHT_2 && window[px][py+1].isoccu == TRUE)//上面 
			DrawBlock(MyXtransX(px),MyYtransY(py+1),window[px][py+1].color);
		if(py>=2 && window[px][py-1].isoccu == TRUE)//左边 
			DrawBlock(MyXtransX(px),MyYtransY(py-1),window[px][py-1].color);		
	}
} 








 

