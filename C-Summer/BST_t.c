#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SPACE_PER_NODE 2

struct tree_elem {
	void* data;
	unsigned int dup_count;
	struct tree_elem* left;
	struct tree_elem* right;
};

typedef struct tree_elem node;
typedef unsigned int uint;

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

node* create_bst();
void insert(node**, void*, int (*cmpfunc)(void*, void*));
void delete_elem(node**, void*, uint, int (*cmpfunc)(void*, void*), void (*freefunc)(void*));
node* search(node*, void*, int (*cmpfunc)(void*, void*));
node* get_parent(node*, node*, int (*cmpfunc)(void*, void*));
node* find_min(node*);
uint is_leaf(node*);
uint max_depth(node*);
void display_helper(node*, int);
void display_tree(node*);

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

node* create_bst(void* data) {
	node* tree = (node*) s_malloc(sizeof(node));
	tree->left = tree->right = NULL;
	tree->data = data;
	tree->dup_count = 0;
	return tree;
}

void insert(node** t, void* val, int (*cmpfunc)(void*, void*)) {
	if(!(*t)) {
		*t = (node*) s_malloc(sizeof(node));
		(*t)->data = val;
		(*t)->left = NULL;
		(*t)->right = NULL;
		(*t)->dup_count = 0;
		return;
	}
	if(cmpfunc((*t)->data, val) < 0)
		insert(&(*t)->right, val, cmpfunc);
	else if(cmpfunc((*t)->data, val) > 0)
		insert(&(*t)->left, val, cmpfunc);
	else
		++(*t)->dup_count;
}

void delete_elem(node** t, void* val, uint elemsize, int (*cmpfunc)(void*, void*), void (*freefunc)(void*)) {
	if(!(*t))
		return;
	if(cmpfunc((*t)->data, val) < 0)
		delete_elem(&(*t)->right, val, elemsize, cmpfunc, s_free);
	else if(cmpfunc((*t)->data, val) > 0)
		delete_elem(&(*t)->left, val, elemsize, cmpfunc, s_free);
	else {
		if((*t)->dup_count)	{						// there were no duplicates
			--(*t)->dup_count;
			printf("Duplicate element removed.\n");
		}
		else {
			node* del;
			if(!(*t)->left) {
				del = (*t);
				(*t) = (*t)->right;
				freefunc(del);
			}
			else if(!(*t)->right) {
				del = (*t);
				(*t) = (*t)->left;
				freefunc(del);
			}
			else {
				del = find_min((*t)->right);
				memcpy((*t)->data, del->data, elemsize);
				delete_elem(&(*t)->right, del->data, elemsize, cmpfunc, s_free);
			}
		}
	}
}

node* search(node* t, void* val, int (*cmpfunc)(void*, void*)) {
	if(!t)
		return NULL;
	if(cmpfunc(t->data, val) == 1)
		return t;
	if(cmpfunc(t->data, val) < 0)
		return search(t->right, val, cmpfunc);
	else
		return search(t->left, val, cmpfunc);
}

node* get_parent(node* t, node* s, int (*cmpfunc)(void*, void*)) {
	if(!t)
		return NULL;
	if(t->left && t->left == s)
		return t;
	if(t->right && t->right == s)
		return t;
	if(cmpfunc(t->data, s->data) < 0)
		return get_parent(t->right, s, cmpfunc);
	else
		return get_parent(t->left, s, cmpfunc);
}

node* find_min(node* t) {
	node* temp = t;
	while(temp->left)
		temp = temp->left;
	return temp;
}

uint is_leaf(node* t) {
	if(t && !t->left && !t->right)
		return 1;
	return 0;
}

uint max_depth(node* t) {
   if (!t)
       return 0;
   int ldepth = max_depth(t->left);
   int rdepth = max_depth(t->right);
   if (ldepth > rdepth)
       return ldepth + 1;
   return rdepth + 1;
}
 
void display_helper(node* t, int spaces) {
    int width = ceil(log10(max_depth(t)+0.01)) + 2;
    wchar_t* sp64 = L"                                                                ";
    if (!t) {
        wprintf(L"\n");
        return;
    }
    display_helper(t->right, spaces + width);
    wprintf(L"%*.*s%d\n", 0, spaces, sp64, *(int *) t->data);
    display_helper(t->left, spaces + width);
}

void display_tree(node* t) {
	if(t)
		display_helper(t, SPACE_PER_NODE);
}

int main() {
	srand(time(NULL));
	int* root_val = (int *) s_malloc(sizeof(node));
	*root_val = 40;
	node* tree = create_bst(root_val);
	for(uint i = 0; i < 10; ++i) {
		int* elem = (int*) s_malloc(sizeof(node));
		*elem = rand() % 100 + 1;
		insert(&tree, elem, IntCmp);
	}
	display_tree(tree);
	
	int input;
	do {
		printf("Enter value to delete: ");
		scanf("%d", &input);
		delete_elem(&tree, &input, sizeof(int), IntCmp, s_free);
		display_tree(tree);
	} while(input != -1);
	
	return 0;
}
