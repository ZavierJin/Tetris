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
#include <string.h>

#include "commonuse.h"
#include "shapebasic.h" 

#include "record.h" 

/***************************************************************
	游戏存档 
***************************************************************/

static struct Record* ReadRecord(int mode);//读取原存档，返回链表头指针 
static void StoreRecord(int mode, struct Record* head);//链表存入文件

struct Record*Create(FILE *fp);//用户信息链表创建 
char* trancolor(int id);//id转颜色
int tranid(char* ch);//颜色转id 
void Print(struct Record* head);
void Fprint(FILE* fp,struct Record* head);
struct Record* DeleteRecList(struct Record* head); 
 

//bool Loadgame();
//bool Savegame();

/*
int main(void){
	FILE *fp,*fp1;
	struct Record* p,*head;


	
	if((fp=fopen("record.txt","r+"))==NULL){
		printf("record open error");
		exit(0);
	}
    if((fp1=fopen("test.txt","w"))==NULL){        
		printf("record open error");
		exit(0);
	}
	head=Create(fp);
	//head=DeleteList(head);
//	Del_record(head,2);
	Print(head);
	Fprint(fp1,head);
	fclose(fp);
	fclose(fp1);
	return 0;
} 
*/
struct Record* ReadRecord(int mode)
{
	FILE *fp;
	struct Record *head;
	char fname[45];
	char id[2];
	head = NULL;
	
	id[0]=mode+'0';
	id[1]='\0';
	strcpy(fname,".\\file\\record");
	strcat(fname,id);
	strcat(fname,".txt");
	//读取原存档 
	if((fp=fopen(fname,"r"))==NULL){   
		exit(-1);
	}
	head = Create(fp);
	fclose(fp);
	return head;
 } 
 
struct Record* SearchRecord(int mode, int num)
{
	struct Record* head=NULL;
	struct Record *cur_rec_ptr;//记录找到的记录的地址 
	struct Record *p, *pre;
//	bool find = FALSE;
	p = pre = NULL;
	
	head = ReadRecord(mode);
	//找记录 
	if(head==NULL){//空表 
		return NULL;
	}else if(head->num==num){//在链表头 
		//find = TRUE;
		cur_rec_ptr = head;
		head=head->next;//链表头指针后移，即从链表中删去这个
	}else{
		for(pre=head,p=head->next; p!=NULL; p=p->next){
			if(p->num==num){
				//find = TRUE;
				cur_rec_ptr = p;
				//链表节点去除
				p=p->next;
				pre->next=p;
				break;
			}
		}
	}
	/* 
	//重新储存 
	if(find){//如果找到了，说明链表有更改，重新储存
		StoreRecord(head);
	}
	DeleteRecList(head);
	*/ 
	DeleteRecList(head);	
	return cur_rec_ptr;//没找到即返回NULL 
} 

void StoreRecord(int mode, struct Record* head)
{
	FILE *fp;
	char fname[45];
	char id[2];
	
	id[0]=mode+'0';
	id[1]='\0';
	strcpy(fname,".\\file\\record");
	strcat(fname,id);
	strcat(fname,".txt");
	//写入文件 
	if((fp=fopen(fname,"w"))==NULL){    
		exit(-1);
	}
	
	Fprint(fp,head);

	//fputc('*',fp);
	fclose(fp);
}

void DeleteRecord(int mode, int num)
{
	struct Record *head=NULL;
	struct Record *p, *pre;
	pre = p = NULL;
	
	head = ReadRecord(mode);
	
	//找记录 
	//修改，防止文件中有重复文件
	while(head!=NULL && head->num==num){//在链表头 
		p = head;
		head=head->next;//链表头指针后移，即从链表中删去这个
		free(p);
	}	
	if(head!=NULL){ 
		pre = head;
		for(p=head->next; p!=NULL; p=p->next){
			if(p->num==num){
				//链表节点去除
				pre->next=p->next;
				free(p);
			}
			pre = p;
		}
	}
	
	StoreRecord(mode, head);
	
	DeleteRecList(head);
}

