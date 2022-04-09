#include<stdbool.h>
#include<stdio.h>
#include"list.h"

void listInit(ListHead* head) {
	head->size = 0;
	head->root = head;
	head->prev = head;
	head->next = head;
}
void __listAdd(ListHead* _new, ListHead* prev, ListHead* next) {
	prev->root->size++;
	_new->root = prev->root;

	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}
void listAdd(ListHead* _new, ListHead* head) {
	__listAdd(_new, head, head->next);
}
void listAddTail(ListHead* _new, ListHead* head) {
	__listAdd(_new, head->prev, head);
}
void listInsert(ListHead* _new, ListHead* pos) { //在pos节点之前插入_new
	listAddTail(_new, pos);
}
//替换链表节点，注意调用析构函数/释放内存
void listReplace(ListHead* old, ListHead* _new) { //注意该函数只移除节点不释放内存
	old->root->size--;
	__listAdd(_new, old->prev, old->next);
}
//节点移出链表，注意调用析构函数/释放内存
void listRemove(ListHead* entry) { 
	entry->root->size--;
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	entry->root=entry->prev = entry->next = NULL;
}
int listSize(ListHead *head) {
	return head->root->size;
}
int listEmpty(ListHead* head) {
	return head->root == head;;
}
ListHead* listMove(ListHead* pos, int step) { 
	if (step == 0) return pos;
	ListHead* dest = pos;
	if (step > 0) {
		for (int i = 0; i < step && dest->next != dest->root;i++, dest = dest->next);  //若步数过长，将在链表的尾节点停止
		return dest;
	}
	else if (step < 0) {
		for (int i = 0; i > step && dest != dest->root;i--, dest = dest->prev);
		return dest;
	}
}
ListHead* listShowPageJump(ListHead* head, int* pageStart, const int pageSize) {
	ListHead* showPage = head;
	while (1) {
		int num = 1;
		listForEach(pos, head) {
			if (num == *pageStart) {
				showPage = pos->prev;
				break;
			}
			num++;
		}
		if (showPage != head || *pageStart == 1) return showPage;
		else *pageStart -= pageSize;
	}
}