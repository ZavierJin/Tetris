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
//#include "record.h"
#include "newmode.h"

/***************************************************************
	����ģʽ 
***************************************************************/

/*****************************************/

#define WINDOW_WIDTH 9 //��Ϸ���ں���Ԫ�����  
#define WINDOW_HEIGHT 21//��Ϸ��������Ԫ����� 

static struct WIN window[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];

static SHAPE prishape[TOTAL_SHAPE];//Ԥ��7����״

static void InitShape(double gcx, double gcy);//��ʼ��ͼ�κ��� 

static void DrawFrame();//���ƿ�ܺ��� 

static void DrawShape(const SHAPEPTR shapeptr);//����ͼ�κ���
static void DrawBlock(double x, double y, const char* color);//���Ʒ��麯��,�����Ǵ��ڽ������� 
 
static void ShapeDrop(SHAPEPTR shapeptr);//ͼ������1���� 
static void ShapeMove(SHAPEPTR shapeptr, int dir);//ͼ�������ƶ�������dir��1���� ��-1���� 
static void ShapeSpin(SHAPEPTR shapeptr, int dir);//ͼ����ת������dir��1˳ʱ�� ��-1��ʱ��
static void StraDrop(SHAPEPTR shapeptr);//ͼ�ε��䵽�׺��� 
 
static int XtransMyX(double x);//����ת����������������ת��Ϊ��Ϸ�������� 
static int YtransMyY(double y);
static double MyXtransX(int mx);//��Ϸ��������ת��Ϊ�������� 
static double MyYtransY(int my);

static bool IsBottom(SHAPEPTR shapeptr);//�ж�ͼ���Ƿ񵽵׺��� 

static void CleanLine(int k);//���һ�к�����k������Ϸ����y���� 

static void Drawbomb(double x,double y,char *ch);

static void DrawBlock2(SHAPEPTR sp);

static void AvoidFric(const SHAPEPTR shapeptr);//������״Ħ���������麯�� 

/*************************************/

//#define VK_P 0x50 

#define DEFAULT_COLOR "Black" //Ĭ����ɫ 
#define TYPE 4

#define TIMER_BLINK1000 1     //1000ms��ʱ��ʱ���־��
#define ft 3000 //����ʱ��Ϊ2000ms
#define TIMER_BLINK_1 1     //1000ms��ʱ��ʱ���־��
#define TIMER_BLINK_2 2
#define TIMER_BLINK 3       //��Ϸ��ʱ��ʱ���־��
#define TIMER_BLINK_f 4     //���������ʱ��ʱ���־��

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

static SHAPE temshape;//�ƶ�����״
static int nextid = -1; //��һ����״��id��-1����δ��ʼ��

static bool isbegin = FALSE;
static bool ispause = FALSE;//�Ƿ���ͣ 
static bool islose = FALSE;//��Ϸ�Ƿ���� 

static bool ispop[POPUP_TOTAL+1];//����,0�Ŵ����Ƿ��е�����1�ſ�ʼ����1�ŵ��� 

static int sco = 0;//���η��� 

//static int cur_state = 0;
 
//�ⲿ���ã����ڽ�����ת���û��浵�� 
extern struct user cur_user;
extern int cur_page;
extern InitMyGui();//���潻������

void CloseNewMode();

extern double ScaleXInches(int x);//�ⲿ���ý�������������תΪӢ��ĺ��� 
extern double ScaleYInches(int y);

static void InitNewWindow();//��ʼ����Ϸ���ں��� 
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

static void Searchbomb(int i,int j);
static void Explode(int i,int j);
static void Renew();
static void Frozen();

int Randi(int i); 

static int I=0;
struct TempBomb{
	double x,y;
}tempbomb[200];
static void VirusBlock();
static void DoubleBomb();
static void Bomb();
static void GrowBlock(); 

static int Chap_id = 0;//��ǰ�ؿ� 
static int mseconds=1000; 
static int Chap_sco=5;//Ŀ��÷� 
static int sum_seconds=120000; //����ʱ

static const int s_second = 1000;  //1s��ʱ�� 
static const int froztime =2000;
static int mseconds1 = 250; //1000ms��ʱ�� 

static bool isfrozen=FALSE;
static int frozentime=0;

static bool IsLose2();
static void RefreshChapter();
static void PrintChap();
static void PrintTime(); 
static void Virus();
static void RandomType();



int Randi(int i){
	int r;
	r=rand()%i;
	return r;
}

static void RandomType(SHAPE *shapeptr){
	int i;
	i=RandomNum(25);
	if(i<5){
		shapeptr->type=0;
		shapeptr->No=-1;
	}
	else if(i<10)
	shapeptr->type=1;
	else if(i<20)
	shapeptr->type=4;
	else
	shapeptr->type=5;
   //shapeptr->type=5;
} 


