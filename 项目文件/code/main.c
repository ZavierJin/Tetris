#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include <string.h>

#include "commonuse.h"
#include "imgui.h" 
#include "shapebasic.h"
#include "animation.h"
#include "user.h" 
#include "record.h"
#include "classicmode.h"
#include "practisemode.h"
#include "newmode.h"

/***************************************************************
	主界面GUI 
***************************************************************/

#define DEFAULT_COLOR "Black" //默认颜色 
#define DEFAULT_POINT_SIZE 13 //默认字体大小 

#define TOTAL_FACE 25  	//界面最多个数 
#define BUTTON_MAX 5 	//一个界面按钮最多个数 

int cur_page = 0;//当前界面id ,从0开始 

int button_jump[TOTAL_FACE][BUTTON_MAX]={
	{1,2,-1},                 	//0 用户登录注册界面 
	{0},                 		//1 登录界面 
	{0},						//2 注册界面 
	{4,21,22,-1},               //3 菜单界面
	{5,20,10,15,3},				//4 模式选择界面 
	
	//经典模式5-9 
	{6,8,4},                    //5 经典模式界面 
	{7,7,7,5},                  //6 经典模式难度选择界面 
	{6},						//7 经典模式游戏界面  
	{5},						//8 读取存档界面
	{},//8-9 
	
	//双人模式10-14 
	{11,12,4},                  //10 双人模式界面 
	{10},						//11 双人模式游戏界面 
	{10},						//12 读取存档界面 
	{},{},//13-14
	
	//病毒模式15-19 （待定
	{16,17,4},                  //15 创新模式界面 
	{15},						//16 病毒模式游戏界面 
	{15},						//17 读取存档界面 
	{},{},//18-19
	
	{4},						//20 练习模式界面
	{3},						//21 游戏帮助界面
	{3},						//22 个人战绩界面
									
}; //按钮跳转，第一个下标是界面id，第二个下标是按钮id 

char* button_label[TOTAL_FACE][BUTTON_MAX]={
	{"登 录","注 册","退 出"},//0 
	{"返 回"},//1 
	{"返 回"},//2 
	{"开始游戏","游戏帮助","个人战绩","退出游戏"},//3 
	{"经典模式","练习模式","双人模式","病毒模式","返 回"},//4
	
	//经典模式5-9 
	{"新 游 戏","读取存档","返 回"},//5 
	{"简 单","普 通","困 难","返 回"},//6 
	{},//7 
	{"返 回"},//8 
	{},//9空 
	
	//双人模式10-14 
	{"新 游 戏","读取存档","返回"},//10 
	{"返 回"}, //11 
	{"返 回"}, // 12 
	{},{},// 13-14 
	
	//病毒模式15-19 （待定
	{"新 游 戏","读取存档","返回"},//15 
	{"返 回"}, //16
	{"返 回"}, // 17 
	{},{},// 18-19 
	
	{"返 回"},
	{"返 回"},
	{"返 回"},

}; //按钮文字，第一个下标是界面id，第二个下标是按钮id  

int button_id_total[TOTAL_FACE]={
	3,1,1,4,5, //0-4 
	3,4,0,1,0, //5-9
	3,0,1,0,0, //10-14
	3,0,1,0,0, //15-19
	0,1,1, 		//20-22
}; //当前界面按钮总数，下标是界面id 

double button_first_px[TOTAL_FACE]={ 
	+1.0, +2.7, +2.7, +2.7, +2.7,
	+2.7, +2.7, +0.0, +2.7, +0.0,
	+2.7, -2.7, +2.7, +0.0, +0.0,
	+2.7, -2.7, +2.7, +0.0, +0.0,
	-2.7, +2.7, +2.7, 
};//当前界面第一个按钮相当中心x坐标，下标是界面id，
																					
double button_first_py[TOTAL_FACE]={ 
	+0.4, -3.2, -3.2, +1.7, +2.0,
	+1.2, +1.7, -0.0, -3.2, +0.0,
	+1.2, +0.7, -3.2, +0.0, +0.0,
	+1.2, +0.7, -3.2, +0.0, +0.0,
	+0.7, -3.2, -3.2, 
};//当前界面第一个按钮相当中心y坐标，下标是界面id，
																						 
