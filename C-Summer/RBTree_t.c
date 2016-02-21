#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

///////////////////////
// DEFINITIONS BEGIN //
///////////////////////

																					// Put this part in a header or something :p

#define SPACE_PER_NODE 2

enum rbtree_node_color { 
	RED, 
	BLACK 
};

typedef int (*compare_func)(void* left, void* right);								// This gives a typedef to a function pointer of this type
typedef void (*free_func)(void*);
typedef unsigned int uint;

typedef struct rbtree_node_t {
    void* key;
    void* value;
    struct rbtree_node_t* left;
    struct rbtree_node_t* right;
    struct rbtree_node_t* parent;
    enum rbtree_node_color color;
} rbtree_node;

typedef struct rbtree_t {
    rbtree_node* root;
    int (*ord)(void*, void*);
    void (*display)(struct rbtree_t*);												// Store the display method in the tree
    void (*free_func)(void*);														// Store the free function for elements in the tree
} rbtree;

typedef rbtree_node node;
typedef enum rbtree_node_color color;

rbtree* rbtree_create(compare_func compare, free_func freefunc);
void assign_display_method(rbtree*, void (*disp)(rbtree *));
void show_tree(rbtree*);
void* rbtree_lookup(rbtree* t, void* key, compare_func compare);					// Since this is a bit complicated, prototypes include what variables are for
void rbtree_insert(rbtree* t, void* key, void* value);
void rbtree_delete(rbtree* t, void* key);

node* grandparent(node* n);
node* sibling(node* n);
node* uncle(node* n);

uint verify_properties(rbtree* t);
uint verify_property_1(node* root);
uint verify_property_2(node* root);
uint verify_property_3(node* root);
uint verify_property_3_helper(node* n, int black_count, int* black_count_path);

color node_color(node* n);
node* new_node(void* key, void* value, color node_color, node* left, node* right);
node* lookup_node(rbtree* t, void* key, compare_func compare);

void rotate_left(rbtree* t, node* n);
void rotate_right(rbtree* t, node* n);

node* maximum_node(node* root);
void replace_node(rbtree* t, node* oldn, node* newn);

void insert_case1(rbtree* t, node* n);
void insert_case2(rbtree* t, node* n);
void insert_case3(rbtree* t, node* n);
void insert_case4(rbtree* t, node* n);
void insert_case5(rbtree* t, node* n);

void delete_case1(rbtree* t, node* n);
void delete_case2(rbtree* t, node* n);
void delete_case3(rbtree* t, node* n);
void delete_case4(rbtree* t, node* n);
void delete_case5(rbtree* t, node* n);
void delete_case6(rbtree* t, node* n);

void* s_malloc(const uint);
void s_free(void*);

/////////////////////
// DEFINITIONS END //
/////////////////////

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

int compare_int(void* leftp, void* rightp) {
    int left = *(int*) leftp;
    int right = *(int*)rightp;
    if (left < right) 
        return -1;
    else if (left > right)
        return 1;
    return 0;
}

int compare_str(void* leftp, void* rightp) {
	return strcmp(leftp, rightp);
}

node* grandparent(node* t) {
	if(t && t->parent && t->parent->parent)											// Not NULL, Not root, not child of root
    	return t->parent->parent;
    return NULL;
}

node* sibling(node* t) {
    if(t && t->parent) {															// Not NULL, has no sibling
		if (t == t->parent->left)
			return t->parent->right;
		return t->parent->left;
	}
	return NULL;
}

node* uncle(node* t) {
	if(t && t->parent && t->parent->parent)											// Not NULL, Not root, Not children of root
    	return sibling(t->parent);
    return NULL;
}

uint verify_properties(rbtree* t) {
    return  verify_property_1(t->root) &&
    		verify_property_2(t->root) &&											// Property 4 is implicit, the ordering
    		verify_property_3(t->root);
}

uint verify_property_1(node* t) {													// Root's color must be black
    if(node_color(t) == BLACK)
    	return 1;
    return 0;
}

uint verify_property_2(node* t) {													// If a node is red, children of it must be black, and it's parent also black (color invariant)
    if (node_color(t) == RED) {
    	if(node_color(t->left) == BLACK && node_color(t->right) == BLACK && node_color(t->parent) == BLACK)
	       return 1;
	    return 0;
    }
    if (!t) 
		return 1;
    return verify_property_2(t->left) && verify_property_2(t->right);
}