struct Record* DeleteRecList(struct Record* head)
{
	struct Record* p=NULL;
	while(head!=NULL){
		p=head;
		head=head->next;
		free(p);
	}
	return NULL;
}
/*
struct Record* Del_record(struct Record* head,int num){
	struct Record* ptr1,*ptr2;
	
    if(head!=NULL&&head->num==num){
    	ptr2=head;
    	head=head->next;
    	free(ptr2);
    	return head;
	}
	ptr1=head,ptr2=head->next;
	
	while(ptr2!=NULL){
		if(ptr2->num==num){
			ptr1->next=ptr2->next;
			free(ptr2);
		}
	    else
	    ptr1=ptr2;
		ptr2=ptr1->next;
	}
	return head;
}
*/
void AddRecord(int mode, struct Record* ptr){ 
	struct Record* head=NULL;
	struct Record* p1, *p, *pre;
	pre=p=p1=NULL;
	
	head = ReadRecord(mode);
	
	//防止文件中有重复文件
	while(head!=NULL && head->num==ptr->num){//在链表头 
		p = head;
		head=head->next;//链表头指针后移，即从链表中删去这个
		free(p);
	}	
	if(head!=NULL){ 
		pre = head;
		for(p=head->next; p!=NULL; p=p->next){
			if(p->num==ptr->num){
				//链表节点去除
				pre->next=p->next;
				free(p);
			}
			pre = p;
		}
	}
	
	
	if(head==NULL){
		if((head=(struct Record*)malloc(sizeof(struct Record)))==NULL){//判断是否申请成功 
			exit(-1);
		}
		*head=*ptr;
		head->next=NULL;
	}else{
		while(head->next!=NULL){
			head=head->next;
		}	
		if((p1=(struct Record*)malloc(sizeof(struct Record)))==NULL){//判断是否申请成功 
			exit(-1);
		}
		head->next=p1;
		*p1=*ptr;
		p1->next=NULL;
	}
	
	StoreRecord(mode, head);//存入 
		
	DeleteRecList(head);
}

char* trancolor(int id){
    char* ch;
	switch(id){
		//方块颜色 
		case 0: ch="Blue"; break;
		case 1: ch="Yellow";break;
		case 2: ch="Red";break;
		case 3: ch="Orange";break;
		case 4: ch="Green";break;
		case 5: ch="Magenta";break;
		case 6: ch="Cyan";break;
		//背景颜色
		case 8: ch="White";break;
		case 9: 
		default: ch="Black";break;
	}
	return ch;
} 

int tranid(char* ch){
	int id;
	if(strcmp(ch,"Blue")==0) 			id=0;
	else if(strcmp(ch,"Yellow")==0) 	id=1;
	else if(strcmp(ch,"Red")==0) 		id=2;
    else if(strcmp(ch,"Orange")==0) 	id=3;
	else if(strcmp(ch,"Green")==0) 		id=4;
	else if(strcmp(ch,"Magenta")==0) 	id=5;
	else if(strcmp(ch,"Cyan")==0) 		id=6;
	//背景颜色
	else if(strcmp(ch,"White")==0) 		id=8;
	else if(strcmp(ch,"Black")==0) 		id=9;
	return id; 
}

void Print(struct Record* head){
	struct Record* p;
	int i,j;
	if(head==NULL){
	  printf("NO RECORD");
	  return;
	}
	
	for(p=head;p!=NULL;p=p->next){
		printf("%d\n%d\n",p->num,p->score);
		for(i=0;i<WINDOW_WIDTH+2;i++){
		  for(j=0;j<WINDOW_HEIGHT+1;j++){
    		 printf("%d ",p->window2[i][j].isoccu);
    		 printf("%s ",p->window2[i][j].color); 
		   }
		   putchar('\n');
	    }
	    printf("%d %d\n",p->sh.id,p->nextid);
	    for(i=0;i<4;i++){
	    	for(j=0;j<2;j++){
	    		printf("%d ",p->sh.point[i][j]);
			}	
		}
		putchar('\n');
	}
	
} 

