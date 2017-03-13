/*********************************************************************
* Copyright (*.cpp) 2001
* All rights reserved.
*
* 文件名称：configdeal.cpp
* 文件标识：
* 摘    要：定义Cconfigdeal类，提供配置表文件读、写、修改等功能
* 
* 当前版本：1.0
* 作    者：肖李
* 完成日期：
*
* 取代版本：1.0 
* 原作者  ：肖李
* 完成日期：
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
功    能：从配置表读指定节指定项的字符串型值，用buf带回
性    质：public
输入参数：section--指定的节；item--指定的数据键；buflen--缓存区buf的大小
输出参数：buf--带回字符串型值的缓存区
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::readcfgdata(char *section, char *item, char *buf, int buflen)
{
	int		section_find_flag;
	int		item_find_flag;
	int		no_require_read_flag;
	FILE 	*fp;						//文件指针
	char	section_buf[201];			//缓存section
	char	getbuf[300];				//READ数据缓存区
	char	retbuf[300];				//找到数据键值缓存区
	char	*pstr;

	pstr = NULL;
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		//文件无法打开
		return -1;
	}
	
	if((section == NULL) || (strlen(section) > (sizeof(section_buf) - 2)))
	{
		//section不合法
		fclose(fp);
		return -1;
	}
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	
	if((item == NULL) || (strlen(item) > sizeof(getbuf)))
	{
		//item不合法
		fclose(fp);
		return -1;
	}

	//从文件中读数据
	section_find_flag = -1;
	item_find_flag = -1;
	no_require_read_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//判断是不是注释
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}
		//找[section]
		if(section_find_flag == -1)
		{
			//找[section]
			if((pstr = strstr(getbuf,section_buf)) != NULL)
			{
				section_find_flag = 0;
				continue;
			}
		}
		//找item
		if(section_find_flag == 0)
		{
			//判断本行是否是section
			if(checklinesection(getbuf) == 0)
			{
				//本section结束,找不到item
				no_require_read_flag = 0;
				break;
			}
			//找item
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//去item和空格
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//找到item
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
		//不需要读文件,退出文件
		if(no_require_read_flag == 0)
		{
			break;
		}
	}
	fclose(fp);
	
	//没有找到
	if(item_find_flag != 0)
	{
		return -1;
	}
	//返回缓冲区不够大
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
功	  能：从配置表读指定节指定项的short型值，用buf带回
性    质：public
输入参数：section--指定的节；item--指定的数据键
输出参数：valuep--带回short型值的指针
返回  值：0--成功；-1--失败
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
功	  能：从配置表读指定节指定项的unsigned short型值，用buf带回
性    质：public
输入参数：section--指定的节；item--指定的数据键
输出参数：valuep--带回unsigned short型值的指针
返回  值：0--成功；-1--失败
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
功	  能：从配置表读指定节指定项的int型值，用buf带回
性    质：public
输入参数：section--指定的节；item--指定的数据键
输出参数：valuep--带回int型值的指针
返回  值：0--成功；-1--失败
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
功	  能：从配置表读指定节指定项的unsigned int型值，用buf带回
性    质：public
输入参数：section--指定的节；item--指定的数据键
输出参数：valuep--带回unsigned int型值的指针
返回  值：0--成功；-1--失败
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
功    能：添加字符串型数据键和数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；buf--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	FILE 	*fp;						//文件指针
	FILE 	*fptmp;						//文件指针
	char	section_buf[201];			//缓存section
	char	getbuf[300];				//READ数据缓存区
	char	retbuf[300];				//找到数据键值缓存区
	char	filetmp[120];				//临时文件名
	char	filebak[120];				//备份文件名
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) == 0)
	{
		//已经存在,出错
		return udpcfgdata(section,item,buf);
	//	return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		if ((fp = fopen(filename, "w")) == NULL) 
		//文件无法打开
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//文件无法创建
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//判断是不是注释
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}
		//找[section]
		if((pstr = strstr(getbuf,section_buf)) != NULL)
		{
			//写入[section]项
			if(fprintf(fptmp,"%s",getbuf) < 0)
			{
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			section_find_flag = 0;
			//写入item项
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
	//如果[section]不存在
	if(section_find_flag != 0)
	{
		//写入[section]项
		if(fprintf(fptmp,"%s\n",section_buf) < 0)
		{
			fclose(fptmp);
			return -1;
		}
		//写入item项
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
		//备份原文件rename失败
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//备份原文件rename失败
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
功    能：添加short型数据键和数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：添加unsigned short型数据键和数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：添加int型数据键和数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：添加unsigned int型数据键和数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::addcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return addcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：修改字符串型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；buf--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	int		item_find_flag;
	FILE 	*fp;						//文件指针
	FILE 	*fptmp;						//文件指针
	char	section_buf[201];			//缓存section
	char	getbuf[300];				//READ数据缓存区
	char	retbuf[300];				//找到数据键值缓存区
	char	filetmp[120];				//临时文件名
	char	filebak[120];				//备份文件名
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) != 0)
	{
		//指定的section或item不存在,出错
		return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		if ((fp = fopen(filename, "w")) == NULL) 
		//文件无法打开
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//文件无法创建
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	item_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//判断是不是注释
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}

		if((section_find_flag == 0) && (item_find_flag != 0))
		{
			//判断本行是否是section
			if(checklinesection(getbuf) == 0)
			{
				//本section结束,找不到item
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			//找item
			pstr = NULL;
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//去item和空格
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//找到item
				if (*pstr == EQUALCHARA)
				{
					item_find_flag = 0;
					//修改从文件读的getbuf为输入的值,即item的值
					sprintf(getbuf,"%s=%s\n",item,buf);
				}
			}
		}
		if(section_find_flag != 0)
		{
			//找[section]
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
		//备份原文件rename失败
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//备份原文件rename失败
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
功    能：修改short型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：修改unsigned short型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：修改unsigned int型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：修改int型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::udpcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return udpcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：设置属性filename
性    质：public
输入参数：filename--文件名
输出参数：无
返回  值：0--成功；-1--失败
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
功    能：检查一行内是否存在一个sectoin
性    质：private
输入参数：buf--行数据缓冲区
输出参数：无
返回  值：0--成功；-1--失败
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
功    能：去掉注释项和行结束符
性    质：private
输入参数：buf--行数据缓冲区
输出参数：无
返回  值：0--成功；-1--失败
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
功	  能：检查一行是否是注释行
性    质：private
输入参数：buf--行数据缓冲区
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::checklinenote(char *line)
{
	char	*p_str;
	
	p_str = line;
	//去掉行首的空格和TAB
	while((*p_str == 0X20) || (*p_str == 0X09))
	{
		p_str++;
	}
	
	//为NULL相当与注释
	if (line == NULL)
	{
		return 0;
	}
	
	//本行是注释行，注释标志为“//”
	if ((*p_str == 0x2f) && (*(p_str+1) == 0x2f))
	{
		return 0;	
	}
	return -1;
}

/***********************************************************************
功    能：删除字符串型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；buf--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, char *buf)
{
	int		section_find_flag;
	int		item_find_flag;
	FILE 	*fp;						//文件指针
	FILE 	*fptmp;						//文件指针
	char	section_buf[201];			//缓存section
	char	getbuf[300];				//READ数据缓存区
	char	retbuf[300];				//找到数据键值缓存区
	char	filetmp[120];				//临时文件名
	char	filebak[120];				//备份文件名
	char	*pstr;

	if(readcfgdata(section,item,retbuf,sizeof(retbuf)) != 0)
	{
		//指定的section或item不存在,出错
		return -1;
	}

	pstr = NULL;
	sprintf(filetmp,"%stmp",filename);
	if ((fp = fopen(filename, "r")) == NULL) 
	{
		//文件无法打开
		return -1;
	}
	if ((fptmp = fopen(filetmp, "w")) == NULL) 
	{
		//文件无法创建
		fclose(fp);
		return -1;
	}
	
	sprintf(section_buf,"%c%s%c",LEFTSQUARE,section,RIGHTSQUARE);
	section_find_flag = -1;
	item_find_flag = -1;
	while(fgets(getbuf, sizeof(getbuf), fp) != NULL) 
	{
		//判断是不是注释
		if (checklinenote(getbuf) == 0)
		{
				continue;
		}

		if((section_find_flag == 0) && (item_find_flag != 0))
		{
			//判断本行是否是section
			if(checklinesection(getbuf) == 0)
			{
				//本section结束,找不到item
				fclose(fp);
				fclose(fptmp);
				return -1;
			}
			//找item
			pstr = NULL;
			if((pstr = strstr(getbuf,item)) != NULL)
			{
				//去item和空格
				pstr += strlen(item);
				while(*pstr == BLANKCHARA)
				{
					pstr++;
				}
				//找到item
				if (*pstr == EQUALCHARA)
				{
					item_find_flag = 0;
					//修改从文件读的getbuf为输入的值,即item的值
					sprintf(getbuf,"\n");
				}
			}
		}
		if(section_find_flag != 0)
		{
			//找[section]
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
		//备份原文件rename失败
		return -1;
	}
	if(rename(filetmp,filename) != 0)
	{
		//备份原文件rename失败
		rename(filebak,filename);
		return -1;
	}
	return 0;
}

/***********************************************************************
功    能：删除short型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, short value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：删除unsigned short型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, unsigned short value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：删除unsigned int型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, unsigned int value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

/***********************************************************************
功    能：删除int型数据键值
性    质：public
输入参数：section--指定的节；item--指定的数据键；value--需要添加的数据键值
输出参数：无
返回  值：0--成功；-1--失败
***********************************************************************/
int Cconfigdeal::delcfgdata(char *section, char *item, int value)
{
	sprintf(buffer,"%d",value);
	return delcfgdata(section, item, buffer);
}

