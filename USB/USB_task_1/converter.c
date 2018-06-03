
#include "converter.h"
#include <stdio.h>
#include <ctype.h>

char *str_conv(char *st)
{
	int i = 0;
	char ch;

	//puts("Conversions:");
	while(st[i] != '\0')
	{
		if(isupper(st[i]))
		{
			ch = tolower(st[i]);
			//printf("Converted to uppercase: [%c]\n", ch);
			st[i] = ch;
		}
		else if(islower(st[i]))
		{
			ch = toupper(st[i]);
			//printf("Converted to lowercase: [%c]\n", ch);
			st[i] = ch;
		}
		else
		{
			ch = st[i];
			//printf("No conversion needed: 	[%c]\n", ch);
		}
		i++;
	}
	return st;
}
