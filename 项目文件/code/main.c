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
	������GUI 
***************************************************************/

#define DEFAULT_COLOR "Black" //Ĭ����ɫ 
#define DEFAULT_POINT_SIZE 13 //Ĭ�������С 

#define TOTAL_FACE 25  	//���������� 
#define BUTTON_MAX 5 	//һ�����水ť������ 

int cur_page = 0;//��ǰ����id ,��0��ʼ 

int button_jump[TOTAL_FACE][BUTTON_MAX]={
	{1,2,-1},                 	//0 �û���¼ע����� 
	{0},                 		//1 ��¼���� 
	{0},						//2 ע����� 
	{4,21,22,-1},               //3 �˵�����
	{5,20,10,15,3},				//4 ģʽѡ����� 
	
	//����ģʽ5-9 
	{6,8,4},                    //5 ����ģʽ���� 
	{7,7,7,5},                  //6 ����ģʽ�Ѷ�ѡ����� 
	{6},						//7 ����ģʽ��Ϸ����  
	{5},						//8 ��ȡ�浵����
	{},//8-9 
	
	//˫��ģʽ10-14 
	{11,12,4},                  //10 ˫��ģʽ���� 
	{10},						//11 ˫��ģʽ��Ϸ���� 
	{10},						//12 ��ȡ�浵���� 
	{},{},//13-14
	
	//����ģʽ15-19 ������
	{16,17,4},                  //15 ����ģʽ���� 
	{15},						//16 ����ģʽ��Ϸ���� 
	{15},						//17 ��ȡ�浵���� 
	{},{},//18-19
	
	{4},						//20 ��ϰģʽ����
	{3},						//21 ��Ϸ��������
	{3},						//22 ����ս������
									
}; //��ť��ת����һ���±��ǽ���id���ڶ����±��ǰ�ťid 

char* button_label[TOTAL_FACE][BUTTON_MAX]={
	{"�� ¼","ע ��","�� ��"},//0 
	{"�� ��"},//1 
	{"�� ��"},//2 
	{"��ʼ��Ϸ","��Ϸ����","����ս��","�˳���Ϸ"},//3 
	{"����ģʽ","��ϰģʽ","˫��ģʽ","����ģʽ","�� ��"},//4
	
	//����ģʽ5-9 
	{"�� �� Ϸ","��ȡ�浵","�� ��"},//5 
	{"�� ��","�� ͨ","�� ��","�� ��"},//6 
	{},//7 
	{"�� ��"},//8 
	{},//9�� 
	
	//˫��ģʽ10-14 
	{"�� �� Ϸ","��ȡ�浵","����"},//10 
	{"�� ��"}, //11 
	{"�� ��"}, // 12 
	{},{},// 13-14 
	
	//����ģʽ15-19 ������
	{"�� �� Ϸ","��ȡ�浵","����"},//15 
	{"�� ��"}, //16
	{"�� ��"}, // 17 
	{},{},// 18-19 
	
	{"�� ��"},
	{"�� ��"},
	{"�� ��"},

}; //��ť���֣���һ���±��ǽ���id���ڶ����±��ǰ�ťid  

int button_id_total[TOTAL_FACE]={
	3,1,1,4,5, //0-4 
	3,4,0,1,0, //5-9
	3,0,1,0,0, //10-14
	3,0,1,0,0, //15-19
	0,1,1, 		//20-22
}; //��ǰ���水ť�������±��ǽ���id 

double button_first_px[TOTAL_FACE]={ 
	+1.0, +2.7, +2.7, +2.7, +2.7,
	+2.7, +2.7, +0.0, +2.7, +0.0,
	+2.7, -2.7, +2.7, +0.0, +0.0,
	+2.7, -2.7, +2.7, +0.0, +0.0,
	-2.7, +2.7, +2.7, 
};//��ǰ�����һ����ť�൱����x���꣬�±��ǽ���id��
																					