double button_dis[TOTAL_FACE];//每个按钮间距 ,便于测试，先全部设为1.0 

char label_color[] = "White"; 
char button_color[] ="Dark Gray";

struct user cur_user;//当前用户 

//登陆 
char old_name[80];//读入的用户名
int old_name_state = 0;
//读入用户名状态，0代表未读入，1代表用户名不合法，2代表用户名已存在，3代表注册成功 

//注册 
char new_name[80];//读入的用户名
int new_name_state = 0;
//读入用户名状态，0代表未读入，1代表用户名不合法，2代表用户名未找到，3代表登录成功 

//经典模式游戏难度，0为为开启，1为简单  
int classic_mode_state = 0;

int classic_rec_state[3]={0,0,0};//经典模式存档状态 

struct Record *cur_rec_p1, *cur_rec_p2, *cur_rec_p3;//经典难度存档指针 

int help_page = 0;//帮助界面第几页 

double cx,cy;//界面中心坐标 



void InitMyGui();//界面交互开启
void CloseMyGui();//界面交互关闭

void OutMouseEventProcess(int x, int y, int Mbutton, int event);//鼠标消息回调函数
void CharEventProcess(char ch); 
void KeyboardEventProcess(int key, int event);
 
void RefreshFace();//刷新界面函数 
void RefreshButton();//刷新按钮函数 
bool ButtonResp(double mx, double my);//按钮响应函数 

void BeginFace(); //程序开始界面

void LoginFace(); //登录界面 
bool CheckOldName();//检查旧名字是否存在 

void RegisterFace(); //注册界面 
bool CheckNewName();//检查新名字是否存在 

void ReadRecordFace(int mode);//读取存档界面，mode：0代表经典模式 
void RecordFaceButtonResp(int mode, double mx, double my); 
//读取存档按钮响应函数，mode：0代表经典模式

void MyScoreFace(); //个人战绩界面 
void HelpFace(); //游戏帮助界面 
void HelpFaceButtonResp(double mx, double my);
void NoBuild();

void Main()
{
	
	InitGraphics();
	InitGUI();
	//InitConsole();
	SetWindowTitle("俄罗斯方块");
	cx= GetWindowWidth()/2.0;
    cy= GetWindowHeight()/2.0;
	
    InitMyGui();
    
	int i;
	for(i=0;i<TOTAL_FACE;i++) button_dis[i]=1.0; //便于测试，按钮间距先全1.0 
	
}


void InitMyGui()
{
	registerCharEvent(CharEventProcess); // 字符
	registerKeyboardEvent(KeyboardEventProcess);// 键盘
	registerMouseEvent(OutMouseEventProcess);      // 鼠标
	//printf("$$\n");
} 

void CloseMyGui()
{
	cancelCharEvent(CharEventProcess); // 字符
	cancelKeyboardEvent(KeyboardEventProcess);// 键盘
	cancelMouseEvent(OutMouseEventProcess);      // 鼠标	
} 


/*鼠标消息回调函数，其中x，y为鼠标当前坐标，event为鼠标事件类型*/ 
void OutMouseEventProcess(int x, int y, int button, int event)
{

	double mx, my;//当前鼠标坐标

	uiGetMouse(x,y,button,event); //GUI获取鼠标
	
	mx = ScaleXInches(x);//像素转英寸，数据类型int转double 
	my = ScaleYInches(y);
	
	//按钮响应 
	switch (event){
		case BUTTON_DOWN://如果鼠标被按下 
			if(button==LEFT_BUTTON){
				ButtonResp(mx,my);
				if(cur_page==8){//经典模式存档读取界面按钮反馈 
					RecordFaceButtonResp(0,mx,my);
				}
				if(cur_page==21){//游戏帮助界面按钮反馈 
					HelpFaceButtonResp(mx,my);
				}
			}
			break;
	}
	
	RefreshFace();
	

}

