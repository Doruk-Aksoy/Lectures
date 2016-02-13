#include <stdio.h>

int main() {
	int x = 0; // changed to 0 from 1
	if((x & 1) == 0) // added paranthesis as operator precedence
		printf("Hello\n");
	return 0;
}