double button_first_py[TOTAL_FACE]={ 
	+0.4, -3.2, -3.2, +1.7, +2.0,
	+1.2, +1.7, -0.0, -3.2, +0.0,
	+1.2, +0.7, -3.2, +0.0, +0.0,
	+1.2, +0.7, -3.2, +0.0, +0.0,
	+0.7, -3.2, -3.2, 
};//��ǰ�����һ����ť�൱����y���꣬�±��ǽ���id��
																						 
double button_dis[TOTAL_FACE];//ÿ����ť��� ,���ڲ��ԣ���ȫ����Ϊ1.0 

char label_color[] = "White"; 
char button_color[] ="Dark Gray";

struct user cur_user;//��ǰ�û� 

//��½ 
char old_name[80];//������û���
int old_name_state = 0;
//�����û���״̬��0����δ���룬1�����û������Ϸ���2�����û����Ѵ��ڣ�3����ע��ɹ� 

//ע�� 
char new_name[80];//������û���
int new_name_state = 0;
//�����û���״̬��0����δ���룬1�����û������Ϸ���2�����û���δ�ҵ���3�����¼�ɹ� 

//����ģʽ��Ϸ�Ѷȣ�0ΪΪ������1Ϊ��  
int classic_mode_state = 0;

int classic_rec_state[3]={0,0,0};//����ģʽ�浵״̬ 

struct Record *cur_rec_p1, *cur_rec_p2, *cur_rec_p3;//�����Ѷȴ浵ָ�� 

int help_page = 0;//��������ڼ�ҳ 

double cx,cy;//������������ 



void InitMyGui();//���潻������
void CloseMyGui();//���潻���ر�

void OutMouseEventProcess(int x, int y, int Mbutton, int event);//�����Ϣ�ص�����
void CharEventProcess(char ch); 
void KeyboardEventProcess(int key, int event);
 
void RefreshFace();//ˢ�½��溯�� 
void RefreshButton();//ˢ�°�ť���� 
bool ButtonResp(double mx, double my);//��ť��Ӧ���� 

void BeginFace(); //����ʼ����

void LoginFace(); //��¼���� 
bool CheckOldName();//���������Ƿ���� 

void RegisterFace(); //ע����� 
bool CheckNewName();//����������Ƿ���� 

void ReadRecordFace(int mode);//��ȡ�浵���棬mode��0������ģʽ 
void RecordFaceButtonResp(int mode, double mx, double my); 
//��ȡ�浵��ť��Ӧ������mode��0������ģʽ

void MyScoreFace(); //����ս������ 
void HelpFace(); //��Ϸ�������� 
void HelpFaceButtonResp(double mx, double my);
void NoBuild();

void Main()
{
	
	InitGraphics();
	InitGUI();
	//InitConsole();
	SetWindowTitle("����˹����");
	cx= GetWindowWidth()/2.0;
    cy= GetWindowHeight()/2.0;
	
    InitMyGui();
    
	int i;
	for(i=0;i<TOTAL_FACE;i++) button_dis[i]=1.0; //���ڲ��ԣ���ť�����ȫ1.0 
	
}


void InitMyGui()
{
	registerCharEvent(CharEventProcess); // �ַ�
	registerKeyboardEvent(KeyboardEventProcess);// ����
	registerMouseEvent(OutMouseEventProcess);      // ���
	//printf("$$\n");
} 

void CloseMyGui()
{
	cancelCharEvent(CharEventProcess); // �ַ�
	cancelKeyboardEvent(KeyboardEventProcess);// ����
	cancelMouseEvent(OutMouseEventProcess);      // ���	
} 