// 用户的字符事件响应函数
void CharEventProcess(char ch)
{
	
	uiGetChar(ch); // GUI字符输入
	
	RefreshFace();

}

// 用户的键盘事件响应函数
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI获取键盘
	
	switch(event){
		case KEY_DOWN:
			switch(key){
				case VK_RETURN://回车 
					switch(cur_page){
						case 1:CheckOldName();break;
						case 2:CheckNewName();break;
					}
						
			}
	}
	RefreshFace();
}



void RefreshFace()
{
	static int pre_page = -1;//前一次页面，初始值为-1 
	static bool page_change = FALSE;//是否变换过页面 
	static bool anim_open = FALSE;//动画是否开启 
	
	if(pre_page!=cur_page){//页面切换 
		page_change = TRUE;
		pre_page = cur_page;
	}else if(page_change==TRUE){ 
		page_change = FALSE;
	} 
		
	if(anim_open && page_change){//动画切换 
		CloseAnim();//关闭动画 
		anim_open = FALSE;
	}
	
	if(page_change){//只画一次的界面 
		switch(cur_page){
			case 0: //开始界面,为修复bug，函数调用移至此函数最后 
			case 3: case 4://动画 
				InitAnim();//初始化动画 
				anim_open = TRUE;	
				break;			
			case 8: //经典模式读取存档界面 
				ReadRecordFace(0);
				break; 
			case 12:case 17:
				NoBuild();
				break;
			case 21://游戏帮助界面
				help_page = 0; 
				break;
			case 22://个人战绩界面 
				MyScoreFace();
				break;
			default:
				break;
		}
	} 
	
	switch(cur_page){//需要反复刷新的界面		
		case 1://登陆界面 
			DisplayClear();
			LoginFace();
			break;
		case 2://注册界面
			DisplayClear();
			RegisterFace();
			break;
		case 21://游戏帮助界面
			HelpFace();
			break;
		case -1://出错，退出程序 
			exit(-1);
			break;
		default:
			break;
	}
	RefreshButton();
	
	if(cur_page==0 && page_change) BeginFace();//???初始界面,放最后防bug 

}


void RefreshButton()
{
	double fx,fy,ty;
	double h = 0.7;  // 控件高度
	double w = 2.0;
	int total;
	int i;
	
	fx = cx+button_first_px[cur_page];
	fy = cy+button_first_py[cur_page];
	ty = button_dis[cur_page];
	total = button_id_total[cur_page];
	
	switch(total){//???
		case 1:break;
		case 3:ty+=0.2;break;
	}
	
	//SetStyle(0);
	SetPointSize(16);
	//printf("%d\n",GetPointSize());
	for(i=0;i<total;i++){
		SetPenColor(button_color);
		drawBox( fx, fy-1.0*i*ty, w, h, 1, button_label[cur_page][i], 'Z', label_color);
	}
	SetPenColor(DEFAULT_COLOR); //将画笔重新设为默认颜色 
	SetPointSize(DEFAULT_POINT_SIZE);//字体大小为默认 
}

bool ButtonResp(double mx, double my)
{
	double fx,fy,ty;
	double h = 0.7;  // 控件高度
	double w = 2.0;
	int total;
	int i;
	bool isresp = FALSE;
	//if(cur_page!=0) return FALSE;
	fx = cx+button_first_px[cur_page];
	fy = cy+button_first_py[cur_page];
	ty = button_dis[cur_page];
	total = button_id_total[cur_page];
	
	switch(total){//???间距 
		case 1:break;
		case 3:ty+=0.2;break;
	}
	
	for(i=0;i<total;i++){
		if(InButton( fx, fy-1.0*i*ty, w, h, mx, my)){
			cur_page = button_jump[cur_page][i];//修改界面id ，进入下一个界面 
			isresp = TRUE;
			DisplayClear();
			break;
		}
	}
	//跳转至游戏界面
	
	switch(cur_page){
		case 7://经典模式 
			classic_mode_state = i+1;//经典模式难度
			CloseMyGui(); //关闭窗口GUI 
			InitClassicMode(classic_mode_state);//进入游戏 
			break;	
		case 20://练习模式
			CloseMyGui(); //关闭窗口GUI 
			InitPracMode();//进入游戏 
			break;	
		case 11://双人模式 
			CloseMyGui(); //关闭窗口GUI 
			InitDoubleMode();
			break;	
		case 16://
			CloseMyGui(); //关闭窗口GUI
			InitNewMode();
			break;	
	}
	
	return isresp;
}


