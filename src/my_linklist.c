#include "my_linklist.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static void init_node(MyNode *node)
{
  node->next = NULL;
  node->prev = NULL;
  node->data = NULL;
}

static MyNode* linklist_append(MyLinkList *self, void *data)
{
  MyNode *node = (MyNode *)malloc(sizeof(MyNode));
  if (node == NULL)
    return NULL;

  init_node(node);
  node->data = data;

  // build connections between former last element and the new last element(node)
  node->prev = self->end->prev;
  node->prev->next = node;

  // build connections between new last element and the end node
  node->next = self->end;
  self->end->prev = node;

  return node;
}

static MyNode* linklist_find(MyLinkList *self, void *data)
{
  MyNode *node = self->head->next;
  while (node != self->end) {
    if (node->data == data)
      break;

    node = node->next;
  }

  if (node == self->end)
    return NULL;

  return node;
}

static int linklist_remove(MyLinkList *self, MyNode *node)
{
  // FIXME: should check whether then node is in this linklist(self)
  if (node->next == NULL || node->prev == NULL)
    return -1;

  node->prev->next = node->next;
  node->next->prev = node->prev;

  free(node);

  return 0;
}

static void linklist_release(MyLinkList *self)
{
  MyNode *node = self->head->next;
  while (node != self->end) {
    MyNode *current = node;
    node = node->next;
    free(current);
  }

  self->head->next = self->end;
  self->end->prev = self->head;
}

static void linklist_destroy(MyLinkList *self)
{
  linklist_release(self);
  free(self->head);
  self->head = NULL;
  free(self->end);
  self->end = NULL;
  free(self);
}

static void init_linklist(MyLinkList *list)
{
  list->append = &linklist_append;
  list->find = &linklist_find;
  list->remove = &linklist_remove;
  list->release = &linklist_release;
  list->destroy = &linklist_destroy;

  list->head = (MyNode *)malloc(sizeof(MyNode));
  assert(list->head != NULL);
  init_node(list->head);

  list->end = (MyNode *)malloc(sizeof(MyNode));
  assert(list->end != NULL);
  init_node(list->end);

  list->head->next = list->end;
  list->end->prev = list->head;
}

MyLinkList* create_linklist()
{
  MyLinkList *list = (MyLinkList*)malloc(sizeof(MyLinkList));
  assert(list != NULL);

  init_linklist(list);

  return list;
}
