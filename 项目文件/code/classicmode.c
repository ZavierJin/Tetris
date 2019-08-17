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
#include "extgraph.h" 

#include "commonuse.h"
#include "shapebasic.h"  
#include "user.h"
#include "record.h"
#include "classicmode.h"

//#define VK_P 0x50 

#define DEFAULT_COLOR "Black" //默认颜色 

#define TIMER_BLINK1000 1     //1000ms定时器时间标志号
//#define TIMER_BLINK100 2     //100ms定时器时间标志号

#define BEGIN_BUTTON 0
#define PAUSE_BUTTON 1
#define RETURN_BUTTON 2 
#define RESTART_BUTTON 3
#define SAVE_BUTTON 4
#define OUT_PAUSE_BUTTON 5

#define POPUP_TOTAL 6 
#define RETURN_POPUP 1 
#define SAVE_TIPS_POPUP 2
#define SAVE_SUCC_POPUP 3
#define RESTART_POPUP 4
#define CHANGE_POPUP 5
#define NO_CHANGE_POPUP 6

/***************************************************************
	经典模式 
***************************************************************/

static double cx,cy;//窗口中心坐标 

static SHAPE temshape;//移动的形状
static int nextid = -1; //下一个形状的id，-1代表未初始化

static bool isbegin = FALSE;
static bool ispause = FALSE;//是否暂停 
static bool islose = FALSE;//游戏是否结束 

static bool ispop[POPUP_TOTAL+1];//弹窗,0号代表是否有弹窗，1号开始代表1号弹窗
static int sur_state = 1;
//界面状态，0未赋值，1游戏中，2暂停，3弹窗，4结束 

static int sco = 0;//单次分数 

static int cur_state = 0;//当前难度 
static int cho_state = 0;//玩家选择切换难度 

static int mseconds = 250; //1000ms定时器 
//const int mseconds100 = 100;  //100ms定时器 


//外部引用，用于界面跳转，用户存档等 
extern struct user cur_user;
extern int cur_page;
extern InitMyGui();//界面交互开启

void CloseClassicMode();

extern double GetFontHeight();
extern double ScaleXInches(int x);//外部引用将鼠标坐标从像素转为英寸的函数 
extern double ScaleYInches(int y);

static void InitClassicWindow();//初始化游戏窗口函数 
static void MouseEventProcess(int x, int y, int button, int event);//鼠标消息回调函数 
static void TimerEventProcess(int timerID);//定时器消息回调函数
static void KeyboardEventProcess(int key, int event);//键盘消息回调函数
static void RefreshWindow();//刷新游戏窗口函数 
static void DrawNextShape();//提示下一个形状函数 
static bool IsLose();//判断游戏是否结束函数 

static void PrintScore();//显示分数函数 
static void PrintHelp();//显示帮助函数
static void PrintTitle();
static void BeforeGame();

static void GameBegin(); //游戏开始函数
static void GameEnd(bool finish); //游戏结束函数,finish记录是否完成  

static void MyButton(int id);//绘制按钮函数

static void PopupWindow(int id);//弹窗 

static void UpdateScore();//更新成绩 
static void RankingList();//排行榜更改 
static void PrintRankingList(char name[][16], const int score[],int ra); //绘制排行榜 

static void SaveRecord();//存档 
static void drawMenu();//画菜单，响应鼠标 
static void drawState(); //画状态栏 

void ContinRecord(int state, struct Record *cur_rec)
{
	int i,j;
	InitGraphics();
	InitGUI();
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape(cx, cy);//初始化形状 
	
	switch(state){
		case 1:mseconds = 900;break;
		case 2:mseconds = 450;break;
		case 3:mseconds = 250;break;
		default:exit(-1);break;
	}
	
	cur_state = state;//当前难度 
	
	registerMouseEvent(MouseEventProcess);      // 鼠标
	
	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//注册定时器消息回调函数
	
	startTimer(TIMER_BLINK1000, mseconds);//1000ms定时器触发
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//弹窗初始化 
	
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			window[i][j].isoccu = cur_rec->window2[i][j].isoccu;
			window[i][j].color = cur_rec->window2[i][j].color;
		}
	sco = cur_rec->score;
	temshape = cur_rec->sh;
	nextid = cur_rec->nextid;
		
	isbegin = TRUE;
	ispause = FALSE;
	islose = FALSE;
	
	DisplayClear();
		
	
	RefreshWindow();
	
	DrawNextShape();
	//DrawFrame();
	
	PrintTitle();
	//PrintScore();
	
	PrintHelp();
	
	MyButton(OUT_PAUSE_BUTTON); 
	MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //重新开始按钮
}
 