void BeginFace()
{
	char title[]="俄 罗 斯 方 块";
	
	SetPointSize(DEFAULT_POINT_SIZE*4); 
	SetStyle(1);
	drawLabel(cx-0.4,cy+2.3,title);//程序标题 
	SetStyle(0);
	SetPointSize(DEFAULT_POINT_SIZE); 
} 

void LoginFace()
{
	static char title[]="用 户 登 录"; 
	static char memo[]="请输入你的用户名：";
	static char fail[]="用户名不合法，请重新输入";
	static char repe[]="用户名未找到，请重新输入";
	static char succ[]="登录成功！";
	static char str[80];//暂时储存当前读入文本
	static char tips_tit[]="输入提示：";
	static char *tips[4]={
		"1.用户名即注册时输入的用户名",
		"2.用户名中只允许包含字母和数字",
		"3.用户名不能超过15个字符",
		"4.按下回车确定输入" 
	};
	double h = 0.5;  // 控件高度
	double w = 2.0;
	double x = cx+0.8;//text位置 
	double y = cy+h/2.0;
	int i;
	
	SetPenColor("Black");
	//界面标题 
	SetPointSize(DEFAULT_POINT_SIZE*2);
	SetStyle(1);
	drawLabel(cx-0.9,cy+2.3,title);
	SetPointSize(DEFAULT_POINT_SIZE);
	
	//输入提示 
	drawLabel(cx-3.3,cy+1.0,tips_tit);
	drawLabel(x,y+h*1.5,memo); //文本框提示语
	SetStyle(0);
	for(i=0;i<4;i++){
		drawLabel(cx-3.0,cy+0.6-i*0.4,tips[i]);
	}
	
	
	//读入文本 
	if( textbox(0,x,y, w, h, str, sizeof(str)) )
		strcpy(old_name,str);	
	//输出状态标签 
	switch(old_name_state){
		case 0:break;
		case 1: 
			drawLabel(x,y-h*0.8,fail);//用户名不合法， 
			break;
		case 2:
			drawLabel(x,y-h*0.8,repe);//用户名未找到， 
			break;
		case 3:
			drawLabel(x,y-h*0.8,succ);//登录成功！ 
			break;
	} 
}

bool CheckOldName()
{
	struct user *head=NULL;
	
	head = OpenUserFile();
	
	if(!Inputcheck(old_name)){//如果用户名不合法 
		old_name_state = 1;
	}else{
		if(!UserLogin(head,old_name,&cur_user)){//如果用户名已存在 
			old_name_state = 2;
		}else {//登录成功，跳转界面 
			old_name_state = 3;
			LoginFace();
			Pause(1.0);//跳转延迟1s
			DisplayClear(); 
			cur_page = 3;
		}
		
	}
	DeleteList(head);
	return FALSE;  
} 



