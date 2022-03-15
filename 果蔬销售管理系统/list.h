#ifndef LIST_H
#define LIST_H
/*����Ŀ��Ҫ��������֯��������������϶࣬������������
����Ŀ�ο���Linux�ں������ʵ�ַ�ʽ��ʵ����һ���̶ȵ�������
ע�⣬���ڸ������Ͷ����������������ڽϴ���죬����ڵ����Ĵ���ɾ����Ҫ���ò�����ʵ�ֲ������ڴ�ķ������ͷ�*/

typedef struct tagListHead {
    int size;
	struct tagListHead *root,* prev, * next;
} ListHead;

//listEntry��: ptrΪ��listHead������ַ,typeΪ��������,memberΪ�����ڸ�listHead�ͱ����ĳ�Ա��(����һ���������ж��list_head)
//ͨ��listHead�����ڴ��ַ��ȥ������������ڴ��ַ��ƫ������������ת�����õ���������ָ��
#define listEntry(ptr,type,member) ((type*) ((char*)(ptr) - (unsigned long)(&((type*)0)->member)) )
//ɾ��/�滻ptr������������  
//#define listDelContainer(ptr,type,member) (listDelEntry(ptr),free(listEntry(ptr,type,member)))
//#define listReplaceContainer(ptr,type,member) (listReplaceEntry(ptr),free(listEntry(ptr,type,member)))
/*˳���������: pos���������ڵ�ĵ���ָ�룬head����ͷ, memberΪ������ListHead�ͱ�����Ա��
ɾȥtypeof�����˸�����typeʹ�����msvc  
���ں������������Ϊ����root��������������������ͷ�ڵ�ֹͣ���Ǵ���Ľڵ�
ps:�޸����ں���������Linus��2022.2.23�����ȱ�� pps:ֻ�Ǽ��˸�����(  */
#define listForEachEntry(type, pos, head, member) \
         for (type *pos = listEntry((head)->next, type, member); \
              &(pos->member) != (head)->root; \
              pos = listEntry(pos->member.next, type, member))

//˳���������������ڵ��ڱ��������б�ɾ�������
#define listForEachEntrySafe(type, pos, head, member) \
         for (type *pos = listEntry((head)->next, type, member), \
              *n= listEntry(pos->member.next, type, member); \
              &(pos->member) != (head)->root; \
              pos=n,n = listEntry(n->member.next, type, member))

//˳�����ListHead�ڵ�
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
void listInsert(ListHead* pos, ListHead* _new); //��pos�ڵ�֮ǰ����_new
void listReplace(ListHead* old, ListHead* _new);
void listRemove(ListHead* entry); //ע��ú���ֻ�Ƴ��ڵ㲻�ͷ��ڴ�
int listSize(ListHead* head);
int listEmpty(ListHead* head);
ListHead* listMove(ListHead* pos, int step);

#endif