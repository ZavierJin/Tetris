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
 
#include<time.h> 
#include<string.h>
#include "commonuse.h"
#include "shapebasic.h" 
#include "animation.h"

/***************************************************************
	GUI���� 
***************************************************************/

#define TIMER_BLINK80 1     //80ms��ʱ��ʱ���־��

static SHAPE temshape;//�ƶ�����״

const int mseconds80 = 80;  //80ms��ʱ�� 

static double cx,cy;

static void TimerEventProcess(int timerID);//��ʱ����Ϣ�ص�����
void DrawStartShape(const SHAPEPTR shapeptr, int move, int spin);
int RandomSpin();//�����ת 
int RandomMove();//����ƶ� 
bool OnBottom(const SHAPEPTR shapeptr);//�ж��Ƿ񴥵� 

void InitAnim()
{
	int i,j; 
	InitGraphics();
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	registerTimerEvent(TimerEventProcess);//ע�ᶨʱ����Ϣ�ص�����
	startTimer(TIMER_BLINK80, mseconds80);//80ms��ʱ������

	//��ʼ�����ڣ���ֹ֮ǰ��Ϸ���� 
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
	
	
	srand((unsigned int) time(0));//����ϵͳʱ�������������
	
	InitShape(cx-3.0,cy);//��ʼ����״ 
	temshape = prishape[RandomId()];
	

	DrawStartShape(&temshape,RandomMove(),RandomSpin());
}

void CloseAnim()
{
	cancelTimerEvent(TimerEventProcess);
	cancelTimer(TIMER_BLINK80);
}

static void TimerEventProcess(int timerID)
{
	switch(timerID){
		case TIMER_BLINK80:
			if(!OnBottom(&temshape)){
				ShapeDrop(&temshape);
			}else{
				SetEraseMode(TRUE);
				DrawShape(&temshape);
				
				temshape = prishape[RandomId()];
				SetEraseMode(FALSE);
				DrawStartShape(&temshape,RandomMove(),RandomSpin());
			}
			
			break;
		}
}


void DrawStartShape(const SHAPEPTR shapeptr, int move, int spin)
{
	int i,j;
	int rex,rey;
	//spin
	for(i=1;i<4;i++){
			for(j=1;j<=spin;j++){//˳ʱ��תspin�� 
				rex = shapeptr->point[i][0]-shapeptr->point[0][0];//ͼ�θ���Ԫ���������ת���ĵ�Ԫ�񣨵�0�ţ������� 
				rey = shapeptr->point[i][1]-shapeptr->point[0][1];
				shapeptr->point[i][0]=shapeptr->point[0][0] + rey;
				shapeptr->point[i][1]=shapeptr->point[0][1] - rex;
			}	
		}
	
	//move
	for(i=0;i<4;i++)
		shapeptr->point[i][0]+=move;
		
	//����
	for(i=0;i<4;i++){
		while(shapeptr->point[i][0] < 1){
			for(j=0;j<4;j++)
				shapeptr->point[j][0] += 1;
		}
		while(shapeptr->point[i][0] > 9){
			for(j=0;j<4;j++)
				shapeptr->point[j][0] -= 1;
		}	
	}
	
	DrawShape(shapeptr);
	DrawFrame();
}

int RandomSpin()
{
	int spin; 
	spin = rand()%4;//���0~3������� 
	return spin;
}

int RandomMove()
{
	int move; 
	move = 1 + rand()%9;//���1~9������� 
	move -= 5;
	return move;
}

bool OnBottom(const SHAPEPTR shapeptr)
{
	bool onbot = FALSE;
	int i;
	for(i=0;i<4;i++)
		if(shapeptr->point[i][1]==1) onbot = TRUE;

	return onbot;
}





