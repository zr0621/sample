#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "platform.h"
#include "cjson_wrap.h"
#include "log.h"


//读取配置文件
//fileName	:文件名
//key		:json转换结构体
//outCfg	:[out]读出的数据。注意，outCfg的类型如果不正确会内存越界。
//返回值		:0成功; <0错误码
int cfg_read(const char *fileName, json_kinfo_t *key, void *outCfg)
{
	char *str = NULL;
	{
		FILE *fp = fopen(fileName, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			int len = ftell(fp);
			str = malloc(len+1);
			
			str[0] = 0;
			fseek(fp, 0, SEEK_SET);
			fread(str, 1, len, fp);
			fclose(fp);
		}
		else
		{
			LOG_ERROR("open %s fail\n", fileName);
		}
	}
	
	if(str)
	{
		memset(outCfg, 0, key[0].csize);
		cjson_string2object(key, str, outCfg);
		free(str);
		return 0;
	}
	return ERR_OPEN_FAIL;
}

//写配置文件
//fileName	:文件名
//key		:json转换结构体
//outCfg	:要写入的数据
//返回值	:0成功; <0错误码
int cfg_write(const char *fileName, json_kinfo_t *key, const void *cfg)
{
	int ret = 0;
	char *str = cjson_object2string(key, (void *)cfg);
	if(str)
	{
		FILE *fp = fopen(fileName, "wb");
		if (fp)
		{
			fwrite(str, 1, strlen(str), fp);
			fclose(fp);
			return 0;
		}
		else
		{
			LOG_ERROR("open %s fail\n", fileName);
			ret = ERR_OPEN_FAIL;
		}
		free(str);
	}
	else
	{
		LOG_ERROR("file:%s bad content\n", fileName);
		ret = ERR_BAD_HANDLE;
	}
	return ret;
}

//读取 数组型的 配置文件
//fileName	:文件名
//key		:json转换结构体
//outAryCfg	:[out]读出的数据。注意，outCfg的类型如果不正确会内存越界。
//maxCnt	:数组最大项数
//返回值	:>=0数组个数，<0错误码
int cfg_read_array(const char *fileName, json_kinfo_t *key, void *outAryCfg, int maxCnt)
{
	char *str = NULL;
	{
		FILE *fp = fopen(fileName, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			int len = ftell(fp);
			str = malloc(len+1);
			
			str[0] = 0;
			fseek(fp, 0, SEEK_SET);
			fread(str, 1, len, fp);
			fclose(fp);
		}
	}
	
	if(str)
	{
		int cnt=0;
		cjson_string2object_array(key, str, outAryCfg, maxCnt, &cnt);
		free(str);
		return cnt;
	}
	return ERR_OPEN_FAIL;
}

//写 数组型的 配置文件
//fileName	:文件名
//key		:json转换结构体
//arrCfg	:要写入的数据数组
//cnt		:数组个数
//返回值	:0成功; <0错误码
int cfg_write_array(const char *fileName, json_kinfo_t *key, void *arrCfg, int cnt)
{
	int ret = 0;
	char *str = cjson_object_array2string(key, arrCfg, cnt);
	if(str)
	{
		FILE *fp = fopen(fileName, "wb");
		if (fp)
		{
			fwrite(str, 1, strlen(str), fp);
			fclose(fp);
			return 0;
		}
		else
		{
			ret = ERR_OPEN_FAIL;
		}
		free(str);
	}
	else
	{
		ret = ERR_BAD_HANDLE;
	}
	return ret;
}

