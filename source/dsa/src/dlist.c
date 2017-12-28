#include "dlist.h"
#include "platform.h"

int dlist_init(dlist *list)
{
	if (list)
	{
		memset(list, 0, sizeof(dlist));
		return SUCCESS;
	}
	return FAILURE;
}

int dlist_count(dlist *list)
{
	if (!list)
		return 0;
	return list->count;
}

int dlist_int_comp(void *a, void *b)  
{  
	return *(long *)b - *(long *)a;  
}  

//注册compare函数，取决于dist存放的数据是整形还是字符串，
//此处一般比较指针是否相同，可以使用上面的dlist_int_comp()接口
int dlist_set_compare(dlist * list, list_compare compare)
{
	if (!list)
		return FAILURE;

	list->compare = compare;

	return SUCCESS;
}

int dlist_add(dlist *list, void *data)
{
	list_node *tmp;

	if (!list)
		return FAILURE;

	tmp = (list_node *)malloc(sizeof(list_node));
	if (!tmp)
		return FAILURE;

	tmp->data = data;
	tmp->next = 0;
	tmp->prev = list->tail;
	if (list->tail)
	{
		list->tail->next = tmp;
	}
	else
	{
		list->head = tmp;
	}
	list->tail = tmp;
	list->count++;

	return SUCCESS;
}

int dlist_insert(dlist *list, void *data)
{
	list_node *tmp, *t;

	if (!list)
		return FAILURE;

	if (list->compare == NULL)
	{
		return dlist_add(list, data);
	}

	t = (list_node *)malloc(sizeof(list_node));
	if (!t)
		return FAILURE;

	t->data = data;
	t->prev = 0;
	t->next = 0;

	tmp = list->tail;
	while (tmp)
	{
		if (list->compare(data, tmp->data) >= 0)
		{
			t->next = tmp->next;
			t->prev = tmp;
			tmp->next = t;
			if (tmp == list->tail)
			{
				list->tail = t;
			}
			break;
		}
		tmp = tmp->prev;
	}
	if (!tmp)
	{
		t->next = list->head;
		list->head = t;
		if (!list->tail)
		{
			list->tail = t;
		}
	}
	list->count++;

	return SUCCESS;
}

int dlist_insert_node(dlist *list, list_node *node)
{
	list_node *tmp;

	if (!list)
		return FAILURE;

	if (list->compare == NULL)
	{
		return dlist_add_node(list, node);
	}

	tmp = list->tail;
	while (tmp)
	{
		if (list->compare(node->data, tmp->data) >= 0)
		{
			node->next = tmp->next;
			node->prev = tmp;
			tmp->next = node;
			if (tmp == list->tail)
			{
				list->tail = node;
			}
			break;
		}
		tmp = tmp->prev;
	}
	if (!tmp)
	{
		node->next = list->head;
		list->head = node;
		if (!list->tail)
		{
			list->tail = node;
		}
	}
	list->count++;

	return SUCCESS;
}

int dlist_add_node(dlist * list, list_node *node)
{
	if ((!list) || (!node))
		return FAILURE;

	node->next = 0;
	node->prev = list->tail;
	if (list->tail)
	{
		list->tail->next = node;
	}
	else
	{
		list->head = node;
	}
	list->tail = node;
	list->count++;

	return SUCCESS;
}

int dlist_move_node(dlist *list, list_node *node)
{
	if ((!list) || (!node))
		return FAILURE;

	if (node->prev)
	{
		node->prev->next = node->next;
	}

	if (node->next)
	{
		node->next->prev = node->prev;
	}

	if (list->head == node)
	{
		list->head = node->next;
	}

	if (list->tail == node)
	{
		list->tail = node->prev;
	}

	list->count--;

	return SUCCESS;
}

int dlist_delete_node(dlist *list, list_node *node)
{
	int ret = FAILURE;

	if (node)
	{
		ret = dlist_move_node(list, node);
		free(node);
	}

	return ret;
}

int dlist_delete(dlist *list, void *data)
{
	list_node *tmp;

	if ((!list) || (!data))
		return FAILURE;

	tmp = list->head;
	while (tmp)
	{
		if (tmp->data == data)
		{
			list->count--;
			if (tmp == list->head)
			{
				list->head = tmp->next;
				if (list->head)
					list->head->prev = 0;
			}
			if (tmp == list->tail)
			{
				list->tail = tmp->prev;
			}
			if (tmp->prev)
				tmp->prev->next = tmp->next;
			if (tmp->next)
				tmp->next->prev = tmp->prev;
			free(tmp);
			return SUCCESS;
		}
		tmp = tmp->next;
	}

	return FAILURE;
}

int dlist_clean(dlist *list)
{
	list_node *tmp, *tmp1;

	if (!list)
		return FAILURE;

	tmp = list->head;
	list->count = 0;
	list->head = 0;
	list->tail = 0;
	while (tmp)
	{
		tmp1 = tmp->next;
		free(tmp);
		tmp = tmp1;
	}
	return SUCCESS;
}

int dlist_clean_data(dlist *list)
{
	list_node *tmp, *tmp1;

	if (!list)
		return FAILURE;

	tmp = list->head;
	list->count = 0;
	list->head = 0;
	list->tail = 0;
	while (tmp)
	{
		tmp1 = tmp->next;
		free(tmp->data);
		tmp->data = NULL;
		free(tmp);
		tmp = tmp1;
	}
	return SUCCESS;
}

