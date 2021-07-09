#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <search.h>

struct student {
	int id;
	char name[32];
};

int id_cmp(const void *a, const void *b)
{
	int id_a = ((struct student *)a)->id;
	int id_b = ((struct student *)b)->id;
	return (id_a == id_b) ? 0 : (id_a > id_b) ? 1 : -1;
}

void print_student(const void *nodep, const VISIT which, const int depth)
{
	struct student *stu = *((struct student **)nodep);
	switch (which) {
	case preorder:
		break;
	case endorder:
		break;
	case postorder:
	case leaf:
		printf("name:%s id: %d\n", stu->name, stu->id);
		break;
	default:
		break;
	}
}

struct student *create_student(char *name)
{
	struct student *stu = malloc(1 * sizeof(struct student));
	stu->id = rand()%100 + 1;
	strncpy(stu->name, name, sizeof(stu->name));
	return stu;
}

#define NUMS 5

int main(void)
{
	int i;
	void *root = NULL;
	char buf[32];
	struct student **students;
 
	srand(time(NULL));

	students = (struct student **)malloc(NUMS * sizeof(struct student *));
	for(i=0; i<NUMS; i++) {
		snprintf(buf, sizeof(buf), "student%c", 'A' + i);
		students[i] = create_student(buf);
	}

	for (i = 0; i < NUMS; i++)
		(void)tsearch((void *)students[i], &root, id_cmp);

	twalk(root, print_student);
	tdestroy(root, free);
	return 0;
}
