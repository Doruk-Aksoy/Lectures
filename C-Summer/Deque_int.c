#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned int uint;

struct deque_elem {
	int data;
	struct deque_elem* prev;
	struct deque_elem* next;
};

typedef struct deque_elem node;

struct Deque {
	node* begin;														// acts like dummy node
	node* end;
	int (*ord)(int, int);												// the priority function ptr
	uint size;
};

typedef struct Deque deque;

deque* create_deque();
void push_front(deque*, int);
void push_back(deque*, int);
void pop_front(deque*);
void pop_back(deque*);
node* front(deque*);
node* back(deque*);

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

deque* create_deque() {													// we declare our priority function here
	deque* d = (deque*) s_malloc(sizeof(deque));
	d->begin = (node*) s_malloc(sizeof(node));
	d->end = d->begin;
	d->begin->next = NULL;
	d->size = 0;
	return d;
}

// We show two variations in the push functions, dummy node and no dummy node (begin / end)

void push_front(deque* d, int elem) {
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

void push_back(deque* d, int elem) {
	node* add = (node*) s_malloc(sizeof(node));
	add->data = elem;
	add->prev = d->end;
	add->next = NULL;									// no dummy node, so no next
	d->end->next = add;
	d->end = add;
	++d->size;
}

void pop_front(deque* d) {
	if(d->begin->next) {
		node* del = d->begin->next;
		d->begin->next = del->next;
		if(d->begin->next)								// could be NULL, end doesn't have this problem.
			d->begin->next->prev = d->begin;
		free(del);
		--d->size;
	}
}

void pop_back(deque* d) {
	if(d->end) {
		node* del = d->end;
		d->end = d->end->prev;
		d->end->next = NULL;
		free(del);
		--d->size;
	}
}

node* front(deque* d) {
	return d->begin->next;
} 

node* back(deque* d) {
	return d->end;
}

void display_deque(deque* d) {
	printf("Front and back of the queue: %d and %d\n", front(d)->data, back(d)->data);
	printf("Deque content in normal order:\n");
	uint i = 0;
	for(node* temp = d->begin; temp != d->end; temp = temp->next, ++i)
		printf("%d. %d\n", i, temp->next->data);									// since we used dummy node, we use next here
	printf("\nDeque content in reverse order:\n");
	i = 0;
	for(node* temp = d->end; temp != d->begin; temp = temp->prev, ++i)				// end is not dummy node, so we didn't use prev here.
		printf("%d. %d\n", i, temp->data);											// these are two variations, pick whichever method you like.
}

void free_deque_front(deque* d) {
	while(front(d))
		pop_front(d);
	// free the dummy node
	free(d->begin);
	free(d);
}

void free_deque_back(deque* d) {
	while(front(d))
		pop_back(d);
	// free the dummy node
	free(d->begin);
	free(d);
}

int main() {
	srand(time(NULL));
	deque* d1 = create_deque();
	for(uint i = 0; i < 10; ++i)
		push_back(d1, (rand() % 100) + 1);
	display_deque(d1);
	printf("\n\nDeque 2 with pushfront\n\n");
	deque* d2 = create_deque();
	for(uint i = 0; i < 10; ++i)
		push_front(d2, (rand() % 100) + 1);
	display_deque(d2);
	free_deque_front(d1);
	free_deque_back(d2);
	return 0;
}