uint verify_property_3(node* t) {													// Check the black depth, the height invariant
    int black_count_path = -1;
    return verify_property_3_helper(t, 0, &black_count_path);
}

uint verify_property_3_helper(node* t, int black_count, int* path_black_count) {
    if (node_color(t) == BLACK)														// if current node color is black, increase (NULL nodes also count as black)
        ++black_count;
    uint res = 0;
    if (!t) {																		// reached a NULL node possibly leaf
        if (*path_black_count == -1)												// did it not change?
            *path_black_count = black_count;										// well it's empty then
        if(black_count == *path_black_count)										// is the black height invariant satisfied?
            return 1;
        return 0;
    }
    
    return  verify_property_3_helper(t->left,  black_count, path_black_count) &&	// check others
			verify_property_3_helper(t->right, black_count, path_black_count);
}

color node_color(node* t) {
    return (!t) ? BLACK : t->color;
}

rbtree* rbtree_create(compare_func compare, free_func freefunc) {
    rbtree* t = (rbtree*) s_malloc(sizeof(rbtree));
    t->ord = compare;
    t->free_func = freefunc;
    t->root = NULL;
    return t;
}

node* new_node(void* key, void* value, color node_color, node* left, node* right) {	// Creates a node given left/right nodes (accepts NULL)
    node* result = (node *) s_malloc(sizeof(node));
    result->key = key;
    result->value = value;
    result->color = node_color;
    result->left = left;
    result->right = right;
    if (left)  
		left->parent = result;
    if (right) 
		right->parent = result;
    result->parent = NULL;
    return result;
}

node* lookup_node(rbtree* t, void* key, compare_func compare) {						// This is basically a search by the key of a node
    node* n = t->root;
    while (n) {
        int comp_result = compare(key, n->key);
        if (comp_result == 0)
            return n;
		else if (comp_result < 0)
            n = n->left;
		else
            n = n->right;
    }
    return n;
}

void* rbtree_lookup(rbtree* t, void* key, compare_func compare) {
    node* n = lookup_node(t, key, compare);
    return (!n) ? NULL : n->value;
}

void rotate_left(rbtree* t, node* n) {
    node* r = n->right;																// the node to replace n
    replace_node(t, n, r);															// replace it
    n->right = r->left;																// replaced node's right is now left
    if (r->left)																	// if the left isn't NULL, update parent
        r->left->parent = n;
    r->left = n;																	// update left
    n->parent = r;																	// update parent
}

void rotate_right(rbtree* t, node* n) {												// Similar principle to the above
    node* l = n->left;
    replace_node(t, n, l);
    n->left = l->right;
    if (l->right)
        l->right->parent = n;
    l->right = n;
    n->parent = l;
}

void replace_node(rbtree* t, node* oldn, node* newn) {								// Helper function to find and replace a given node
    if (!oldn->parent)
        t->root = newn;
	else {
        if (oldn == oldn->parent->left)
            oldn->parent->left = newn;
        else
            oldn->parent->right = newn;
    }
    if (newn)
        newn->parent = oldn->parent;
}

void rbtree_insert(rbtree* t, void* key, void* value) {								// This is very similar to BST insert, but it branches off to other cases
    node* inserted_node = new_node(key, value, RED, NULL, NULL);
    if (!t->root)
        t->root = inserted_node;
	else {
        node* n = t->root;
        while (1) {
            int comp_result = t->ord(key, n->key);
            if (!comp_result) {														// update value if same key is entered
				n->value = value;
                return;
            } 
			else if (comp_result < 0) {
                if (!n->left) {
                    n->left = inserted_node;
                    break;
                } 
				else
                    n = n->left;
            } 
			else {
                if (n->right == NULL) {
                    n->right = inserted_node;
                    break;
                } 
				else
                    n = n->right;
            }
        }
        inserted_node->parent = n;
    }
    insert_case1(t, inserted_node);
}

void insert_case1(rbtree* t, node* n) {
    if (!n->parent)
        n->color = BLACK;															// No parent? Must be root
    else
        insert_case2(t, n);															// Move on to case 2
}

void insert_case2(rbtree* t, node* n) {
    if (node_color(n->parent) == BLACK)
        return; 																	// Tree is still valid
    else
        insert_case3(t, n);
}

