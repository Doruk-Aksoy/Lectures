#include <stdio.h>

// true if odd, false if even
int evenodd(int a) {
	return a & 1;
}

int main() {
	int a;
	printf("Enter a number: ");
	scanf("%d", &a);
	printf("Even / Odd result: %d", evenodd(a));
	return 0;
}