static void Virus(){
	int i,j;
	int virus[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];
	int k,px,py,x,y,t;
	int dir[4][2]={{0,-1},{1,0},{-1,0},{0,1}}; //�������� 
	
	for(i=1;i<WINDOW_WIDTH+1;i++){
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].type==5){ 
				virus[i][j]=1;
			}
			else
			virus[i][j]=0;
		}
	}
	
	for(i=1;i<WINDOW_WIDTH+1;i++){
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(virus[i][j]==1){
				px = i; py = j;
				for(k=0;k<4;k++){//�ĸ����� 
					
					x = px + dir[k][0];
					y = py + dir[k][1];
					if(window[x][y].isoccu&&window[x][y].type==0&&x<WINDOW_WIDTH+1&&x>0&&y>0&&y<WINDOW_HEIGHT+1){//��ռ�ã���Ⱦ 
						window[x][y].type=5;
						window[x][y].color="Violet";
						break;
			       }
				}
			}
		}
	}
	
	
}

static void GrowBlock()//state��1�������� 
{
	int i,j,k;
	int state;
	int root=4;
	int cur_high=0;
	int px, py, x, y;
	int dir[4][2]={{0,1},{1,0},{-1,0},{0,-1}};//0�ϣ�1�ң�2��
	int tem[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];//�ݴ�֮ǰ��������λ�� 
	int vaca = 0;
	
	if(Chap_id==0) cur_high=0;
	else if(Chap_id<=1) cur_high=2;
	else if(Chap_id<=2) cur_high=4;
	else if(Chap_id<=3) cur_high=6;
	else if(Chap_id<=5) cur_high=8;
	else cur_high=9;
	
	//�ݴ� 
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].type == 3){//�������� 
				tem[i][j]=1;
			}else
				tem[i][j]=0;
		}
		
	//�������� 
	root = RandomNum(9)+1;//1-18����� 
	while(window[root][1].isoccu){
		root = RandomNum(9)+1;
	}
	tem[root][1]=1;
	
	state = RandomNum(4);//4��֮һ���ʳ����� 
	//��ǽ����
	if(state==0 && cur_high!=0){
		vaca=1;
		x = RandomNum(2);
		if(x==0) x = 9;
		for(j=1;j<=cur_high && window[x][j].isoccu;j++) continue;
		if(j==cur_high+1){//��������� 
			x = (x==1)? 9: 1;//�л� 
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
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
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
	
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(tem[i][j]){//�������� 
				vaca = 0;//���л��м�����ȱ 
				for(k=1;k<WINDOW_WIDTH+1;k++){
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
	
	//if(sum_seconds<10000) SetPenColor("Red");//С��10���� 
	SetStyle(1);
	drawLabel(cx-0.7,cy+3.2,memo);//������ʾ 
	drawLabel(cx+3.2,cy+0.3,memo);
	SetStyle(0);
	//if(sum_seconds<10000) SetPenColor("Black");
}

static void Frozen(){
	int i,j,k,x,y;
    int succ=0;
	int dir[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
	static int temp[WINDOW_WIDTH+1][WINDOW_HEIGHT+1];
	
	SetEraseMode(FALSE);
	
	for(i=1;i<WINDOW_WIDTH+2;i++){
		for(j=1;j<WINDOW_HEIGHT+1;j++){
		temp[i][j]=0;
		}
    }
	for(i=1;i<WINDOW_WIDTH+2;i++){
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			succ=0;
				if(window[i][j].type==4){
						for(k=0;k<4;k++){
							x = i + dir[k][0];
							y = j + dir[k][1];
							if(window[x][y].type==4) {
								temp[x][y]=1;
						
							}
						}
				}	
		}
   	}
   
  	for(i=1;i<WINDOW_WIDTH+1;i++){
		for(j=1;j<WINDOW_HEIGHT+1;j++){
				if(temp[i][j]==1){
					isfrozen=TRUE;
					frozentime+=ft;
				//	window[i][j].isoccu=FALSE;
					window[i][j].type=0;
					window[i][j].color="Yellow"; 
					SetPenColor("Blue");
					drawRectangle(MyXtransX(i)-0.5*L,MyYtransY(j)-0.5*L,L,L,1);	
										
				}
			}
	}
	
	if(isfrozen)
    Pause(0.5);
	//RefreshWindow();
}


static void Explode(int i,int j){
	int m,n;
	
	for(m=i-1;m<=i+1;m++){
		for(n=j-1;n<=j+1;n++){
			if(m>0&&m<10&&n>0&&n<21){
				DrawBlock(MyXtransX(m),MyYtransY(n),"Red");
		       if(window[m][n].type==1||window[m][n].type==2){
		          DrawBlock(MyXtransX(m),MyYtransY(n),"Red");
	      	    window[m][n].isoccu=FALSE;//�����Ϊδռ��״̬ 
                window[m][n].type=0;//ը�������Ϊ��״̬ 
                 InitConsole();
                 printf("%d ",I);
                SetEraseMode(FALSE);
                SetPenColor("Red");
            	SetPenSize(1);
            	
                drawRectangle(MyXtransX(m)-0.5*L,MyYtransY(n)-0.5*L,L,L,1);
                //Pause(15);
                tempbomb[I].x=MyXtransX(m);
                tempbomb[I].y=MyYtransY(n);
                
                I+=1;
                sco+=1; 
                Explode(m,n);
               }
               else if(m>0&&m<10&&n>0&&n<21){
               	DrawBlock(MyXtransX(m),MyYtransY(n),"Red"); 
                window[m][n].isoccu=FALSE;//�����Ϊδռ��״̬ 
                window[m][n].type=0;//ը�������Ϊ��״̬ 
                sco+=1; 
                /*SetPenColor("Red");
                SetPenSize(1);
                drawRectangle(MyXtransX(m)-0.5*L,MyYtransY(n)-0.5*L,L,L,1);*/////
                   
                tempbomb[I].x=MyXtransX(m);
                tempbomb[I].y=MyYtransY(n);
                printf("%d ",I);
                I+=1;
		    	}
	            
			}
		}
	}
	
}

static int tembomb[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];

static void DoubleBomb(int px, int py)
{
	int k,p;
	int x,y;
	for(k=-3;k<=3;k++){
		for(p=-3;p<=3;p++){
			x = px + k;
			y = py + p;
			if(x>0 && x<WINDOW_WIDTH+1 && y>0 && y<WINDOW_HEIGHT+1){
				if(window[x][y].type==1 && tembomb[x][y]==0){
					sco++;
					tembomb[x][y]=1;
					DoubleBomb(x,y);
				}else if(tembomb[x][y]==0) 
					tembomb[x][y]=2;
			}
		}
	}
}

static void Bomb()
{
	bool isbomb = FALSE;
	int dir[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
	int i,j,k,p;
	int x,y;
	
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");		
		}
		
	PrintScore();
	
	//���»�����Ϸ���� 
	SetEraseMode(FALSE);
	DrawFrame();
	PrintScore();
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].isoccu){
				DrawBlock(MyXtransX(i),MyYtransY(j),window[i][j].color);
			}
		} 
	
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			tembomb[i][j]=0;		
		}
		
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].type==1){
				for(k=0;k<4;k++){
					x = i + dir[k][0];
					y = j + dir[k][1];
					if(window[x][y].type==1) {
						sco++;
						tembomb[i][j] = tembomb[x][y] = 1;//���� 
					}
				}
			}		
		}
		
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(tembomb[i][j]==1){
				DoubleBomb(i,j);
			}		
		}
		
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(tembomb[i][j]==1){
				isbomb = TRUE;
				for(k=-3;k<=3;k++){
					for(p=-3;p<=3;p++){
						x = i + k;
						y = j + p;
						if(x>0 && x<WINDOW_WIDTH+1 && y>0 && y<WINDOW_HEIGHT+1){
							
							SetPenColor("Red");
	            			SetPenSize(1);
	            			drawRectangle(MyXtransX(x)-0.5*L,MyYtransY(y)-0.5*L,L,L,1);
						}
					
					}
				}
			}
			if(tembomb[i][j]==1 || tembomb[i][j]==2){
				window[i][j].isoccu=FALSE;//�����Ϊδռ��״̬ 
                window[i][j].type=0;//ը�������Ϊ��״̬
			}		
		}	
	
	if(isbomb) Pause(1.0);
}
 

