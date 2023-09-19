#include "revert_string.h"

void RevertString(char *str)
{
	int length = 0;
	while (*(str + length) != '\0')
	{
		length++;
	}

	char* reverted = (char*) malloc(length * sizeof(char));

	int i = 0;
	for (char* p = str + length - 1; p >= str; p--, i++)
	{
		*(reverted + i) = *p;
	}
	for (int i = 0; i < length; i++)
	{
		str[i] = reverted[i];
	}
}
