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

#include "user.h"

/***************************************************************
	用户及排行榜 
***************************************************************/

struct user* OpenUserFile(){
	FILE *fp;
	int i;
	int num;
	int score[MODE_NUM];
	char name[16];
	struct user *head,*p,*tail;
	tail=head=NULL;
	
	if((fp=fopen(".\\file\\user.txt","r"))==NULL){
		exit(-1);
	}
	
	fscanf(fp,"%d %s ",&num,name);	
	for(i=0;i<MODE_NUM;i++){
		fscanf(fp,"%d",&score[i]);
	}
	//从文件中读取已有记录形成链表
	
	while(num!=0){//当记录未到达文件末尾时进入循环,以数字0为结束标志 
		if((p=(struct user*)malloc(sizeof(struct user)))==NULL){//判断是否申请成功 
			exit(-1);
		}
		p->num=num;
		for(i=0;i<MODE_NUM;i++) p->score[i]=score[i];
		strcpy(p->name,name);
		p->next=NULL;
		if(head==NULL) head=p;
		else tail->next=p;
		tail=p;
		
		fscanf(fp,"%d %s ",&num,name);	
		for(i=0;i<MODE_NUM;i++){
			fscanf(fp,"%d",&score[i]);
		}
	}
	
	fclose(fp);
	return head; 
}
//将链表数据存入用户文件 
void ModifyUserFile(struct user* head)
{
	int i;
	FILE *fp;
	struct user *p = NULL;
	
	if((fp=fopen(".\\file\\user.txt","w"))==NULL){
		exit(-1);
	}
	
	for(p=head;p!=NULL;p=p->next){
		fprintf(fp,"%d %s ",p->num,p->name);
		for(i=0;i<MODE_NUM;i++){
			fprintf(fp,"%d%c",p->score[i],(i==MODE_NUM-1)?'\n':' ');
		} 
	}
	fprintf(fp,"0");
	fclose(fp);

}

bool UserLogin(struct user *p,const char* name,struct user *cur_u)
{
	int num;
	bool succ = FALSE;
	struct user *tail=NULL;
    while(p!=NULL){
    	if(strcmp(name,p->name)==0)//如果在已有记录中找到该用户则返回用户记录的指针； 
    	{
    		*cur_u = *p;
    		succ = TRUE;
    		break;
		}
    	num=p->num;
    	tail=p;
    	p=p->next;
	}
    return succ;
} 

bool UserRegister(struct user *p,const char* name,struct user *cur_u)
{
	int i;
	int num;
	bool succ = TRUE;
	struct user *tail=NULL;
    while(p!=NULL){
    	if(strcmp(name,p->name)==0)//如果在已有记录中找到该用户则返回用户记录的指针； 
    	{
    		succ = FALSE;
    		break;
		}
    	tail=p;
    	p=p->next;
	}
	if(succ){
		if((p=(struct user*)malloc(sizeof(struct user)))==NULL){//判断是否申请成功 
			exit(-1);
		}
		if(tail!=NULL){
			tail->next=p;
			p->num=tail->num+1;
		}else p->num=1;
		
	    p->next=NULL;
	    strcpy(p->name,name);
	    for(i=0;i<MODE_NUM;i++) p->score[i]=0;
	    
	    *cur_u = *p;
	    
	}
    return succ;
}

bool Inputcheck(const char *s)
{
	char ch;
	int i;
	bool islegal = FALSE;
	for(i=0;i<16;i++){
		ch = s[i];
		if(ch=='\0'){//先读到反斜杠0，说明无误，避免超16字符 
			islegal = TRUE;
		}
		else if(ch>='0'&&ch<='9'||ch>='a'&&ch<='z'||ch>='A'&&ch<='Z')
			continue;
		else{//数字与字母之外不合法字符 
			islegal = FALSE;
			break;
		}
	}
	return islegal;
}

struct user* SearchUser(struct user* head, const int num)
{
	bool succ = FALSE;
	struct user* p=NULL;
	for(p=head;p!=NULL;p=p->next){
		if(p->num==num){
			succ = TRUE;
			break;
		}
	}
	if(succ) return p;
	else return NULL;
}

void DeleteList(struct user* head)
{
	struct user* p=NULL;
	while(head!=NULL){
		p=head;
		head=head->next;
		free(p);
	}
}


void RankingFile(int mode_id, char name[][16], int score[], short state)
{
	FILE *fp;
	int i;
	char tem_name[10][16];
	char fname[35];
	char id[2];
	id[0] = mode_id +'0';
	id[1] = '\0';
	strcpy(fname,".\\file\\rank");
	strcat(fname,id);
	strcat(fname,".txt");
	switch(state){
		case 0://读取 
			if((fp=fopen(fname,"r"))==NULL){
				exit(-1);
			}
			for(i=0;i<10;i++){
				fscanf(fp,"%s %d\n",tem_name[i],&score[i]);
				strcpy(name[i],tem_name[i]);
			}
			break;
		case 1://修改 
			if((fp=fopen(fname,"w"))==NULL){
				exit(-1);
			}
			for(i=0;i<10;i++){
				fprintf(fp,"%s %d\n",name[i],score[i]);
			}
			break;	
	}
	
	fclose(fp);
}




