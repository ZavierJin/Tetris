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
	GUI动画 
***************************************************************/

#define TIMER_BLINK80 1     //80ms定时器时间标志号

static SHAPE temshape;//移动的形状

const int mseconds80 = 80;  //80ms定时器 

static double cx,cy;

static void TimerEventProcess(int timerID);//定时器消息回调函数
void DrawStartShape(const SHAPEPTR shapeptr, int move, int spin);
int RandomSpin();//随机旋转 
int RandomMove();//随机移动 
bool OnBottom(const SHAPEPTR shapeptr);//判断是否触底 

void InitAnim()
{
	int i,j; 
	InitGraphics();
	cx = GetWindowWidth()/2.0;
	cy = GetWindowHeight()/2.0;
	
	registerTimerEvent(TimerEventProcess);//注册定时器消息回调函数
	startTimer(TIMER_BLINK80, mseconds80);//80ms定时器触发

	//初始化窗口，防止之前游戏遗留 
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
	
	
	srand((unsigned int) time(0));//根据系统时间重置随机种子
	
	InitShape(cx-3.0,cy);//初始化形状 
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
			for(j=1;j<=spin;j++){//顺时针转spin次 
				rex = shapeptr->point[i][0]-shapeptr->point[0][0];//图形各单元格相对于旋转中心单元格（第0号）的坐标 
				rey = shapeptr->point[i][1]-shapeptr->point[0][1];
				shapeptr->point[i][0]=shapeptr->point[0][0] + rey;
				shapeptr->point[i][1]=shapeptr->point[0][1] - rex;
			}	
		}
	
	//move
	for(i=0;i<4;i++)
		shapeptr->point[i][0]+=move;
		
	//调整
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
	spin = rand()%4;//获得0~3的随机数 
	return spin;
}

int RandomMove()
{
	int move; 
	move = 1 + rand()%9;//获得1~9的随机数 
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





