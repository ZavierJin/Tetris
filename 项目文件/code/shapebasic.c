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
#include "commonuse.h"
#include "shapebasic.h"
#include "imgui.h"

/***************************************************************
	������״���� 
***************************************************************/

static double cx,cy;//������������ 
//static double ox,oy;

static void AvoidFric(const SHAPEPTR shapeptr);//������״Ħ���������麯�� 


void InitShape(double gcx, double gcy)
{
	int i;
	
	srand((unsigned int) time(0));//����ϵͳʱ�������������
	
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
				prishape[i].color="Blue";
				break;
			case 1:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;//��ת����	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[2][1]=WINDOW_HEIGHT-2;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Yellow";
				break;
			case 2:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-3;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Red";
				break;
			case 3:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-4;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-3;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-4;
				prishape[i].color="Orange";
				break;
			case 4:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2 + 1; prishape[i].point[3][1]=WINDOW_HEIGHT-3;
				prishape[i].color="Green";
				break;
			case 5:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-3;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-4;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2 - 1; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[3][1]=WINDOW_HEIGHT-2;
				prishape[i].color="Magenta";
				break;
			case 6:
				prishape[i].point[0][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[0][1]=WINDOW_HEIGHT-2;	
				prishape[i].point[1][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[1][1]=WINDOW_HEIGHT-3;
				prishape[i].point[2][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[2][1]=WINDOW_HEIGHT-4;
				prishape[i].point[3][0]=(WINDOW_WIDTH+1)/2; prishape[i].point[3][1]=WINDOW_HEIGHT-1;
				prishape[i].color="Cyan";
				break;
		}
	}
}


int XtransMyX(double x)
{
	int i;
	if(x+0.001<cx-(WINDOW_WIDTH+1)/2.0*L) return -1;
	for(i=1;i<WINDOW_WIDTH+3;i++){
		if(x+0.01 < i*L + cx-(WINDOW_WIDTH+1)/2.0*L) return i-1;
	}
	return -1;//���������� 
}

int YtransMyY(double y)
{
	int i;
	if(y+0.001 < cy-(WINDOW_HEIGHT-1)/2.0*L) return 0;
	for(i=1;i<(WINDOW_HEIGHT+1);i++){
		if(y+0.01 < i*L + (cy-(WINDOW_HEIGHT-1)/2.0*L)) return i;
	}
	return -1;
}

double MyXtransX(int mx)
{
	if(mx>=0 && mx<=(WINDOW_WIDTH+1)) return cx-(WINDOW_WIDTH+1)/2.0*L+mx*L;
	else return -0.1;
}


double MyYtransY(int my)
{
	if(my>=1 && my<=(WINDOW_HEIGHT)) return cy-(WINDOW_HEIGHT-1)/2.0*L+(my-1)*L;
	else return -0.1;
}

void DrawFrame()
{
	SetPenColor("black");
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

void DrawBlock(double x, double y, const char* color)
{
	double i;
	
	
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


void DrawShape(const SHAPEPTR shapeptr)
{
	int i;
	for(i=0;i<4;i++)
		DrawBlock(MyXtransX(shapeptr->point[i][0]),MyYtransY(shapeptr->point[i][1]),shapeptr->color);
}

void ShapeDrop(SHAPEPTR shapeptr)
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

void ShapeMove(SHAPEPTR shapeptr, int dir)
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

void ShapeSpin(SHAPEPTR shapeptr, int dir)
{
	SHAPE preshape = *shapeptr;
	bool canspin = TRUE;
	int rex,rey;
	int i;
	
	SetEraseMode(TRUE);
	DrawShape(shapeptr);
	
	if(shapeptr->id==0) canspin = FALSE;//���Ϊ0��ͼ���޷���ת 
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

void StraDrop(SHAPEPTR shapeptr)
{
	int dropdis;//���䵽�׾��� 
	int px,py;//ͼ����һ����Ԫ������� 
	int i,j;
	
	dropdis=WINDOW_HEIGHT;
	for(i=0;i<4;i++){
		px=shapeptr->point[i][0];
		py=shapeptr->point[i][1];
		for(j=py-1;j>=0;j--){
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
	}
	
}

bool IsBottom(SHAPEPTR shapeptr)
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


void CleanLine(int k)
{
	int i,j;
	//������У�����������ж����ƣ������һ�б�֤ȫ�ǿհף� 
	for(j=k;j<WINDOW_HEIGHT;j++){
		for(i=1;i<WINDOW_WIDTH+1;i++) 
			window[i][j]=window[i][j+1];
	}
}

//������״Ħ���������麯��
void AvoidFric(const SHAPEPTR shapeptr)
{
	int i;
	int px,py;
	for(i=0;i<4;i++){
		px = shapeptr->point[i][0];//ͼ������һ�������ԭ���� 
		py = shapeptr->point[i][1];
		if(px>=2 && window[px-1][py].isoccu == TRUE)//��� 
			DrawBlock(MyXtransX(px-1),MyYtransY(py),window[px-1][py].color);
		if(px<=WINDOW_WIDTH-1 && window[px+1][py].isoccu == TRUE)//�ұ� 
			DrawBlock(MyXtransX(px+1),MyYtransY(py),window[px+1][py].color);
		if(py<=WINDOW_HEIGHT && window[px][py+1].isoccu == TRUE)//���� 
			DrawBlock(MyXtransX(px),MyYtransY(py+1),window[px][py+1].color);
		if(py>=2 && window[px][py-1].isoccu == TRUE)//��� 
			DrawBlock(MyXtransX(px),MyYtransY(py-1),window[px][py-1].color);		
	}
} 



