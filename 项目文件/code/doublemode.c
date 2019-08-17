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

#include "imgui.h" //�򵥽��������� 

#include "commonuse.h"
#include "user.h"
#include "doublemode.h"
//#include "record.h"

/***************************************************************
	˫��ģʽ 
***************************************************************/


/*****************************************/

#define WINDOW_WIDTH_2 18 //��Ϸ���ں���Ԫ�����  
#define WINDOW_HEIGHT_2 21//��Ϸ��������Ԫ����� 


static struct WIN window[WINDOW_WIDTH_2+2][WINDOW_HEIGHT_2+1];


static SHAPE prishape[2][TOTAL_SHAPE];//Ԥ��7����״


static void InitShape1(double gcx, double gcy);//��ʼ��ͼ�κ��� 
static void InitShape2(double gcx, double gcy);//��ʼ��ͼ�κ��� 
static void DrawFrame();//���ƿ�ܺ��� 



static void DrawShape(const SHAPEPTR shapeptr);//����ͼ�κ���
static void DrawBlock(double x, double y, const char* color);//���Ʒ��麯��,�����Ǵ��ڽ������� 
 
static void ShapeDrop_2(SHAPEPTR shapeptr1,SHAPEPTR shapeptr2);//ͼ������1���� 
static void ShapeMove_2(SHAPEPTR shapeptr1, int dir,SHAPEPTR shapeptr2);//ͼ�������ƶ�������dir1��1���� ��-1���󣬲��ж�ͼ���Ƿ�����һ����ײ 
static void ShapeSpin_2(SHAPEPTR shapeptr1, int dir1,SHAPEPTR shapeptr2);//ͼ����ת������dir1��1˳ʱ�� ��-1��ʱ�룬���ж��Ƿ�����һ����ײ 
static bool StraDrop_2(SHAPEPTR shapeptr1,SHAPEPTR shapeptr2);//ͼ�ε��䵽�׺��� 
 
static int XtransMyX(double x);//����ת����������������ת��Ϊ��Ϸ�������� 
static int YtransMyY(double y);
static double MyXtransX(int mx);//��Ϸ��������ת��Ϊ�������� 
static double MyYtransY(int my);
static void IsKnock(SHAPEPTR shapeptr);
static bool IsBottom(SHAPEPTR shapeptr);//�ж�ͼ���Ƿ񵽵׺��� 

static void CleanLine(int k);//���һ�к�����k������Ϸ����y���� 

static void AvoidFric(const SHAPEPTR shapeptr);//������״Ħ���������麯�� 


/******************************/


//#define VK_P 0x50 

#define DEFAULT_COLOR "Black" //Ĭ����ɫ

#define TIMER_BLINK_1 1     //1000ms��ʱ��ʱ���־��
#define TIMER_BLINK_2 2
#define TIMER_BLINK 3       //��Ϸ��ʱ��ʱ���־��

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


static double cx,cy;//������������ 

static bool ispop[POPUP_TOTAL+1];//����,0�Ŵ����Ƿ��е�����1�ſ�ʼ����1�ŵ��� 

static int Chap_id = 0;//��ǰ�ؿ� 

static SHAPE temshape1;//�ƶ�����״
static SHAPE temshape2;
static int nextid1 = -1; //��һ����״��id��-1����δ��ʼ��
static int nextid2 = -1; //��һ����״��id��-1����δ��ʼ��

static int sco = 0;//���η��� 
static int self_sco[2]={0,0};//���Ե÷� 

static int drop_id=-1;//��ǰ���䵽�׵����id��0�����һ�� 

static int mseconds=1000; 
static int Chap_sco=5;//Ŀ��÷� 
static int sum_seconds=120000; //����ʱ
 
static const int s_second = 1000;  //1s��ʱ�� 


static bool isbegin = FALSE;//�Ƿ�ʼ 
static bool ispause = FALSE;//�Ƿ���ͣ 
static bool islose = FALSE;//��Ϸ�Ƿ���� 





//�ⲿ���ã����ڽ�����ת���û��浵�� 
extern struct user cur_user;
extern int cur_page;
extern InitMyGui();//���潻������


static void CloseDoubleMode();//�ر�˫��ģʽ 

extern double ScaleXInches(int x);//�ⲿ���ý�������������תΪӢ��ĺ��� 
extern double ScaleYInches(int y);