void RegisterFace()
{
	static char title[]="用 户 注 册"; 
	static char memo[]="请输入你的用户名：";
	static char fail[]="用户名不合法，请重新输入";
	static char repe[]="用户名已存在，请重新输入";
	static char succ[]="注册成功！";
	static char str[80];//暂时储存当前读入文本 
		static char tips_tit[]="输入提示：";
	static char *tips[4]={
		"1.注册的用户名下次游戏用于登录",
		"2.用户名中只允许包含字母和数字",
		"3.用户名不能超过15个字符",
		"4.按下回车确定输入" 
	};
	double h = 0.5;  // 控件高度
	double w = 2.0;
	double x = cx+0.8;//text位置 
	double y = cy+h/2.0; 
	int i;
	
	SetPenColor("Black");
	//界面标题 
	SetPointSize(DEFAULT_POINT_SIZE*2);
	SetStyle(1);
	drawLabel(cx-0.9,cy+2.3,title);
	SetPointSize(DEFAULT_POINT_SIZE);
	
	//输入提示 
	drawLabel(cx-3.3,cy+1.0,tips_tit);
	drawLabel(x,y+h*1.5,memo); //文本框提示语
	SetStyle(0);
	for(i=0;i<4;i++){
		drawLabel(cx-3.0,cy+0.6-i*0.4,tips[i]);
	}
	
	
	 
	if( textbox(0,x,y, w, h, str, sizeof(str)) )
		strcpy(new_name,str);	
	//输出状态标签 
	switch(new_name_state){
		case 0:break;
		case 1: 
			drawLabel(x,y-h*0.8,fail);//用户名不合法， 
			break;
		case 2:
			drawLabel(x,y-h*0.8,repe);//用户名已存在， 
			break;
		case 3:
			drawLabel(x,y-h*0.8,succ);//注册成功！ 
			break;
	} 
}


//检查新名字是否存在
bool CheckNewName()
{
	struct user *head=NULL;
	
	head = OpenUserFile();
	
	if(!Inputcheck(new_name)){//如果用户名不合法 
		new_name_state = 1;
	}else{
		if(!UserRegister(head,new_name,&cur_user)){//如果用户名已存在 
			new_name_state = 2;
		}else {//登录成功，跳转界面 
			ModifyUserFile(head);
			new_name_state = 3;
			RegisterFace();
			Pause(1.0);//跳转延迟1s
			DisplayClear(); 
			cur_page = 3;
		}
	}
	DeleteList(head);
	return FALSE;  
} 

void HelpFaceButtonResp(double mx, double my)
{
	switch(help_page){
		case 0:
			if(InButton(cx+3.4,cy+2.35,1.15,0.7,mx,my)){
				help_page=1;
			}
			break;
		case 1:
			if(InButton(cx-4.55,cy+2.35,1.15,0.7,mx,my)){
				help_page=0;
			}
			break;
	}
}

void HelpFace()
{
	double w, h;//宽，高 
	double th, tj;//行宽,间距
	double x, y;//存档输出当前位置 
	double w1, w2;
	int i,j;
	char* help[50]={
		"1.在开始游戏界面选择模式进入游戏",
		"2.选择读取存档，可以继续完成上次存档游戏（目前只有经典模式有此功能）",
		"3.选择经典模式",
		"        ""a.可以选择难度\"简单\"\"普通\"\"困难\"，不同难度方块掉落速度不同",
		"        ""b.操作方向键进行游戏，F1 键暂停游戏",
		"        ""c.游戏没有时间限制，当方块超出掉落起始线则游戏结束",
		"4.选择练习模式",
		"        ""a.该模式用于练习，操作方式同经典模式",
		"        ""b.可通过X 键（加速）和Z 键（减速）自行调节方块下落速度",
		"5.选择双人模式",
		"        ""a.操作方向键和WASD键双人合作闯关",
		"        ""b.方块碰撞一边墙体可瞬间跳至另一边",
		"        ""c.游戏中途会随机生成绿色的生长方块，每个生长方块会向旁边生长，关卡切换时生长方块不被清除",
		"6.选择病毒模式",
		"        ""a.操作方式同经典模式",
		"        ""b.本模式中有3种特殊方块        黑色：炸弹方块         蓝色：冰冻方块        紫色：病毒方块",
		"          ""炸弹方块：随机生成，黑色为炸弹方块的引爆点，多个引爆点直接接触可触发爆炸效果，爆炸可引发连环爆炸",
		"          ""冰冻方块：随机生成，蓝色为炸弹方块的冰冻点，多个冰冻点直接接触可触发冰冻效果，冰冻状态下病毒方块失效，",
		"                              ""并被冰冻为深蓝色，冰冻状态持续一段时间，触发冰冻方块，时间越长" ,
		"          ""病毒方块：随机生成，紫色为被病毒侵染部分，病毒会向四周扩散开，若某一行中有一个方块被病毒侵染，" ,
		"                              ""则该行不能被普通消除",
		"7.结束游戏后可以查看当前模式排行榜",
		"8.选择个人战绩，可查看不同模式个人历史最高得分",
		"    """,
		"    """,
		"    """,
		"    """,
		"    """,
		"    """,
	};
	
	w = 9.5; h = 6.5;
	th = 0.35 ; tj = 0.2;
	w1 = 2.3; w2 = w - 2*tj - w1;
	x = cx-w/2.0+tj;
	y = cy+h/2.0-3*th-2*tj;
	
	//画底色 
	SetPenColor("Gray");
	drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
	//画标题 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,"游 戏 帮 助",'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	
	switch(help_page){
		case 0://第一页 
			SetPointSize(16);
			SetPenColor("Dark Gray");
			drawBox(cx+3.4,cy+2.35,1.15,0.7,1,"下一页",'Z',"White");
			SetPointSize(13);
			for(i=0;i<13;i++){
				SetPenColor("White");
				drawBox(x,y,w-2*tj,th,1,help[i],'L',"Black");
				y -= th;
			}
			break;
		case 1://第二页 
			SetPointSize(16);
			SetPenColor("Dark Gray");
			drawBox(cx-4.55,cy+2.35,1.15,0.7,1,"上一页",'Z',"White");
			SetPointSize(13);
			for(i=13;i<23;i++){
				SetPenColor("White");
				drawBox(x,y,w-2*tj,th,1,help[i],'L',"Black");
				y -= th;
			}
			break;	
	}
	
}

