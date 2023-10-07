#include "revert_string.h"

void RevertString(char *str)
{
	int n = 0;
	while (str[n] != '\0') {
		++n;
	}
	for (int i = 0; i < n/2; ++i) {
		char temp = str[i];
		str[i] = str[n-1-i];
		str[n-1-i] = temp;
	}
}