static void InitDoubleWindow();//��ʼ����Ϸ���ں��� 
static void MouseEventProcess(int x, int y, int button, int event);//�����Ϣ�ص����� 
static void TimerEventProcess(int timerID);//��ʱ����Ϣ�ص�����
static void KeyboardEventProcess(int key, int event);//������Ϣ�ص�����
static void RefreshWindow();//ˢ����Ϸ���ں��� 
static void DrawNextShape1();//��ʾ��һ����״���� 
static void DrawNextShape2();
static bool IsLose1();//�ж���Ϸ�Ƿ�������� 
static bool IsLose2();//ʱ�䵽���Ƿ񵽴���� 

static void RefreshChapter();//����ˢ�½���ؿ����� 
static void PrintTime();//��ʾ����ʱ����
static void PrintChap();//��ʾ��ǰ�ؿ� 

static void PrintScore();//��ʾ�������� 
static void PrintHelp();//��ʾ��������
static void PrintTitle();//��ʾ���� 

static void GameBegin(); //��Ϸ��ʼ����
static void GameEnd(bool finish); //��Ϸ��������,finish��¼�Ƿ����  

static void MyButton(int id);//���ư�ť����

static void PopupWindow(int id);//���� 

static void UpdateScore();//���³ɼ� 
static void RankingList();//���а���� 
static void PrintRankingList(char name[][16], const int score[],int ra); //�������а� 

static void GrowBlock();//�������� 
static void BeforeGame();//��Ϸ����ǰ��ʾ 

static void BeforeGame()
{
	int i; 
	char* tips[7]={
		"�����ִ̼���˫��ģʽ",
		"ʱ�䡢�÷֡��߶ȵȶ�����ս", 
		"���������Ĵ����ڼ�����",
		"�������ǵ�����\"����\"��ȥս����", 
		"�������跽�����WASD��",
		"F1 ����ͣ��Ϸ", 
		"P.S. С����Щ��ɫ�ļһ�",
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
	int dir[4][2]={{0,1},{1,0},{-1,0},{0,-1}};//0�ϣ�1�ң�2��
	int tem[WINDOW_WIDTH_2+2][WINDOW_HEIGHT_2+1];//�ݴ�֮ǰ��������λ�� 
	int vaca = 0;
	
	//���������߶� 
	if(Chap_id==0) cur_high=0;
	else if(Chap_id<=1) cur_high=2;
	else if(Chap_id<=2) cur_high=4;
	else if(Chap_id<=3) cur_high=6;
	else if(Chap_id<=5) cur_high=8;
	else cur_high=9;
	
	//�ݴ� 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 3){//�������� 
				tem[i][j]=1;
			}else
				tem[i][j]=0;
		}
		
	//�������� 
	root = RandomNum(18)+1;//1-18����� 
	while(window[root][1].isoccu){
		root = RandomNum(18)+1;
	}
	tem[root][1]=1;
	
	state = RandomNum(4);//4��֮һ���ʳ����� 
	//��ǽ����
	if(state==0 && cur_high!=0){
		vaca=1;
		x = RandomNum(2);
		if(x==0) x = 18;
		for(j=1;j<=cur_high && window[x][j].isoccu;j++) continue;
		if(j==cur_high+1){//��������� 
			x = (x==1)? 18 : 1;//�л� 
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
	
	//�������� 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 3){//��������
				px = i; py = j;
				for(k=0;k<4;k++){//�ĸ����� 
					
					x = px + dir[k][0];
					y = py + dir[k][1];
					if(!window[x][y].isoccu){//û��ռ�ã����� 
						tem[x][y]=1; 
						break;
					}
				} 
			}
		}
	


	//���� 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(tem[i][j]){//�������� 
				vaca = 0;//���л��м�����ȱ 
				for(k=1;k<WINDOW_WIDTH_2+1;k++){
					if(!window[k][j].isoccu) vaca++;
				}
				if(vaca>1){//��ȱ����һ�������� 
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
	
	
	
	registerMouseEvent(MouseEventProcess);      // ���
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//������ʼ�� 
	
	InitDoubleWindow();
	

	//InitConsole();
	//printf("%.4f,%.4f",cx,cy);
	
}

void CloseDoubleMode()
{
	int i,j;
	//��ʼ�����ݣ���ֹ���� 
	nextid1 = -1;
	nextid2 = -1;
	sco = 0;
	isbegin = FALSE;
	ispause = FALSE;
	islose = FALSE;
	
	//��ʼ�����ڣ���ֹ��Ϸ���� 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			if(i==0 || i==WINDOW_WIDTH_2+1 || j==0){//ǽ�ڵ�Ԫ�� 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
			window[i][j].type = 0;//��ͨ���� 
		}
	
	GameEnd(FALSE);//�رռ�ʱ�ͼ��̺���
	cancelMouseEvent(MouseEventProcess);
	 
}