void MyScoreFace()
{
	char ch_score[SCORE_MAX_CAP+1];
	char *s_title[5]={
		"经典模式（简单）",
		"经典模式（普通）",
		"经典模式（困难）",
		"双人模式 ",
		"病毒模式 " 
	};
	char name[25];
	double w, h;//宽，高 
	double th, tj;//行宽,间距
	double x, y;//存档输出当前位置 
	double w1, w2;
	int i,j;
	
	w = 5.0; h = 3.6;
	th = 0.35 ; tj = 0.2;
	w1 = 2.3; w2 = w - 2*tj - w1;
	x = cx-w/2.0+tj;
	y = cy+h/2.0-3*th-2*tj;
	
	//画底色 
	SetPenColor("Gray");
	drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
	//画标题 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,"个 人 战 绩",'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	//显示个人战绩 
	for(i=0;i<MODE_NUM;i++){
		ScoreTrans(ch_score, cur_user.score[i]);
				
		SetPenColor("White");
		drawBox(x,y,w1,th,1,s_title[i],'Z',"Black");
		SetPenColor("White");
		drawBox(x+w1,y,w2,th,1,ch_score,'Z',"Black");
		
		y -= th;
	}
	//显示用户名 
	y -= tj;
	strcpy(name,"用户名：");
	strcat(name,cur_user.name);
	SetPenColor("White");
	drawBox(x+2.4,y,2.2,th,1,name,'Z',"Black");
	
	SetPenColor(DEFAULT_COLOR);	
}

