#ifndef LIST_H
#define LIST_H
/*本项目需要以链表组织的数据类型种类较多，针对这种情况，
本项目参考了Linux内核链表的实现方式，实现了一定程度的链表泛型
注意，鉴于各种类型对象构造析构操作存在较大差异，链表节点对象的创建删除需要调用层自行实现并管理内存的分配与释放*/

typedef struct tagListHead {
    int size;
	struct tagListHead *root,* prev, * next;
} ListHead;

//listEntry宏: ptr为该listHead变量地址,type为容器类型,member为容器内该listHead型变量的成员名(考虑一个容器内有多个list_head)
//通过listHead变量内存地址减去其相对于容器内存地址的偏移量，再类型转换，得到其容器的指针
#define listEntry(ptr,type,member) ((type*) ((char*)(ptr) - (unsigned long)(&((type*)0)->member)) )
//删除/替换ptr对象所属容器  
//#define listDelContainer(ptr,type,member) (listDelEntry(ptr),free(listEntry(ptr,type,member)))
//#define listReplaceContainer(ptr,type,member) (listReplaceEntry(ptr),free(listEntry(ptr,type,member)))
/*顺序遍历容器: pos遍历容器节点的迭代指针，head链表头, member为容器中ListHead型变量成员名
删去typeof并加了个参数type使其兼容msvc  
与内核链表的区别：因为引入root，遍历会在链表真正的头节点停止而非传入的节点
ps:修复了内核链表中由Linus在2022.2.23提出的缺陷 pps:只是加了个声明(  */
#define listForEachEntry(type, pos, head, member) \
         for (type *pos = listEntry((head)->next, type, member); \
              &(pos->member) != (head)->root; \
              pos = listEntry(pos->member.next, type, member))

//顺序遍历容器并处理节点在遍历过程中被删除的情况
#define listForEachEntrySafe(type, pos, head, member) \
         for (type *pos = listEntry((head)->next, type, member), \
              *n= listEntry(pos->member.next, type, member); \
              &(pos->member) != (head)->root; \
              pos=n,n = listEntry(n->member.next, type, member))

//顺序遍历ListHead节点
#define listForEach(pos,head) \
        for (ListHead *pos = (head)->next; pos != (head)->root; pos = pos->next)

#define listForEachSafe(pos,head) \
        for (ListHead *pos = (head)->next, *n=pos->next;pos!=(head)->root;pos=n,n=n->next)


/*#define listAllClear(type, head, member) \
        listForEachSafe(pos,head){ \
            listDelContainer(pos,type,member) */
                

void listInit(ListHead* head);
ListHead* listCreate(void);
void __listAdd(ListHead* _new, ListHead* prev, ListHead* next);
void listAdd(ListHead* _new,ListHead* head);
void listAddTail(ListHead* _new,ListHead* head);
void listInsert(ListHead* pos, ListHead* _new); //在pos节点之前插入_new
void listReplace(ListHead* old, ListHead* _new);
void listRemove(ListHead* entry); //注意该函数只移除节点不释放内存
int listSize(ListHead* head);
int listEmpty(ListHead* head);
ListHead* listMove(ListHead* pos, int step);

#endif