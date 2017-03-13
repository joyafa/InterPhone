/*********************************************************************
* Copyright (*.cpp) 2001
* All rights reserved.
*
* �ļ����ƣ�configdeal.cpp
* �ļ���ʶ��
* ժ    Ҫ������Cconfigdeal�࣬�ṩ���ñ��ļ�����д���޸ĵȹ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�Ф��
* ������ڣ�
*
* ȡ���汾��1.0 
* ԭ����  ��Ф��
* ������ڣ�
**********************************************************************/
#include "stdAfx.h"
#include "configdeal.h"

Cconfigdeal::Cconfigdeal()
{
	filename[0] = '\0';
}

Cconfigdeal::~Cconfigdeal()
{

}

/***********************************************************************
��    �ܣ������ñ��ָ����ָ������ַ�����ֵ����buf����
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���buflen--������buf�Ĵ�С
���������buf--�����ַ�����ֵ�Ļ�����
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, char *buf, int buflen)
{
	int		section_find_flag;
	int		item_find_flag;
	int		no_require_read_flag;
	FILE 	*fp;						//�ļ�ָ��
	char	section_buf[201];			//����section
	char	getbuf[300];				//READ���ݻ�����
	char	retbuf[300];				//�ҵ����ݼ�ֵ������
	char	*pstr;

	pstr = NULL;
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		//�ļ��޷���
		return -1;
	}
	
	if((section == NULL) || (strlen(section) > (sizeof(section_buf) - 2)))
	{
		//section���Ϸ�
		fclose(fp);
		return -1;
	}
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	
	if((item == NULL) || (strlen(item) > sizeof(getbuf)))
	{
		//item���Ϸ�
		fclose(fp);
		return -1;
	}

	//���ļ��ж�����
	section_find_flag = -1;
	item_find_flag = -1;
	no_require_read_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//�ж��ǲ���ע��
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}
		//��[section]
		if(section_find_flag == -1)
		{
			//��[section]
			if((pstr = strstr(getbuf,section_buf)) != NULL)
			{
				section_find_flag = 0;
				continue;
			}
		}
		//��item
		if(section_find_flag == 0)
		{
			//�жϱ����Ƿ���section
			if(checklinesection(getbuf) == 0)
			{
				//��section����,�Ҳ���item
				no_require_read_flag = 0;
				break;
			}
			//��item
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//ȥitem�Ϳո�
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//�ҵ�item
				if (*pstr == EQUALCHARA)
				{
					item_find_flag = 0;
					no_require_read_flag = 0;
					pstr++;
					strcpy(retbuf,pstr);
					delitemtail(retbuf);
					break;
				}
			}
		}
		//����Ҫ���ļ�,�˳��ļ�
		if(no_require_read_flag == 0)
		{
			break;
		}
	}
	fclose(fp);
	
	//û���ҵ�
	if(item_find_flag != 0)
	{
		return -1;
	}
	//���ػ�����������
	if(buflen < (int)strlen(retbuf))
	{
		return -1;
	}
	memcpy(buf,retbuf, (int)strlen(retbuf));
	if (buflen > (int)strlen(retbuf))
	{
		strcpy(buf,retbuf);
	}
	else
	{
		memcpy(buf,retbuf, (int)strlen(retbuf));
	}
	return 0;
}

/***********************************************************************
��	  �ܣ������ñ��ָ����ָ�����short��ֵ����buf����
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ�
���������valuep--����short��ֵ��ָ��
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, short *valuep)
{
	if(readcfgdata(section,item,buffer,sizeof(buffer)) != 0)
	{
		return -1;
	}
	*valuep = (short)atoi(buffer);
	return 0;
}

/***********************************************************************
��	  �ܣ������ñ��ָ����ָ�����unsigned short��ֵ����buf����
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ�
���������valuep--����unsigned short��ֵ��ָ��
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, unsigned short *valuep)
{
	if(readcfgdata(section,item,buffer,sizeof(buffer)) != 0)
	{
		return -1;
	}
	*valuep = (unsigned short)atoi(buffer);
	return 0;
}

/***********************************************************************
��	  �ܣ������ñ��ָ����ָ�����int��ֵ����buf����
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ�
���������valuep--����int��ֵ��ָ��
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, int *valuep)
{
	if(readcfgdata(section,item,buffer,sizeof(buffer)) != 0)
	{
		return -1;
	}
	*valuep = atoi(buffer);
	return 0;
}

/***********************************************************************
��	  �ܣ������ñ��ָ����ָ�����unsigned int��ֵ����buf����
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ�
���������valuep--����unsigned int��ֵ��ָ��
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, unsigned int *valuep)
{
	if(readcfgdata(section,item,buffer,sizeof(buffer)) != 0)
	{
		return -1;
	}
	*valuep = (unsigned int)atoi(buffer);
	return 0;
}

/***********************************************************************
��    �ܣ�����ַ��������ݼ������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���buf--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	FILE 	*fp;						//�ļ�ָ��
	FILE 	*fptmp;						//�ļ�ָ��
	char	section_buf[201];			//����section
	char	getbuf[300];				//READ���ݻ�����
	char	retbuf[300];				//�ҵ����ݼ�ֵ������
	char	filetmp[120];				//��ʱ�ļ���
	char	filebak[120];				//�����ļ���
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) == 0)
	{
		//�Ѿ�����,����
		return udpcfgdata(section,item,buf);
	//	return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		if ((fp = fopen(filename, "w")) == NULL) 
		//�ļ��޷���
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//�ļ��޷�����
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//�ж��ǲ���ע��
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}
		//��[section]
		if((pstr = strstr(getbuf,section_buf)) != NULL)
		{
			//д��[section]��
			if(fprintf(fptmp,"%s",getbuf) < 0)
			{
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			section_find_flag = 0;
			//д��item��
			if(fprintf(fptmp,"%s=%s\n",item,buf) < 0)
			{
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
		}
		else
		{
			if(fprintf(fptmp,"%s",getbuf) < 0)
			{
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
		}
	}
	fclose(fp);
	//���[section]������
	if(section_find_flag != 0)
	{
		//д��[section]��
		if(fprintf(fptmp,"%s\n",section_buf) < 0)
		{
			fclose(fptmp);
			return -1;
		}
		//д��item��
		if(fprintf(fptmp,"%s=%s\n",item,buf) < 0)
		{
			fclose(fptmp);
			return -1;
		}
	}
	fclose(fptmp);
	sprintf(filebak,"%sbak",filename);
	remove(filebak);
	if(rename(filename,filebak) != 0)
	{
		//����ԭ�ļ�renameʧ��
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//����ԭ�ļ�renameʧ��
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
��    �ܣ����short�����ݼ������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ����unsigned short�����ݼ������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ����int�����ݼ������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ����unsigned int�����ݼ������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ��޸��ַ��������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���buf--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	int		item_find_flag;
	FILE 	*fp;						//�ļ�ָ��
	FILE 	*fptmp;						//�ļ�ָ��
	char	section_buf[201];			//����section
	char	getbuf[300];				//READ���ݻ�����
	char	retbuf[300];				//�ҵ����ݼ�ֵ������
	char	filetmp[120];				//��ʱ�ļ���
	char	filebak[120];				//�����ļ���
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) != 0)
	{
		//ָ����section��item������,����
		return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		if ((fp = fopen(filename, "w")) == NULL) 
		//�ļ��޷���
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//�ļ��޷�����
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	item_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//�ж��ǲ���ע��
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}

		if((section_find_flag == 0) && (item_find_flag != 0))
		{
			//�жϱ����Ƿ���section
			if(checklinesection(getbuf) == 0)
			{
				//��section����,�Ҳ���item
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			//��item
			pstr = NULL;
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//ȥitem�Ϳո�
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//�ҵ�item
				if (*pstr == EQUALCHARA)
				{
					item_find_flag = 0;
					//�޸Ĵ��ļ�����getbufΪ�����ֵ,��item��ֵ
					sprintf(getbuf,"%s=%s\n",item,buf);
				}
			}
		}
		if(section_find_flag != 0)
		{
			//��[section]
			if((pstr = strstr(getbuf,section_buf)) != NULL)
			{
				section_find_flag = 0;
			}
		}
		if(fprintf(fptmp,"%s",getbuf) < 0)
		{
			fclose(fp);
			fclose(fptmp);
			return -1;
		}
	}
	fclose(fp);
	fclose(fptmp);
	sprintf(filebak,"%sbak",filename);
	remove(filebak);
	if(rename(filename,filebak) != 0)
	{
		//����ԭ�ļ�renameʧ��
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//����ԭ�ļ�renameʧ��
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
��    �ܣ��޸�short�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ��޸�unsigned short�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ��޸�unsigned int�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ��޸�int�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ���������filename
��    �ʣ�public
���������filename--�ļ���
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::setfilename(char *file)
{
	if(strlen(file) > sizeof(filename))
	{
		return -1;
	}
	strcpy(filename,file);
	return 0;
}

/***********************************************************************
��    �ܣ����һ�����Ƿ����һ��sectoin
��    �ʣ�private
���������buf--�����ݻ�����
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::checklinesection(char *line)
{
	char	*pstr;
	
	pstr = NULL;
	if ((pstr = strchr(line, LEFTSQUARE)) == NULL) 
	{
		return -1;
	}
	return 0;
}

/***********************************************************************
��    �ܣ�ȥ��ע������н�����
��    �ʣ�private
���������buf--�����ݻ�����
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delitemtail(char *buf)
{
	char *pstr;

	if((pstr = strchr(buf,ENDLINE)) != NULL)
	{
		*pstr = 0;
	}
	if((pstr = strchr(buf,ENTERLINE)) != NULL)
	{
		*pstr = 0;
	}
	if((pstr = strstr(buf,NOTEFLAG)) != NULL)
	{
		*pstr = 0;
	}
	return 0;	
}

/***********************************************************************
��	  �ܣ����һ���Ƿ���ע����
��    �ʣ�private
���������buf--�����ݻ�����
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::checklinenote(char *line)
{
	char	*p_str;
	
	p_str = line;
	//ȥ�����׵Ŀո��TAB
	while((*p_str == 0X20) || (*p_str == 0X09))
	{
		p_str++;
	}
	
	//ΪNULL�൱��ע��
	if (line == NULL)
	{
		return 0;
	}
	
	//������ע���У�ע�ͱ�־Ϊ��//��
	if ((*p_str == 0x2f) && (*(p_str+1) == 0x2f))
	{
		return 0;	
	}
	return -1;
}

/***********************************************************************
��    �ܣ�ɾ���ַ��������ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���buf--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	int		item_find_flag;
	FILE 	*fp;						//�ļ�ָ��
	FILE 	*fptmp;						//�ļ�ָ��
	char	section_buf[201];			//����section
	char	getbuf[300];				//READ���ݻ�����
	char	retbuf[300];				//�ҵ����ݼ�ֵ������
	char	filetmp[120];				//��ʱ�ļ���
	char	filebak[120];				//�����ļ���
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) != 0)
	{
		//ָ����section��item������,����
		return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		//�ļ��޷���
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//�ļ��޷�����
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	item_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//�ж��ǲ���ע��
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}

		if((section_find_flag == 0) && (item_find_flag != 0))
		{
			//�жϱ����Ƿ���section
			if(checklinesection(getbuf) == 0)
			{
				//��section����,�Ҳ���item
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			//��item
			pstr = NULL;
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//ȥitem�Ϳո�
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//�ҵ�item
				if (*pstr == EQUALCHARA)
				{
					item_find_flag = 0;
					//�޸Ĵ��ļ�����getbufΪ�����ֵ,��item��ֵ
					sprintf(getbuf,"\n");
				}
			}
		}
		if(section_find_flag != 0)
		{
			//��[section]
			if((pstr = strstr(getbuf,section_buf)) != NULL)
			{
				section_find_flag = 0;
			}
		}
		if(fprintf(fptmp,"%s",getbuf) < 0)
		{
			fclose(fp);
			fclose(fptmp);
			return -1;
		}
	}
	fclose(fp);
	fclose(fptmp);
	sprintf(filebak,"%sbak",filename);
	remove(filebak);
	if(rename(filename,filebak) != 0)
	{
		//����ԭ�ļ�renameʧ��
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//����ԭ�ļ�renameʧ��
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
��    �ܣ�ɾ��short�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ�ɾ��unsigned short�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ�ɾ��unsigned int�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
��    �ܣ�ɾ��int�����ݼ�ֵ
��    �ʣ�public
���������section--ָ���Ľڣ�item--ָ�������ݼ���value--��Ҫ��ӵ����ݼ�ֵ
�����������
����  ֵ��0--�ɹ���-1--ʧ��
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