void ReadRecordFace(int mode)
{
	struct Record *head1, *head2, *head3;//三个难度存档 
	struct Record cur_rec;//储存当前玩家存档
	 
	
	char tit_p[]="我的存档";
	char title[]="我的存档（经典模式）"; 
	char ch_score[SCORE_MAX_CAP+1];

	double w, h;//宽，高 
	double th, tj;//行宽,间距
	//double sy;//存档输出起始y位置
	double x, y;//存档输出当前位置 
	double w1, w2, w3;
	bool find = FALSE;
	int i,j;
	
	cur_rec_p1 = cur_rec_p2 = cur_rec_p3 = NULL;
	head1 = head2 = head3 = NULL; 
	strcpy(title,tit_p);
	for(i=0;i<3;i++) classic_rec_state[i]=0;//初始化 
	
	switch(mode){
		case 0://经典模式 
			strcat(title,"（经典模式）");
			w = 5.0; h = 2.75;
			th = 0.35 ; tj = 0.2;
			w1 = 1.8; w2 = w - 1.0 - 2*tj - w1;
			x = cx-w/2.0+tj;
			y = cy+h/2.0-2*th-1*tj;
			//画底色 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
			//画标题 
			SetStyle(1);
			SetPointSize(16);
			SetPenColor("White");
			drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,title,'Z',"Black");
			SetPointSize(13);
			SetStyle(0);
			
			//简单模式 
			cur_rec_p1 = SearchRecord(0,cur_user.num);//找存档 
			if(cur_rec_p1!=NULL){//如果找到记录 
				find = TRUE;
				classic_rec_state[0]=1;
				//得分由整型化为字符，前面补0 
				ScoreTrans(ch_score, cur_rec_p1->score); 
				//画存档
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"简 单 ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"继续",'Z',"White");
				SetPenColor(DEFAULT_COLOR);	
			}
			
			//普通模式 
			cur_rec_p2 = SearchRecord(1,cur_user.num);//找存档 
			if(cur_rec_p2!=NULL){
				find = TRUE;
				classic_rec_state[1]=1;
				//得分由整型化为字符，前面补0 
				ScoreTrans(ch_score, cur_rec_p2->score); 
				//画存档
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"普 通 ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"继续",'Z',"White");
				SetPenColor(DEFAULT_COLOR);
			}
			
			//困难模式 
			cur_rec_p3 = SearchRecord(2,cur_user.num);//找存档 
			if(cur_rec_p3!=NULL){
				find = TRUE;
				classic_rec_state[2]=1;
				//得分由整型化为字符，前面补0 
				ScoreTrans(ch_score, cur_rec_p3->score); 
				//画存档
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"困 难 ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"继续",'Z',"White");
				SetPenColor(DEFAULT_COLOR);
			}
			
			if(!find){
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w- 2*tj,th,1,"这里空空如也",'Z',"Black");
			}
			//RecordFaceButtonResp(0);
			
			break;
	}
	//free(cur_rec_p1);
	
}

void RecordFaceButtonResp(int mode, double mx, double my)
{
	double w, h;//宽，高 
	double th, tj;//行宽,间距
	double x, y;//存档输出起始位置 
	double w1, w2, w3;
	int but_resp_pos = -1;//按钮响应存档位置,-1未响应
	int i,j; 
	
	switch(mode){
		case 0:
			w = 5.0; h = 2.75;
			th = 0.35 ; tj = 0.2;
			w1 = 1.5; w2 = w - 1.0 - 2*tj - w1;
			x = cx-w/2.0+tj;
			y = cy+h/2.0-3*th-2*tj;
			for(i=0;i<3;i++){
				if(InButton(x+w1+w2+tj,y-i*(th+tj),0.8,th,mx,my)) but_resp_pos = i;
			}
			for(i=0;i<3;i++){
				if(classic_rec_state[i]){
					if(but_resp_pos==0){
						switch(i){
							case 0://跳转到简单模式 
								classic_mode_state = 1;//经典模式难度
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //关闭窗口GUI 
								ContinRecord(i+1,cur_rec_p1);//进入游戏
								free(cur_rec_p1);
								break;
							case 1://跳转到简单模式 
								classic_mode_state = 2;//经典模式难度
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //关闭窗口GUI 
								ContinRecord(i+1,cur_rec_p2);//进入游戏
								free(cur_rec_p2);
								break;
							case 2://跳转到简单模式 
								classic_mode_state = 3;//经典模式难度
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //关闭窗口GUI 
								ContinRecord(i+1,cur_rec_p3);//进入游戏
								free(cur_rec_p3);
								break; 		
						}
						break;	
					}else {
						but_resp_pos--;
					}
				}
			}
			break;
	}
}

void NoBuild()
{
	SetPointSize(26);
	SetStyle(1);
	drawLabel(cx-1.0,cy+0.2,"敬 请 期 待");
	SetPointSize(13);
	SetStyle(0);
}



