#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <limits.h>

#define arr_len(arr) (sizeof(arr) / sizeof(arr[0]))
#define NODES_COUNT 200000

typedef struct {
	long num;
	avl_node_t avl_node;
} outer_t;

AVL_DEFINE_ROOT(outer_root_t, outer_t);

int comparator(void *node1, void *node2) {
	if (((outer_t *)node1)->num == ((outer_t *)node2)->num)
		return 0;
	if (((outer_t *)node1)->num <  ((outer_t *)node2)->num)
		return -1;
	return 1;
}

void fill_random(outer_t nodes[]) {
	srandom(time(NULL));
	for (size_t i = 0; i < NODES_COUNT; ++i)
		nodes[i].num = random();
}

void fill_linear(outer_t nodes[]) {
	for (size_t i = 0; i < NODES_COUNT; ++i)
		nodes[i].num = i;
}

void remove_all(outer_root_t *root, outer_t nodes[]) {
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *deleted = avl_remove(root, &nodes[i]);
		assert(deleted == NULL || comparator(deleted, &nodes[i]) == 0);
		assert(!avl_contains(root, &nodes[i]));
	}
}

void insert_random(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	fill_random(nodes);
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *replaced = avl_insert(root, &nodes[i]);
		assert(replaced == NULL || comparator(replaced, &nodes[i]) == 0);
		assert(avl_contains(root, &nodes[i]));
	}
}

void test_remove(outer_root_t *root, outer_t nodes[]) {
	insert_random(root, nodes);
	remove_all(root, nodes);
}

void insert_linear(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	fill_linear(nodes);
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *replaced = avl_insert(root, &nodes[i]);
		assert(replaced == NULL);
		assert(avl_contains(root, &nodes[i]));
	}
}

void test_find(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_random(root, nodes);
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *found = avl_find(root, &nodes[i]);
		assert(found->num == nodes[i].num);
	}
}

void test_min(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_linear(root, nodes);
	assert(comparator(avl_min(root), &nodes[0]) == 0);
}

void test_max(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_linear(root, nodes);
	assert(comparator(avl_max(root), &nodes[NODES_COUNT - 1]) == 0);
}

void test_next(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_linear(root, nodes);
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *next = avl_next(root, &nodes[i]);
		assert((next == NULL && i == NODES_COUNT - 1) || next->num == i + 1);
	}
}

void test_prev(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_linear(root, nodes);
	for (size_t i = 0; i < NODES_COUNT; ++i) {
		outer_t *prev = avl_prev(root, &nodes[i]);
		assert((prev == NULL && i == 0) || prev->num == i - 1);
	}
}

void test_iterator(outer_root_t *root, outer_t nodes[]) {
	remove_all(root, nodes);
	insert_random(root, nodes);

	avl_iterator_t iter = avl_get_iterator(root, &nodes[0], &nodes[1]);
	outer_t *prev = avl_advance(root, &iter);
	for (outer_t *cur; (cur = avl_advance(root, &iter));) {
		assert(comparator(prev, cur) < 0);
		prev = cur;
	}

	iter = avl_get_iterator(root, &nodes[0], &nodes[1], false);
	prev = avl_advance(root, &iter);
	for (outer_t *cur; (cur = avl_advance(root, &iter));) {
		assert(comparator(prev, cur) > 0);
		prev = cur;
	}

	remove_all(root, nodes);
	insert_linear(root, nodes);

	iter = avl_get_iterator(root, &nodes[0], &nodes[NODES_COUNT - 1]);
	for (size_t i = 0; i < NODES_COUNT; ++i)
		assert(i == avl_advance(root, &iter)->num);


	iter = avl_get_iterator(root, &nodes[NODES_COUNT - 1], &nodes[0]);
	assert(avl_advance(root, &iter) == NULL);
}

typedef void (*test_func)(outer_root_t *, outer_t[]);

void run_test(test_func func, outer_root_t *root, outer_t nodes[], char *msg, int repeat) {
	for (int i = 1; i <= repeat; ++i) {
		printf("\r");
		fflush(stdout);
		printf("%s\t\t%2d/%d", msg, i, repeat);
		(*func)(root, nodes);
	}
	puts("\t\033[1;32mOK\033[0m");
}

int main() {
	outer_root_t root = AVL_NEW(outer_root_t, outer_t, avl_node, comparator);
	outer_t nodes[NODES_COUNT];

	run_test(insert_random, &root, nodes, "test_insert", 10);
	run_test(test_remove, &root, nodes, "test_remove", 10);
	run_test(test_find, &root, nodes, "test_find", 10);
	run_test(test_min, &root, nodes, "test_min", 10);
	run_test(test_max, &root, nodes, "test_max", 10);
	run_test(test_next, &root, nodes, "test_next", 10);
	run_test(test_prev, &root, nodes, "test_prev", 10);
	run_test(test_iterator, &root, nodes, "test_iterator", 10);
	puts("All tests passed successfully! 👍");
}