void InitClassicMode(int state)
{
	int i;
	InitGraphics();
	InitGUI();
	
	switch(state){
		case 1:mseconds = 900;break;
		case 2:mseconds = 450;break;
		case 3:mseconds = 250;break;
		default:exit(-1);break;
	}
	cur_state = state;//当前难度
	
	registerMouseEvent(MouseEventProcess);      // 鼠标
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//弹窗初始化 
	
	InitClassicWindow();
	

	//InitConsole();
	//printf("%.4f,%.4f",cx,cy);
	
}

void CloseClassicMode()
{
	int i,j;
	//初始化数据，防止遗留 
	nextid = -1;
	sco = 0;
	isbegin = FALSE;
	ispause = FALSE;
	islose = FALSE;
	
	//初始化窗口，防止游戏遗留 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			if(i==0 || i==WINDOW_WIDTH+1 || j==0){//墙壁单元格 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
		}
	
	GameEnd(FALSE);//关闭计时和键盘函数
	cancelMouseEvent(MouseEventProcess);
	 
}

static void InitClassicWindow()
{
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape(cx, cy);//初始化形状 
	
	
	MyButton(BEGIN_BUTTON);
	BeforeGame();
}

static void GameBegin()
{
	int i,j; 

	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//注册定时器消息回调函数
	
	startTimer(TIMER_BLINK1000, mseconds);//1000ms定时器触发
//	startTimer(TIMER_BLINK100, mseconds100);//100ms定时器触发
	
	//初始化窗口单元格 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			if(i==0 || i==WINDOW_WIDTH+1 || j==0){//墙壁单元格 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
		}
		
	DisplayClear();
	
	temshape = prishape[RandomId()];
	
	DrawShape(&temshape);

	DrawNextShape();
	
	ispause = FALSE;
	islose = FALSE;
	
	PrintTitle(); 
	/*
	SetEraseMode(TRUE);//擦除上次得分，防止重影 
	PrintScore();
	SetEraseMode(FALSE);*/
	sco = 0;//单次分数
	PrintScore();
	
	DrawFrame();
	PrintHelp();
	
	
	
	//drawLabel(cx+3.3, cy-3.0, maker);
	
	MyButton(OUT_PAUSE_BUTTON); 
	MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //重新开始按钮
	//InitMyGui();
	//PopupWindow(SAVE_TIPS_POPUP);
}

static void GameEnd(bool finish)
{
	cancelTimerEvent(TimerEventProcess);
	cancelTimer(TIMER_BLINK1000);
	
	cancelKeyboardEvent(KeyboardEventProcess);
	if(finish){
		UpdateScore();
		RankingList();
		//DisplayClear();
		SetEraseMode(TRUE); //擦掉外面的暂停和存档按钮 
		drawRectangle(cx-4.5-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//防止擦不干净（w,h已经直接带入） 
		drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//防止擦不干净 
		SetEraseMode(FALSE);
		
		 
	}
} 

static void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;//当前鼠标坐标
	
	double h = 0.5;  // 控件高度
	double w = 1.0;

	uiGetMouse(x,y,button,event); //GUI获取鼠标
	
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
							CloseClassicMode();//退出游戏模式 
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
					}
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
					}
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
					else if(ispop[5]){//5号弹窗，切换难度 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//是 
							ispop[5] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();
							InitClassicMode(cho_state);
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//否 
							ispop[5] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							 
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}
					else if(ispop[6]){//6号弹窗，当前即此难度 
						if(InButton(cx-1.4, cy-0.4, 1.0, 0.4, mx, my)){//继续 
							ispop[6] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//继续游戏
							
							SetEraseMode(TRUE); //擦掉弹窗 
							drawRectangle(cx-2.05, cy-0.61, 4.1, 1.62, 1);//防止擦不干净 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}else if(InButton(cx+0.4, cy-0.4, 1.0, 0.4, mx, my)){//退出 
							ispop[6] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();//退出游戏 
							InitMyGui();
							cur_page = 3;//跳转
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
						}
						if(InButton(cx-4.5, cy-2.3, w, h, mx, my)){//存档按钮,会暂停 
							ispause = TRUE;
							PopupWindow(SAVE_TIPS_POPUP);
							ispop[2] = TRUE;
							ispop[0] = TRUE;	
						}
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
	
	if(isbegin || islose) {
		drawMenu();
		drawState();
	}
}


