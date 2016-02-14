#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct ARR_T {
	unsigned int nextpos;
	unsigned int capacity;
	unsigned int size;
	unsigned int checkprev;
	int* elems;
};

typedef unsigned int uint;
typedef struct ARR_T arr_t;

void* s_malloc(const uint);
void s_free(void*);

arr_t* create_array(uint);
void add_element(arr_t*, int);
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
		x->nextpos = 0;											// set current nextpos to beginning
		x->checkprev = 0;										// no signal to check all previous points for possible insertion point
		x->capacity = N;										// our array capacity (fixed)
		x->size = 0;											// our array size
		return x;
	}
	return NULL;
}

void add_element(arr_t* arr, int elem) {
	if(arr->size != arr->capacity) {							// if we can add, do add
		arr->elems[arr->nextpos] = elem;
		++arr->size;
		set_next_pos(arr);										// update the nextpos
		update_check_signal(arr);								// update the check signal
	}
}

void set_next_pos(arr_t* arr) {
	if(!arr->checkprev)											// if we don't have to check previous points
		++arr->nextpos;											// update to next
	else {														// there was some deletion involved, we must check all points
		for(uint i = 0; i < arr->nextpos; ++i)
			if(!arr->elems[i]) {								// 'NULL' element reached (our 0), use this point
				arr->nextpos = i;
				break;
			}
	}
}

void update_check_signal(arr_t* arr) {							// go through all elements, up to arr->size
	for(uint i = 0; i < arr->size; ++i)
		if(!arr->elems[i])										// if you find a 'NULL' element in between, must keep the check
			return;
	arr->checkprev = 0;											// no 'NULL' found, we can proceed with insertion to nextpos++
}

void delete_element(arr_t* arr, uint pos) {
	if(pos >= arr->capacity || !arr->elems[pos])				// if pos entered is out of range or is 'NULL', silently ignore
		return;
	arr->elems[pos] = 0;										// set it to our 'NULL'
	--arr->size;												// decrease size
	arr->checkprev = 1;											// our array order is gone, check all points till it's fixed
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
		add_element(A, (rand() % 100) + 1);
	display_array(A);
	int input = 0;
	do {
		printf("Enter position to delete: ");
		scanf("%d", &input);
		if(input == -1)
			break;
		delete_element(A, input);
		display_array(A);
	} while(input != -1);
	return 0;
}
