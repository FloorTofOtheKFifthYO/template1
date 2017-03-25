#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

#include "stm32f4xx.h"
#include "configuration.h"

typedef struct node{
    void *data;
    struct node *link;
}list_node,*link_list;


int list_init(link_list *first);
void list_clear(link_list *first);
int list_get_length(link_list *first);
int list_isempty(link_list *first);


list_node* list_search(link_list *first,void *tmp,bool (*cmp)(void *,void *));
list_node *list_locate(link_list *first,int i);
int list_remove_num(link_list *first, int no);
int node_move(link_list* first,int i,list_node *p);
int list_insert(link_list *first,int i,void *new_data);
int list_remove(link_list *first,void *data,bool (*cmp)(void *,void *));
void list_copy(link_list *dest,link_list *src);
//void list_print(USART_TypeDef *USARTx,link_list *first,int param_group);

#endif