int dlist_append(dlist *toList, dlist *addList)
{
	if (!toList || !addList || !addList->tail)
		return FAILURE;

	toList->count += addList->count;
	addList->head->prev = toList->tail;
	if (toList->tail)
	{
		toList->tail->next = addList->head;
	}
	else
	{
		toList->head = addList->head;
	}
	toList->tail = addList->tail;
	addList->count = 0;
	addList->head = 0;
	addList->tail = 0;

	return SUCCESS;
}

int dlist_get_index(dlist *list, void *data)
{
	list_node *tmp;
	int n;

	if (!list)
		return -1;

	tmp = list->head;
	n = 0;
	while (tmp)
	{
		if (tmp->data == data)
			return n;
		n++;
		tmp = tmp->next;
	}

	return -1;
}

void *dlist_get_data(dlist *list, int index)
{
	list_node *tmp;
	int n;

	if (!list)
		return NULL;

	if (index >= list->count)
		return NULL;

	tmp = list->head;
	for (n = 0; n < index; n++)
	{
		if (tmp)
			tmp = tmp->next;
		else
			return NULL;
	}
	if (tmp)
		return tmp->data;
	else
		return NULL;
}

void *dlist_pop(dlist *list)
{
	void *data = NULL;
	list_node *node = dlist_pop_node(list);
	if (node)
	{
		data = node->data;
		free(node);
	}

	return data;
}

list_node *dlist_pop_node(dlist * list)
{
	list_node *node = NULL;
	if (list)
	{
		node = list->head;
		if (node)
		{
			list->head = node->next;
			list->count--;
			if (node == list->tail)
			{
				list->tail = NULL;
			}
			node->next = 0;
			node->prev = 0;
		}
		if (list->head)
		{
			list->head->prev = NULL;
		}
	}
	return node;
}

list_node *dlist_move_data(dlist * list, void *data)
{
	list_node *tmp;

	if (!list)
		return NULL;

	tmp = list->head;
	while (tmp)
	{
		if (tmp->data == data)
		{
			break;
		}
		tmp = tmp->next;
	}

	if (tmp)
	{
		if (tmp->prev)
		{
			tmp->prev->next = tmp->next;
		}
		else
		{
			list->head = tmp->next;
			if (list->head)
			{
				list->head->prev = NULL;
			}
		}
		if (tmp->next)
		{
			tmp->next->prev = tmp->prev;
		}
		else
		{
			list->tail = tmp->prev;
		}
		list->count--;
	}

	return tmp;
}

list_node *dlist_get_first_node(dlist *list)
{
	if (list)
		return list->head;
	else
		return NULL;
}

list_node *dlist_get_last_node(dlist *list)
{
	if (list)
		return list->tail;
	else
		return NULL;
}

list_node *dlist_get_next_node(dlist *list, list_node *base_node)
{
	if (!list || !base_node)
		return NULL;

	return base_node->next;
}

list_node *dlist_get_prev_node(dlist *list, list_node *base_node)
{
	if (!list || !base_node)
		return NULL;

	return base_node->prev;
}

list_node *dlist_find_node(dlist *list, void *data)
{
	list_node *tmp;
	if (!list)
		return NULL;

	tmp = list->head;
	while (tmp)
	{
		if (tmp->data == data)
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

dlist *dlist_create()
{
	dlist *tmp = (dlist *)malloc(sizeof(dlist));
	if (!tmp)
		return NULL;

	dlist_init(tmp);

	return tmp;
}

int dlist_destrory(dlist *list)
{
	if (!list)
		return FAILURE;

	dlist_clean(list);
	free(list);

	return SUCCESS;
}

//实现用户输入一组单词，记录字符串，然后将这些字符串排列组合拼接然后显示
int dlist_test()
{
	dlist list;
	char *input_string;
	char temp1[32];
	char temp2[32];
	char *result;
	int input_enable = 1;
	int i,j;
	dlist_init(&list);
	//dlist_set_compare(&list, _int_comp);
	printf("please input word, press 0 stop\n");
	do
	{
		input_string = malloc(32);
		memset(input_string, 0, 32);
		scanf("%s",input_string);
		if(input_string[0] == '0')
		{
			input_enable = 0;
			break;
		}
		dlist_add(&list,input_string);
	}while(input_enable);
	printf("list.count = %d\n",list.count);
	
	for(i = 0; i < list.count; i++)
	{	
		for(j = 0; j < list.count; j++)
		{
			if(i == j)
				continue;
			
			memset(temp1, 0, 32);
			strcpy(temp1,dlist_get_data(&list, i));
			memset(temp2, 0, 32);
			strcpy(temp2,dlist_get_data(&list, j));

			result = (char *)strcat(temp1,temp2);
			printf("%s\n",result);
			
			memset(temp1, 0, 32);
			strcpy(temp1,dlist_get_data(&list, i));
			
			result = (char *)strcat(temp2,temp1);
			printf("%s\n",result);
		}
	}
	dlist_clean_data(&list);//in order to prevent memory leaks
	return 0;
}
