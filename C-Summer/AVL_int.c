#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SPACE_PER_NODE 2

enum delete_flags {
	DELETE_NO_FORCE,
	DELETE_FORCE
};

typedef unsigned int uint;

struct tree_elem {
	int data;
	uint dup_count;
	int height;
	struct tree_elem* left;
	struct tree_elem* right;
};

typedef struct tree_elem node;

node* create_bst();
void insert(node**, int);
void delete_elem(node**, int, uint);
node* search(node*, int);
node* get_parent(node*, node*);
node* find_min(node*);
node* get_successor(node*, node*);
uint max_depth(node*);
void display_helper(node*, int);
void display_tree(node*);
int get_height(node*);
void rotate_once_left(node**);
void rotate_once_right(node**);
void rotate_twice_left(node**);
void rotate_twice_right(node**);
int MAX(int, int);
int get_balance(node*);
int is_balanced(node*);

// max macro should not be used! max(x,y) + 1 would affect the macro! needs extra paranthesis!
int Max(int x, int y) {
	return (x > y) ? x : y;
}

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

node* create_bst(int data) {
	node* tree = (node*) s_malloc(sizeof(node));
	tree->left = tree->right = NULL;
	tree->data = data;
	tree->height = 0;
	return tree;
}

void insert(node** t, int val) {
	if(!(*t)) {
		*t = (node*) s_malloc(sizeof(node));
		(*t)->data = val;
		(*t)->left = (*t)->right = NULL;
		(*t)->dup_count = 0;
		(*t)->height = 0;
		return;
	}
	if((*t)->data < val) {
		insert(&(*t)->right, val);
		
		if(get_height((*t)->right) - get_height((*t)->left) >= 2) {
			if((*t)->right->data < val)
				rotate_once_right(t);
			else if((*t)->right->data > val)
				rotate_twice_right(t);
		}
	}
	else if((*t)->data > val) {
		insert(&(*t)->left, val);
		
		if(get_height((*t)->left) - get_height((*t)->right) >= 2) {
			if((*t)->left->data > val)
				rotate_once_left(t);
			else if((*t)->left->data < val)
				rotate_twice_left(t);
		}
	}
	else {
		++(*t)->dup_count;
		return;																// this is important! if there are duplicates, they might cause an unwanted height change!
	}
	(*t)->height = Max(get_height((*t)->left), get_height((*t)->right)) + 1;
}

node* get_successor(node* t, node* s) {
    if(s->right)
        return find_min(s->right);
    node* suc = NULL;
    node* temp = t;
    // Start from root and search for successor in the tree
    while (temp) {
        if (s->data < temp->data) {
            suc = temp;
            temp = temp->left;
        }
        else if (s->data > temp->data)
            temp = temp->right;
        else
           break;
    }
    return suc;
}

void free_tree (node* t) {
	if (!t)
		return;
	free_tree(t->left);
	free_tree(t->right);
    free(t);
}