static void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI获取键盘
	
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
						ShapeSpin(&temshape,1);
						break;
					case VK_LEFT:
						ShapeMove(&temshape,-1);
						break;
					case VK_RIGHT:
						ShapeMove(&temshape,1);
						break;
					case VK_DOWN: 
					case VK_SPACE:
						StraDrop(&temshape);
						temshape = prishape[nextid];//重新获得随机图形 
						DrawNextShape();
						RefreshWindow();
						break;
				}
				break;
		}
	}
	if(isbegin || islose) drawMenu();
}

static void TimerEventProcess(int timerID)
{
	if(islose || ispause) return;
	
	switch(timerID){
	case TIMER_BLINK1000:
		if(!IsLose()){
			if(!IsBottom(&temshape)){
				ShapeDrop(&temshape);
			}else{
				temshape = prishape[nextid];
				DrawNextShape();
				RefreshWindow();//?	
			}
		} 
		break;
	
	}
}

static void drawState()
{
	char tips[30];

	if(!islose){
		if(!ispause) sur_state = 1;
		else if(ispop[0]) sur_state = 3;
		else sur_state = 2;
	}else 
		sur_state = 4;
	
		
	strcpy(tips,"当前状态：");
	switch(sur_state){
		case 1:strcat(tips,"游戏中");break;
		case 2:strcat(tips,"暂停中");break;
		case 3:strcat(tips,"弹窗");break;
		case 4:strcat(tips,"游戏结束");break;
		default:strcat(tips,"出错");break;
	}
	
	SetEraseMode(TRUE);
	drawRectangle(cx+2.8-0.03,0.0,2.2+0.02,0.3+0.02,1);
	SetEraseMode(FALSE);
	SetPenColor("Dark Gray");
	SetStyle(1); 
	drawBox(cx+2.8-0.02,0.01,2.2,0.3,0,tips,'Z',"Black");
	SetStyle(0); 
	SetPenColor("Black");
} 

static void drawMenu()
{ 
	static char * menuListClose[] = {"         ""结    束",  
		"    ""存档  |  Ctrl-T", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"    ""重来  |  Ctrl-R",
		"    ""退出  |  Ctrl-E"};
	static char * menuListState[] = {"         ""难    度",
		"    ""简单  |  Ctrl-F", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"    ""普通  |  Ctrl-G",
		"    ""困难  |  Ctrl-H"};
	/*	
	static char * menuListHelp[] = {"查看",
		"个人战绩  | Ctrl-M",
		};*/ 
	static char * selectedLabel = NULL;
	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = 2*cy;
	double h = fH*1.5; // 控件高度
	double w; // 控件宽度
	double wlist;
	double xindent = 2*cx/20; // 缩进
	int    selection;

	w = wlist = 1.65;
	
	SetEraseMode(TRUE);
	drawRectangle(x,y-4*h-0.01,cx-1.55,4*h+0.02,1);
	SetEraseMode(FALSE);
	
	setMenuColors("Dark Gray","White","Yellow","Red",1); 
	// 结束 菜单
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListClose, sizeof(menuListClose)/sizeof(menuListClose[0]));
	switch(selection){
		case 1://存档 
			if(!islose){
				ispause = TRUE;
				PopupWindow(SAVE_TIPS_POPUP);
				ispop[2] = TRUE;
				ispop[0] = TRUE;
			} 
			break;
		case 2://重来 
			if(!islose){
				ispause = TRUE;
				PopupWindow(RESTART_POPUP);
				ispop[4] = TRUE;
				ispop[0] = TRUE;
			}else{
				GameBegin();
				isbegin = TRUE;//开始状态切换
			}
			break;
		case 3://退出 
			if(!islose){
				ispause = TRUE;
				PopupWindow(RETURN_POPUP);
				ispop[1] = TRUE;
				ispop[0] = TRUE;
			}else{
				CloseClassicMode();//退出游戏模式 
				InitMyGui();
				cur_page = 3;//跳转
			}
			break;
	}
	// 切换难度 菜单
	selection = menuList(GenUIID(0),x+w,  y-h, w, wlist,h, menuListState,sizeof(menuListState)/sizeof(menuListState[0]));
	if(selection>0){
		cho_state = selection;//选择难度 
		if(islose){
			CloseClassicMode();
			InitClassicMode(cho_state);
		}else{
			if(cho_state>0 && cho_state<4 && cho_state!=cur_state){
					PopupWindow(CHANGE_POPUP);
					ispop[5] = TRUE;	
			}else{
				PopupWindow(NO_CHANGE_POPUP);
				ispop[6] = TRUE;
			}
			ispause = TRUE;
			ispop[0] = TRUE;
		}
			
	}
	
}


