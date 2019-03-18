#ifndef MY_C_STRING_DEAL_TOOLS
#define MY_C_STRING_DEAL_TOOLS

#include <string.h>
#include <stdio.h>

void replaceFirst(char *str1,char *str2,char *str3);
void replace(char *str1,char *str2,char *str3);
void substring(char *dest,char *src,int start,int end);
char charAt(char *src,int index);
int indexOf(char *str1,char *str2);
int lastIndexOf(char *str1,char *str2);
void ltrim(char *str);
void rtrim(char *str);
void trim(char *str);


/*将str1字符串中第一次出现的str2字符串替换成str3*/
void replaceFirst(char *str1,char *str2,char *str3)
{
	char str4[strlen(str1)+1];
	char *p;
	strcpy(str4,str1);
	if((p=strstr(str1,str2))!=NULL)/*p指向str2在str1中第一次出现的位置*/
	{
		while(str1!=p&&str1!=NULL)/*将str1指针移动到p的位置*/
		{
			str1++;
		}
		str1[0]='\0';/*将str1指针指向的值变成\0,以此来截断str1,舍弃str2及以后的内容，只保留str2以前的内容*/
		strcat(str1,str3);/*在str1后拼接上str3,组成新str1*/
		strcat(str1,strstr(str4,str2)+strlen(str2));/*strstr(str4,str2)是指向str2及以后的内容(包括str2),strstr(str4,str2)+strlen(str2)就是将指针向前移动strlen(str2)位，跳过str2*/
	}
}
/*将str1出现的所有的str2都替换为str3*/
void replace(char *str1,char *str2,char *str3)
{
	while(strstr(str1,str2)!=NULL)
	{
		replaceFirst(str1,str2,str3);
	}
}
/*截取src字符串中,从下标为start开始到end-1(end前面)的字符串保存在dest中(下标从0开始)*/
void substring(char *dest,char *src,int start,int end)
{
	char *p=src;
	int i=start;
	if(start>strlen(src))return;
	if(end>strlen(src))
		end=strlen(src);
	while(i<end)
	{	
		dest[i-start]=src[i];
		i++;
	}
	dest[i-start]='\0';
	return;
}
/*返回src中下标为index的字符*/
char charAt(char *src,int index)
{
	char *p=src;
	int i=0;
	if(index<0||index>strlen(src))
		return 0;
	while(i<index)i++;
	return p[i];
}
/*返回str2第一次出现在str1中的位置(下表索引),不存在返回-1*/
int indexOf(char *str1,char *str2)
{
	char *p=str1;
	int i=0;
	p=strstr(str1,str2);
	if(p==NULL)
		return -1;
	else{
		while(str1!=p)
		{
			str1++;
			i++;
		}
	}
	return i;
}
/*返回str1中最后一次出现str2的位置(下标),不存在返回-1*/
int lastIndexOf(char *str1,char *str2)
{
	char *p=str1;
	int i=0,len=strlen(str2);
	p=strstr(str1,str2);
	if(p==NULL)return -1;
	while(p!=NULL)
	{
		for(;str1!=p;str1++)i++;
		p=p+len;
		p=strstr(p,str2);
	}
	return i;
}
/*删除str左边第一个非空白字符前面的空白字符(空格符和横向制表符)*/
void ltrim(char *str)
{
	int i=0,j,len=strlen(str);
	while(str[i]!='\0')
	{
		if(str[i]!=32&&str[i]!=9)break;/*32:空格,9:横向制表符*/
		i++;
	}
	if(i!=0)
	for(j=0;j<=len-i;j++)
	{	
		str[j]=str[j+i];/*将后面的字符顺势前移,补充删掉的空白位置*/
	}
}
/*删除str最后一个非空白字符后面的所有空白字符(空格符和横向制表符)*/
void rtrim(char *str)
{
	char *p=str;
	int i=strlen(str)-1;
	while(i>=0)
	{
		if(p[i]!=32&&p[i]!=9)break;
		i--;
	}
	str[++i]='\0';
}
/*删除str两端的空白字符*/
void trim(char *str)
{
	ltrim(str);
	rtrim(str);
}

#endif //MY_C_STRING_DEAL_TOOLS
