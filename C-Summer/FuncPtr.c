#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int strcmp_opp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
    	++s1;
		++s2;
	}
	// in normal strcmp, this is s1 - s2. We wanted the opposite.
    return *(const unsigned char *)s2 - *(const unsigned char *)s1;
}

int strcmp_capital(const char* s1, const char* s2) {
	int s1cnt = 0, s2cnt = 0;
    while(*s1 && *s2) {
    	if(isupper(*s1))
    		++s1cnt;
    	if(isupper(*s2))
    		++s2cnt;
    	++s1;
    	++s2;
	}
	// > 0 if s1 has more capitals, == 0 if they have same, < 0 if s1 has less.
	return (s1cnt - s2cnt);
}

const char* findmin(const char* s1, const char* s2, int (*cmpfnc) (const char*, const char*)) {
	if(cmpfnc(s1, s2) >= 0)
		return s1;
	return s2;
}

int main() {
	char s1[] = { "Hello" };
	char s2[] = { "raBiD" };
	
	int compres1 = strcmp_opp(s1, s2);
	int compres2 = strcmp_capital(s1, s2);
	
	printf("Comparison 1st function: %d\nComparison 2nd function: %d\n\n", compres1, compres2);
	
	int (*cmpfnc) (const char*, const char*);
	cmpfnc = &strcmp_opp;
	printf("Comparison 1st function: %s\n", findmin(s1, s2, cmpfnc));
	cmpfnc = &strcmp_capital;
	printf("Comparison 2nd function: %s\n", findmin(s1, s2, cmpfnc));
	return 0;
}
