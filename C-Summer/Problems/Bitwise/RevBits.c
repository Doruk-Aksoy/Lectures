#include <stdio.h>

int revbits(int a) {
	int temp = a;
	int bitsize = sizeof(a) * 8 - 1;
	for (a >>= 1; a; a >>= 1) {
		temp <<= 1;
		temp |= a & 1;
		--bitsize;
	}
	return temp << bitsize;
}

int main() {
	int a;
	printf("Enter an integer: ");
	scanf("%d", &a);
	printf("Rev bits: %d", revbits(a));
	return 0;
}