static void InitDoubleWindow()
{
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape1(cx, cy);//��ʼ����״ 
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
	sum_seconds=120000;//��ʼ������ʱ2���� 
	
	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//ע�ᶨʱ����Ϣ�ص�����
	
	startTimer(TIMER_BLINK_1, mseconds);//1000ms��ʱ������
	startTimer(TIMER_BLINK, s_second);//1s��ʱ�� 
	
	//��ʼ�����ڵ�Ԫ�� 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			if(i==0 || i==WINDOW_WIDTH_2+1 || j==0){//ǽ�ڵ�Ԫ�� 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
			window[i][j].type = 0;//��ͨ���� 
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
	sco = 0;//���η���
	self_sco[0] = self_sco[1] = 0;
	
	PrintScore();
	PrintTitle();
	PrintTime();
	PrintHelp();
	PrintChap();
	
	MyButton(OUT_PAUSE_BUTTON); 
	//MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //���¿�ʼ��ť
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
		MyButton(RESTART_BUTTON); //���¿�ʼ��ť
		
		UpdateScore();
		RankingList();
		/*
		SetEraseMode(TRUE); //�����������ͣ�ʹ浵��ť 
		drawRectangle(cx-4.5-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//��ֹ�����ɾ���w,h�Ѿ�ֱ�Ӵ��룩 
		drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//��ֹ�����ɾ� 
		SetEraseMode(FALSE);
		*/
		 
	}
} 

