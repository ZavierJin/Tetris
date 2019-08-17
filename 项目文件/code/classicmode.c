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
#include "extgraph.h" 

#include "commonuse.h"
#include "shapebasic.h"  
#include "user.h"
#include "record.h"
#include "classicmode.h"

//#define VK_P 0x50 

#define DEFAULT_COLOR "Black" //Ĭ����ɫ 

#define TIMER_BLINK1000 1     //1000ms��ʱ��ʱ���־��
//#define TIMER_BLINK100 2     //100ms��ʱ��ʱ���־��

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
	����ģʽ 
***************************************************************/

static double cx,cy;//������������ 

static SHAPE temshape;//�ƶ�����״
static int nextid = -1; //��һ����״��id��-1����δ��ʼ��

static bool isbegin = FALSE;
static bool ispause = FALSE;//�Ƿ���ͣ 
static bool islose = FALSE;//��Ϸ�Ƿ���� 

static bool ispop[POPUP_TOTAL+1];//����,0�Ŵ����Ƿ��е�����1�ſ�ʼ����1�ŵ���
static int sur_state = 1;
//����״̬��0δ��ֵ��1��Ϸ�У�2��ͣ��3������4���� 

static int sco = 0;//���η��� 

static int cur_state = 0;//��ǰ�Ѷ� 
static int cho_state = 0;//���ѡ���л��Ѷ� 

static int mseconds = 250; //1000ms��ʱ�� 
//const int mseconds100 = 100;  //100ms��ʱ�� 


//�ⲿ���ã����ڽ�����ת���û��浵�� 
extern struct user cur_user;
extern int cur_page;
extern InitMyGui();//���潻������

void CloseClassicMode();

extern double GetFontHeight();
extern double ScaleXInches(int x);//�ⲿ���ý�������������תΪӢ��ĺ��� 
extern double ScaleYInches(int y);

static void InitClassicWindow();//��ʼ����Ϸ���ں��� 
static void MouseEventProcess(int x, int y, int button, int event);//�����Ϣ�ص����� 
static void TimerEventProcess(int timerID);//��ʱ����Ϣ�ص�����
static void KeyboardEventProcess(int key, int event);//������Ϣ�ص�����
static void RefreshWindow();//ˢ����Ϸ���ں��� 
static void DrawNextShape();//��ʾ��һ����״���� 
static bool IsLose();//�ж���Ϸ�Ƿ�������� 

static void PrintScore();//��ʾ�������� 
static void PrintHelp();//��ʾ��������
static void PrintTitle();
static void BeforeGame();

static void GameBegin(); //��Ϸ��ʼ����
static void GameEnd(bool finish); //��Ϸ��������,finish��¼�Ƿ����  

static void MyButton(int id);//���ư�ť����

static void PopupWindow(int id);//���� 

static void UpdateScore();//���³ɼ� 
static void RankingList();//���а���� 
static void PrintRankingList(char name[][16], const int score[],int ra); //�������а� 

static void SaveRecord();//�浵 
static void drawMenu();//���˵�����Ӧ��� 
static void drawState(); //��״̬�� 

void ContinRecord(int state, struct Record *cur_rec)
{
	int i,j;
	InitGraphics();
	InitGUI();
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape(cx, cy);//��ʼ����״ 
	
	switch(state){
		case 1:mseconds = 900;break;
		case 2:mseconds = 450;break;
		case 3:mseconds = 250;break;
		default:exit(-1);break;
	}
	
	cur_state = state;//��ǰ�Ѷ� 
	
	registerMouseEvent(MouseEventProcess);      // ���
	
	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//ע�ᶨʱ����Ϣ�ص�����
	
	startTimer(TIMER_BLINK1000, mseconds);//1000ms��ʱ������
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//������ʼ�� 
	
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
	MyButton(RESTART_BUTTON); //���¿�ʼ��ť
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
	cur_state = state;//��ǰ�Ѷ�
	
	registerMouseEvent(MouseEventProcess);      // ���
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//������ʼ�� 
	
	InitClassicWindow();
	

	//InitConsole();
	//printf("%.4f,%.4f",cx,cy);
	
}

