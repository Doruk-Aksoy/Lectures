
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct ARR_T {
	unsigned int lastpos;
	void* lastelem;
	unsigned int capacity;
	unsigned int size;
	void** elems;
};

typedef unsigned int uint;
typedef struct ARR_T arr_t;

void* s_malloc(const uint);
void s_free(void*);

arr_t* create_array(uint, uint);
void add_element(arr_t*, void*, uint, uint);
void set_next_pos(arr_t*);
void update_check_signal(arr_t*);
void delete_element(arr_t*, uint, void (*free_func)(void*));
void display_array_int(arr_t*);

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

arr_t* create_array(uint N, uint elemsize) {
	if(N) {
		arr_t* x = (arr_t*) s_malloc(sizeof(arr_t));					// allocate the array struct ptr
		x->elems = (void **) s_malloc(sizeof(void*) * N);							// allocate the array itself
		for(uint i = 0; i < N; ++i) {									// initialize all array elements to NULL
			x->elems[i] = s_malloc(sizeof(char) * elemsize);
			x->elems[i] = NULL; 										// now this is needed
		}
		x->lastpos = 0;													// set current nextpos to beginning
		x->capacity = N;												// our array capacity (fixed)
		x->size = 0;													// our array size
		return x;
	}
	return NULL;
}

void add_element(arr_t* arr, void* elem, uint pos, uint elemsize) {
	if(arr->size != arr->capacity && pos < arr->capacity && !arr->elems[pos]) {
		arr->elems[pos] = elem;
		arr->lastelem = elem;
		arr->lastpos = pos;
		++arr->size;
	}
}

void delete_element(arr_t* arr, uint pos, void (*free_func)(void*)) {
	if(pos >= arr->capacity || !arr->elems[pos])						// if pos entered is out of range or is 'NULL', silently ignore
		return;
	free_func(arr->elems[pos]);
	arr->elems[pos] = NULL;												// set it to our 'NULL'
	--arr->size;														// decrease size
	if(!pos) {
		arr->lastpos = 0;
		arr->lastelem = arr->elems[0];
	}
	else {
		// here, this is a bug. If we remove consecutive spots, this will crash! Why did we not spot this in our previous implementation?
		for(uint i = pos - 1; i > 0; --i)
			if(arr->elems[i]) {
				arr->lastpos = i;
				break;
			}
		arr->lastelem = arr->elems[arr->lastpos];						
	}
}

void display_array_int(arr_t* arr) {
	printf("Array contents:\n");
	if(!arr->size)
		printf("It's empty!\n");
	for(uint i = 0; i < arr->capacity; ++i) {
		if(!arr->elems[i])
			continue;
		printf("%d. -- %d\n", i, *(int *)arr->elems[i]);
	}
}

void display_array_str(arr_t* arr) {
	printf("Array contents:\n");
	if(!arr->size)
		printf("It's empty!\n");
	for(uint i = 0; i < arr->capacity; ++i) {
		if(!arr->elems[i])
			continue;
		printf("%d. -- %s\n", i, (char *)arr->elems[i]);
	}
}

int main() {
	srand(time(NULL));
	uint s = 10;
	
	arr_t* A = create_array(s, sizeof(int));
	for(uint i = 0; i < s; ++i) {
		int* elem = (int *) s_malloc(sizeof(int)); // needed, as if you use a simple int elem, every element will land on same memory! (potentially bad)
		*elem = (rand() % 100) + 1;
		add_element(A, elem, i, sizeof(int));
	}
	display_array_int(A);
	int input = 0;
	do {
		printf("Enter position to delete: ");
		scanf("%d", &input);
		if(input == -1)
			break;
		delete_element(A, input, s_free);
		printf("Last element: %d\n", *(int*)A->lastelem);
		display_array_int(A);
	} while(input != -1);
	int add1 = 1337, add2 = 2465;
	add_element(A, &add1, 6, sizeof(int));
	add_element(A, &add2, 7, sizeof(int));
	display_array_int(A);
	
	arr_t* B = create_array(4, sizeof(char*));
	char* s1 = strdup("John"); // if these are s1[], ie. not dynamically allocated on heap, the delete will crash. Why?
	char* s2 = strdup("Jack");
	char* s3 = strdup("Samantha");
	char* s4 = strdup("Mason");
	add_element(B, s1, 0, sizeof(char*));
	add_element(B, s2, 1, sizeof(char*));
	add_element(B, s3, 2, sizeof(char*));
	add_element(B, s4, 3, sizeof(char*));
	display_array_str(B);
	printf("Enter position to delete: ");
	scanf("%d", &input);
	delete_element(B, input, s_free);
	display_array_str(B);
	return 0;
}
