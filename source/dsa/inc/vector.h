
#ifndef VECTOR_H_
#define VECTOR_H_

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef Handle Vector;

/**
 *@brief create a vector.
 *
 *@param itemSize size of item
 *@param defCnt default cnt of the vector. 0 if you not sure.
 *
 *@return handle
 */
Vector vector_create(int item_size, int def_cnt);

/**
 *@brief destroy the vector
 *
 *@param handle retval of #vector_create
 *
 *@return 0
 */
int vector_destroy(Vector handle);

//复制src_vector到dst_vector
//dup出的vector的数据是独立的。destroy源vector不会影响dup的vector
//src_vector	:源vecotor
//dst_vector	:目的vecotor，如果为NULL，会创建一个vector
//返回值	:dup出的vector
Vector vector_dup(Handle src_vector, Handle dst_vector);

/**
 *@brief clear the vector
 *
 *@param handle retval of #vector_create
 *
 *@return 0
 */
int vector_clear(Vector handle);

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param item item to add
 *
 *@return 0
 */
int vector_append(Vector handle, const void *item);

/**
 *@brief insert item into vector
 *
 *@param handle retval of #vector_create
 *@param item item to add
 *@param pos position to insert
 *
 *@return 0
 */
int vector_insert(Vector handle, const void *item, int pos);

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param index index to delete
 *
 *@return 0
 */
int vector_del(Vector handle, int index);

/**
 *@brief set item at index
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return 0
 */
int vector_set_at(Vector handle, int index, const void *item);

/**
 *@brief swap two item
 *
 *@param handle retval of #vector_create
 *@param a position of item a
 *@param b position of item b
 *
 *@return 0
 */
int vector_swap(Vector handle, int a, int b);

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return 0
 */
void *vector_at(Vector handle, int index);

/**
 *@brief add item into vector
 *
 *@param handle retval of #vector_create
 *
 *@return count of the vector.
 */
int vector_cnt(Vector handle);

/**
 *@brief find item in vector
 *
 *@param handle retval of #vector_create
 *@param index
 *
 *@return index of the item.
 */
int vector_find(Vector handle, const void *item);

int vector_get_item_size(Vector handle);

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_H_ */
