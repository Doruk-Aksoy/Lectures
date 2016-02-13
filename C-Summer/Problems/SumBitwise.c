#include <stdio.h>

// Hardware logic (the last bit is sign bit, so carry will stop at sign bit and report it accordingly)
int myAdd(unsigned int a, unsigned int b) {
    unsigned int carry = a & b;
    unsigned int result = a ^ b;
    while(carry != 0) {
        unsigned int shiftedcarry = carry << 1;
        carry = result & shiftedcarry;
        result ^= shiftedcarry;
    }
    return result;
}

int main() {
	int a, b;
	printf("Enter two numbers (space seperated): ");
	scanf("%d %d", &a, &b);
	printf("Sum = %d", myAdd(a, b));
	return 0;
}
