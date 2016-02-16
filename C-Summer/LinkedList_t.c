#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// cast to int, knowing the contained is of type int
int IntCmp(void* e1, void* e2) {
	int* i_e1 = (int *)e1;
	int* i_e2 = (int *)e2;
	
	return (*i_e1 > *i_e2) - (*i_e1 < *i_e2);
}

// cast to char*, knowing the contained is of type char* (this is a generic way of casting void ptr to a usable format (risky though))
int strcmp2(void* e1, void* e2) {
	char* c_e1 = (char *) e1;
	char* c_e2 = (char *) e2;
	
	return strcmp(c_e1, c_e2);
}

struct linked_list {
	void* data;
	unsigned int in_use;
	struct linked_list* next;
};

typedef struct linked_list node;
typedef unsigned int uint;

struct memory_pool {
	node** pool;
	uint capacity;
};

typedef struct memory_pool mem_p;

struct linked_list_record {
	node* head;
	node* tail;
	uint size;
	mem_p* mp;
};

typedef struct linked_list_record list;

void* s_malloc(const uint);
void s_free(void*);

list* create_list(uint);
void add_element(list*, void*, uint);
void delete_element(list*, void*, int (*cmpfnc)(void*, void*), void (*freefnc)(void*));
void free_list(list*);
node* get_addr_from_pool(list*);

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

list* create_list(uint poolsize) {
	list* res = (list*) s_malloc(sizeof(list));
	res->mp = (mem_p*) s_malloc(sizeof(mem_p));
	res->mp->pool = (node**) s_malloc(sizeof(node*) * (poolsize + 1)); 	// +1 for the head node
	res->mp->capacity = poolsize + 1;
	for(uint i = 0; i < poolsize + 1; ++i) {
		res->mp->pool[i] = (node*) s_malloc(sizeof(node));
		res->mp->pool[i]->in_use = 0;
	}
	res->head = res->mp->pool[0];
	res->head->in_use = 1;
	res->head->next = NULL;
	res->tail = res->head;
	res->size = 0;
	return res;
}

node* get_addr_from_pool(list* l) {
	for(uint i = 1; i < l->mp->capacity; ++i) {							// 0 reserved on the head
		if(!l->mp->pool[i]->in_use) {
			l->mp->pool[i]->in_use = 1;
			return l->mp->pool[i];
		}
	}
	return NULL;
}

void add_element(list* l, void* elem, uint pos) {
	if(l->size < l->mp->capacity - 1) {
		if(pos > l->size)
			pos = l->size;
		node* add = get_addr_from_pool(l);
		add->data = elem;
		if(pos == l->size) {
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
		}
		++l->size;
	}
}

void delete_element(list* l, void* elem, int (*cmpfnc)(void*, void*), void (*freefnc)(void*)) {
	if(l->size) {
		node* temp = l->head;
		for(; temp->next && cmpfnc(temp->next->data, elem); temp = temp->next);
		if(!temp->next)
			printf("Element was not in the list.\n");
		else {
			node* del = temp->next;
			temp->next = del->next;
			// we don't free in memory pool, we instead just set the info to null in there (it's already allocated)
			// however we will delete the list alongside with the pool
			// we must also let the pool know that this is no longer in_use, so it's 0. A new node, if added later, will use this.
			// Do not free del though! Remember the purpose of memory pool! Free the data in the del!
			del->in_use = 0;
			freefnc(del->data);
			--l->size;
			if(!temp->next)
				l->tail = temp;
		}
		if(!l->size)
			l->tail = l->head;
	}
	else
		printf("The list is empty.\n");
}

void display_list_int(list* l) {
	printf("List contents:\n");
	if(!l->size)
		printf("It's empty!\n");
	uint i = 0;
	for(node* temp = l->head; temp->next; temp = temp->next, ++i)
		printf("%d. -- %d\n", i, *(int *)temp->next->data);
}

int main() {
	srand(time(NULL));
	list* l = create_list(10);									// create with memory pool size 10 (a list of only 10 elements) can allow for more!
	for(uint i = 0; i < 10; ++i) {
		int* elem = (int *) s_malloc(sizeof(int));
		*elem = (rand() % 100) + 1;
		add_element(l, elem, i);
	}
	display_list_int(l);
	int input = 0;
	
	do {
		printf("Enter element to delete: ");
		scanf("%d", &input);
		if(input == -1)
			break;
		delete_element(l, &input, IntCmp, s_free);
		display_list_int(l);
	} while(input != -1);
	
	int* e1 = (int *) s_malloc(sizeof(int));
	int* e2 = (int *) s_malloc(sizeof(int));
	*e1 = 1337;
	*e2 = 2460;
	add_element(l, e1, 13);
	add_element(l, e2, 0);
	display_list_int(l);
	return 0;
}
