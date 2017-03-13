/*********************************************************************
* Copyright (c) 2001
* All rights reserved.
*
* �ļ����ƣ�configdeal.h   
* �ļ���ʶ��
* ժ    Ҫ������Cconfigdeal�࣬�ṩ���õĽӿ�
* 
* ��ǰ�汾��1.0
* ��    �ߣ�Ф��
* ������ڣ�2001��11��15��
*
* ȡ���汾��1.0 
* ԭ����  ��Ф��
* ������ڣ�2001��11��15��
**********************************************************************/

#if !defined(AFX_CONFIGDEAL_H__7D30F8B5_8CD2_45D2_B758_EB7D6751E3BB__INCLUDED_)
#define AFX_CONFIGDEAL_H__7D30F8B5_8CD2_45D2_B758_EB7D6751E3BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ENDLINE			0x0A				// end of line character
#define ENTERLINE		0x0D				//���з�
#define LEFTSQUARE		'['					//��[
#define RIGHTSQUARE		']'					//��[
#define EQUALCHARA		'='					//���ں�
#define BLANKCHARA		0X20				//�ո�
#define NOTEFLAG		"//"				//ע�ͱ�־//

class Cconfigdeal  
{
public:
	int setfilename(char *filename);
	int delcfgdata(char *section, char *item, int value);
	int delcfgdata(char *section, char *item, unsigned int value);
	int delcfgdata(char *section, char *item, unsigned short value);
	int delcfgdata(char *section, char *item, short value);
	int delcfgdata(char *section, char *item, char *buf);
	int udpcfgdata(char *section, char *item, int value);
	int udpcfgdata(char *section, char *item, unsigned int value);
	int udpcfgdata(char *section, char *item, unsigned short value);
	int udpcfgdata(char *section, char *item, short value);
	int udpcfgdata(char *section, char *item, char *buf);
	int addcfgdata(char *section, char *item, unsigned int value);
	int addcfgdata(char *section, char *item, int value);
	int addcfgdata(char *section, char *item, unsigned short value);
	int addcfgdata(char *section, char *item, short value);
	int addcfgdata(char *section, char *item, char *buf);
	int readcfgdata(char *section, char *item, unsigned int *valuep);
	int readcfgdata(char *section, char *item, int *valuep);
	int readcfgdata(char *section, char *item, unsigned short *valuep);
	int readcfgdata(char *section, char *item, short *valuep);
	int readcfgdata(char *section, char *item, char *buf, int buflen);
	Cconfigdeal();
	virtual ~Cconfigdeal();

private:
	int checklinenote(char *line);
	int delitemtail(char *buf);
	int checklinesection(char *line);
	char buffer[201];
	char filename[301];
};

#endif // !defined(AFX_CONFIGDEAL_H__7D30F8B5_8CD2_45D2_B758_EB7D6751E3BB__INCLUDED_)
