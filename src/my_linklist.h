#ifndef _MY_LINKLIST_H_
#define _MY_LINKLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct MyNode {
    struct MyNode *next;
    struct MyNode *prev;
    void *data;

  } MyNode;

  typedef struct MyLinkList {
    MyNode *head; // a guard previous to the first element
    MyNode *end; // a guard element next to the last element

    MyNode* (*append)(struct MyLinkList *self, void *data);
    MyNode* (*find)(struct MyLinkList *self, void *data);
    int (*remove)(struct MyLinkList *self, MyNode *node);
    void (*release)(struct MyLinkList *self);
    void (*destroy)(struct MyLinkList *self);
  } MyLinkList;

  MyLinkList* create_linklist();

#ifdef __cplusplus
}
#endif

#endif // _MY_LINKLIST_H_
