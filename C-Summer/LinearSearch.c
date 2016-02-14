#include <stdio.h>
#include <string.h>

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

void* lsearch(void* key, void* base, int n, int elemSize, int (*cmpfunc)(void*, void*)) {
	int i;
	for(i = 0; i < n; i++) {
		void* elemAddr = (char *)base + sizeof(char) * i * elemSize;
		if(!cmpfunc(key, elemAddr))
			return elemAddr;
	}
	
	return NULL;
}

// all generic pointers are of same size, so char* and char** are 4 bytes. Same for int* and int**.

int main() {
	int arr[] = { 1, 5, 16, 24, 36, 40, -7, 32768, 9, 115 };
	char names[][9] = { "John", "James", "Samantha", "Emily", "Georgio", "Nick", "Zac" };
	
	int find1 = 5, find2 = 63;
	char find3[5] = "Jack";
	char find4[9] = "Emily";
	// input to lsearch => address of thing to find, the array, the size of the array (important!), size of the element, a comparison function
	int* res1 = (int*) lsearch(&find1, arr, 10, sizeof(int), IntCmp);
	int* res2 = (int*) lsearch(&find2, arr, 10, sizeof(int), IntCmp);
	char* res3 = (char*) lsearch(&find3, names, 9, sizeof(*names), strcmp2);
	char* res4 = (char*) lsearch(&find4, names, 9, sizeof(*names), strcmp2);
	// res - arr gives the index at which the element was found (it wont be >= 0 if it's not found! (returns null, which means it's always less than an address))
	// bit different for char*, play around by changing jack to john etc.
	printf("Found 5: %d at %d\nFound 32768: %d\n", res1, res1 - arr, res2 - arr);
	printf("Jack found at %d %d\nSamantha found at %d %d\n", res3, (res3 - *names) / sizeof(*names), res4, (res4 - *names) / sizeof(*names));
	
	return 0;
}
