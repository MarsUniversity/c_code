#include <stdlib.h>
#include <stdio.h>

// I need to put in some pop/insert functions and
// maybe sorting.


typedef struct ListNode {
  void* data;
  struct ListNode *next;
} node_t;

typedef struct LinkedList {
  node_t *head;
  node_t *tail;
  size_t size;
} list_t;

node_t* node_create(void* data) {
  // I don't like malloc() usage
  node_t *p = (node_t*)malloc(sizeof(node_t));
  p->data = data;
  p->next = NULL;
  return p;
}

void list_init(list_t *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void list_insert_end(list_t* list, void* data) {
  // printf("insert start\n");
  node_t *node = node_create(data);

  if (list->head == NULL) {
    list->head = node;
    list->tail = node;
    list->size = 0;
  }
  else {
    list->tail->next = node;
    list->tail = node;
  }
  
  list->size += 1;
  // printf("insert: %d\n", (int)list->size);
}

void list_free(list_t *list) {
  // printf("start\n");
  node_t *p = NULL;
  int i = 0;
  while (list->head != NULL) {
    // printf("loop\n");
    p = list->head;
    list->head = list->head->next;
    // printf("%d: %p %p\n", *(int*)p->data, p, list->head);
    free(p->data);
    free(p);
  }
}


int main() {
  printf("hello\n");

  list_t list;
  list_init(&list);

  // node_t *list = NULL;
  for (int i = 0; i<20; ++i) {
    int* p = (int*)malloc(sizeof(int));
    *p = i;
    list_insert_end(&list, (void*)p);
    printf("%p\n", list.head);
  }

  printf("list size: %d\n", (int)list.size);

  // int i =0;
  // node_t *p = list;
  // while (p != NULL) {
  //   printf("i = %d\n", *(int*)(p->data));
  //   p = p->next;
  // }

  list_free(&list);

  return 0;
}