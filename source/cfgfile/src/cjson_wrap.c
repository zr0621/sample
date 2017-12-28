#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "cjson.h"
#include "cjson_wrap.h"
#include "vector.h"

#define KEY_ADDR(obj, offset)  (((unsigned char *)obj) + offset)

#define KEY_ADDR_U8(obj, offset) ((unsigned char *)KEY_ADDR(obj, offset))
#define KEY_ADDR_U16(obj, offset) ((unsigned short *)KEY_ADDR(obj, offset))
#define KEY_ADDR_U32(obj, offset) ((unsigned int *)KEY_ADDR(obj, offset))
#define KEY_ADDR_FLOAT(obj, offset) ((float *)KEY_ADDR(obj, offset))
#define KEY_ADDR_DOUBLE(obj, offset) ((double *)KEY_ADDR(obj, offset))
#define KEY_ADDR_S8(obj, offset) ((char *)KEY_ADDR(obj, offset))
#define KEY_ADDR_S16(obj, offset) ((short *)KEY_ADDR(obj, offset))
#define KEY_ADDR_S32(obj, offset) ((int *)KEY_ADDR(obj, offset))
#if (JSON_SUPPORT_VECTOR)
#include "vector.h"
#define KEY_ADDR_VECTOR(obj, offset) ((Vector)KEY_ADDR(obj, offset))
#endif

#if defined WIN32 || defined WIN64
#define json_assert(x,express) do{\
		if (!(x))\
		{\
			printf("json_assert failed\n");\
			express;\
		}\
	}while(0)
#else
#define json_assert(x,express) do{\
		if (!(x))\
		{\
			printf("[%s] json_assert failed\n", __func__);\
			express;\
		}\
	}while(0)
#endif