static void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;//��ǰ�������
	
	double h = 0.5;  // �ؼ��߶�
	double w = 1.0;

	
	mx = ScaleXInches(x);//����תӢ�磬��������intתdouble 
	my = ScaleYInches(y);
	//uiGetMouse(x,y,button,event); //GUI��ȡ���
	switch(event){
		case BUTTON_DOWN://�����걻���� 
			if(button==LEFT_BUTTON){
				//����״̬ 
				if(ispop[0]){
					if(ispop[1]){//1�ŵ��������� 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[1] = FALSE;
							ispop[0] = FALSE;
							CloseDoubleMode();//�˳���Ϸģʽ 
							InitMyGui();
							cur_page = 3;//��ת
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[1] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							 
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}/*
					else if(ispop[2]){//2�ŵ������浵��ʾ 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							SaveRecord();//�浵 
							ispop[2] = FALSE;
							
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							
							ispop[3] = TRUE; //�浵�ɹ����� 
							PopupWindow(SAVE_SUCC_POPUP);
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[2] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}
					}
					else if(ispop[3]){//3�ŵ������浵�ɹ� 
						if(InButton(cx-1.4, cy-0.4, 1.0, 0.4, mx, my)){//���� 
							ispop[3] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.61, 4.1, 1.62, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}else if(InButton(cx+0.4, cy-0.4, 1.0, 0.4, mx, my)){//�˳� 
							ispop[3] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();//�˳���Ϸ 
							InitMyGui();
							cur_page = 3;//��ת
						}
					}*/
					else if(ispop[4]){//4�ŵ��������¿�ʼ 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[4] = FALSE;
							ispop[0] = FALSE;
							GameEnd(FALSE); 
							GameBegin();
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[4] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							 
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}
				}
				//δ����״̬ 
				else{
					if(!isbegin){ //δ��ʼ״̬
						if(!islose){//��һ�ν��� 
							if(InButton(cx-w/2.0, cy-3.0, w, h, mx, my) ) {//��ʼ��ť 
								GameBegin();
								isbegin = TRUE;//��ʼ״̬�л�
							}
						}else{
							if(InButton(cx+3.4, cy-3.0, w, h, mx, my) ) {//���¿�ʼ��ť 
								GameBegin();
								isbegin = TRUE;//��ʼ״̬�л�
							}
							if(InButton(cx-4.5, cy-3.0, w, h, mx, my)){//�˳���ť������ģʽ��Ϸ
								CloseClassicMode();//�˳���Ϸģʽ 
								InitMyGui();
								cur_page = 3;//��ת
							}
						}
					}else{//��ʼ״̬
						if(InButton(cx-4.5, cy-3.0, w, h, mx, my)){//�˳���ť������ģʽ��Ϸ
							ispause = TRUE;
							PopupWindow(RETURN_POPUP);
							ispop[1] = TRUE;
							ispop[0] = TRUE;
						}
						if(InButton(cx+3.4, cy-2.3, w, h, mx, my) && !ispause){//��ͣ��ť
							ispause = TRUE; 
							MyButton(PAUSE_BUTTON);//������ť 
							SetEraseMode(TRUE); //�����������ͣ��ť 
							drawRectangle(cx+3.4-0.01, cy-2.3-0.01, w+0.02, h+0.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
						}/*
						if(InButton(cx-4.5, cy-2.3, w, h, mx, my)){//�浵��ť,����ͣ 
							ispause = TRUE;
							PopupWindow(SAVE_TIPS_POPUP);
							ispop[2] = TRUE;
							ispop[0] = TRUE;	
						}*/
						if(InButton(cx+3.4, cy-3.0, w, h, mx, my) ) {//���¿�ʼ��ť 
							ispause = TRUE;
							PopupWindow(RESTART_POPUP);
							ispop[4] = TRUE;
							ispop[0] = TRUE;
						}
					}
					
					if(ispause) {//��ͣ״̬ 
						if(InButton(cx-w/2.0, cy, w, h, mx, my) ) {//������ť 
							RefreshWindow();
							ispause = FALSE;//��ͣ״̬�л�
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
	//��Ӧ��ͣ�л�����
	if(event==KEY_DOWN && key==VK_F1 && !ispop[0]) {
		ispause = !ispause;//��ͣ״̬�л� 
		if(ispause){
			MyButton(PAUSE_BUTTON);
			SetEraseMode(TRUE); //�����������ͣ��ť 
			drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//��ֹ�����ɾ���w,h�Ѿ�ֱ�Ӵ��룩 
			SetEraseMode(FALSE);
		}else{
			RefreshWindow();
			MyButton(OUT_PAUSE_BUTTON); 
		}
	} 
		
	if(!ispause){//���δ��ͣ�ͽ�������Ӧ��Ϸ���� 
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
							temshape1 = prishape[0][nextid1];//���»�����ͼ�� 
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
							temshape2 = prishape[1][nextid2];//���»�����ͼ�� 
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
		case TIMER_BLINK_1://��׹����ʱ 
			if(!IsLose1()){//û���� 
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
			
		case TIMER_BLINK://1�룬����ʱˢ�� 
			if(sum_seconds>0){//���ʱ��û�� 
				if((sum_seconds+2000)%5000 == 0) //5��������������һ�� 
					GrowBlock();
				sum_seconds-=1000;	//��һ�� 
				PrintTime();
			}else{//���ʱ�䵽�� 
				if(!IsLose2()){//�ж��Ƿ񵽴���� 
					RefreshChapter();
				}else{//���� 
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
	int conclean = 0;//����������� 
	int c_sco=0;//����ˢ�µ÷�
	 
	//�����Ϸ���� 
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");	
		}
	PrintScore();
	//�ж��Ƿ����һ�� 
	for(j=1;j<WINDOW_HEIGHT_2+1;j++){
		isclean=TRUE;
		for(i=1;i<WINDOW_WIDTH_2+1;i++){
			if(!window[i][j].isoccu){//�����һ����һ����Ԫ��Ϊ��ռ�ã�����в���Ҫ��� 
				isclean=FALSE;
				break;
			}
		}
		if(isclean){
			CleanLine(j);
			j--; //��Ϊ�����֮������һ�������ˣ����Դ���Ҫ�����ж� 
			conclean += 1;
		}else if(conclean!=0){
			c_sco += 2 * conclean - 1; 
			conclean = 0;
		}
	}
	sco+=c_sco;
	self_sco[drop_id]+=c_sco;
	drop_id=-1;//��
	
	//���»�����Ϸ���� 
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
	if(nextid1!=-1){//���֮ǰ�л������Ȳ�����һ����״ 
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
	drawLabel(cx+3.2, cy+2.3, "��һ����");
	SetStyle(0);

}	

static void DrawNextShape2()
{
	SHAPEPTR shapeptr2;
	int i;
	if(nextid2!=-1){//���֮ǰ�л������Ȳ�����һ����״ 
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
	drawLabel(cx-4.8, cy+2.3, "��һ����");
	SetStyle(0);
}	


static bool IsLose1()
{
	int i;
	
	
	//�жϴ��ڵ�Ԫ���Ƿ񳬳������ߣ���������Ϸ���� 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		if(window[i][WINDOW_HEIGHT_2-4].isoccu) {
			islose = TRUE;
			break;
		}
	

	if(islose){
		GameEnd(TRUE); 
		
		SetPenColor("Red");
		drawLabel(cx-0.45, cy+3.1, "�� Ϸ �� ��");
		
		isbegin = FALSE;
		
		return TRUE;
	}else 
		return FALSE;//�����Ƿ���� 
}
static bool IsLose2()
{
	
	int i;
	
	if(sco< Chap_sco){//δ�ﵽĿ��� 
		islose = TRUE;
	}

	if(islose){
		GameEnd(TRUE); 
		
			
		SetPenColor("Red");
		drawLabel(cx-0.45, cy+3.1, "�� Ϸ �� ��");
		
		isbegin = FALSE;
		
		
		return TRUE;
	}else 
		return FALSE;//�����Ƿ���� 
}

static void PrintTitle()
{
	
	SetStyle(1);
	SetPointSize(26);
	drawLabel(cx-4.8, cy+3.0, "˫��ģʽ");
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
	strcpy(total,"�ܵ÷֣�");
	ScoreTrans(score,temsco);
	strcat(total,score);
	drawLabel(cx+3.2, cy-0.8, total);
	if(sco>=Chap_sco) SetPenColor("Black"); 
	
	strcpy(self,"P1 �÷֣�"); 
	ScoreTrans(score,self_sco[0]);
	strcat(self,score);
	drawLabel(cx+3.2, cy-1.2, self);
	
	strcpy(self,"P2 �÷֣�"); 
	ScoreTrans(score,self_sco[1]);
	strcat(self,score);
	drawLabel(cx+3.2, cy-1.5, self);
	
	SetStyle(0);
	
	
}

static void PrintTime()
{
	char memo[38]="����ʱ��";
	static char sum_sec[10];
	itoa(sum_seconds/1000,sum_sec,10);
	strcat(memo,sum_sec);
	strcat(memo," ��");
	
	SetEraseMode(TRUE);
	drawRectangle(cx+3.2-0.01,cy+0.3-0.03,1.5,0.25+0.04,1);
	drawRectangle(cx-0.7-0.01,cy+3.2-0.03,1.5,0.22+0.04,1);
	SetEraseMode(FALSE); 
	
	if(sum_seconds<10000) SetPenColor("Red");//С��10���� 
	SetStyle(1);
	drawLabel(cx-0.7,cy+3.2,memo);//������ʾ 
	drawLabel(cx+3.2,cy+0.3,memo);
	SetStyle(0);
	if(sum_seconds<10000) SetPenColor("Black");
}

static void PrintHelp()
{
	int i;
	char *helptips[3] = {
		"P1�����������",
		"P2��WASD����",
		"| F1�� | ��ͣ" 
	};
	SetStyle(1);
	drawLabel(cx-4.8, cy+0.75, "����ָ�ϣ�");
	SetStyle(0);
	for(i=0;i<3;i++){
		drawLabel(cx-4.7, cy+0.45-i*0.3, helptips[i]);
	}
}





//���ƻ������ť 
static void MyButton(int id)
{
	double h = 0.5;  // �ؼ��߶�
	double w = 1.0;
	char label_color[]= "White";
	char button_color[]="Dark Gray";
	char* button_label = NULL;//��ǰ��ť��ǩ 
	
	SetPenColor(button_color);
	switch(id){
		case BEGIN_BUTTON:
			drawBox( cx-w/2.0, cy-3.0, w, h, 1, "�� ʼ", 'Z', label_color);
			break;
		case PAUSE_BUTTON:
			drawBox( cx-w/2.0, cy, w, h, 1, "�� ��", 'Z', label_color);
			break;
		case OUT_PAUSE_BUTTON:
			drawBox( cx+3.4, cy-2.3, w, h, 1, "�� ͣ", 'Z', label_color);
			break;
		case SAVE_BUTTON:
			drawBox( cx-4.5, cy-2.3, w, h, 1, "�� ��", 'Z', label_color);
			break;
		case RETURN_BUTTON:
			drawBox( cx-4.5, cy-3.0, w, h, 1, "�� ��", 'Z', label_color);
			break;
		case RESTART_BUTTON:
			drawBox( cx+3.4, cy-3.0, w, h, 1, "���¿�ʼ", 'Z', label_color);
			break;
	}
	SetPenColor(DEFAULT_COLOR); //������������ΪĬ����ɫ 
}
//�����û��ɼ� 
static void UpdateScore()
{
	struct user *head, *p, *pnext;;
	bool needmo = FALSE;
	head = p = pnext = NULL;
	
	if(cur_user.score[3]<sco){ //�Ƚ��Ƿ񳬹���ǰģʽ��ѳɼ� 
		cur_user.score[3] = sco;
		needmo = TRUE;
		head = OpenUserFile();
		if((p=SearchUser(head,cur_user.num))!=NULL){
			pnext = p->next;//��֮��ڵ����� 
			*p=cur_user;
			p->next = pnext;
		}
		ModifyUserFile(head);
		DeleteList(head);
	}
	
}

static void RankingList()
{
	bool ismod = FALSE;//�Ƿ���Ҫ�޸����а� 
	bool check = FALSE;//�Ƿ�������а� 
	char name[10][16];
	int score[10];
	int cur_sco;
	int rank=-1;//�����а������� 
	int i;
	
	
	for(i=0;i<10;i++) score[i]=0;
	
	RankingFile(3,name,score,0);//�������а�
	cur_sco = cur_user.score[3]; 
	for(i=0;i<10;i++){
		if(strcmp(name[i],cur_user.name)==0){
			if(score[i]<cur_sco){
				ismod = TRUE;
			}
			rank = i;
			check = TRUE;//���ҵ����� 
			break;
		} 
	}
	if(!check){//������а�û�д����֣��Ӻ���ǰ�� 
		for(i=9;i>=0 && score[i]<cur_sco;i--)
			continue;
		rank=i+1;
		if(rank!=10){
			check = TRUE;//�������а� 
			ismod = TRUE;
			for(i=8;i>=rank;i--){
				strcpy(name[i+1],name[i]);
				score[i+1]=score[i];
			}
			strcpy(name[rank],cur_user.name);
			score[rank]=cur_sco;
		}
	}else if(ismod){//���а������֣����ұ�֮ǰ�ߣ��ӵ�ǰλ�ü�����ǰ�� 
		for(i=rank-1;i>=0 && score[i]<cur_sco;i--){
			strcpy(name[i+1],name[i]);
			score[i+1]=score[i];
		}
		strcpy(name[i+1],cur_user.name);
		score[i+1]=cur_sco;
		rank = i+1;
	}
	
	if(!check) rank = -1;
	if(ismod) RankingFile(3,name,score,1);//�޸����а� 
	
	PrintRankingList(name,score,rank);//չʾ���а� 
}

static void PrintRankingList(char name[][16], const int score[],int ra)
{
	char* title = "�� �� ��";
	char* note1 = "�� ��"; 
	char* note2 = "�� �� ��";
	char* note3 = "�� ��";
	char rank[4];
	double cex, cey;//���а��������� 
	double x, y;
	double w, h;//���� 
	double th, tj;//�п�,���, 
	double w1, w2 ,w3;
	int i,j; 
	bool full;//���а��Ƿ��� 
	char ch_score[SCORE_MAX_CAP+1];
	int temsco;
	
	
	cex = cx; cey = cy;
	w = 4.0;  h = 5.8; 
	th = 0.35 ; tj = 0.2;
	w1 = 0.9; w2 = 1.8; w3 = w-2*tj-w1-w2;
	
	//�������ֽ��� 
	SetPenColor("White");
	drawRectangle(cx-2.0,cy-3.5,w,7.0,1);

	//�����а��ɫ 
	SetPenColor("Gray");
	drawRectangle(cex-w/2.0,cey-h/2.0,w,h,1);
	//������ 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cex-w/2.0+tj,cey+h/2.0-2*(tj+th),w-tj*2,2*th,1,title,'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	//��С���� 
	SetPenColor("White");
	drawBox(cex-w/2.0+tj,cey+h/2.0-3*(tj+th),w1,th,1,note1,'Z',"Black");
	SetPenColor("White");
	drawBox(cex-w/2.0+tj+w1,cey+h/2.0-3*(tj+th),w2,th,1,note2,'Z',"Black");
	SetPenColor("White");
	drawBox(cex-w/2.0+tj+w1+w2,cey+h/2.0-3*(tj+th),w3,th,1,note3,'Z',"Black");
	//������� 
	x = cex-w/2.0+tj;
	y = cey+h/2.0-4*(tj+th);
	full = TRUE; 
	for(i=0;i<10;i++){
		if(strcmp(name[i],"0")==0){
			full = FALSE;//���а�δ�� 
			break;	
		}
		
		if(i!=9){
			rank[0]=i+1+'0';
			rank[1]='\0';
		}else strcpy(rank,"10");
		//�÷������ͻ�Ϊ�ַ���ǰ�油0 
		temsco = score[i];
		for(j=SCORE_MAX_CAP-1;j>=0;j--){
			ch_score[j] = temsco % 10 + '0';
			temsco/=10;
		}
		ch_score[SCORE_MAX_CAP]='\0';
		//���� 
		SetPenColor("White");
		drawBox(x,y-i*th,w1,th,1,rank,'Z',"Black");
		//�û������÷� 
		SetPenColor("White");
		drawBox(x+w1,y-i*th,w2,th,1,name[i],'Z',"Black");
		SetPenColor("White");
		drawBox(x+w1+w2,y-i*th,w3,th,1,ch_score,'Z',"Black");	
		
		
		
	}
	if(!full){//������а�δ�������հ� 
		SetPenColor("White");
		drawRectangle(x,y-9*th,w-2*tj,(10-i)*th,1);
	}
	
	if(ra!=-1){//����������а��е�λ�� 
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
	
	cur_rec.num = cur_user.num;//�û���� 
	cur_rec.score = sco;//��ǰ�÷�
	
	//���ڴ浵 
	for(i=0;i<WINDOW_WIDTH_2+2;i++)
		for(j=0;j<WINDOW_HEIGHT_2+1;j++){
			cur_rec.window2[i][j]=window[i][j];
		}
	
	cur_rec.sh = temshape;//��ǰ��״
	cur_rec.nextid = nextid;//��һ����״���
	
	cur_rec.next = NULL;
	
	AddRecord(cur_state-1, &cur_rec);//�����´浵

}
*/
static void PopupWindow(int id)
{
	double w, h;//���� 
	double th, tj;//�п�,���
	double x, y;//�����ʼλ�� 

	th = 0.4 ; tj = 0.2;

	switch(id){
		case RETURN_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//���ı�//ǰ��8���ո���������֣�18������һ�� 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"���棡����",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"��ǰ��Ϸ��δ���棬ȷ��Ҫ������Ϸ��",'L',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," �� ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," �� ",'Z',"White");
			break;
		case SAVE_TIPS_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//���ı�//ǰ��8���ո���������֣�18������һ�� 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"���棡����",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"�˲������Ḳ��ԭ�д浵���Ƿ������",'L',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," �� ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," �� ",'Z',"White");
			break;
		case SAVE_SUCC_POPUP:
			w = 4.05; h = 1.6;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//���ı�//ǰ��8���ո���������֣�18������һ�� 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"  ""�浵�ɹ���",'Z',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.4,y,1.0,th,1,"����",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,1.0,th,1,"�˳�",'Z',"White");
			break;
		case RESTART_POPUP:
			w = 4.05; h = 2.0;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//���ı�//ǰ��8���ո���������֣�18������һ�� 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"���棡����",'L',"Red");
			y-=th;
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"��ǰ��Ϸ��δ���棬ȷ��Ҫ���¿�ʼ��",'L',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," �� ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," �� ",'Z',"White");
			break;
	}
	
	SetPenColor(DEFAULT_COLOR);
}

static void PrintChap()
{
	char chap1[35];
	char c_id[20];
	char c_sco[20];
	char memo[35]="Ŀ��֣�"; 

	
	strcpy(chap1,"�ؿ���"); 
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
	
	
	if(!islose){//�м䵯���ڼ���
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
	
	Chap_id++;//�ؿ� 
		
	sum_seconds=120000;//����ʱ���� 
	
	if(Chap_id<=5) //�����ٶ� 
		mseconds=1000-Chap_id*100;
	else
		mseconds=2500/Chap_id;

	Chap_sco = Chap_id*(2*Chap_id+7) +5;//Ŀ��ֶ������� 
	
	startTimer(TIMER_BLINK_1, mseconds);
	

	//��ʼ�����ڵ�Ԫ�� 
	for(i=1;i<WINDOW_WIDTH_2+1;i++)
		for(j=1;j<WINDOW_HEIGHT_2+1;j++){
			if(window[i][j].type == 0){//������������ 
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
��ֹ�;���ģʽ�����������ݣ�ֱ��ճ��
**************************************/ 

static void InitShape1(double gcx, double gcy)
{
	int i;
	
	srand((unsigned int) time(0));//����ϵͳʱ�������������
	
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
				prishape[0][i].point[0][0]=(WINDOW_WIDTH_2+6)/2; prishape[0][i].point[0][1]=WINDOW_HEIGHT_2-3;//��ת����	
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
	
	srand((unsigned int) time(0));//����ϵͳʱ�������������
	
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
				prishape[1][i].point[0][0]=(WINDOW_WIDTH_2-6)/2; prishape[1][i].point[0][1]=WINDOW_HEIGHT_2-3;//��ת����	
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
	return -1;//���������� 
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
	SetPenSize(4);//�߿��ϸ0.1*L 
	
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
	//�ж�������Ƿ����Ҫ��  
	for(i=0;i<4;i++){
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) candrop = FALSE;//��ֹ��ԭ���ı����� 
	}
	if(!candrop) *shapeptr1 = preshape;//����������䣬�����¶�ȡͼ��֮ǰ��λ�� 
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿�
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
	//��ǽ 
	IsKnock(shapeptr1);
	for(i=0;i<4;i++){
		if(!canmove) break;
		for(j=0;j<4;j++){//��ֹ������ 
			if(shapeptr1->point[i][0]==shapeptr2->point[j][0]&&shapeptr1->point[i][1]==shapeptr2->point[j][1]){
				canmove=FALSE;
				break;
			}
		
				
		}
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) canmove = FALSE;//��ֹ��ԭ���ı����� 
	}
	

	if(!canmove){
		*shapeptr1 = preshape;
	}
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿� 
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
	
	if(shapeptr1->id==0) canspin = FALSE;//���Ϊ0��ͼ���޷���ת 
	if(canspin){
		for(i=1;i<4;i++){
			rex = shapeptr1->point[i][0]-shapeptr1->point[0][0];//ͼ�θ���Ԫ���������ת���ĵ�Ԫ�񣨵�0�ţ������� 
			rey = shapeptr1->point[i][1]-shapeptr1->point[0][1];
			switch(dir1){//�ж�˳ʱ�뻹����ʱ�� 
				case 1://˳ʱ�� 
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
		for(j=0;j<4;j++){//��ֹ������ 
			if(shapeptr1->point[i][0]==shapeptr2->point[j][0]&&shapeptr1->point[i][1]==shapeptr2->point[j][1]){
				canspin=FALSE;
				break;
			}
		
				
		}
		if(window[shapeptr1->point[i][0]][shapeptr1->point[i][1]].isoccu) canspin = FALSE;//��ֹ��ԭ���ı����� 
	}
	
	if(!canspin) *shapeptr1 = preshape;
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿�
	DrawShape(shapeptr1);
	DrawShape(shapeptr2);
} 

static bool StraDrop_2(SHAPEPTR shapeptr1, SHAPEPTR shapeptr2)
{
	SHAPE preshape = *shapeptr1;
	int dropdis;//���䵽�׾��� 
	int px,py;//ͼ����һ����Ԫ������� 
	int i,j;
	bool candrop = TRUE;
	dropdis=WINDOW_HEIGHT_2;
	for(i=0;i<4 && candrop;i++){
		px=shapeptr1->point[i][0];
		py=shapeptr1->point[i][1];
		for(j=0;j<4;j++){//��ֹ������ 
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
	if(!candrop) *shapeptr1 = preshape;//����������䣬�����¶�ȡͼ��֮ǰ��λ��
	else{
		//����֮����´��ڵ�Ԫ�� 
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
	for(i=0;i<4;i++){//���ͼ���·��Ĵ��ڵ�Ԫ��ռ�ã����Ѿ����� 
		if(window[shapeptr->point[i][0]][shapeptr->point[i][1]-1].isoccu) isbot = TRUE;
	}
	if(isbot){
		for(i=0;i<4;i++){
			window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu = TRUE;
			window[shapeptr->point[i][0]][shapeptr->point[i][1]].color = shapeptr->color;
		}
		
	}
	return isbot;//�����Ƿ񵽵� 
}


static void CleanLine(int k)
{
	int i,j;
	//������У�����������ж����ƣ������һ�б�֤ȫ�ǿհף� 
	for(j=k;j<WINDOW_HEIGHT_2;j++){
		for(i=1;i<WINDOW_WIDTH_2+1;i++) 
			window[i][j]=window[i][j+1];
	}
}

//������״Ħ���������麯��
static void AvoidFric(const SHAPEPTR shapeptr)
{
	int i;
	int px,py;
	for(i=0;i<4;i++){
		px = shapeptr->point[i][0];//ͼ������һ�������ԭ���� 
		py = shapeptr->point[i][1];
		if(px>=2 && window[px-1][py].isoccu == TRUE)//��� 
			DrawBlock(MyXtransX(px-1),MyYtransY(py),window[px-1][py].color);
		if(px<=WINDOW_WIDTH_2-1 && window[px+1][py].isoccu == TRUE)//�ұ� 
			DrawBlock(MyXtransX(px+1),MyYtransY(py),window[px+1][py].color);
		if(py<=WINDOW_HEIGHT_2 && window[px][py+1].isoccu == TRUE)//���� 
			DrawBlock(MyXtransX(px),MyYtransY(py+1),window[px][py+1].color);
		if(py>=2 && window[px][py-1].isoccu == TRUE)//��� 
			DrawBlock(MyXtransX(px),MyYtransY(py-1),window[px][py-1].color);		
	}
} 








 

