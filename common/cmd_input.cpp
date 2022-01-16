#include<iostream>
#include <cstdio>
#include <conio.h>
#include<ctime>
#include<cstring>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#endif
using namespace std;
/*—°‘Ò–°Ã‚*/
int menu(const char* output, int cmd_num, const char type[]) 
{
	int a = 0;
	cout << output;
	while (1)
	{
		a = _getch();
		if (a <= '0' + cmd_num && a >= '0')
		{
			if (strcmp(type, "hanoi") == 0)
			{
				putchar(a);
				cout << endl << endl << endl;
			}
			break;
		}
		else
			continue;
	}
	if (a == '0')
		return 0;
	if (strcmp(type, "mine_sweep") == 0)
		cct_cls();
	return (a - 48);
}