static void RefreshWindow()
{
	int i,j;
	bool isclean;
	int conclean = 0;//连续清除几行 
	//清除游戏界面 
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");	
		}
	PrintScore();
	//判断是否清除一行 
	for(j=1;j<WINDOW_HEIGHT+1;j++){
		isclean=TRUE;
		for(i=1;i<WINDOW_WIDTH+1;i++){
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
			sco += 2 * conclean - 1; 
			conclean = 0;
		}
	}
	//重新绘制游戏界面 
	SetEraseMode(FALSE);
	DrawFrame();
	PrintScore();
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].isoccu)
				DrawBlock(MyXtransX(i),MyYtransY(j),window[i][j].color);
		}
	DrawShape(&temshape);
}

static void DrawNextShape()
{
	SHAPEPTR shapeptr;
	int i;
	if(nextid!=-1){//如果之前有画过，先擦除上一个形状 
		SetEraseMode(TRUE);
		shapeptr = &prishape[nextid];
		for(i=0;i<4;i++)
			DrawBlock(MyXtransX(shapeptr->point[i][0])+3.2,MyYtransY(shapeptr->point[i][1]),shapeptr->color);
	}
	 
	SetEraseMode(FALSE);
	nextid = RandomId();
	shapeptr = &prishape[nextid];
	for(i=0;i<4;i++)
		DrawBlock(MyXtransX(shapeptr->point[i][0])+3.2,MyYtransY(shapeptr->point[i][1]),shapeptr->color);
	
	SetStyle(1);
	drawLabel(cx+2.2, cy+2.6, "下一个:");
	SetStyle(0);
}	

static bool IsLose()
{
	char over[]="游 戏 结 束";
	char label_color[]= "Red";
	int i;
	//判断窗口单元格是否超出所画线，超出则游戏结束 
	for(i=1;i<WINDOW_WIDTH+1;i++)
		if(window[i][WINDOW_HEIGHT-4].isoccu) {
			islose = TRUE;
			break;
		}
	
	if(islose){
		GameEnd(TRUE); 
		
			
		SetPenColor(label_color);
		SetPointSize(26);
		SetStyle(1);
		drawLabel(cx-0.95, cy+2.8, over);
		SetStyle(0);
		SetPointSize(13);
		
		isbegin = FALSE;
		//MyButton(BEGIN_BUTTON,TRUE);
	}
	return islose;//返回是否结束 
}

static void BeforeGame()
{
	int i; 
	char* tips[5]={
		"非常经典的经典模式",
		"拼的是智力、手速还有运气", 
		"无时间限制，就看你能撑多久",  
		"操作仅需方向键",
		"F1 键暂停游戏", 
	};
	PrintTitle();
	
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("Brown");
	for(i=0;i<3;i++){
		drawLabel(cx-1.5, cy+1.5-i*0.5, tips[i]);
	}
	SetPenColor("Blue");
	for(i=3;i<5;i++){
		drawLabel(cx-1.5, cy+1.5-i*0.5, tips[i]);
	}
	SetPenColor("Black");
	SetPointSize(13);
	SetStyle(0);
}

