#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct linked_list {
	int data;
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
	int size;
	mem_p* mp;
};

typedef struct linked_list_record list;

void* s_malloc(const uint);
void s_free(void*);

list* create_list(uint);
void add_element(list*, int, uint);
void delete_element(list*, int);
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

void add_element(list* l, int elem, uint pos) {
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

void delete_element(list* l, int elem) {
	if(l->size) {
		node* temp = l->head;
		for(; temp->next && temp->next->data != elem; temp = temp->next);
		if(!temp->next)
			printf("Element %d was not in the list.\n", elem);
		else {
			node* del = temp->next;
			temp->next = del->next;
			// we don't free in memory pool, we instead just set the info to null in there (it's already allocated)
			// however we will delete the list alongside with the pool
			// we must also let the pool know that this is no longer in_use, so it's 0. A new node, if added later, will use this.
			del->in_use = 0;
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
	list* l = create_list(10);									// create with memory pool size 10 (a list of only 10 elements) can allow for more!
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