static void _cjson2obj(json_kinfo_t *kinfo, cJSON *json, void *obj)
{
	int i,j,cnt;
	cJSON *sub, *item;

	for (i=0;kinfo[i].key != NULL;i++)
	{
		sub = cJSON_GetObjectItem(json,kinfo[i].key);
		if (!sub)
			continue;
		switch(kinfo[i].type)
		{
		default:
			break;
		case KEY_TYPE_U8:
			*KEY_ADDR_U8(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_U16:
			*KEY_ADDR_U16(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_U32:
			*KEY_ADDR_U32(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_S8:
			*KEY_ADDR_S8(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_S16:
			*KEY_ADDR_S16(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_S32:
			*KEY_ADDR_S32(obj, kinfo[i].offset) = sub->valueint;
			break;
		case KEY_TYPE_FLOAT:
			*KEY_ADDR_FLOAT(obj, kinfo[i].offset) = (float)sub->valuedouble;
			break;
		case KEY_TYPE_DOUBLE:
			*KEY_ADDR_DOUBLE(obj, kinfo[i].offset) = sub->valuedouble;
			break;
		case KEY_TYPE_STRING:
			{
				//这里有个问题，sub->valuestring的长度是实际字符串长度+1, 如果小于kinfo[i].ksize，用memcpy会有越界访问。
				//memcpy(KEY_ADDR_U8(obj, kinfo[i].offset), sub->valuestring, kinfo[i].ksize);
				KEY_ADDR_U8(obj, kinfo[i].offset) [0] = '\0';
				if(sub->valuestring)
					strncpy((char*)KEY_ADDR_U8(obj, kinfo[i].offset), sub->valuestring, kinfo[i].ksize);
				//set the last byte be '\0', to avoid no string end.
				KEY_ADDR_U8(obj, kinfo[i].offset) [kinfo[i].ksize-1] = '\0';
			}
			break;
		case KEY_TYPE_STRING_PTR:
		{
			char *temp = strdup(sub->valuestring);
			unsigned int *temp_int = (unsigned int *)temp;
			*(KEY_ADDR_U32(obj, kinfo[i].offset) + j) = *temp_int;
		}
			break;
		case KEY_TYPE_ARRAY:
			cnt = cJSON_GetArraySize(sub);
			switch(kinfo[i].arraytype)
			{
			default:
				printf(" %d Not Support\n", kinfo[i].arraytype);
				break;
			case KEY_TYPE_STRING:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					int strMaxLen = (kinfo[i].ksize / kinfo[i].arraycnt);
					item = cJSON_GetArrayItem(sub, j);
					memcpy(KEY_ADDR_U8(obj, kinfo[i].offset) + j * strMaxLen, item->valuestring, strMaxLen);
					//set the last byte be '\0', to avoid no string end.
					(KEY_ADDR_U8(obj, kinfo[i].offset) + j * strMaxLen)[strMaxLen-1] = '\0';
				}
				break;
			case KEY_TYPE_STRING_PTR:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					//int strMaxLen = (kinfo[i].ksize / kinfo[i].arraycnt);
					item = cJSON_GetArrayItem(sub, j);
					{
						char *temp = strdup(item->valuestring);
						unsigned int *temp_int = (unsigned int *)temp;
						*(KEY_ADDR_U32(obj, kinfo[i].offset) + j) = *temp_int;
					}
				}
				break;
			case KEY_TYPE_U8:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_U8(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_U16:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_U16(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_U32:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_U32(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_S8:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_S8(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_S16:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_S16(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_S32:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_S32(obj, kinfo[i].offset)[j] = item->valueint;
				}
				break;
			case KEY_TYPE_FLOAT:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_FLOAT(obj, kinfo[i].offset)[j] = (float)item->valuedouble;
				}
				break;
			case KEY_TYPE_DOUBLE:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						KEY_ADDR_DOUBLE(obj, kinfo[i].offset)[j] = item->valuedouble;
				}
				break;
			case KEY_TYPE_OBJECT:
				for (j=0;j<cnt && j<kinfo[i].arraycnt;j++)
				{
					item = cJSON_GetArrayItem(sub, j);
					if (item)
						_cjson2obj(kinfo[i].sub_key, item, KEY_ADDR_U8(obj, kinfo[i].offset) + (kinfo[i].sub_key->csize * j));
				}
				break;
			}
			break;
#if (JSON_SUPPORT_VECTOR)
		case KEY_TYPE_JVECTOR:
			{
				Vector *pHandleVector = KEY_ADDR_VECTOR(obj, kinfo[i].offset);
				cnt = cJSON_GetArraySize(sub);
				*pHandleVector = vector_create(kinfo[i].arrayitem_size, cnt);
				switch(kinfo[i].arraytype)
				{
				default:
					printf(" %d Not Support\n", kinfo[i].arraytype);
					break;
				case KEY_TYPE_STRING:
				{
					int strMaxLen = kinfo[i].arrayitem_size;
					char *pStrItem = malloc(strMaxLen);
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
						{
							//set the last byte be '\0', to avoid no string end.
							pStrItem[0] = '\0';
							if(item->valuestring)
								strncpy(pStrItem, item->valuestring, strMaxLen);
							pStrItem[strMaxLen-1] = '\0';
							vector_append(*pHandleVector, pStrItem);
						}
					}
					free(pStrItem);
					pStrItem = NULL;
				}
					break;
				case KEY_TYPE_STRING_PTR:
					for (j=0;j<cnt;j++)
					{
						//int strMaxLen = (kinfo[i].ksize / kinfo[i].arraycnt);
						item = cJSON_GetArrayItem(sub, j);
						{
							char *temp = strdup(item->valuestring);
							vector_append(*pHandleVector, &temp);
						}
					}
					break;
				case KEY_TYPE_U8:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_U16:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_U32:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_S8:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_S16:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_S32:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valueint);
					}
					break;
				case KEY_TYPE_FLOAT:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valuedouble);
					}
					break;
				case KEY_TYPE_DOUBLE:
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
							vector_append(*pHandleVector, &item->valuedouble);
					}
					break;
				case KEY_TYPE_OBJECT:
				{
					void *pItemObj = malloc(kinfo[i].arrayitem_size);
					for (j=0;j<cnt;j++)
					{
						item = cJSON_GetArrayItem(sub, j);
						if (item)
						{
							memset(pItemObj, 0, kinfo[i].arrayitem_size);
							_cjson2obj(kinfo[i].sub_key, item, pItemObj);
							vector_append(*pHandleVector, pItemObj);
						}
					}
					free(pItemObj);
					pItemObj = NULL;
				}
					break;
				}
			}
			break;
#endif
		case KEY_TYPE_OBJECT:
			_cjson2obj(kinfo[i].sub_key, sub, KEY_ADDR_U8(obj, kinfo[i].offset));
			break;
		}
	}
}

void *cjson_string2object(json_kinfo_t *kinfo, char *string, void *obj)
{
	cJSON *json = cJSON_Parse(string);

	if (obj == NULL)
		obj = malloc(kinfo[0].csize);
	memset(obj, 0, kinfo[0].csize);
	json_assert(json, return NULL);
	_cjson2obj(kinfo, json, obj);
	cJSON_Delete(json);
	return obj;
}

