#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct ARR_T {
	unsigned int lastpos;
	int	lastelem;
	unsigned int capacity;
	unsigned int size;
	int* elems;
};

typedef unsigned int uint;
typedef struct ARR_T arr_t;

void* s_malloc(const uint);
void s_free(void*);

arr_t* create_array(uint);
void add_element(arr_t*, int, uint);
void set_next_pos(arr_t*);
void update_check_signal(arr_t*);
void display_array(arr_t*);
void delete_element(arr_t*, uint);

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

arr_t* create_array(uint N) {
	if(N) {
		arr_t* x = (arr_t*) s_malloc(sizeof(arr_t));			// allocate the array struct ptr
		x->elems = (int*) s_malloc(sizeof(int) * N);			// allocate the array itself
		for(uint i = 0; i < N; ++i)								// initialize all array elements to NULL
			x->elems[i] = 0; 									// suppose our 'NULL' element is 0.
		x->lastpos = 0;											// set current nextpos to beginning
		x->capacity = N;										// our array capacity (fixed)
		x->size = 0;											// our array size
		return x;
	}
	return NULL;
}

void add_element(arr_t* arr, int elem, uint pos) {
	if(arr->size != arr->capacity && pos < arr->capacity && !arr->elems[pos]) {
		arr->elems[pos] = elem;
		arr->lastelem = elem;
		arr->lastpos = pos;
		++arr->size;
	}
}

void delete_element(arr_t* arr, uint pos) {
	if(pos >= arr->capacity || !arr->elems[pos])				// if pos entered is out of range or is 'NULL', silently ignore
		return;
	arr->elems[pos] = 0;										// set it to our 'NULL'
	--arr->size;												// decrease size
	if(!pos) {
		arr->lastpos = 0;
		arr->lastelem = arr->elems[0];
	}
	else {
		arr->lastpos = pos - 1;
		arr->lastelem = arr->elems[arr->lastpos];
	}
}

void display_array(arr_t* arr) {
	printf("Array contents:\n");
	if(!arr->size)
		printf("It's empty!\n");
	for(uint i = 0; i < arr->capacity; ++i) {
		if(!arr->elems[i])
			continue;
		printf("%d. -- %d\n", i, arr->elems[i]);
	}
}

int main() {
	srand(time(NULL));
	uint s = 10;
	arr_t* A = create_array(s);
	for(uint i = 0; i < s; ++i)
		add_element(A, (rand() % 100) + 1, i);
	display_array(A);
	int input = 0;
	do {
		printf("Enter position to delete: ");
		scanf("%d", &input);
		if(input == -1)
			break;
		delete_element(A, input);
		printf("Last element: %d\n", A->lastelem);
		display_array(A);
	} while(input != -1);
	add_element(A, 1337, 6);
	add_element(A, 2456, 7);
	display_array(A);
	return 0;
}