//static void SaveRecord();//�浵 

/*
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
	//MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //���¿�ʼ��ť
}
 */
void InitNewMode()
{
	int i;
	InitGraphics();
	InitGUI();
	
	
	registerMouseEvent(MouseEventProcess);      // ���
	
	for(i=0;i<POPUP_TOTAL+1;i++) ispop[i]=FALSE;//������ʼ�� 
	
	InitNewWindow();
	

	//InitConsole();
	//printf("%.4f,%.4f",cx,cy);
	
}

void CloseNewMode()
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

static void InitNewWindow()
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
	
	srand((unsigned int) time(0));//����ϵͳʱ�������������
	
	DisplayClear();

	Chap_id = 0;
	Chap_sco=5;	
	sum_seconds=120000;//��ʼ������ʱ2���� 

	registerKeyboardEvent(KeyboardEventProcess);
	registerTimerEvent(TimerEventProcess);//ע�ᶨʱ����Ϣ�ص�����
	
	startTimer(TIMER_BLINK1000, mseconds1);//1000ms��ʱ������
	startTimer(TIMER_BLINK_1, mseconds);//1000ms��ʱ������
	startTimer(TIMER_BLINK, s_second);//1s��ʱ�� //	startTimer(TIMER_BLINK100, mseconds100);//100ms��ʱ������
	startTimer(TIMER_BLINK_f, froztime);
	
	//��ʼ�����ڵ�Ԫ�� 
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			if(i==0 || i==WINDOW_WIDTH+1 || j==0){//ǽ�ڵ�Ԫ�� 
				window[i][j].isoccu = TRUE;
				window[i][j].color = "Black";
			}else{
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}	window[i][j].type=0;
		}
	DrawFrame();
		
	temshape.type=0;//���䷽���ʼ��
	temshape = prishape[RandomId()];
	
