/*
 * cfgfile.h
 *
 *	配置文件相关接口。
 		每个模块都有自己的配置文件。
 		文件是json格式
 */

#ifndef CFGFILE_H
#define CFGFILE_H
#include "cjson_wrap.h"

#ifdef __cplusplus
extern "C"{
#endif

#define CFG_PATH "../cfg/"

//获取配置目录
#define get_config_path() CFG_PATH


//读取配置文件
//fileName	:文件名
//key		:json转换结构体
//outCfg	:[out]读出的数据
//返回值	:0成功; <0错误码
int cfg_read(const char *fileName, json_kinfo_t *key, void *outCfg);

//写配置文件
//fileName	:文件名
//key		:json转换结构体
//outCfg	:要写入的数据
//返回值	:0成功; <0错误码
int cfg_write(const char *fileName, json_kinfo_t *key, const void *cfg);


//读取 数组型的 配置文件
//fileName	:文件名
//key		:json转换结构体
//outAryCfg	:[out]读出的数据。注意，outCfg的类型如果不正确会内存越界。
//maxCnt	:数组最大项数
//返回值	:>=0数组个数，<0错误码
int cfg_read_array(const char *fileName, json_kinfo_t *key, void *outAryCfg, int maxCnt);

//写 数组型的 配置文件
//fileName	:文件名
//key		:json转换结构体
//arrCfg	:要写入的数据数组
//cnt		:数组个数
//返回值	:0成功; <0错误码
int cfg_write_array(const char *fileName, json_kinfo_t *key, void *arrCfg, int cnt);

#ifdef __cplusplus
}
#endif

#endif /* CFGFILE_H */