void insert_case3(rbtree* t, node* n) {
    if (node_color(uncle(n)) == RED) {												// Is uncle red?
        n->parent->color = BLACK;													// Repaint the parent to black
        uncle(n)->color = BLACK;													// Repaint uncle to black
        grandparent(n)->color = RED;												// Repaint grandparent to red
        insert_case1(t, grandparent(n));											// Repeat the procedure on grandparent
    } 
	else
        insert_case4(t, n);															// No? Move on
}

void insert_case4(rbtree* t, node* n) {
    if (n == n->parent->right && n->parent == grandparent(n)->left) {				// Is this node the right child of parent
        rotate_left(t, n->parent);													// And is the parent the left child of grandparent?
        n = n->left;																// Rotate and update
    } 
	else if (n == n->parent->left && n->parent == grandparent(n)->right) {			// Is it the other way around?
        rotate_right(t, n->parent);													// Then rotate the other way around
        n = n->right;
    }
    insert_case5(t, n);																// Fix the remaining in case 5
}

void insert_case5(rbtree* t, node* n) {
    n->parent->color = BLACK;														// Recolor parent to black
    grandparent(n)->color = RED;													// Recolor grandparent to red
    if (n == n->parent->left && n->parent == grandparent(n)->left)					// if left of parent and it's parent is left of grandparent
        rotate_right(t, grandparent(n));
	else
        rotate_left(t, grandparent(n));
}

void rbtree_delete(rbtree* t, void* key) {
    node* child;
    node* n = lookup_node(t, key, t->ord);
    if (!n) 																		// Key not found, do nothing
		return;  																
    if (n->left && n->right) {										
        node* pred = maximum_node(n->left);											// Copy key/value from predecessor and then delete it instead
        n->key = pred->key;
        n->value = pred->value;
        n = pred;
    }

    child = (n->right == NULL) ? n->left : n->right;
    if (node_color(n) == BLACK) {
        n->color = node_color(child);
        delete_case1(t, n);
    }
    
    replace_node(t, n, child);
    if (!n->parent && child) 														// root should be black
        child->color = BLACK;
    t->free_func(n);
}

node* maximum_node(node* t) {
    while (t->right)
        t = t->right;
    return t;
}

void delete_case1(rbtree* t, node* n) {
    if (!n->parent)																	// This means n is the new root
        return;																		// We are done, no need to do anything
    delete_case2(t, n);
}

void delete_case2(rbtree* t, node* n) {
    if (node_color(sibling(n)) == RED) {											// Is sibling red?
        n->parent->color = RED;														// Swap colors of sibling and parent
        sibling(n)->color = BLACK;
        if (n == n->parent->left)													// If this is parent's left child
            rotate_left(t, n->parent);												// then rotate to left
        else
            rotate_right(t, n->parent);												// rotate right if otherwise
    }
    delete_case3(t, n);
}

void delete_case3(rbtree* t, node* n) {
    if (
			node_color(n->parent) == BLACK 				&&							// Is the parent black?
        	node_color(sibling(n)) == BLACK 			&&							// Sibling also?
        	node_color(sibling(n)->left) == BLACK 		&&							// It's children also?
        	node_color(sibling(n)->right) == BLACK
	   )
    {
    																				// Preserve black height constraint
        sibling(n)->color = RED;													// Recolor sibling to red
        delete_case1(t, n->parent);													// perform rebalance on parent
    }
    else
        delete_case4(t, n);															// Move on to case 4
}

void delete_case4(rbtree* t, node* n) {
    if (
			node_color(n->parent) == RED 				&&							// Is the parent red?
       		node_color(sibling(n)) == BLACK			 	&&							// Is the sibling black?
        	node_color(sibling(n)->left) == BLACK 		&&							// Children also black?
       		node_color(sibling(n)->right) == BLACK
	   )
    {
        sibling(n)->color = RED;													// Swap colors
        n->parent->color = BLACK;
    }
    else
        delete_case5(t, n);															// No? Case 5.
}

void delete_case5(rbtree* t, node* n) {
    if (
			n == n->parent->left 						&&							// Is the node left child?
        	node_color(sibling(n)) == BLACK 			&&							// Sibling is black?
        	node_color(sibling(n)->left) == RED 		&&							// Left child of sibling is red?
        	node_color(sibling(n)->right) == BLACK									// Right child is black?
	   )
    {
        sibling(n)->color = RED;													// Swap colors with sibling
        sibling(n)->left->color = BLACK;											// And it's left child
        rotate_right(t, sibling(n));												// Do a rotation to right
    }
    else if (
				n == n->parent->right 					&&							// If the direction changed
            	node_color(sibling(n)) == BLACK 		&&							// Simply change rotation
             	node_color(sibling(n)->right) == RED 	&&
            	node_color(sibling(n)->left) == BLACK
			)
    {
        sibling(n)->color = RED;
        sibling(n)->right->color = BLACK;
        rotate_left(t, sibling(n));
    }
    delete_case6(t, n);
}

