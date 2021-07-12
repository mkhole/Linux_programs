#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

struct student {
	int id;
	char name[32];
	struct list_head entry;
};

static void print_list(struct list_head *head)
{
	struct student *stu;
	list_for_each_entry(stu, head, entry) {
		printf("[id: %d name: %s]", stu->id, stu->name);
		if (!list_is_last(&stu->entry, head))
			printf(" -> ");
	}
	printf("\n");
}

static struct student *search_list(struct list_head *head, int id)
{
	struct student *stu;
	list_for_each_entry(stu, head, entry) {
		if (stu->id == id)
			return stu;
	}

	return NULL;
}

static struct student *create_student(int id, char *name)
{
	struct student *stu = malloc(1 * sizeof(struct student));
	stu->id = id;
	strncpy(stu->name, name, sizeof(stu->name));
	return stu;
}

#define NUMS 5

int main(void)
{
	int i;
	char buf[32];
	struct student *stu, *tmp;
	struct student **students;
	struct list_head student_list;

	INIT_LIST_HEAD(&student_list);

	/* insert into the list */
	students = (struct student **)malloc(NUMS * sizeof(struct student *));
	for (i = 0; i < NUMS; i++) {
		snprintf(buf, sizeof(buf), "student%c", 'A' + i);
		students[i] = create_student(i, buf);
		list_add(&students[i]->entry, &student_list);
	}
	print_list(&student_list);

	/* search from the list */
	stu = search_list(&student_list, 2);
	if (!stu) {
		printf("No student found\n");
		return -1;
	}
	printf("Found student: %s\n", stu->name);

	/* delete from the list */
	list_del(&stu->entry);
	print_list(&student_list);

	list_del_init(&student_list);
	if (list_empty(&student_list))
		printf("List is empty\n");

	/* insert at the end of the list */
	for (i = 0; i < NUMS; i++) {
		list_add_tail(&students[i]->entry, &student_list);
	}
	print_list(&student_list);

	/* free all entry in the list */
	list_for_each_entry_safe(stu, tmp, &student_list, entry) {
		printf("Free student: %s\n", stu->name);
		free(stu);
	}

	return 0;
}