void Fprint(FILE* fp,struct Record* head){
	struct Record* p;
	int i,j;
	if(head==NULL){
		fputc('0',fp);
		return ;
	}
	for(p=head;p!=NULL;p=p->next){
		fprintf(fp,"%d\n%d\n",p->num,p->score);
		for(i=0;i<WINDOW_WIDTH+2;i++){
		  for(j=0;j<WINDOW_HEIGHT+1;j++){
    		 fprintf(fp,"%02d ",p->window2[i][j].isoccu*10+tranid(p->window2[i][j].color));
		   }
		   fputc('\n',fp);
	    }
	    fprintf(fp,"%d %d\n",p->sh.id,p->nextid);
	    for(i=0;i<4;i++){
	    	for(j=0;j<2;j++){
	    		fprintf(fp,"%d ",p->sh.point[i][j]);
			}	
		}
		fputc('\n',fp);
	}
	fputc('0',fp); 
}

struct Record* Create(FILE *fp){
    int num,id,nextid,i,j,score;
    //int block[WINDOW_HEIGHT+1][WINDOW_WIDTH+2];
    int block[WINDOW_WIDTH+2][WINDOW_HEIGHT+1];
    int nowblock[4][2];
	struct Record *head,*p,*tail;
	tail=head=NULL;
	
	fscanf(fp,"%d\n%d\n",&num,&score);
    for(i=0;i<WINDOW_WIDTH+2;i++){
		for(j=0;j<WINDOW_HEIGHT+1;j++){
    		fscanf(fp,"%d ",&block[i][j]);
		}
	}
	
	fscanf(fp,"%d %d\n",&id,&nextid);
	for(i=0;i<4;i++){
		for(j=0;j<2;j++)
		fscanf(fp,"%d ",&nowblock[i][j]);
	}
	
	
/*	printf("%d\n",num);
    for(i=0;i<WINDOW_HEIGHT+1;i++){
		for(j=0;j<WINDOW_WIDTH+2;j++){
    		printf("%d ",block[i][j]);
		}
		putchar('\n');
	}
	
	printf("%d %d\n",id,nextid);
	for(i=0;i<4;i++){
		for(j=0;j<2;j++)
		printf("%d ",nowblock[i][j]);
		fgetc(fp);//读取换行符 
	}

	 exit(0);*/
	while(num!=0){//当记录未到达文件末尾时进入循环,以数字0为结束标志  
	   // printf("%d",num);
		p=(struct Record*)malloc(sizeof(struct Record));
		p->num=num;
		p->score=score;
		p->nextid=nextid;
		for(i=0;i<WINDOW_WIDTH+2;i++){
    	  for(j=0;j<WINDOW_HEIGHT+1;j++){
    		  p->window2[i][j].isoccu=block[i][j]/10;
    		   p->window2[i][j].color=trancolor(block[i][j]%10);
			  }
		}
		p->sh.id=id;
		p->sh.color=trancolor(id);
		for(i=0;i<4;i++){
    	   for(j=0;j<2;j++){
    	   p->sh.point[i][j]=nowblock[i][j];
		   }
		}
		
		p->next=NULL;
		if(head==NULL)
		head=p;
		else   
		tail->next=p;
		tail=p;
		
	    fscanf(fp,"%d %d\n",&num,&score);
        for(i=0;i<WINDOW_WIDTH+2;i++){
	  	    for(j=0;j<WINDOW_HEIGHT+1;j++){
    		   fscanf(fp,"%d ",&block[i][j]);
		    }
	    }
	
	    fscanf(fp,"%d %d\n",&id,&nextid);
	    for(i=0;i<4;i++){
		    for(j=0;j<2;j++)
		       fscanf(fp,"%d ",&nowblock[i][j]);
	    }
	    
	}
	return head; 
}