/*�����Ϣ�ص�����������x��yΪ��굱ǰ���꣬eventΪ����¼�����*/ 
void OutMouseEventProcess(int x, int y, int button, int event)
{

	double mx, my;//��ǰ�������

	uiGetMouse(x,y,button,event); //GUI��ȡ���
	
	mx = ScaleXInches(x);//����תӢ�磬��������intתdouble 
	my = ScaleYInches(y);
	
	//��ť��Ӧ 
	switch (event){
		case BUTTON_DOWN://�����걻���� 
			if(button==LEFT_BUTTON){
				ButtonResp(mx,my);
				if(cur_page==8){//����ģʽ�浵��ȡ���水ť���� 
					RecordFaceButtonResp(0,mx,my);
				}
				if(cur_page==21){//��Ϸ�������水ť���� 
					HelpFaceButtonResp(mx,my);
				}
			}
			break;
	}
	
	RefreshFace();
	

}

// �û����ַ��¼���Ӧ����
void CharEventProcess(char ch)
{
	
	uiGetChar(ch); // GUI�ַ�����
	
	RefreshFace();

}

// �û��ļ����¼���Ӧ����
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI��ȡ����
	
	switch(event){
		case KEY_DOWN:
			switch(key){
				case VK_RETURN://�س� 
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
	static int pre_page = -1;//ǰһ��ҳ�棬��ʼֵΪ-1 
	static bool page_change = FALSE;//�Ƿ�任��ҳ�� 
	static bool anim_open = FALSE;//�����Ƿ��� 
	
	if(pre_page!=cur_page){//ҳ���л� 
		page_change = TRUE;
		pre_page = cur_page;
	}else if(page_change==TRUE){ 
		page_change = FALSE;
	} 
		
	if(anim_open && page_change){//�����л� 
		CloseAnim();//�رն��� 
		anim_open = FALSE;
	}
	
	if(page_change){//ֻ��һ�εĽ��� 
		switch(cur_page){
			case 0: //��ʼ����,Ϊ�޸�bug���������������˺������ 
			case 3: case 4://���� 
				InitAnim();//��ʼ������ 
				anim_open = TRUE;	
				break;			
			case 8: //����ģʽ��ȡ�浵���� 
				ReadRecordFace(0);
				break; 
			case 12:case 17:
				NoBuild();
				break;
			case 21://��Ϸ��������
				help_page = 0; 
				break;
			case 22://����ս������ 
				MyScoreFace();
				break;
			default:
				break;
		}
	} 
	
	switch(cur_page){//��Ҫ����ˢ�µĽ���		
		case 1://��½���� 
			DisplayClear();
			LoginFace();
			break;
		case 2://ע�����
			DisplayClear();
			RegisterFace();
			break;
		case 21://��Ϸ��������
			HelpFace();
			break;
		case -1://�����˳����� 
			exit(-1);
			break;
		default:
			break;
	}
	RefreshButton();
	
	if(cur_page==0 && page_change) BeginFace();//???��ʼ����,������bug 

}


void RefreshButton()
{
	double fx,fy,ty;
	double h = 0.7;  // �ؼ��߶�
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
	SetPenColor(DEFAULT_COLOR); //������������ΪĬ����ɫ 
	SetPointSize(DEFAULT_POINT_SIZE);//�����СΪĬ�� 
}

bool ButtonResp(double mx, double my)
{
	double fx,fy,ty;
	double h = 0.7;  // �ؼ��߶�
	double w = 2.0;
	int total;
	int i;
	bool isresp = FALSE;
	//if(cur_page!=0) return FALSE;
	fx = cx+button_first_px[cur_page];
	fy = cy+button_first_py[cur_page];
	ty = button_dis[cur_page];
	total = button_id_total[cur_page];
	
	switch(total){//???��� 
		case 1:break;
		case 3:ty+=0.2;break;
	}
	
	for(i=0;i<total;i++){
		if(InButton( fx, fy-1.0*i*ty, w, h, mx, my)){
			cur_page = button_jump[cur_page][i];//�޸Ľ���id ��������һ������ 
			isresp = TRUE;
			DisplayClear();
			break;
		}
	}
	//��ת����Ϸ����
	
	switch(cur_page){
		case 7://����ģʽ 
			classic_mode_state = i+1;//����ģʽ�Ѷ�
			CloseMyGui(); //�رմ���GUI 
			InitClassicMode(classic_mode_state);//������Ϸ 
			break;	
		case 20://��ϰģʽ
			CloseMyGui(); //�رմ���GUI 
			InitPracMode();//������Ϸ 
			break;	
		case 11://˫��ģʽ 
			CloseMyGui(); //�رմ���GUI 
			InitDoubleMode();
			break;	
		case 16://
			CloseMyGui(); //�رմ���GUI
			InitNewMode();
			break;	
	}
	
	return isresp;
}


void BeginFace()
{
	char title[]="�� �� ˹ �� ��";
	
	SetPointSize(DEFAULT_POINT_SIZE*4); 
	SetStyle(1);
	drawLabel(cx-0.4,cy+2.3,title);//������� 
	SetStyle(0);
	SetPointSize(DEFAULT_POINT_SIZE); 
} 

void LoginFace()
{
	static char title[]="�� �� �� ¼"; 
	static char memo[]="����������û�����";
	static char fail[]="�û������Ϸ�������������";
	static char repe[]="�û���δ�ҵ�������������";
	static char succ[]="��¼�ɹ���";
	static char str[80];//��ʱ���浱ǰ�����ı�
	static char tips_tit[]="������ʾ��";
	static char *tips[4]={
		"1.�û�����ע��ʱ������û���",
		"2.�û�����ֻ���������ĸ������",
		"3.�û������ܳ���15���ַ�",
		"4.���»س�ȷ������" 
	};
	double h = 0.5;  // �ؼ��߶�
	double w = 2.0;
	double x = cx+0.8;//textλ�� 
	double y = cy+h/2.0;
	int i;
	
	SetPenColor("Black");
	//������� 
	SetPointSize(DEFAULT_POINT_SIZE*2);
	SetStyle(1);
	drawLabel(cx-0.9,cy+2.3,title);
	SetPointSize(DEFAULT_POINT_SIZE);
	
	//������ʾ 
	drawLabel(cx-3.3,cy+1.0,tips_tit);
	drawLabel(x,y+h*1.5,memo); //�ı�����ʾ��
	SetStyle(0);
	for(i=0;i<4;i++){
		drawLabel(cx-3.0,cy+0.6-i*0.4,tips[i]);
	}
	
	
	//�����ı� 
	if( textbox(0,x,y, w, h, str, sizeof(str)) )
		strcpy(old_name,str);	
	//���״̬��ǩ 
	switch(old_name_state){
		case 0:break;
		case 1: 
			drawLabel(x,y-h*0.8,fail);//�û������Ϸ��� 
			break;
		case 2:
			drawLabel(x,y-h*0.8,repe);//�û���δ�ҵ��� 
			break;
		case 3:
			drawLabel(x,y-h*0.8,succ);//��¼�ɹ��� 
			break;
	} 
}

bool CheckOldName()
{
	struct user *head=NULL;
	
	head = OpenUserFile();
	
	if(!Inputcheck(old_name)){//����û������Ϸ� 
		old_name_state = 1;
	}else{
		if(!UserLogin(head,old_name,&cur_user)){//����û����Ѵ��� 
			old_name_state = 2;
		}else {//��¼�ɹ�����ת���� 
			old_name_state = 3;
			LoginFace();
			Pause(1.0);//��ת�ӳ�1s
			DisplayClear(); 
			cur_page = 3;
		}
		
	}
	DeleteList(head);
	return FALSE;  
} 



void RegisterFace()
{
	static char title[]="�� �� ע ��"; 
	static char memo[]="����������û�����";
	static char fail[]="�û������Ϸ�������������";
	static char repe[]="�û����Ѵ��ڣ�����������";
	static char succ[]="ע��ɹ���";
	static char str[80];//��ʱ���浱ǰ�����ı� 
		static char tips_tit[]="������ʾ��";
	static char *tips[4]={
		"1.ע����û����´���Ϸ���ڵ�¼",
		"2.�û�����ֻ���������ĸ������",
		"3.�û������ܳ���15���ַ�",
		"4.���»س�ȷ������" 
	};
	double h = 0.5;  // �ؼ��߶�
	double w = 2.0;
	double x = cx+0.8;//textλ�� 
	double y = cy+h/2.0; 
	int i;
	
	SetPenColor("Black");
	//������� 
	SetPointSize(DEFAULT_POINT_SIZE*2);
	SetStyle(1);
	drawLabel(cx-0.9,cy+2.3,title);
	SetPointSize(DEFAULT_POINT_SIZE);
	
	//������ʾ 
	drawLabel(cx-3.3,cy+1.0,tips_tit);
	drawLabel(x,y+h*1.5,memo); //�ı�����ʾ��
	SetStyle(0);
	for(i=0;i<4;i++){
		drawLabel(cx-3.0,cy+0.6-i*0.4,tips[i]);
	}
	
	
	 
	if( textbox(0,x,y, w, h, str, sizeof(str)) )
		strcpy(new_name,str);	
	//���״̬��ǩ 
	switch(new_name_state){
		case 0:break;
		case 1: 
			drawLabel(x,y-h*0.8,fail);//�û������Ϸ��� 
			break;
		case 2:
			drawLabel(x,y-h*0.8,repe);//�û����Ѵ��ڣ� 
			break;
		case 3:
			drawLabel(x,y-h*0.8,succ);//ע��ɹ��� 
			break;
	} 
}


//����������Ƿ����
bool CheckNewName()
{
	struct user *head=NULL;
	
	head = OpenUserFile();
	
	if(!Inputcheck(new_name)){//����û������Ϸ� 
		new_name_state = 1;
	}else{
		if(!UserRegister(head,new_name,&cur_user)){//����û����Ѵ��� 
			new_name_state = 2;
		}else {//��¼�ɹ�����ת���� 
			ModifyUserFile(head);
			new_name_state = 3;
			RegisterFace();
			Pause(1.0);//��ת�ӳ�1s
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
	double w, h;//���� 
	double th, tj;//�п�,���
	double x, y;//�浵�����ǰλ�� 
	double w1, w2;
	int i,j;
	char* help[50]={
		"1.�ڿ�ʼ��Ϸ����ѡ��ģʽ������Ϸ",
		"2.ѡ���ȡ�浵�����Լ�������ϴδ浵��Ϸ��Ŀǰֻ�о���ģʽ�д˹��ܣ�",
		"3.ѡ�񾭵�ģʽ",
		"        ""a.����ѡ���Ѷ�\"��\"\"��ͨ\"\"����\"����ͬ�Ѷȷ�������ٶȲ�ͬ",
		"        ""b.���������������Ϸ��F1 ����ͣ��Ϸ",
		"        ""c.��Ϸû��ʱ�����ƣ������鳬��������ʼ������Ϸ����",
		"4.ѡ����ϰģʽ",
		"        ""a.��ģʽ������ϰ��������ʽͬ����ģʽ",
		"        ""b.��ͨ��X �������٣���Z �������٣����е��ڷ��������ٶ�",
		"5.ѡ��˫��ģʽ",
		"        ""a.�����������WASD��˫�˺�������",
		"        ""b.������ײһ��ǽ���˲��������һ��",
		"        ""c.��Ϸ��;�����������ɫ���������飬ÿ��������������Ա��������ؿ��л�ʱ�������鲻�����",
		"6.ѡ�񲡶�ģʽ",
		"        ""a.������ʽͬ����ģʽ",
		"        ""b.��ģʽ����3�����ⷽ��        ��ɫ��ը������         ��ɫ����������        ��ɫ����������",
		"          ""ը�����飺������ɣ���ɫΪը������������㣬���������ֱ�ӽӴ��ɴ�����ըЧ������ը������������ը",
		"          ""�������飺������ɣ���ɫΪը������ı����㣬���������ֱ�ӽӴ��ɴ�������Ч��������״̬�²�������ʧЧ��",
		"                              ""��������Ϊ����ɫ������״̬����һ��ʱ�䣬�����������飬ʱ��Խ��" ,
		"          ""�������飺������ɣ���ɫΪ��������Ⱦ���֣���������������ɢ������ĳһ������һ�����鱻������Ⱦ��" ,
		"                              ""����в��ܱ���ͨ����",
		"7.������Ϸ����Բ鿴��ǰģʽ���а�",
		"8.ѡ�����ս�����ɲ鿴��ͬģʽ������ʷ��ߵ÷�",
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
	
	//����ɫ 
	SetPenColor("Gray");
	drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
	//������ 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,"�� Ϸ �� ��",'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	
	switch(help_page){
		case 0://��һҳ 
			SetPointSize(16);
			SetPenColor("Dark Gray");
			drawBox(cx+3.4,cy+2.35,1.15,0.7,1,"��һҳ",'Z',"White");
			SetPointSize(13);
			for(i=0;i<13;i++){
				SetPenColor("White");
				drawBox(x,y,w-2*tj,th,1,help[i],'L',"Black");
				y -= th;
			}
			break;
		case 1://�ڶ�ҳ 
			SetPointSize(16);
			SetPenColor("Dark Gray");
			drawBox(cx-4.55,cy+2.35,1.15,0.7,1,"��һҳ",'Z',"White");
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
		"����ģʽ���򵥣�",
		"����ģʽ����ͨ��",
		"����ģʽ�����ѣ�",
		"˫��ģʽ ",
		"����ģʽ " 
	};
	char name[25];
	double w, h;//���� 
	double th, tj;//�п�,���
	double x, y;//�浵�����ǰλ�� 
	double w1, w2;
	int i,j;
	
	w = 5.0; h = 3.6;
	th = 0.35 ; tj = 0.2;
	w1 = 2.3; w2 = w - 2*tj - w1;
	x = cx-w/2.0+tj;
	y = cy+h/2.0-3*th-2*tj;
	
	//����ɫ 
	SetPenColor("Gray");
	drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
	//������ 
	SetStyle(1);
	SetPointSize(16);
	SetPenColor("White");
	drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,"�� �� ս ��",'Z',"Black");
	SetPointSize(13);
	SetStyle(0);
	//��ʾ����ս�� 
	for(i=0;i<MODE_NUM;i++){
		ScoreTrans(ch_score, cur_user.score[i]);
				
		SetPenColor("White");
		drawBox(x,y,w1,th,1,s_title[i],'Z',"Black");
		SetPenColor("White");
		drawBox(x+w1,y,w2,th,1,ch_score,'Z',"Black");
		
		y -= th;
	}
	//��ʾ�û��� 
	y -= tj;
	strcpy(name,"�û�����");
	strcat(name,cur_user.name);
	SetPenColor("White");
	drawBox(x+2.4,y,2.2,th,1,name,'Z',"Black");
	
	SetPenColor(DEFAULT_COLOR);	
}

void ReadRecordFace(int mode)
{
	struct Record *head1, *head2, *head3;//�����Ѷȴ浵 
	struct Record cur_rec;//���浱ǰ��Ҵ浵
	 
	
	char tit_p[]="�ҵĴ浵";
	char title[]="�ҵĴ浵������ģʽ��"; 
	char ch_score[SCORE_MAX_CAP+1];

	double w, h;//���� 
	double th, tj;//�п�,���
	//double sy;//�浵�����ʼyλ��
	double x, y;//�浵�����ǰλ�� 
	double w1, w2, w3;
	bool find = FALSE;
	int i,j;
	
	cur_rec_p1 = cur_rec_p2 = cur_rec_p3 = NULL;
	head1 = head2 = head3 = NULL; 
	strcpy(title,tit_p);
	for(i=0;i<3;i++) classic_rec_state[i]=0;//��ʼ�� 
	
	switch(mode){
		case 0://����ģʽ 
			strcat(title,"������ģʽ��");
			w = 5.0; h = 2.75;
			th = 0.35 ; tj = 0.2;
			w1 = 1.8; w2 = w - 1.0 - 2*tj - w1;
			x = cx-w/2.0+tj;
			y = cy+h/2.0-2*th-1*tj;
			//����ɫ 
			SetPenColor("Gray");
			drawRectangle(cx-w/2.0,cy-h/2.0,w,h,1);
			//������ 
			SetStyle(1);
			SetPointSize(16);
			SetPenColor("White");
			drawBox(cx-w/2.0+tj,cy+h/2.0-2*th-tj,w-tj*2,2*th,1,title,'Z',"Black");
			SetPointSize(13);
			SetStyle(0);
			
			//��ģʽ 
			cur_rec_p1 = SearchRecord(0,cur_user.num);//�Ҵ浵 
			if(cur_rec_p1!=NULL){//����ҵ���¼ 
				find = TRUE;
				classic_rec_state[0]=1;
				//�÷������ͻ�Ϊ�ַ���ǰ�油0 
				ScoreTrans(ch_score, cur_rec_p1->score); 
				//���浵
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"�� �� ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"����",'Z',"White");
				SetPenColor(DEFAULT_COLOR);	
			}
			
			//��ͨģʽ 
			cur_rec_p2 = SearchRecord(1,cur_user.num);//�Ҵ浵 
			if(cur_rec_p2!=NULL){
				find = TRUE;
				classic_rec_state[1]=1;
				//�÷������ͻ�Ϊ�ַ���ǰ�油0 
				ScoreTrans(ch_score, cur_rec_p2->score); 
				//���浵
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"�� ͨ ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"����",'Z',"White");
				SetPenColor(DEFAULT_COLOR);
			}
			
			//����ģʽ 
			cur_rec_p3 = SearchRecord(2,cur_user.num);//�Ҵ浵 
			if(cur_rec_p3!=NULL){
				find = TRUE;
				classic_rec_state[2]=1;
				//�÷������ͻ�Ϊ�ַ���ǰ�油0 
				ScoreTrans(ch_score, cur_rec_p3->score); 
				//���浵
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w1,th,1,"�� �� ",'R',"Black");
				SetPenColor("White");
				drawBox(x+w1,y,w2,th,1,ch_score,'L',"Black");
				SetPenColor("Dark Gray");
				drawBox(x+w1+w2+tj,y,0.8,th,1,"����",'Z',"White");
				SetPenColor(DEFAULT_COLOR);
			}
			
			if(!find){
				y -= (th+tj);
				SetPenColor("White");
				drawBox(x,y,w- 2*tj,th,1,"����տ���Ҳ",'Z',"Black");
			}
			//RecordFaceButtonResp(0);
			
			break;
	}
	//free(cur_rec_p1);
	
}

void RecordFaceButtonResp(int mode, double mx, double my)
{
	double w, h;//���� 
	double th, tj;//�п�,���
	double x, y;//�浵�����ʼλ�� 
	double w1, w2, w3;
	int but_resp_pos = -1;//��ť��Ӧ�浵λ��,-1δ��Ӧ
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
							case 0://��ת����ģʽ 
								classic_mode_state = 1;//����ģʽ�Ѷ�
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //�رմ���GUI 
								ContinRecord(i+1,cur_rec_p1);//������Ϸ
								free(cur_rec_p1);
								break;
							case 1://��ת����ģʽ 
								classic_mode_state = 2;//����ģʽ�Ѷ�
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //�رմ���GUI 
								ContinRecord(i+1,cur_rec_p2);//������Ϸ
								free(cur_rec_p2);
								break;
							case 2://��ת����ģʽ 
								classic_mode_state = 3;//����ģʽ�Ѷ�
								cur_page = 7;
								DeleteRecord(i,cur_user.num);
								CloseMyGui(); //�رմ���GUI 
								ContinRecord(i+1,cur_rec_p3);//������Ϸ
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
	drawLabel(cx-1.0,cy+0.2,"�� �� �� ��");
	SetPointSize(13);
	SetStyle(0);
}