void CloseClassicMode()
{
	int i,j;
	//��ʼ�����ݣ���ֹ���� 
	nextid = -1;
	sco = 0;
	isbegin = FALSE;
	ispause = FALSE;
	islose = FALSE;
	
	//��ʼ�����ڣ���ֹ��Ϸ���� 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			if(i==0 || i==WINDOW_WIDTH+1 || j==0){//ǽ�ڵ�Ԫ�� 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	
		}
	
	GameEnd(FALSE);//�رռ�ʱ�ͼ��̺���
	cancelMouseEvent(MouseEventProcess);
	 
}

static void InitClassicWindow()
{
	
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	InitShape(cx, cy);//��ʼ����״ 
	
	
	MyButton(BEGIN_BUTTON);
	BeforeGame();
}

static void GameBegin()
{
	int i,j; 

	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//ע�ᶨʱ����Ϣ�ص�����
	
	startTimer(TIMER_BLINK1000, mseconds);//1000ms��ʱ������
//	startTimer(TIMER_BLINK100, mseconds100);//100ms��ʱ������
	
	//��ʼ�����ڵ�Ԫ�� 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			if(i==0 || i==WINDOW_WIDTH+1 || j==0){//ǽ�ڵ�Ԫ�� 
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
	SetEraseMode(TRUE);//�����ϴε÷֣���ֹ��Ӱ 
	PrintScore();
	SetEraseMode(FALSE);*/
	sco = 0;//���η���
	PrintScore();
	
	DrawFrame();
	PrintHelp();
	
	
	
	//drawLabel(cx+3.3, cy-3.0, maker);
	
	MyButton(OUT_PAUSE_BUTTON); 
	MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //���¿�ʼ��ť
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
		SetEraseMode(TRUE); //�����������ͣ�ʹ浵��ť 
		drawRectangle(cx-4.5-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//��ֹ�����ɾ���w,h�Ѿ�ֱ�Ӵ��룩 
		drawRectangle(cx+3.4-0.01, cy-2.3-0.01, 1.0+0.02, 0.5+0.02, 1);//��ֹ�����ɾ� 
		SetEraseMode(FALSE);
		
		 
	}
} 

