#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "vector.h"

typedef struct{
	int capacity;	//vector 的容量，能容纳的元素个数
	int cnt;		//当前数量
	int item_size;	//一个元素的大小
	void *buffer;
}_vector_info_t;

#define _get_ptr(vector, pos)	((unsigned char *)(vector->buffer) + (pos)*(vector->item_size))

/*
 *@brief create a vector.
 *
 *@param item_size size of item
 *@param def_cnt default cnt of the vector. 8 if you not sure.
 *
 *@return handle
*/
Vector vector_create(int item_size, int def_cnt)
{
	_vector_info_t *vector;

	if (def_cnt <= 0)
		def_cnt = 8;

	vector = malloc(sizeof(_vector_info_t));
	if (vector)
	{
		vector->capacity = def_cnt;
		vector->cnt = 0;
		vector->item_size = item_size;
		vector->buffer = malloc(item_size * def_cnt);
		if (!vector->buffer)
		{
			free(vector);
			printf("failed malloc memory: %d in vector_create\n", item_size * def_cnt);
			return NULL;
		}
	}

	return vector;
}

/*
 *@brief destroy the vector
 *
 *@param handle retval of #vector_create
 *
 *@return 0
*/
int vector_destroy(Vector handle)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	if (vector->buffer)
	{
		free(vector->buffer);
	}
	free(vector);

	return 0;
}

//复制src_vector到dst_vector
//dup出的vector的数据是独立的。destroy源vector不会影响dup的vector
//src_vector	:源vecotor
//dst_vector	:目的vecotor，如果为NULL，会创建一个vector
//返回值	:dup出的vector
Vector vector_dup(Handle src_vector, Handle dst_vector)
{
	if(!src_vector)
		return NULL;

	if(!src_vector)
	{
		_vector_info_t *src = (_vector_info_t *)src_vector;
		_vector_info_t *dst = (_vector_info_t *)malloc(sizeof(_vector_info_t));
		if(!dst)
			return NULL;
		memset(dst, 0, sizeof(_vector_info_t));
		dst->capacity = src->capacity;
		dst->cnt = src->cnt;
		dst->item_size = src->item_size;
		dst->buffer = malloc(src->item_size * src->capacity);
		memcpy(dst->buffer, src->buffer, (src->item_size * src->capacity));
		return dst;
	}
	else
	{
		vector_clear(src_vector);
		int i;
		int cnt = vector_cnt(src_vector);
		for(i=0; i<cnt; i++)
		{
			void *item = vector_at(src_vector, i);
			if(item)
				vector_append(src_vector, item);
		}
		return src_vector;
	}
}

/**
 *@brief clear the vector
 *
 *@param handle retval of #vector_create
 *
 *@return 0
 */
int vector_clear(Vector handle)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	vector->cnt = 0;
	return 0;
}

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param item item to add
 *
 *@return 0
 */
int vector_append(Vector handle, const void *item)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;
	if (vector->cnt >= vector->capacity)
	{
		vector->capacity *= 2;
		vector->buffer = realloc(vector->buffer, vector->item_size * vector->capacity);
	}

	memcpy(_get_ptr(vector, vector->cnt), item, vector->item_size);
	vector->cnt++;

	return 0;
}

/**
 *@brief insert item into vector
 *
 *@param handle retval of #vector_create
 *@param item item to add
 *@param pos position to insert
 *
 *@return 0
 */
int vector_insert(Vector handle, const void *item, int pos)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	if (pos > vector->cnt)
	{
		return ERR_BAD_PARAM;
	}

	if (vector->cnt >= vector->capacity)
	{
		vector->capacity *= 2;
		vector->buffer = realloc(vector->buffer, vector->item_size * vector->capacity);
	}

	if (pos < vector->cnt)
	{
		memmove(_get_ptr(vector, pos+1), _get_ptr(vector, pos), (vector->cnt-pos)*vector->item_size);
	}
	memcpy(_get_ptr(vector, pos), item, vector->item_size);
	vector->cnt++;

	return 0;
}

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param index index to delete
 *
 *@return 0
 */
int vector_del(Vector handle, int index)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	if (index >= vector->cnt)
	{
		return ERR_BAD_PARAM;
	}

	vector->cnt--;
	if (index < vector->cnt)
	{
		memmove(_get_ptr(vector, index), _get_ptr(vector, index+1), (vector->cnt-index)*vector->item_size);
	}

	return 0;
}

/**
 *@brief set item at index
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return 0
 */
int vector_set_at(Vector handle, int index, const void *item)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;
	if (index < vector->cnt && index >= 0)
	{
		memcpy(_get_ptr(vector, index), item, vector->item_size);
		return 0;
	}
	return ERR_NOT_FIND;
}


/**
 *@brief swap two item
 *
 *@param handle retval of #vector_create
 *@param a position of item a
 *@param b position of item b
 *
 *@return 0
 */
int vector_swap(Vector handle, int a, int b)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	if (a >= vector->cnt || b >= vector->cnt)
	{
		return ERR_BAD_PARAM;
	}

	{
		unsigned char temp[256];
		unsigned char *p = NULL;
		if (vector->item_size < sizeof(temp))
		{
			p = temp;
		}
		else
		{
			p = malloc(vector->item_size);
		}

		memcpy(p, _get_ptr(vector, a), vector->item_size);
		memcpy(_get_ptr(vector, a), _get_ptr(vector, b), vector->item_size);
		memcpy(_get_ptr(vector, b), p, vector->item_size);

		if (p != temp)
			free(p);
	}
	return 0;
}

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return 0
 */
void *vector_at(Vector handle, int index)
{
	if(!handle)
		return NULL;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	if (index < vector->cnt && index >= 0)
		return _get_ptr(vector, index);
	return NULL;
}

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *
 *@return count of the vector.
 */
int vector_cnt(Vector handle)
{
	if(!handle)
		return 0;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	return vector->cnt;
}

/**
 *@brief find item in vector
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return index of the item.
 */
int vector_find(Vector handle, const void *item)
{
	if(!handle)
		return ERR_BAD_HANDLE;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	int i;
	int cnt = vector->cnt;
	for(i=0; i<cnt; i++)
	{
		void *p = _get_ptr(vector, i);
		if(memcmp(p, item, vector->item_size) == 0)
			return i;
	}
	return -1;
}

int vector_get_item_size(Vector handle)
{
	if(!handle)
		return 0;
	
	_vector_info_t *vector = (_vector_info_t *)handle;

	return vector->item_size;
}