/*	switch(Randi(4)){
		case 0 :temshape.No=-1;break;
		case 1 :temshape.No=1;break;
		case 2 :temshape.No=2;break;
		case 3 :temshape.No=3;break;
	}*/
	RandomType(&temshape);
	
	DrawShape(&temshape);
	
	nextid=-1; 
	DrawNextShape();
	
	ispause = FALSE;
	islose = FALSE;
	
	sco = 0;//���η���
	
	PrintTitle(); 
	PrintScore();
	PrintHelp();

	
	MyButton(OUT_PAUSE_BUTTON); 
//	MyButton(SAVE_BUTTON); 
	MyButton(RETURN_BUTTON);
	MyButton(RESTART_BUTTON); //���¿�ʼ��ť
	//InitMyGui();
	//PopupWindow(SAVE_TIPS_POPUP);
}

static void GameEnd(bool finish)
{
	cancelTimerEvent(TimerEventProcess);
	cancelTimer(TIMER_BLINK1000);
	cancelTimer(TIMER_BLINK_1);
	cancelTimer(TIMER_BLINK);
	cancelTimer(TIMER_BLINK_f);
	
	cancelKeyboardEvent(KeyboardEventProcess);
	
	if(finish){
		DisplayClear();
		
		PrintScore();
		PrintTitle();
		PrintHelp();
		
		MyButton(RETURN_BUTTON);
		MyButton(RESTART_BUTTON); //���¿�ʼ��ť
		
		UpdateScore();
		RankingList();
		 
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
							CloseNewMode();//�˳���Ϸģʽ 
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
					/*
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
					*/
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
								CloseNewMode();//�˳���Ϸģʽ 
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
						/*
						if(InButton(cx-4.5, cy-2.3, w, h, mx, my)){//�浵��ť,����ͣ 
							ispause = TRUE;
							PopupWindow(SAVE_TIPS_POPUP);
							ispop[2] = TRUE;
							ispop[0] = TRUE;	
						}
						*/
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
						RefreshWindow();//??
						Frozen();
						Bomb();
						temshape = prishape[nextid];//���»�����ͼ�� 
						DrawNextShape();
						RefreshWindow();
					
						break;
				}
				break;
		}
	}
}

static void TimerEventProcess(int timerID)
{
	bool isclose=FALSE;
	if(islose || ispause) return;
	
	switch(timerID){
		
	case TIMER_BLINK1000:
		if(!IsLose()){
			if(!IsBottom(&temshape)){
				ShapeDrop(&temshape);
			}else{
				temshape = prishape[nextid];
				DrawNextShape();
				//Renew();//?
				Frozen();
				Bomb();	
				RefreshWindow();
				
			}
		} 
		break;
	case TIMER_BLINK://1�룬����ʱˢ��
	if(!isfrozen){
		if(sum_seconds>0){//���ʱ��û�� 
				if((sum_seconds+2000)%5000 == 0) {//5��������������һ�� 
					//	GrowBlock();
							
						
				}
				if(sum_seconds%10000==0){
					Virus();
					RefreshWindow();//10�벡��������Ⱦһ��  
				}
						
				    	
						sum_seconds-=1000;	//��һ�� 
					//	PrintTime();
		}
		
		else{//���ʱ�䵽�� 
					sum_seconds=120000;
					}			
	} 
		
	break;
	case TIMER_BLINK_f:{
		if(frozentime<=0){
			isfrozen=FALSE;
			RefreshWindow();
		}
			
			
		if(isfrozen){
			frozentime-=ft;
			
		}
				
		break;
	}
				
	}
}

static void CleanLine2(i);
static void RefreshWindow()
{
	int i,j;
	bool isclean,isvirus;
	int conclean = 0;//����������� 
	//�����Ϸ���� 
	SetEraseMode(TRUE);
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			DrawBlock(MyXtransX(i),MyYtransY(j),"White");
		}
/*	SetEraseMode(FALSE);
	SetPenColor("Red");	*/
    drawRectangle(cx+3.2-0.01, cy-0.8-0.01,1.7,0.5,1);//����ԭ���÷� 
	
	//�ж��Ƿ����һ�� 
	for(j=1;j<WINDOW_HEIGHT+1;j++){
		isclean=TRUE,isvirus=FALSE;
		for(i=1;i<WINDOW_WIDTH+1;i++){
			if(window[i][j].type==5)
			isvirus=TRUE;
			if(!window[i][j].isoccu||(window[i][j].type==5&&!isfrozen)){//�����һ����һ����Ԫ��Ϊ��ռ�ã�����в���Ҫ��� ||(window[i][j].type==5&&!isfrozen)
				isclean=FALSE;
				break;
			}
		}
		
		if(isclean){
			CleanLine(j);
			j--; //��Ϊ�����֮������һ�������ˣ����Դ���Ҫ�����ж� 
		//	if(!isvirus)
			conclean += 1;
		}else if(conclean!=0){
			sco += 2 * conclean - 1; 
			conclean = 0;
		}
	}
	//���»�����Ϸ���� 
	SetEraseMode(FALSE);
	
	
	
	DrawFrame();
	SetPenColor("Black");
	PrintScore();
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].isoccu){			
			 	if(isfrozen&&(window[i][j].type==5||window[i][j].type==3))
				DrawBlock(MyXtransX(i),MyYtransY(j),"Blue");
				else
				DrawBlock(MyXtransX(i),MyYtransY(j),window[i][j].color);
			}
		} 
		
		DrawShape(&temshape);
	
