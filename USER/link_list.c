#include "link_list.h"
#include "configuration.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

//初始化链表
int list_init(link_list *first){
    *first = (list_node *)malloc(sizeof(list_node));
    if(first == NULL){
        return -1;
    }
    (*first)->link = NULL;
		return 1;
}

void list_clear(link_list *first){
    list_node *q;
    while((*first)->link != NULL){
        q = (*first)->link;
        (*first)->link = q->link;
        free(q);
    }
}

int list_get_length(link_list *first){
    list_node *p = (*first)->link;
    int k = 0;
    while(p != NULL){
        k++;
        p = p->link;
    }
    return k;
}

int list_isempty(link_list *first){
    return ((*first)->link == NULL);
}

list_node* list_search(link_list *first,void *tmp,bool (*cmp)(void *,void *)){
    list_node *p = (*first)->link;
    while(p != NULL && !(*cmp)(p->data,tmp)){
        p = p->link;
    }
    return p;
}


list_node *list_locate(link_list *first,int i){
	int k = 0;
	list_node *p = *first;
    if(i < 0){
        return NULL;
    }
    while(p != NULL && k < i){
        k++;
        p = p->link;
    }
    return p;  
}


int list_insert(link_list *first,int i,void *new_data){
    list_node *new_node;
	list_node *p = list_locate(first,i-1);   //定位到第i-1个节点
    if(p == NULL){
        return 0;  //指针是空的，也即是没有那么长的链表
    }
    new_node = (list_node *)malloc(sizeof(list_node));
	if (new_node == NULL)
		return 0;
    new_node->data = new_data;
    new_node->link = p->link;
    p->link = new_node;
    return 1;
}

int list_remove(link_list *first,void *data,bool (*cmp)(void *,void *)){
    list_node *q = *first,*p = (*first)->link;
    while(p != NULL&&!cmp(data,p->data)){
        q = q->link;
        p = p->link;
    }
    if(p == NULL){
        return 0;
    }
    q->link = p->link;
//    free(p->data);      //    attention
    free(p);
    return 1;
}

int list_remove_num(link_list *first, int no){
	list_node *q = *first;
	list_node *p = (*first)->link;
	int k = 1;
	while(p != NULL && k <no){
		q = q->link;
		p = p->link;
		k++;
	}
	if (p == NULL)
		return 0;
	q->link = p->link;
	free(p);
	return 1;
}

void list_copy(link_list *dest,link_list *src){
    list_node *src_ptr = (*src)->link;
    list_node *dest_ptr = (*dest)->link;
    list_node *new_node;
    while(src_ptr != NULL){
        new_node->data = src_ptr->data;
        new_node->link = NULL;
        dest_ptr->link = new_node;
        dest_ptr = dest_ptr->link;
        src_ptr = src_ptr->link;
    }
    dest_ptr->link = NULL;
}

int node_move(link_list* first,int i,list_node *p){
	list_node *q=(*first)->link;
	while (q!=NULL && q->link!=p)
		q=q->link;
	if(q == NULL)
		return 0;
	q->link=p->link;
	list_insert(first,i,p->data);
	free(p);
	return 1;
}


//void list_print(USART_TypeDef *USARTx,link_list *first,int param_group){
//    int i,j;
//    if((*first)->link != NULL){
//        uprintf(USARTx,"%s=%.3f",(*first)->link->data->param_name,(*first)->link->data->param_value[param_group]);
//        uprintf(USARTx,"\n");
////				for(i = 0;i < 10;i++)
////					for(j = 0;j < 100;j++); //延迟那么一小会儿
//        list_print(USARTx,&((*first)->link),param_group);  //递归
//    }
//    
//}