/**
 *@brief 
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param string 输入 要转化的字符串
 *@param obj_array 输出 结构体数组的指针。如果为NULL，会自动分配内存，需要释放
 *@param maxCnt 输入 最多能装载的obj的个数
 *@param cnt 输出 实际得到的obj的个数
 *
 *@return obj。失败时返回NULL
 *
 */
void *cjson_string2object_array(json_kinfo_t *kinfo, char *string, void *obj_array, int maxCnt, int *cnt)
{
	cJSON *json = cJSON_Parse(string);
	cJSON *sub;
	int i;

	json_assert(json, return NULL);
	*cnt = cJSON_GetArraySize(json);

	if (obj_array == NULL)
	{
		obj_array = malloc(kinfo[0].csize * (*cnt));
		json_assert(obj_array, return NULL);
	}
	else if (*cnt > maxCnt)
	{
		*cnt = maxCnt;
	}
	memset(obj_array, 0, kinfo[0].csize * (*cnt));
	
	for (i=0;i<*cnt;i++)
	{
		sub = cJSON_GetArrayItem(json,i);
		_cjson2obj(kinfo, sub, (char*)obj_array + (kinfo->csize * i));
	}
	cJSON_Delete(json);
	return obj_array;
}

static cJSON *_obj2cjson(json_kinfo_t *kinfo, cJSON *json, void *obj)
{
	int i,j;
	cJSON *sub;
	unsigned char *addr;
	unsigned int temp_int = 0;

	for (i=0;kinfo[i].key != NULL;i++)
	{
		switch(kinfo[i].type)
		{
		default:
			printf(" %d Not Support\n", kinfo[i].arraytype);
			break;
		case KEY_TYPE_U8:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_U8(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_U16:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_U16(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_U32:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_U32(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_S8:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_S8(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_S16:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_S16(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_S32:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_S32(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_FLOAT:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_FLOAT(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_DOUBLE:
			cJSON_AddNumberToObject(json,kinfo[i].key, *KEY_ADDR_DOUBLE(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_STRING:
			cJSON_AddStringToObject(json, kinfo[i].key, (char *)KEY_ADDR_U8(obj, kinfo[i].offset));
			break;
		case KEY_TYPE_STRING_PTR:
			temp_int = (*KEY_ADDR_U32(obj, kinfo[i].offset));
			cJSON_AddStringToObject(json, kinfo[i].key, (char *)&temp_int);
			break;
		case KEY_TYPE_ARRAY:
			switch(kinfo[i].arraytype)
			{
			default:
				printf(" %d Not Support\n", kinfo[i].arraytype);
				sub = NULL;
				break;
			case KEY_TYPE_STRING:
				//printf(" %d Not Support\n", kinfo[i].arraytype);
				/*注意，这里只支持二维数组方式保存的字符串数组*/
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub,
						cJSON_CreateString((char *)KEY_ADDR_U8(obj, kinfo[i].offset) + j * (kinfo[i].ksize / kinfo[i].arraycnt)));
				}
				break;
			case KEY_TYPE_STRING_PTR:
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					temp_int = (*(KEY_ADDR_U32(obj, kinfo[i].offset) + j));
					cJSON_AddItemToArray(sub,
						cJSON_CreateString((char *)&temp_int)
						);
				}
				break;
			case KEY_TYPE_U8:
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(KEY_ADDR_U8(obj, kinfo[i].offset)[j]));
				}
				break;
			case KEY_TYPE_U16:
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(KEY_ADDR_U16(obj, kinfo[i].offset)[j]));
				}
				break;
			case KEY_TYPE_U32:
				sub = cJSON_CreateIntArray((int*)KEY_ADDR_U32(obj, kinfo[i].offset), kinfo[i].arraycnt);
				break;
			case KEY_TYPE_S8:
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(KEY_ADDR_S8(obj, kinfo[i].offset)[j]));
				}
				break;
			case KEY_TYPE_S16:
				sub = cJSON_CreateArray();
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(KEY_ADDR_S16(obj, kinfo[i].offset)[j]));
				}
				break;
			case KEY_TYPE_S32:
				sub = cJSON_CreateIntArray(KEY_ADDR_S32(obj, kinfo[i].offset), kinfo[i].arraycnt);
				break;
			case KEY_TYPE_FLOAT:
				sub = cJSON_CreateFloatArray(KEY_ADDR_FLOAT(obj, kinfo[i].offset), kinfo[i].arraycnt);
				break;
			case KEY_TYPE_DOUBLE:
				sub = cJSON_CreateDoubleArray(KEY_ADDR_DOUBLE(obj, kinfo[i].offset), kinfo[i].arraycnt);
				break;
			case KEY_TYPE_OBJECT:
				if (kinfo[i].sub_key == NULL)
				{
					printf("ERROR: kinfo[i].sub_key should not be NULL\n");
					break;
				}
				sub = cJSON_CreateArray();
				addr = KEY_ADDR_U8(obj, kinfo[i].offset);

				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					cJSON_AddItemToArray(sub, _obj2cjson(kinfo[i].sub_key, cJSON_CreateObject(), addr + (kinfo[i].sub_key->csize * j)));
				}
				break;
			}
			cJSON_AddItemToObject(json, kinfo[i].key, sub);
			break;
