#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct linked_list {
	int data;
	struct linked_list* next;
};

typedef struct linked_list node;

struct linked_list_record {
	node* head;
	node* tail;
	int size;
};

typedef struct linked_list_record list;
typedef unsigned int uint;

void* s_malloc(const uint);
void s_free(void*);

list* create_list();
void add_element(list*, int, uint);
void delete_element(list*, int);
void free_list(list*);

void* s_malloc (const uint t) {
	void* p = malloc(t);
	if(!p) {
		printf("Out of memory.\n");
		exit(EXIT_FAILURE);
	}
	return p;
}

void s_free (void* p) {
	if(!p) {
		printf("Error: Tried to free NULL ptr.\n");
		exit(EXIT_FAILURE);		
	}
	else
		free(p);
}

list* create_list() {
	list* res = (list*) s_malloc(sizeof(list));
	res->head = (node*) s_malloc(sizeof(node));
	res->head->next = NULL;
	res->tail = res->head;
	res->size = 0;
	return res;
}

void add_element(list* l, int elem, uint pos) {
	node* add = (node *) s_malloc(sizeof(node));
	add->data = elem;
	if(pos > l->size)
		pos = l->size + 1;
	if(pos == l->size + 1) {
		l->tail->next = add;
		l->tail = add;
		add->next = NULL;
	}
	else {
		node* temp = l->head;
		for(uint i = 0; i < pos; ++i)
			temp = temp->next;
		add->next = temp->next;
		temp->next = add;
		l->tail = add;
	}
	++l->size;
}

void delete_element(list* l, int elem) {
	if(l->size) {
		node* temp = l->head;
		for(; temp->next && temp->next->data != elem; temp = temp->next);
		if(!temp->next)
			printf("Element %d was not in the list.\n", elem);
		else {
			node* del = temp->next;
			temp->next = temp->next->next;
			free(del);
			--l->size;
		}
		if(!l->size)
			l->tail = l->head;
	}
	else
		printf("The list is empty.\n");
}

void display_list(list* l) {
	printf("List contents:\n");
	if(!l->size)
		printf("It's empty!\n");
	uint i = 0;
	for(node* temp = l->head; temp->next; temp = temp->next, ++i)
		printf("%d. -- %d\n", i, temp->next->data);
}

int main() {
	srand(time(NULL));
	list* l = create_list();
	for(uint i = 0; i < 10; ++i)
		add_element(l, (rand() % 100) + 1, i);
	display_list(l);
	int input = 0;
	
	do {
		printf("Enter element to delete: ");
		scanf("%d", &input);
		if(input == -1)
			break;
		delete_element(l, input);
		display_list(l);
	} while(input != -1);
	add_element(l, 1337, 13);
	add_element(l, 2460, 0);
	display_list(l);
	return 0;
}
