#include <stdio.h>

unsigned int closestpow2(unsigned int a) {
	unsigned int res = 0;
	if(a && !(a & (a - 1)))
		return a;
	while(a) {
		a >>= 1;
		++res;
	}
	return 1 << res;
}

int main() {
	int a;
	printf("Enter a number: ");
	scanf("%d", &a);
	printf("Closest power of 2 to %d = %d", a, closestpow2(a));
	return 0;
}
