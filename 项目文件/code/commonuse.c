
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

#include <string.h>
#include "commonuse.h"

/***************************************************************
	通用函数 
***************************************************************/

int RandomNum(int total) 
{
	int id; 
	id = rand()%(total);
	return id;
} 

int RandomId() 
{
	int id; 
	id = rand()%(7);
	return id;
} 

void ScoreTrans(char* ch_score, int score)
{
	int j;
	//得分由整型化为字符，前面补0 
		for(j=SCORE_MAX_CAP-1;j>=0;j--){
			ch_score[j] = score % 10 + '0';
			score/=10;
		}
		ch_score[SCORE_MAX_CAP]='\0';
}

//判断坐标是否在按钮内
bool InButton(double x, double y, double w, double h, double mx, double my)
{
	bool inbutton = FALSE;
	if(mx>x && my>y && mx<x+w && my<y+h) inbutton = TRUE;
	return inbutton;
}