void delete_case6(rbtree* t, node* n) {
    sibling(n)->color = node_color(n->parent);										// Exchange colors with sibling
    n->parent->color = BLACK;														// And the parent
    if (n == n->parent->left) {														// Direction matters, again
        sibling(n)->right->color = BLACK;											// Direction will decide which child
        rotate_left(t, n->parent);													// Of the sibling has their color changed
    }
    else {
        sibling(n)->left->color = BLACK;
        rotate_right(t, n->parent);
    }
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
 
void display_helper_int(node* t, int spaces) {
    int width = ceil(log10(max_depth(t)+0.01)) + 4;
    wchar_t* sp64 = L"                                                                                ";
    if (!t) {
        wprintf(L"\n");
        return;
    }
    display_helper_int(t->right, spaces + width);
    char tcolor = (t->color == BLACK) ? 'B' : 'R';
    wprintf(L"%*.*s<%d> - <%d> |%c|\n", 0, spaces, sp64, *(int *)t->key, *(int *)t->value, tcolor);
    display_helper_int(t->left, spaces + width);
}

void display_tree_int(rbtree* t) {
	if(t && t->root)
		display_helper_int(t->root, SPACE_PER_NODE);
}

// There's a problem regarding wprintf (wchar_t) and printing chars, so I couldn't fix it and here it is :p
void display_helper_str(node* t, int spaces) {
    int width = ceil(log10(max_depth(t)+0.01)) + 4;
    char* sp64 = "                                                                                ";
    if (!t) {
        wprintf(L"\n");
        return;
    }
    display_helper_str(t->right, spaces + width);
    char tcolor = (t->color == BLACK) ? 'B' : 'R';
    printf("%*.*s<%d> - <%s> |%c|\n", 0, spaces, sp64, *(int *)t->key, (char *)t->value, tcolor);
    display_helper_str(t->left, spaces + width);
}

void display_tree_str(rbtree* t) {
	if(t && t->root)
		display_helper_str(t->root, SPACE_PER_NODE);
}

void assign_display_method(rbtree* t, void (*disp)(rbtree *)) {
	t->display = disp;
}

void show_tree(rbtree* t) {
	t->display(t);
}

int main() {
	srand(time(NULL));
	/*
	rbtree* tree = rbtree_create(compare_int, s_free);
	assign_display_method(tree, display_tree_int);
	for(uint i = 0; i < 15; ++i) {
		int* value = (int *) s_malloc(sizeof(int));
		*value = rand() % 1000 + 1;
		int* key = (int *) s_malloc(sizeof(int));
		*key = rand() % 1000 + 1;
		rbtree_insert(tree, key, value);
	}
	show_tree(tree);
	printf("Is R&B Tree: %d\n", verify_properties(tree));
	int input;
	do {
		printf("Enter value to delete: ");
		scanf("%d", &input);
		rbtree_delete(tree, (void *)&input);
		show_tree(tree);
		printf("Is R&B Tree: %d\n", verify_properties(tree));
	} while(input != -1);
	*/
	rbtree* tree = rbtree_create(compare_str, s_free);
	assign_display_method(tree, display_tree_str);
	const char* test_names[15] = { "Samantha", "James", "John", "Jack", "Jill", "Mason", "Doruk", "Batu", "George", "Wilson", "Kruger", "Steve", "Bob", "Trenton", "Graves" };
	for(uint i = 0; i < 15; ++i) {
		int* key = (int *) s_malloc(sizeof(int));
		*key = rand() % 1000 + 1;
		char* str = strdup(test_names[rand() % 15]);
		rbtree_insert(tree, key, str);
	}
	show_tree(tree);
	printf("Is R&B Tree: %d\n", verify_properties(tree));
	int input;
	do {
		printf("Enter value to delete: ");
		scanf("%d", &input);
		rbtree_delete(tree, (void *)&input);
		show_tree(tree);
		printf("Is R&B Tree: %d\n", verify_properties(tree));
	} while(input != -1);
	return 0;
}
