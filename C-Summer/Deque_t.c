#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned int uint;

struct deque_elem {
	void* data;
	struct deque_elem* prev;
	struct deque_elem* next;
};

typedef struct deque_elem node;

struct Deque {
	node* begin;														// acts like dummy node
	node* end;
	int (*ord)(void*, void*);											// the priority function ptr
	uint size;
};

typedef struct Deque deque;

deque* create_deque(int (*priority)(void*, void*));
void push_front(deque*, void*);
void push_back(deque*, void*);
void pop_front(deque*, void (*freefunc)(void*));
void pop_back(deque*, void (*freefunc)(void*));
node* front(deque*);
node* back(deque*);
void reorder_deque(deque*, uint);

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

deque* create_deque(int (*priority)(void*, void*)) {		// we declare our priority function here
	deque* d = (deque*) s_malloc(sizeof(deque));
	d->begin = (node*) s_malloc(sizeof(node));
	d->end = d->begin;
	d->begin->next = NULL;
	d->ord = priority;
	d->size = 0;
	return d;
}

// We show two variations in the push functions, dummy node and no dummy node (begin / end)

void push_front(deque* d, void* elem) {
	node* add = (node*) s_malloc(sizeof(node));
	add->data = elem;
	add->next = d->begin->next;
	add->prev = d->begin;								// there's dummy node, so prev must be it!
	if(d->begin->next)
		d->begin->next->prev = add;
	else
		d->end = add;
	d->begin->next = add;
	++d->size;
}

void push_back(deque* d, void* elem) {
	node* add = (node*) s_malloc(sizeof(node));
	add->data = elem;
	add->prev = d->end;
	add->next = NULL;									// no dummy node, so no next
	d->end->next = add;
	d->end = add;
	++d->size;
}

void pop_front(deque* d, void (*freefunc)(void*)) {
	if(d->begin->next) {
		node* del = d->begin->next;
		d->begin->next = del->next;
		if(d->begin->next)								// could be NULL, end doesn't have this problem.
			d->begin->next->prev = d->begin;
		freefunc(del);
		--d->size;
	}
}

void pop_back(deque* d, void (*freefunc)(void*)) {
	if(d->end) {
		node* del = d->end;
		d->end = d->end->prev;
		d->end->next = NULL;
		freefunc(del);
		--d->size;
	}
}

node* front(deque* d) {
	return d->begin->next;
} 

node* back(deque* d) {
	return d->end;
}

int int_cmp(void* x, void* y) {
	return *(int *)x >= *(int *)y;
}

int str_cmp(void* x, void* y) {
	return strcmp((char*) x, (char*) y);
}

// most generalized swap function
void swap(void* x, void* y, uint size) {
	char temp[size];
	memcpy(temp, y, size);
	memcpy(y, x, size);
	memcpy(x, temp, size);
}

// if somebody wants their deque sorted according a sorting method, they can provide their own comparison during the creation of the deque
// if we really want a different sorting method, we can add a new function that allows changing of the function pointer anyway!
void reorder_deque(deque* d, uint elemsize) {
	if(d->ord) {
		// we will use a sorting algorithm named 'Bubble Sort' here. We will go into details of how it works later.
		// There are many better algorithms out there, but we will cover them later, again.
		for(node* t1 = d->begin->next; t1; t1 = t1->next) {
			for(node* t2 = t1->next; t2; t2 = t2->next) {
				if(d->ord(t1->data, t2->data))
					swap(&t1->data, &t2->data, elemsize);
			}
		}
	}
}

void display_deque_int(deque* d) {
	printf("Front and back of the queue: %d and %d\n", *(int *)front(d)->data, *(int *)back(d)->data);
	printf("Deque content in normal order:\n");
	uint i = 0;
	for(node* temp = d->begin; temp != d->end; temp = temp->next, ++i)
		printf("%d. %d\n", i, *(int *)temp->next->data);							// since we used dummy node, we use next here
	printf("\nDeque content in reverse order:\n");
	i = 0;
	for(node* temp = d->end; temp != d->begin; temp = temp->prev, ++i)				// end is not dummy node, so we didn't use prev here.
		printf("%d. %d\n", i, *(int *)temp->data);									// these are two variations, pick whichever method you like.
}

void display_deque_str(deque* d) {
	printf("Front and back of the queue: %s and %s\n", (char *)front(d)->data, (char *)back(d)->data);
	printf("Deque content in normal order:\n");
	uint i = 0;
	for(node* temp = d->begin; temp != d->end; temp = temp->next, ++i)
		printf("%d. %s\n", i, (char *)temp->next->data);							// since we used dummy node, we use next here
	printf("\nDeque content in reverse order:\n");
	i = 0;
	for(node* temp = d->end; temp != d->begin; temp = temp->prev, ++i)				// end is not dummy node, so we didn't use prev here.
		printf("%d. %s\n", i, (char *)temp->data);									// these are two variations, pick whichever method you like.
}

// There's usually only one type of these, but they are given as both just to show your style shouldn't be limited :p

void free_deque_front(deque* d, void (*freefunc)(void*)) {
	while(front(d))
		pop_front(d, freefunc);
	// free the dummy node
	freefunc(d->begin);
	freefunc(d);
}

void free_deque_back(deque* d, void (*freefunc)(void*)) {
	while(front(d))
		pop_back(d, freefunc);
	// free the dummy node
	freefunc(d->begin);
	freefunc(d);
}

int main() {
	srand(time(NULL));
	deque* d1 = create_deque(int_cmp);
	for(uint i = 0; i < 10; ++i) {
		int* elem = (int*) s_malloc(sizeof(int));
		*elem = (rand() % 100) + 1;
		push_back(d1, elem);
	}
	display_deque_int(d1);
	printf("\n\nDeque 2 with pushfront\n\n");
	deque* d2 = create_deque(int_cmp);
	for(uint i = 0; i < 10; ++i) {
		int* elem = (int*) s_malloc(sizeof(int));
		*elem = (rand() % 100) + 1;
		push_front(d2, elem);
	}
	display_deque_int(d2);
	reorder_deque(d1, sizeof(int));
	display_deque_int(d1);
	free_deque_front(d1, s_free);
	free_deque_back(d2, s_free);
	
	deque* d3 = create_deque(str_cmp);
	char* s1 = strdup("Jessica");
	char* s2 = strdup("Melanie");
	char* s3 = strdup("Batuhan");
	char* s4 = strdup("Graves");
	push_back(d3, s1);
	push_back(d3, s2);
	push_back(d3, s3);
	push_back(d3, s4);
	printf("\n\nDeque 3\n\n");
	display_deque_str(d3);
	free_deque_front(d3, s_free);
	return 0;
}
