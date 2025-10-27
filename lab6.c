// example_1.c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr)                                                           \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s\n", #expr);                        \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TEST(expr)                                                             \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed: %s\n", #expr);                             \
      exit(1);                                                                 \
    } else {                                                                   \
      printf("Test passed: %s\n", #expr);                                      \
    }                                                                          \
  }

typedef struct node1 {
  uint64_t data;
  struct node1 *next;
} node1_t;

node1_t *head1 = NULL;

void insert_sorted_1(uint64_t data) {
  node1_t *new_node = malloc(sizeof(node1_t));
  ASSERT(new_node != NULL);
  new_node->data = data;
  new_node->next = NULL;

  if (head1 == NULL || data < head1->data) {
    new_node->next = head1;
    head1 = new_node;
  } else {
    node1_t *curr = head1;
    while (curr->next != NULL && curr->next->data < data) {
      curr = curr->next;
    }
    new_node->next = curr->next;
    curr->next = new_node;
  }
}

int index_of_1(uint64_t data) {
  node1_t *curr = head1;
  int index = 0;

  while (curr != NULL) {
    if (curr->data == data) {
      return index;
    }
    curr = curr->next;
    index++;
  }

  return -1;
}

int main_example1() {
  insert_sorted_1(1);
  insert_sorted_1(2);
  insert_sorted_1(5);
  insert_sorted_1(3);

  TEST(index_of_1(3) == 2);

  insert_sorted_1(0);
  insert_sorted_1(4);

  TEST(index_of_1(4) == 4);

  return 0;
}

// example_2.c
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT2(expr)                                                          \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s\nFile: %s, Line: %d\n", #expr,     \
              __FILE__, __LINE__);                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TEST2(expr)                                                            \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed: %s\n", #expr);                             \
      exit(1);                                                                 \
    }                                                                          \
  }

typedef struct node2 {
  uint64_t data;
  struct node2 *next;
} node2_t;

typedef struct info {
  uint64_t sum;
} info_t;

node2_t *head2 = NULL;
info_t info = {0};

void insert_sorted_2(uint64_t data) {
  node2_t *new_node = malloc(sizeof(node2_t));
  new_node->data = data;
  new_node->next = NULL;

  if (head2 == NULL) {
    head2 = new_node;
  } else if (data < head2->data) {
    new_node->next = head2;
    head2 = new_node;
  } else {
    node2_t *curr = head2;
    node2_t *prev = NULL;

    while (curr != NULL) {
      if (data < curr->data) {
        break;
      }
      prev = curr;
      curr = curr->next;
    }

    prev->next = new_node;
    new_node->next = curr;
  }

  info.sum += data;
}

int index_of_2(uint64_t data) {
  node2_t *curr = head2;
  int index = 0;

  while (curr != NULL) {
    if (curr->data == data) {
      return index;
    }
    curr = curr->next;
    index++;
  }
  return -1;
}

uint64_t list_sum() {
  uint64_t total = 0;
  node2_t *curr = head2;
  while (curr != NULL) {
    total += curr->data;
    curr = curr->next;
  }
  return total;
}

int main_example2() {
  insert_sorted_2(1);
  insert_sorted_2(3);
  insert_sorted_2(5);
  insert_sorted_2(2);

  TEST2(info.sum == 1 + 3 + 5 + 2);
  ASSERT2(list_sum() == info.sum);
  TEST2(index_of_2(2) == 1);

  return 0;
}
