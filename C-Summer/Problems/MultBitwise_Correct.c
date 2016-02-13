#include <stdio.h>

// Hardware logic (the last bit is sign bit, so carry will stop at sign bit and report it accordingly)
int myAdd(unsigned int a, unsigned int b) {
    unsigned int carry = a & b;
    unsigned int result = a ^ b;
    while(carry) {
        unsigned int shiftedcarry = carry << 1;
        carry = result & shiftedcarry;
        result ^= shiftedcarry;
    }
    return result;
}

int myMult(int a, int b) {
	int res = 0;
	while(b) {
		if (b & 1)
			res = myAdd(res, a);
		a <<= 1;
		b = (unsigned) b >> 1; // <---- THIS LINE FIXES B BEING NEGATIVE IN INFINITE LOOP
	}
}

int main() {
	int a, b;
	printf("Enter two numbers (space seperated): ");
	scanf("%d %d", &a, &b);
	printf("Multiplication = %d", myMult(a, b));
	return 0;
}