//	PrintTime();
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


static void RefreshChapter()
{

	int i, j;
	
	cancelTimer(TIMER_BLINK_1);
	
	Chap_id++;//�ؿ� 
		
	sum_seconds=120000;//����ʱ���� 
	
	if(Chap_id<=5) //�����ٶ� 
		mseconds=1000-Chap_id*100;
	else
		mseconds1=2500/Chap_id;

	Chap_sco = Chap_id*(2*Chap_id+7) +5;//Ŀ��ֶ������� 
	
	startTimer(TIMER_BLINK_1, mseconds);
	

	//��ʼ�����ڵ�Ԫ�� 
	for(i=1;i<WINDOW_WIDTH+1;i++)
		for(j=1;j<WINDOW_HEIGHT+1;j++){
			if(window[i][j].type != 1){//������������ 
				window[i][j].isoccu = FALSE;
				window[i][j].color = "White";
			}
		}
	
	temshape= prishape[RandomId()];
	RefreshWindow();
	
	//PrintChap();

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

static void DrawNextShape()
{
	SHAPEPTR shapeptr;
	int i;
	if(nextid!=-1){//���֮ǰ�л������Ȳ�����һ����״ 
		SetEraseMode(TRUE);
		shapeptr = &prishape[nextid];
		for(i=0;i<4;i++){
		    char ch[10]="Yellow"; 
			if(shapeptr->No==i){
			      switch(shapeptr->type){
				      case 1: strcpy(ch,"Black");break;
				      case 2: strcpy(ch,"Red");break;
				      case 3: strcpy(ch,"Green");break;
				      case 4: strcpy(ch,"Cyan");break;
				      case 5: strcpy(ch,"Violet");break;
				      default: strcpy(shapeptr->color,"Yellow");break; 
	         		}		
      		}
		DrawBlock(MyXtransX(shapeptr->point[i][0])+3.2,MyYtransY(shapeptr->point[i][1]),ch);
		} 
	}
	 
	SetEraseMode(FALSE);
	nextid = RandomId();
	shapeptr = &prishape[nextid];
	RandomType(shapeptr);
/*	switch(Randi(4)){
			case 0: shapeptr->type=1;break;
			case 1: shapeptr->type=1;break;
			case 2: shapeptr->type=3;break;
			case 3: shapeptr->type=4;break;
		
	}*/
	
//	shapeptr->type=5;
	
/*	switch(Randi(4)){
		case 0 :shapeptr->No=-1;break;
		case 1 :shapeptr->No=1;break;
		case 2 :shapeptr->No=2;break;
		case 3 :shapeptr->No=3;break;
	}
	shapeptr->No=1;	*/
	
	for(i=0;i<4;i++){
		int t=0;
	   char ch[10]="Yellow"; 
		if(shapeptr->No==i&&t==0){
			t=1;
		      switch(shapeptr->type){
			      case 1: strcpy(ch,"Black");break;
			      case 2: strcpy(ch,"Red");break;
			      case 3: strcpy(ch,"Green");break;
			      case 4: strcpy(ch,"Cyan");break;
			      case 5: strcpy(ch,"Violet");break;
			      default: strcpy(ch,"Yellow");break; 
	         }
      	}
		DrawBlock(MyXtransX(shapeptr->point[i][0])+3.2,MyYtransY(shapeptr->point[i][1]),ch);
	} 
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
		drawLabel(cx-0.45, cy+3.1, over);
		
		isbegin = FALSE;
		//MyButton(BEGIN_BUTTON,TRUE);
	}
	return islose;//�����Ƿ���� 
}

static void BeforeGame()
{
	int i; 
	char* tips[5]={
		"         ""��",
	    "���Ͳ������־�����Ϸ", 
		"�����������ڽ���������",  
		"�������е�����ķ���",
	    "�������ѵõ����пռ�" 
	};
	PrintTitle();
	
	SetStyle(1);
	SetPointSize(26);
	SetPenColor("Red");
   	drawLabel(cx-1.5, cy+1.5-0*0.5, tips[0]);
   	SetPointSize(16);
   	
	for(i=1;i<2;i++){
		drawLabel(cx-1.5, cy+1.5-i*0.5, tips[i]);
	}
	 SetPenColor("Cyan");
    drawLabel(cx-1.5, cy+1.5-2*0.5, tips[2]);
    SetPenColor("Brown");
	for(i=3;i<5;i++){
		drawLabel(cx-1.5, cy+1.5-i*0.5, tips[i]);
	}
	SetPenColor("Black");
	SetPointSize(13);
	SetStyle(0);
}