#if (JSON_SUPPORT_VECTOR)
		case KEY_TYPE_JVECTOR:
		{
			Vector *pHandleVector = KEY_ADDR_VECTOR(obj, kinfo[i].offset);
			int nCount = vector_cnt(*pHandleVector);
			switch(kinfo[i].arraytype)
			{
			default:
				printf(" %d Not Support\n", kinfo[i].arraytype);
				sub = NULL;
				break;
			case KEY_TYPE_STRING:
				//printf(" %d Not Support\n", kinfo[i].arraytype);
				/*注意，这里只支持二维数组方式保存的字符串数组*/
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,
						cJSON_CreateString((char *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_STRING_PTR:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					temp_int = (*(unsigned int *)(vector_at(*pHandleVector,j)));
					cJSON_AddItemToArray(sub,
						cJSON_CreateString((char *)&temp_int)
						);
				}
				break;
			case KEY_TYPE_U8:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(unsigned char *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_U16:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(unsigned short *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_U32:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(unsigned int *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_S8:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(char *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_S16:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(short *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_S32:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(int *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_FLOAT:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(float *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_DOUBLE:
				sub = cJSON_CreateArray();
				for (j=0;j<nCount;j++)
				{
					cJSON_AddItemToArray(sub,cJSON_CreateNumber(*(double *)vector_at(*pHandleVector,j)));
				}
				break;
			case KEY_TYPE_OBJECT:
				if (kinfo[i].sub_key == NULL)
				{
					printf("ERROR: kinfo[i].sub_key should not be NULL\n");
					break;
				}
				sub = cJSON_CreateArray();
				addr = KEY_ADDR_U8(obj, kinfo[i].offset);

				for (j=0;j<nCount;j++)
				{
					void *pItemObj = vector_at(*pHandleVector,j);
					cJSON_AddItemToArray(sub, _obj2cjson(kinfo[i].sub_key, cJSON_CreateObject(), pItemObj));
				}
				break;
			}
			cJSON_AddItemToObject(json, kinfo[i].key, sub);
		}
			break;
#endif
		case KEY_TYPE_OBJECT:
			cJSON_AddItemToObject(json, kinfo[i].key, _obj2cjson(kinfo[i].sub_key, cJSON_CreateObject(), KEY_ADDR(obj, kinfo[i].offset)));
			break;
		}
	}
	return json;
}

void cjson_string2object_free(json_kinfo_t *kinfo, void *obj)
{
	int i,j;
	//cJSON *sub;
	//unsigned char *addr;

	for (i=0;kinfo[i].key != NULL;i++)
	{
		switch(kinfo[i].type)
		{
		default:
			break;
		case KEY_TYPE_STRING_PTR:
		{
			unsigned int temp_int = *(KEY_ADDR_U32(obj, kinfo[i].offset) + j);
			char *temp = (char *)(&temp_int);
//			printf("test free: %d : %x\n", __LINE__, temp);
			if (temp)
				free(temp);
		}
			break;
		case KEY_TYPE_ARRAY:
			switch(kinfo[i].arraytype)
			{
			default:
				break;
			case KEY_TYPE_STRING_PTR:
				for (j=0;j<kinfo[i].arraycnt;j++)
				{
					unsigned int temp_int = *(KEY_ADDR_U32(obj, kinfo[i].offset) + j);
					char *temp = (char *)(&temp_int);
//					printf("test free: %d : %x\n", __LINE__, temp);
					if (temp)
						free(temp);
				}
				break;
			}
			break;
		}
	}
	return ;
}

/**
 *@brief 将#cjson_string2object 或 #cjson_string2object_array 转化后的结构体，做必要的释放操作。
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param obj_array 需要释放的资源
 *@param cnt 输入 实际得到的obj的个数。对于 #cjson_string2object ，cnt为1
 *
 */
void cjson_string2object_array_free(json_kinfo_t *kinfo, void *obj_array, int cnt)
{
	int i;
	for (i=0;i<cnt;i++)
	{
		cjson_string2object_free(kinfo, (char*)obj_array + (kinfo->csize * i));
	}
}

char *cjson_object2string(json_kinfo_t *kinfo, void *obj)
{
	cJSON *json = cJSON_CreateObject();
	char *out;

	_obj2cjson(kinfo, json, obj);
	out = cJSON_Print(json);
	cJSON_Delete(json);

	return out;
}

/**
 *@brief 
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param obj_array 输入 要转化的结构体数组的指针
 *@param cnt 数组个数
 *
 *@return json格式的字符串。NULL if failed
 *
 *@note 其返回的字符串，需要用free释放
 *
 */
char *cjson_object_array2string(json_kinfo_t *kinfo, void *obj_array, int cnt)
{
	cJSON *json = cJSON_CreateArray();
	cJSON *item;
	char *out;
	int i;

	for (i=0;i<cnt;i++)
	{
		item = cJSON_CreateObject();
		_obj2cjson(kinfo, item, (char*)obj_array + (kinfo->csize * i));
		cJSON_AddItemToArray(json,item);
	}

	out = cJSON_Print(json);
	cJSON_Delete(json);
	return out;
}

//下面几个函数用于json和字符串之间进行转换
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *@brief 将#cjson_json2object 或 #cjson_json2object_array 转化后的结构体，做必要的释放操作。
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param obj_array 需要释放的资源
 *@param cnt 输入 实际得到的obj的个数。对于 #cjson_json2object ，cnt为1
 *
 */
void cjson_json2object_array_free(json_kinfo_t *kinfo, void *obj_array, int cnt)
{
	int i;
	for (i=0;i<cnt;i++)
	{
		cjson_string2object_free(kinfo, (char*)obj_array + (kinfo->csize * i));
	}
}

/**
 *@brief 将JSON结构体转化为结构体
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param json 输入 要转化的json结构体
 *@param obj 输出 结构体指针。如果为NULL，会自动分配内存，需要调用cjson_json2object_array_free释放
 *
 *@return obj。失败时返回NULL
 *
 */
void *cjson_json2object(json_kinfo_t *kinfo, cJSON *json, void *obj)
{
	json_assert(json, return NULL);
	if (obj == NULL)
		obj = malloc(kinfo[0].csize);
	_cjson2obj(kinfo, json, obj);
	return obj;
}

/**
 *@brief 将JSON结构体转化为结构体数组
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param json 输入 要转化的json
 *@param obj_array 输出 结构体数组的指针。如果为NULL，会自动分配内存，需要释放
 *@param maxCnt 输入 最多能装载的obj的个数
 *@param cnt 输出 实际得到的obj的个数
 *
 *@return obj。失败时返回NULL
 *
 */
void *cjson_json2object_array(json_kinfo_t *kinfo, cJSON *json, void *obj_array, int maxCnt, int *cnt)
{
	cJSON *sub;
	int i;

	json_assert(json, return NULL);
	*cnt = cJSON_GetArraySize(json);

	if (obj_array == NULL)
	{
		obj_array = malloc(kinfo[0].csize * (*cnt));
		json_assert(obj_array, return NULL);
	}
	else if (*cnt > maxCnt)
		*cnt = maxCnt;

	for (i=0;i<*cnt;i++)
	{
		sub = cJSON_GetArrayItem(json,i);
		_cjson2obj(kinfo, sub, (char*)obj_array + (kinfo->csize * i));
	}
	return obj_array;
}

/**
 *@brief 将结构体转化为json
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param obj 输入 要转化的结构体地址
 *
 *@return json。NULL if failed
 *
 *@note 其返回的json，需要用cJSON_Delete释放
 *
 */
cJSON *cjson_object2json(json_kinfo_t *kinfo, void *obj)
{
	cJSON *json = cJSON_CreateObject();

	_obj2cjson(kinfo, json, obj);

	return json;
}

/**
 *@brief 将结构体数组转化为JSON
 *@param kinfo 输入 结构体信息，用于识别结构体各成员
 *@param obj_array 输入 要转化的结构体数组的指针
 *@param cnt 数组个数
 *
 *@return json。NULL if failed
 *
 *@note 其返回的字符串，需要用cJSON_Delete释放
 *
 */
cJSON *cjson_object_array2json(json_kinfo_t *kinfo, void *obj_array, int cnt)
{
	cJSON *json = cJSON_CreateArray();
	cJSON *item;
	int i;

	for (i=0;i<cnt;i++)
	{
		item = cJSON_CreateObject();
		_obj2cjson(kinfo, item, (char*)obj_array + (kinfo->csize * i));
		cJSON_AddItemToArray(json,item);
	}

	return json;
}