void delete_elem(node** t, int val, uint delete_flag) {
	if(!(*t))
		return;
	if((*t)->data < val)
		delete_elem(&(*t)->right, val, delete_flag);
	else if((*t)->data > val)
		delete_elem(&(*t)->left, val, delete_flag);
	else {																	// found elem
																			// if there's a duplicate, remove it
		if(delete_flag == DELETE_NO_FORCE && (*t)->dup_count)	{
			--(*t)->dup_count;
			printf("Duplicate element removed. %d left.\n", (*t)->dup_count);
		}
		else { 																// there were no duplicates
			node* del;
			if(!(*t)->left && (*t)->right) {								// no left but right
				del = (*t);
				(*t) = (*t)->right;
				free(del);
			}
			else if(!(*t)->right && (*t)->left) {							// no right but left
				del = (*t);
				(*t) = (*t)->left;
				free(del);
			}
			else if(!(*t)->left && !(*t)->right) {							// additionally, you can use the is_leaf function we wrote before, in here
				free(*t);
				*t = NULL;
			}
			else {															// has both children
				del = get_successor(*t, *t);								// get the in-order successor in tree *t of *t
				(*t)->data = del->data;										// swap the successor's data with t's data
				(*t)->dup_count = del->dup_count;							// IMPORTANT! We support duplicates! Duplicate count must also be swapped!
				delete_elem(&(*t)->right, del->data, DELETE_FORCE);			// We must now delete this guy forcefully!
			}
		}
	}
			
	if(!(*t))																// if t was deleted and not needed anymore, don't bother. Update height if it exists
		return;
	
	(*t)->height = Max(get_height((*t)->left), get_height((*t)->right)) + 1;
	int bfactor = get_balance(*t);
	
	if(bfactor > 1 && get_balance((*t)->left) >= 0)							// left outweighs right
		rotate_once_left(t);
	else if(bfactor > 1 && get_balance((*t)->left) < 0)						// need right left rotation
		rotate_twice_left(t);
	else if(bfactor < -1 && get_balance((*t)->right) <= 0)					// right outweighs left
		rotate_once_right(t);
	else if(bfactor < -1 && get_balance((*t)->right) > 0)					// need left right rotation
		rotate_twice_right(t);
}

node* search(node* t, int val) {
	if(!t)
		return NULL;
	if(t->data == val)
		return t;
	else if(t->data < val)
		return search(t->right, val);
	return search(t->left, val);
}

node* find_min(node* t) {
	node* temp = t;
	while(temp->left)
		temp = temp->left;
	return temp;
}

int get_balance(node* t) {
	if(!t)
		return 0;
	return get_height(t->left) - get_height(t->right);
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
    wprintf(L"%*.*s%d\n", 0, spaces, sp64, t->data);
    display_helper(t->left, spaces + width);
}

void display_tree(node* t) {
	if(t)
		display_helper(t, SPACE_PER_NODE);
}

int get_height(node* t) {
	if(!t)
		return -1;															// if nothing exists, that side must be outweighed. -1 is put.
	return t->height;
}

void rotate_once_left(node** k1) {
	node* temp = (*k1)->left;
	(*k1)->left = temp->right;
	temp->right = *k1;

	(*k1)->height = Max(get_height((*k1)->left), get_height((*k1)->right)) + 1;
	temp->height = Max(get_height(temp->left), (*k1)->height) + 1;
	
	*k1 = temp;
}

void rotate_once_right(node** k1) {
	node* temp = (*k1)->right;
	(*k1)->right = temp->left;
	temp->left = *k1;

	(*k1)->height = Max(get_height((*k1)->left), get_height((*k1)->right)) + 1;
	temp->height = Max(get_height(temp->right), (*k1)->height) + 1;
	
	*k1 = temp;
}

void rotate_twice_left(node** k1) {
	rotate_once_right(&(*k1)->left);
	rotate_once_left(k1);
}

void rotate_twice_right(node** k1) {
	rotate_once_left(&(*k1)->right);
    rotate_once_right(k1);
}

int is_balanced(node* t) {
	if(!t)
		return 1; 
	int lefth = get_height(t->left), righth = get_height(t->right);
	
	if( abs(lefth - righth) <= 1 && is_balanced(t->left) && is_balanced(t->right))
		return 1;
	return 0;
}

int main() {
	srand(time(NULL));
	node* tree = create_bst(rand() % 15 + 1);
	for(uint i = 0; i < 14; ++i) {
		/*
		// create unique elements from 1 to 15.
		do {
			elem = rand() % 15 + 1;
		} while (search(tree, elem));
		*/
		int elem = rand() % 15 + 1;
		insert(&tree, elem);
	}
	display_tree(tree);
	printf("Tree AVL result: %d\n", is_balanced(tree));
	int input;
	do {
		printf("Enter value to delete: ");
		scanf("%d", &input);
		delete_elem(&tree, input, DELETE_NO_FORCE);
		display_tree(tree);
		printf("Tree AVL result: %d\n", is_balanced(tree));
	} while(input != -1);
	return 0;
}