static void PrintScore()
{
	char total[50];
	static char score[SCORE_MAX_CAP+1];
	int temsco = sco;
	int i;
	
	SetStyle(1);
	//if(sco>=Chap_sco) SetPenColor("Red"); 
	strcpy(total,"��ǰ�÷֣�");
	ScoreTrans(score,temsco);
	strcat(total,score);
	drawLabel(cx+3.2, cy-0.8, total);
	//if(sco>=Chap_sco) SetPenColor("Black"); 
	
	SetStyle(0);
	
}

static void PrintTitle()
{
	SetPointSize(26);
	SetStyle(1);
	drawLabel(cx-4.8, cy+3.0, "�� �� ģ ʽ");
	SetPointSize(13);
	
	
	SetStyle(0);
}

static void PrintHelp()
{
	int i;
	char *helptips[10] = {
		" | ��� | ����",
		" | �Ҽ� | ����",
		" | �ϼ� | ˳ʱ��ת",
		" | �¼� �� �ո� | ׹��", 
		" | F1�� | ��ͣ" ,
		"��ɫ�������ǿ��Ļ�������", 
		"��ɫ��������ʱ�ı���ֵ���������Ⱦ",
		"��ɫ���������˿־���ٶ���Ⱦһ��",
		"����а���������ֹ����" , 
		"ͬɫ���ⷽ���������ɴ�����������"
		
	};
	SetStyle(1);
	drawLabel(cx-4.7, cy+1.8+0.35, "����ָ�ϣ�"); 
	SetStyle(0);
		SetPenColor("Brown");
	for(i=0;i<10;i++){
		if(i==5)
		SetPenColor("Red");
		if(i==6)
		SetPenColor("Cyan");
		if(i==7)
		SetPenColor("Violet");
		if(i==9)
		SetPenColor("Red");
		drawLabel(cx-4.7, cy+1.8-i*0.3, helptips[i]);
	}
	SetPenColor("Black");
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
	
	if(cur_user.score[4]<sco){ //�Ƚ��Ƿ񳬹���ǰģʽ��ѳɼ� 
		cur_user.score[4] = sco;
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
	
	RankingFile(4,name,score,0);//�������а�
	cur_sco = cur_user.score[4]; 
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
	if(ismod) RankingFile(4,name,score,1);//�޸����а� 
	
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
	SetPenColor("Black");
	PrintTitle();//��ֹ������ 
	SetPenColor("White");
	drawRectangle(cx+2.1,cy+0.8,2.5,2.9,1);
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
	for(i=0;i<WINDOW_WIDTH+2;i++)
		for(j=0;j<WINDOW_HEIGHT+1;j++){
			cur_rec.window2[i][j]=window[i][j];
		}
	
	cur_rec.sh = temshape;//��ǰ��״
	cur_rec.nextid = nextid;//��һ����״���
	
	cur_rec.next = NULL;
	
	AddRecord(4, &cur_rec);//�����´浵

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

/**************************************/

static void InitShape(double gcx, double gcy)
{
	int i;
	
	
	
	cx = gcx;
	cy = gcy;
	
	
	for(i=0;i<TOTAL_SHAPE;i++){
		prishape[i].id = i;
		switch(i){
			case 0:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-4;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-3;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[2][1]=WINDOW_HEIGHT-3;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Yellow";
				prishape[i].No=0;
				break;
			case 1:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;//��ת����	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[2][1]=WINDOW_HEIGHT-2;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Yellow";
				prishape[i].No=3;
				break;
			case 2:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-3;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Yellow";
	            prishape[i].No=2;				
				break;
			case 3:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-4;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-3;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Yellow";
				prishape[i].No=1;	
				break;
			case 4:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-3;
				prishape[i].color="Yellow";
				prishape[i].No=3;	
				break;
			case 5:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[3][1]=WINDOW_HEIGHT-2;
				prishape[i].color="Yellow";
				prishape[i].No=2;	
				break;
			case 6:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-2;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-3;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[3][1]=WINDOW_HEIGHT-1;
				prishape[i].color="Yellow";
				prishape[i].No=3;	
				break;
		}
	}
}


static int XtransMyX(double x)
{
	int i;
	if(x+0.001<cx-(WINDOW_WIDTH+1)/2.0*L) return -1;
	for(i=1;i<WINDOW_WIDTH+3;i++){
		if(x+0.01 < i*L + cx-(WINDOW_WIDTH+1)/2.0*L) return i-1;
	}
	return -1;//���������� 
}

static int YtransMyY(double y)
{
	int i;
	if(y+0.001 < cy-(WINDOW_HEIGHT-1)/2.0*L) return 0;
	for(i=1;i<(WINDOW_HEIGHT+1);i++){
		if(y+0.01 < i*L + (cy-(WINDOW_HEIGHT-1)/2.0*L)) return i;
	}
	return -1;
}

static double MyXtransX(int mx)
{
	if(mx>=0 && mx<=(WINDOW_WIDTH+1)) return cx-(WINDOW_WIDTH+1)/2.0*L+mx*L;
	else return -0.1;
}


static double MyYtransY(int my)
{
	if(my>=1 && my<=(WINDOW_HEIGHT)) return cy-(WINDOW_HEIGHT-1)/2.0*L+(my-1)*L;
	else return -0.1;
}

static void DrawFrame()
{
	SetPenColor("Black");
	SetPenSize(4);//�߿��ϸ0.1*L 
	
	MovePen(MyXtransX(1)-0.5*L-FRAME_SIZE/3.0, MyYtransY(1)-0.5*L-FRAME_SIZE/3.0*2.0);
	DrawLine(0.0, (WINDOW_HEIGHT)*1.0*L+FRAME_SIZE);
	DrawLine((WINDOW_WIDTH)*1.0*L+FRAME_SIZE, 0.0);
	DrawLine(0.0, -(WINDOW_HEIGHT)*1.0*L-FRAME_SIZE);
	DrawLine(-(WINDOW_WIDTH)*1.0*L-FRAME_SIZE, 0.0);
	
	SetPenSize(1);
	MovePen(MyXtransX(1)-0.5*L-FRAME_SIZE/3.0, MyYtransY(WINDOW_HEIGHT-4)-0.5*L);
	DrawLine((WINDOW_WIDTH)*1.0*L+FRAME_SIZE, 0.0);
} 

static void DrawBlock(double x, double y, const char* color)
{
	double i;
	
	
	SetPenColor(color);
	SetPenSize(1);
	drawRectangle(x-0.5*L,y-0.5*L,L,L,1);
	/*for(i=0.0;i<=L;i+=0.01){
		MovePen(x-0.5*L,y-0.5*L+i);
		DrawLine(L,0.0);
	}*/
	
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
	   DrawBlock2(shapeptr);
}

static void ShapeDrop(SHAPEPTR shapeptr)
{
	SHAPE preshape = *shapeptr;
	bool candrop = TRUE;
	
	SetEraseMode(TRUE);
	DrawShape(shapeptr);
		
	int i;
	for(i=0;i<4;i++)
		shapeptr->point[i][1]--;
	//�ж�������Ƿ����Ҫ�� �����Կ���ɾ������ 
	for(i=0;i<4;i++){
		if(window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu) candrop = FALSE;
	}
	if(!candrop) *shapeptr = preshape;//����������䣬�����¶�ȡͼ��֮ǰ��λ�� 
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿�
	DrawShape(shapeptr);
	
}

static void ShapeMove(SHAPEPTR shapeptr, int dir)
{
	SHAPE preshape = *shapeptr;
	bool canmove = TRUE;
	int i;
	 
	SetEraseMode(TRUE);
	DrawShape(shapeptr);
	
	
	for(i=0;i<4;i++)
		shapeptr->point[i][0]+=dir;
		
	for(i=0;i<4;i++){
		if(window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu) canmove = FALSE;
	}
	if(!canmove) *shapeptr = preshape;
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿� 
	DrawShape(shapeptr);
}

static void ShapeSpin(SHAPEPTR shapeptr, int dir)
{
	SHAPE preshape = *shapeptr;
	bool canspin = TRUE;
	int rex,rey;
	int i;
	
	SetEraseMode(TRUE);
	DrawShape(shapeptr);
	
	if(shapeptr->id==0) canspin = TRUE;//���Ϊ0��ͼ���޷���ת 
	if(canspin){
		for(i=1;i<4;i++){
			rex = shapeptr->point[i][0]-shapeptr->point[0][0];//ͼ�θ���Ԫ���������ת���ĵ�Ԫ�񣨵�0�ţ������� 
			rey = shapeptr->point[i][1]-shapeptr->point[0][1];
			switch(dir){//�ж�˳ʱ�뻹����ʱ�� 
				case 1://˳ʱ�� 
					shapeptr->point[i][0]=shapeptr->point[0][0] + rey;
					shapeptr->point[i][1]=shapeptr->point[0][1] - rex;
					break;
				case -1:
					shapeptr->point[i][0]=shapeptr->point[0][0] - rey;
					shapeptr->point[i][1]=shapeptr->point[0][1] + rex;
					break;
			}
		}
	}
	
	for(i=0;i<4;i++){
		if(window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu) canspin = FALSE;
	}
	if(!canspin) *shapeptr = preshape;
	
	SetEraseMode(FALSE);
	DrawFrame();
	AvoidFric(&preshape);//��ֹĦ��������������߿�
	DrawShape(shapeptr);
} 

static void StraDrop(SHAPEPTR shapeptr)
{
	int dropdis;//���䵽�׾��� 
	int px,py;//ͼ����һ����Ԫ������� 
	int i,j;
	
	dropdis=WINDOW_HEIGHT;
	for(i=0;i<4;i++){
		px=shapeptr->point[i][0];
		py=shapeptr->point[i][1];
		for(j=py;j>=0;j--){
			if(window[px][j].isoccu){
				if(py-j-1 < dropdis) dropdis = py - j - 1;
				break;
			}
		}
	}
	
	for(i=0;i<4;i++){
		shapeptr->point[i][1]-=dropdis;
	}
	
	//����֮����´��ڵ�Ԫ�� 
	for(i=0;i<4;i++){
		window[shapeptr->point[i][0]][shapeptr->point[i][1]].isoccu = TRUE;
		window[shapeptr->point[i][0]][shapeptr->point[i][1]].color = shapeptr->color;
		window[shapeptr->point[i][0]][shapeptr->point[i][1]].type = 0;
		if(shapeptr->No==i) {
			window[shapeptr->point[i][0]][shapeptr->point[i][1]].type = shapeptr->type;
	    	switch( window[shapeptr->point[i][0]][shapeptr->point[i][1]].type){
		    	  case 1: 	window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Black";break;
			      case 2: 	window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Red";break;
			      case 3:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Green";break;
			      case 4:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Cyan";break;
			      case 5:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Violet";break;
			      default:  window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Yellow";break; 
			}
	    }
	}
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
		    
		    if(i==shapeptr->No){
		        window[shapeptr->point[i][0]][shapeptr->point[i][1]].type = shapeptr->type;
		    	switch( window[shapeptr->point[i][0]][shapeptr->point[i][1]].type){
		    	  case 1: 	window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Black";break;
			      case 2: 	window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Red";break;
			      case 3:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Green";break;
			      case 4:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Cyan";break;
			      case 5:   window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Violet";break;
			      default:  window[shapeptr->point[i][0]][shapeptr->point[i][1]].color="Yellow";break; 
				}
			}
		}
		
	}
	return isbot;//�����Ƿ񵽵� 
}


