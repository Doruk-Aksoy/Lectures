#include <stdio.h>

int find_bits(int x) {
	int res = 1;
	while(!((x >>= 1) & 1))
		++res;
	return res;
}

unsigned int flipBits (unsigned int n, int k) {
    unsigned int mask = (1 << k) - 1;
    return ~n & mask;
}

int main() {
	int x = 8; // some power of 2
	int numbits = find_bits(x); // find bits till 1.
	printf("Number of bits till 1 in x: %d\n", numbits);
	printf("Bitwise inverse: %d\n", ~x);
	
	unsigned int y = flipBits(x, numbits);
		
	printf("Inverse with respect to 3 bits: %d", y);
	
	return 0;
}
