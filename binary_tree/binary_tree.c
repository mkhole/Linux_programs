#define _GNU_SOURCE     /* Expose declaration of tdestroy() */
#include <search.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
void *root = NULL;
void *
xmalloc(unsigned n)
{
	void *p;
	p = malloc(n);
	if (p)
		return p;
	fprintf(stderr, "insufficient memory\n");
	exit(EXIT_FAILURE);
}
int
compare(const void *pa, const void *pb)
{
	if (*(int *) pa < * (int *) pb)
		return -1;
	if (*(int *) pa > *(int *) pb)
		return 1;
	return 0;
}
void print_node(const void *nodep, const VISIT which, const int depth)
{

	int *datap = *((int **) nodep);
	char space[20];
	int cnt;
	for (cnt = 0; cnt < depth; cnt++) {
		space[cnt] = '\t';
	}
	space[cnt] = '\0';
	switch (which) {

//	case preorder:
//		printf("pre %s%d val: [%d]\n", space, depth, *datap);
//		break;
//	case postorder:
//		printf("in %s%d val: [%d]\n", space, depth, *datap);
//		break;
	case endorder:
		printf("post %s%d val: [%d]\n", space, depth, *datap);
		break;
	case leaf:
		printf("leaf %s%d val: [%d]\n", space, depth, *datap);
		break;
	default:
		break;
	}
}
int
main(void)
{

	int i, array[6] = {0, 1, 2, 3, 4, 5}, *ptr;
	void *val;
	srand(time(NULL));
	for (i = 0; i < 6; i++) {
		ptr = xmalloc(sizeof(int));
		*ptr = array[i];
		printf("%d\n", *ptr);
		val = tsearch((void *) ptr, &root, compare);
		if (val == NULL)
			exit(EXIT_FAILURE);
		else if ((*(int **) val) != ptr)
			free(ptr);
	}
	twalk(root, print_node);
	tdestroy(root, free);
	exit(EXIT_SUCCESS);
}
