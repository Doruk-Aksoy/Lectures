#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SPACE_PER_NODE 2

struct tree_elem {
	int data;
	unsigned int dup_count;
	struct tree_elem* left;
	struct tree_elem* right;
};

typedef struct tree_elem node;
typedef unsigned int uint;

node* create_bst();
void insert(node**, int);
void delete_elem(node**, int);
node* search(node*, int);
node* get_parent(node*, node*);
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

node* create_bst(int data) {
	node* tree = (node*) s_malloc(sizeof(node));
	tree->left = tree->right = NULL;
	tree->data = data;
	return tree;
}

void insert(node** t, int val) {
	if(!(*t)) {
		*t = (node*) s_malloc(sizeof(node));
		(*t)->data = val;
		(*t)->left = NULL;
		(*t)->right = NULL;
		(*t)->dup_count = 0;
		return;
	}
	if((*t)->data < val)
		insert(&(*t)->right, val);
	else if((*t)->data > val)
		insert(&(*t)->left, val);
	else
		++(*t)->dup_count;
}

void delete_elem(node** t, int val) {
	if(!(*t))
		return;
	if((*t)->data < val)
		delete_elem(&(*t)->right, val);
	else if((*t)->data > val)
		delete_elem(&(*t)->left, val);
	else {
		if((*t)->dup_count)							// there were no duplicates
			--(*t)->dup_count;
		else {
			node* del;
			if(!(*t)->left) {
				del = (*t);
				(*t) = (*t)->right;
				free(del);
			}
			else if(!(*t)->right) {
				del = (*t);
				(*t) = (*t)->left;
				free(del);
			}
			else {
				del = find_min((*t)->right);
				(*t)->data = del->data;
				delete_elem(&(*t)->right, del->data);
			}
		}
	}
}

node* search(node* t, int val) {
	if(!t)
		return NULL;
	if(t->data == val)
		return t;
	if(t->data < val)
		return search(t->right, val);
	else
		return search(t->left, val);
}

node* get_parent(node* t, node* s) {
	if(!t)
		return NULL;
	if(t->left && t->left == s)
		return t;
	if(t->right && t->right == s)
		return t;
	if(t->data < s->data)
		return get_parent(t->right, s);
	else
		return get_parent(t->left, s);
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
    wprintf(L"%*.*s%d\n", 0, spaces, sp64, t->data);
    display_helper(t->left, spaces + width);
}

void display_tree(node* t) {
	if(t)
		display_helper(t, SPACE_PER_NODE);
}

void find_max_leaf(node* t, int* sum, int cur_sum, node** leaf) {
    if (!t)
        return;
 
    // update the current sum
    cur_sum = cur_sum + t->data;
 
    // update if this is a leaf node, and it produced a better sum
    if (is_leaf(t)) {
        if (cur_sum > *sum) {
            *sum = cur_sum;
            *leaf = t;
        }
    }
 
    // not a leaf, we must branch now
    find_max_leaf(t->left, sum, cur_sum, leaf);
    find_max_leaf(t->right, sum, cur_sum, leaf);
}

int max_path(node* t) {
    if (!t)
        return 0;
    node *temp;
    int sum = INT_MIN;
    // get the leaf with most sum
    find_max_leaf(t, &sum, 0, &temp);
    return sum;
}

int main() {
	srand(time(NULL));
	node* tree = create_bst(40);
	for(uint i = 0; i < 10; ++i) {
		int elem = rand() % 100 + 1;
		insert(&tree, elem);
	}
	display_tree(tree);
	printf("Max Root to Leaf sum is %d.", max_path(tree));
	return 0;
}
