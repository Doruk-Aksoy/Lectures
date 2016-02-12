#include <stdio.h>

int is_2(int x) {
	return !(x & (x - 1));
}

int main() {
	int x = 16, y = 32, z = 25, t = 313;
	printf("Is power of 2?\nX : %d\nY : %d\nZ : %d\nT : %d", is_2(x), is_2(y), is_2(z), is_2(t));
	return 0;
}