static void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;//��ǰ�������
	
	double h = 0.5;  // �ؼ��߶�
	double w = 1.0;

	uiGetMouse(x,y,button,event); //GUI��ȡ���
	
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
							CloseClassicMode();//�˳���Ϸģʽ 
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
					}
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
					}
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
					else if(ispop[5]){//5�ŵ������л��Ѷ� 
						if(InButton(cx-1.2, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[5] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();
							InitClassicMode(cho_state);
						}else if(InButton(cx+0.4, cy-0.6, 0.8, 0.4, mx, my)){//�� 
							ispop[5] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							 
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.81, 4.1, 2.02, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}	
					}
					else if(ispop[6]){//6�ŵ�������ǰ�����Ѷ� 
						if(InButton(cx-1.4, cy-0.4, 1.0, 0.4, mx, my)){//���� 
							ispop[6] = FALSE;
							ispop[0] = FALSE;
							ispause = FALSE;//������Ϸ
							
							SetEraseMode(TRUE); //�������� 
							drawRectangle(cx-2.05, cy-0.61, 4.1, 1.62, 1);//��ֹ�����ɾ� 
							SetEraseMode(FALSE);
							RefreshWindow();
							MyButton(OUT_PAUSE_BUTTON); 
						}else if(InButton(cx+0.4, cy-0.4, 1.0, 0.4, mx, my)){//�˳� 
							ispop[6] = FALSE;
							ispop[0] = FALSE;
							CloseClassicMode();//�˳���Ϸ 
							InitMyGui();
							cur_page = 3;//��ת
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
						}
						if(InButton(cx-4.5, cy-2.3, w, h, mx, my)){//�浵��ť,����ͣ 
							ispause = TRUE;
							PopupWindow(SAVE_TIPS_POPUP);
							ispop[2] = TRUE;
							ispop[0] = TRUE;	
						}
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
	
	if(isbegin || islose) {
		drawMenu();
		drawState();
	}
}


static void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI��ȡ����
	
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
						temshape = prishape[nextid];//���»�����ͼ�� 
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
	
		
	strcpy(tips,"��ǰ״̬��");
	switch(sur_state){
		case 1:strcat(tips,"��Ϸ��");break;
		case 2:strcat(tips,"��ͣ��");break;
		case 3:strcat(tips,"����");break;
		case 4:strcat(tips,"��Ϸ����");break;
		default:strcat(tips,"����");break;
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
	static char * menuListClose[] = {"         ""��    ��",  
		"    ""�浵  |  Ctrl-T", // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
		"    ""����  |  Ctrl-R",
		"    ""�˳�  |  Ctrl-E"};
	static char * menuListState[] = {"         ""��    ��",
		"    ""��  |  Ctrl-F", // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
		"    ""��ͨ  |  Ctrl-G",
		"    ""����  |  Ctrl-H"};
	/*	
	static char * menuListHelp[] = {"�鿴",
		"����ս��  | Ctrl-M",
		};*/ 
	static char * selectedLabel = NULL;
	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = 2*cy;
	double h = fH*1.5; // �ؼ��߶�
	double w; // �ؼ����
	double wlist;
	double xindent = 2*cx/20; // ����
	int    selection;

	w = wlist = 1.65;
	
	SetEraseMode(TRUE);
	drawRectangle(x,y-4*h-0.01,cx-1.55,4*h+0.02,1);
	SetEraseMode(FALSE);
	
	setMenuColors("Dark Gray","White","Yellow","Red",1); 
	// ���� �˵�
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListClose, sizeof(menuListClose)/sizeof(menuListClose[0]));
	switch(selection){
		case 1://�浵 
			if(!islose){
				ispause = TRUE;
				PopupWindow(SAVE_TIPS_POPUP);
				ispop[2] = TRUE;
				ispop[0] = TRUE;
			} 
			break;
		case 2://���� 
			if(!islose){
				ispause = TRUE;
				PopupWindow(RESTART_POPUP);
				ispop[4] = TRUE;
				ispop[0] = TRUE;
			}else{
				GameBegin();
				isbegin = TRUE;//��ʼ״̬�л�
			}
			break;
		case 3://�˳� 
			if(!islose){
				ispause = TRUE;
				PopupWindow(RETURN_POPUP);
				ispop[1] = TRUE;
				ispop[0] = TRUE;
			}else{
				CloseClassicMode();//�˳���Ϸģʽ 
				InitMyGui();
				cur_page = 3;//��ת
			}
			break;
	}
	// �л��Ѷ� �˵�
	selection = menuList(GenUIID(0),x+w,  y-h, w, wlist,h, menuListState,sizeof(menuListState)/sizeof(menuListState[0]));
	if(selection>0){
		cho_state = selection;//ѡ���Ѷ� 
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
	int conclean = 0;//����������� 
	//�����Ϸ���� 
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");	
		}
	PrintScore();
	//�ж��Ƿ����һ�� 
	for(j=1;j<WINDOW_HEIGHT+1;j++){
		isclean=TRUE;
		for(i=1;i<WINDOW_WIDTH+1;i++){
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
			sco += 2 * conclean - 1; 
			conclean = 0;
		}
	}
	//���»�����Ϸ���� 
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
	if(nextid!=-1){//���֮ǰ�л������Ȳ�����һ����״ 
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
	drawLabel(cx+2.2, cy+2.6, "��һ��:");
	SetStyle(0);
}	

static bool IsLose()
{
	char over[]="�� Ϸ �� ��";
	char label_color[]= "Red";
	int i;
	//�жϴ��ڵ�Ԫ���Ƿ񳬳������ߣ���������Ϸ���� 
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
	return islose;//�����Ƿ���� 
}

static void BeforeGame()
{
	int i; 
	char* tips[5]={
		"�ǳ�����ľ���ģʽ",
		"ƴ�������������ٻ�������", 
		"��ʱ�����ƣ��Ϳ����ܳŶ��",  
		"�������跽���",
		"F1 ����ͣ��Ϸ", 
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
	char tips[]="��ǰ�÷֣�";
	char total[sizeof(tips)+SCORE_MAX_CAP+1];
	static char score[SCORE_MAX_CAP+1];
	int temsco = sco;
	int i;
	//�÷������ͻ�Ϊ�ַ���ǰ�油0 
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
	char title[]="����ģʽ���򵥣�"; 
	switch(cur_state){
		case 1:strcpy(title,"����ģʽ���򵥣�");break;
		case 2:strcpy(title,"����ģʽ����ͨ��");break;
		case 3:strcpy(title,"����ģʽ�����ѣ�");break;
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
		" | ��� | ����",
		" | �Ҽ� | ����",
		" | �ϼ� | ˳ʱ��ת",
		" | �¼� �� �ո� | ׹��", 
		" | F1�� | ��ͣ" 
	};
	SetStyle(1);
	drawLabel(cx-4.7, cy+1.0+0.35, "����ָ�ϣ�"); 
	SetStyle(0);
	for(i=0;i<5;i++){
		drawLabel(cx-4.7, cy+1.0-i*0.3, helptips[i]);
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
	struct user *head, *p, *pnext;
	bool needmo = FALSE;
	head = p = pnext = NULL;
	
	if(cur_user.score[cur_state-1]<sco){ //�Ƚ��Ƿ񳬹���ǰģʽ��ѳɼ� 
		cur_user.score[cur_state-1] = sco;
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
	
	RankingFile(cur_state-1,name,score,0);//�������а�
	cur_sco = cur_user.score[cur_state-1]; 
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
	if(ismod) RankingFile(cur_state-1,name,score,1);//�޸����а� 
	
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
	
	
	cex = cx; cey = cy-0.58;
	w = 4.0;  h = 5.8; 
	th = 0.35 ; tj = 0.2;
	w1 = 0.9; w2 = 1.8; w3 = w-2*tj-w1-w2;
	
	//�������ֽ��� 
	SetEraseMode(TRUE);
	drawRectangle(0,cy*2-4*0.3-0.01,cx-1.55,4*0.3+0.02,1);
	drawRectangle(cx-2.0,cy-3.5,w,7.0,1);
	drawRectangle(cx+2.1,cy+0.8,2.5,2.9,1);
	SetEraseMode(FALSE);
	SetPenColor("Black");
	PrintTitle();//��ֹ������ 
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

static void SaveRecord()
{
	struct Record cur_rec;
	struct Record *head=NULL;
	int i,j;
	
	cur_rec.num = cur_user.num;//�û���� 
	cur_rec.score = sco;//��ǰ�÷�
	
	//���ڴ浵 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			cur_rec.window2[i][j]=window[i][j];
		}
	
	cur_rec.sh = temshape;//��ǰ��״
	cur_rec.nextid = nextid;//��һ����״���
	
	cur_rec.next = NULL;
	
	AddRecord(cur_state-1, &cur_rec);//�����´浵

}

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
		case CHANGE_POPUP:
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
			drawBox(x,y,w-2*tj,th,1,"��ǰ��Ϸ��δ���棬ȷ��Ҫ�л��Ѷ���",'L',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.2,y,0.8,th,1," �� ",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,0.8,th,1," �� ",'Z',"White");
			break;
		case NO_CHANGE_POPUP:
			w = 4.05; h = 1.6;
			x = cx-w/2.0+tj;
			y = cy+0.2+h/2.0-th-tj;
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy+0.2-h/2.0,w,h,1);
			//���ı�//ǰ��8���ո���������֣�18������һ�� 
			SetPenColor("White");
			drawBox(x,y,w-2*tj,th,1,"��ǰ�����Ѷ�",'Z',"Black");
			//����ť 
			y -= (th+2*tj);
			SetPenColor("Dark Gray");
			drawBox(cx-1.4,y,1.0,th,1,"����",'Z',"White");
			SetPenColor("Dark Gray");
			drawBox(cx+0.4,y,1.0,th,1,"�˳�",'Z',"White");
			break;
	}
	
	SetPenColor(DEFAULT_COLOR);
}

 