static void PrintScore()
{
	char tips[]="当前得分：";
	char total[sizeof(tips)+SCORE_MAX_CAP+1];
	static char score[SCORE_MAX_CAP+1];
	int temsco = sco;
	int i;
	//得分由整型化为字符，前面补0 
	for(i=SCORE_MAX_CAP-1;i>=0;i--){
		score[i] = temsco % 10 + '0';
		temsco/=10;
	}
	score[SCORE_MAX_CAP]='\0';
	
	strcpy(total,tips);
	strcat(total,score);
	SetStyle(1);
	drawLabel(cx+2.2, cy, total);
	SetStyle(0);
	
}

static void PrintTitle()
{
	char title[]="经典模式（简单）"; 
	switch(cur_state){
		case 1:strcpy(title,"经典模式（简单）");break;
		case 2:strcpy(title,"经典模式（普通）");break;
		case 3:strcpy(title,"经典模式（困难）");break;
	} 
	SetPointSize(26);
	SetStyle(1);
	drawLabel(cx+1.75, cy+3.15, title);
	SetPointSize(13);
	
	
	SetStyle(0);
}

static void PrintHelp()
{
	int i;
	char *helptips[5] = {
		" | 左键 | 左移",
		" | 右键 | 右移",
		" | 上键 | 顺时针转",
		" | 下键 或 空格 | 坠地", 
		" | F1键 | 暂停" 
	};
	SetStyle(1);
	drawLabel(cx-4.7, cy+1.0+0.35, "操作指南："); 
	SetStyle(0);
	for(i=0;i<5;i++){
		drawLabel(cx-4.7, cy+1.0-i*0.3, helptips[i]);
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
	struct user *head, *p, *pnext;
	bool needmo = FALSE;
	head = p = pnext = NULL;
	
	if(cur_user.score[cur_state-1]<sco){ //比较是否超过当前模式最佳成绩 
		cur_user.score[cur_state-1] = sco;
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
	
	RankingFile(cur_state-1,name,score,0);//读入排行榜
	cur_sco = cur_user.score[cur_state-1]; 
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
	if(ismod) RankingFile(cur_state-1,name,score,1);//修改排行榜 
	
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
	
	
	cex = cx; cey = cy-0.58;
	w = 4.0;  h = 5.8; 
	th = 0.35 ; tj = 0.2;
	w1 = 0.9; w2 = 1.8; w3 = w-2*tj-w1-w2;
	
	//擦除部分界面 
	SetEraseMode(TRUE);
	drawRectangle(0,cy*2-4*0.3-0.01,cx-1.55,4*0.3+0.02,1);
	drawRectangle(cx-2.0,cy-3.5,w,7.0,1);
	drawRectangle(cx+2.1,cy+0.8,2.5,2.9,1);
	SetEraseMode(FALSE);
	SetPenColor("Black");
	PrintTitle();//防止误差标题 
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

static void SaveRecord()
{
	struct Record cur_rec;
	struct Record *head=NULL;
	int i,j;
	
	cur_rec.num = cur_user.num;//用户编号 
	cur_rec.score = sco;//当前得分
	
	//窗口存档 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			cur_rec.window2[i][j]=window[i][j];
		}
	
	cur_rec.sh = temshape;//当前形状
	cur_rec.nextid = nextid;//下一个形状编号
	
	cur_rec.next = NULL;
	
	AddRecord(cur_state-1, &cur_rec);//加入新存档

}

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
		case CHANGE_POPUP:
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
			drawBox(x,y,w-2*tj,th,1,"当前游戏还未保存，确定要切换难度吗？",'L',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," 是 ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," 否 ",'Z',"White");
			break;
		case NO_CHANGE_POPUP:
			w = 4.05; h = 1.6;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//画文本//前打8个空格空两个汉字，18个汉字一行 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"当前即此难度",'Z',"Black");
			//画按钮 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.4,y,1.0,th,1,"继续",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,1.0,th,1,"退出",'Z',"White");
			break;
	}
	
	SetPenColor(DEFAULT_COLOR);
}

 