static void CleanLine(int k)
{
	int i,j;
	//������У�����������ж����ƣ������һ�б�֤ȫ�ǿհף� 

	for(i=1;i<WINDOW_WIDTH+1;i++) {
		for(j=k;j<WINDOW_HEIGHT;j++){ 
			window[i][j]=window[i][j+1];
		}
	}
	
}
/*static void CleanLine(int k)
{
	int i,j;
	//������У�����������ж����ƣ������һ�б�֤ȫ�ǿհף� 

	for(i=1;i<WINDOW_WIDTH+1;i++) {
		
		if(window[i][k].type==5&&!isfrozen)
		for(j=k;j<WINDOW_HEIGHT;j++){ 
			window[i][j]=window[i][j+1];
		}
	}
	
}*/

//������״Ħ���������麯��
static void AvoidFric(const SHAPEPTR shapeptr)
{
	int i;
	int px,py;
	for(i=0;i<4;i++){
		px = shapeptr->point[i][0];//ͼ������һ�������ԭ���� 
		py = shapeptr->point[i][1];
		if(px>=2 && window[px-1][py].isoccu == TRUE)//��� 
		{
		   DrawBlock(MyXtransX(px-1),MyYtransY(py),window[px-1][py].color);
		   
		}
		   
		if(px<=WINDOW_WIDTH-1 && window[px+1][py].isoccu == TRUE)//�ұ� 
		{
		   DrawBlock(MyXtransX(px+1),MyYtransY(py),window[px+1][py].color);
		   
		}
		
		if(py<=WINDOW_HEIGHT && window[px][py+1].isoccu == TRUE)//���� 
		{
		   DrawBlock(MyXtransX(px),MyYtransY(py+1),window[px][py+1].color);
		  
		}
		
		if(py>=2 && window[px][py-1].isoccu == TRUE)//�±� 
		{
		   DrawBlock(MyXtransX(px),MyYtransY(py-1),window[px][py-1].color);
		  
		}
	}
} 


static void Drawbomb(double x,double y,char* color){
	double l;
	l=2*L/3;
		SetPenColor(color);
		StartFilledRegion(1);

		MovePen(x+l/2,y);
		DrawArc(l/2,0,360);
		EndFilledRegion();
}

static void DrawBlock2 (SHAPEPTR shapeptr){
	int i;

	for(i=0;i<4;i++){
		char ch[10]="Yellow";
		if(shapeptr->No==i){
		      switch(shapeptr->type){
			      case 1: strcpy(ch,"Black");break;
			      case 2: strcpy(ch,"Red");break;
			      case 3: strcpy(ch,"Green");break;
			      case 4: strcpy(ch,"Cyan");break;
			      case 5: strcpy(ch,"Violet");break;
			      default: strcpy(ch,"Yellow");break; 
	         }
      	}
      
		DrawBlock(MyXtransX(shapeptr->point[i][0]),MyYtransY(shapeptr->point[i][1]),ch);
	}
}

/*********************************/

 

