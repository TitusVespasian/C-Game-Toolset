/*2053628 ������ �����*/
#include<iostream>
#include <cstdio>
#include <conio.h>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"90-01-b2-mine_sweeper.h"
#endif
using namespace std;
void BFS(char bomb[20][34],int display[20][34],int hang,int lie,int y,int x)
{
	int row[8] = { 1,1,1,0,0,-1,-1,-1 };
	int col[8] = { 1,-1,0,1,-1,0,1,-1 };
	int Next=0;
	if (display[y][x] != 2)
	{
		display[y][x] = 1;
		if (bomb[y][x] == '0')
		{
			for (Next = 0; Next <= 7; Next++)
			{
				if (y + row[Next] >= 0 && y + row[Next] <= hang - 1 && x + col[Next] >= 0 && x + col[Next] <= lie - 1)
				{
					/*cout << y + row[Next] << '=';
					cout << x + col[Next] << endl;*/
					if (bomb[y + row[Next]][x + col[Next]] != '*' && display[y + row[Next]][x + col[Next]] == 0)
					{
						display[y + row[Next]][x + col[Next]] = 1;
						if (bomb[y + row[Next]][x + col[Next]] == '0')
						{
							display[y + row[Next]][x + col[Next]] = 1;
							BFS(bomb, display, hang, lie, y + row[Next], x + col[Next]);
						}
						else
							continue;
					}
				}
				else
					continue;
			}
		}
	}
	return;
}
void mark(int control,int special,int display[20][34],int inputy,int inputx,int bomb_num)
{
	if (control == 4)
	{
		if (special == '!' && display[inputy][inputx] == 0)
			display[inputy][inputx] = 2;
		else if (special == '#' && display[inputy][inputx] == 2)
			display[inputy][inputx] = 0;
	}
	else if (control >= 8)
	{
		if (special == '!' && display[inputy][inputx] == 0)
		{
			display[inputy][inputx] = 2;
		}
		else if (special == '!' && display[inputy][inputx] == 2)
			display[inputy][inputx] = 0;
	}
	if (control == 9&& special == '!')
	{
		cct_gotoxy(0, 0);
		cct_setcolor(COLOR_BLACK, COLOR_HYELLOW);
		for (int i = 0; i <= 19; i++)
			for (int j = 0; j <= 33; j++)
			{
				if (display[i][j] == 2)
					bomb_num--;
			}
		if (inputx >= 0 && inputy >= 0)
		{
			cout << "                                             ";
			cct_gotoxy(0, 0);
			cout << "ʣ��������" << bomb_num;
			cct_setcolor();
			cout << "ESC�˳����ո���ʾʱ��";
		}
	}
	return;
}
/*y��Ӧlie,x��Ӧhang*/
int mouse_input(int& inputy, int& inputx,int hang, int lie, int control)
{
	int MY, MX, Mmove=0,key1=0,key2=0,ret;
	cct_enable_mouse();
	ret=cct_read_keyboard_and_mouse(MX, MY, Mmove,key1,key2);
	if (ret == CCT_MOUSE_EVENT)
	{
		if (MX % 6 != 2 && MX % 6 != 3 && MX >= 2 && MX <= 6 * lie + 3)
		{
			inputx = (MX - 2) / 6;
		}
		else
			inputx = -1;
		if (MY % 3 != 2 && MY >= 2 && MY <= 3 * hang + 2)
		{
			inputy = MY / 3 - 1;
		}
		else
			inputy = -1;
		if (Mmove == MOUSE_LEFT_BUTTON_CLICK)
			return 'q';
		else if (Mmove == MOUSE_RIGHT_BUTTON_CLICK)
			return '!';
	}
	else if (ret == CCT_KEYBOARD_EVENT)
	{
		inputx= TEMP_NO_MOVE;
		inputy = TEMP_NO_MOVE;
		if (key1 == VK_ESCAPE)
			return VK_ESCAPE;
		else if (key1 == ' ' && control == 9)
			return ' ';
	}
	//cct_gotoxy(11, 3 * hang + 3);
	//cout<<key
	return 0;
}
void transfer_input(int input,char axe)
{
	if (axe == 'y')
		cout << (char)('A' + input);
	else if (axe == 'x' )
		cout << input;
	return ;
}
/*ѡ���Ѷ�*/
int menueins()
{
	int aeins;
	cout << "��ѡ���Ѷȣ�" << endl;
	cout << "1.����(9 * 9 - 10����)" << endl;
	cout << "2.�м�(16 * 16 - 40����)" << endl;
	cout << "3.�߼�(16 * 30 - 99����)" << endl;
	cout << "������[1..3]��" << endl;
	while (1)
	{
		aeins = _getch();
		if (aeins == '1' || aeins == '2' || aeins == '3')
		{
			break;
		}
		else
			continue;
	}
	cct_cls();
	return (aeins - 48);
}
/*ѡ������*/
void inputxy(int& inputy, int& inputx, int hang, int lie, int control, int cinyyn)
{
	if (control != 4)
		cout << "�������λ�õ��������꣨���к��У��ϸ����ִ�Сд������G1/Af����Q/q�˳�����";
	while (cinyyn == 1)
	{
		inputy = _getch();
		if (control != 4)
			if ((inputy == 'q') || (inputy == 'Q'))
			{
				putchar(inputy);
				inputx = 0;
				return;
			}
		if ((inputy >= 'A' && inputy <= 'A' + hang - 1))
		{
			putchar(inputy);
			inputy = inputy - 'A';
			break;
		}
	}
	while (1)
	{
		inputx = _getch();
		if (inputx >= '1' && inputx <= '9')
		{
			putchar(inputx);
			inputx = inputx - '1';
			break;
		}
		else if (inputx >= 'a' && inputx <= lie - 10 + 'a')
		{
			putchar(inputx);
			inputx = inputx - 'a' + 9;
			break;
		}
	}
	cout << endl << endl;
	return;
}
int inputxy_extra(int& inputy, int& inputx, int hang, int lie, long start)
{
	cout << "��������˵����& - ��Ϸ������ʱ��(���ַ����ɣ�����Ҫ������)" << endl;
	cout << "              !- ��Ǹ�����Ϊ��(����!E3)" << endl;
	cout << "              # - ȡ�����     (����#E3)" << endl;
	cout << "�������λ�õ��������꣨���к��У��ϸ����ִ�Сд������G1/Af����Q/q�˳�����";
	int special = 0;
	while (1)
	{
		inputy = _getch();
		if ((inputy == 'q') || (inputy == 'Q'))
		{
			putchar('q');
			return 0;
		}
		if ((inputy >= 'A' && inputy <= 'A' + hang - 1))
		{
			putchar(inputy);
			special = 0;//�൱��ֱ�Ӱ����
			inputy = inputy - 'A';
			break;
		}
		else if (inputy == '!' || inputy == '#' || inputy == '&')
		{
			putchar(inputy);
			special = inputy;
			inputy = 0;
			break;
		}
	}
	if (special == '&')//�ո�9
	{
		cout << endl << "������ʱ�䣺" << (clock() - (double)start) / CLOCKS_PER_SEC << "��" << endl;
		return special;
	}
	else if (special == '#' || special == '!')//�Ҽ�
	{
		inputxy(inputy, inputx, hang, lie, 4, 1);
		return special;
	}
	else
	{
		inputxy(inputy, inputx, hang, lie, 4, 0);
		return special;
	}
}
void showmouse(int display[20][34], int control, int& special, int hang, int lie, int& inputy, int& inputx, long start)
{
	while (control == 6 || control == 7 || control == 8 || control == 9)
	{
		special = mouse_input(inputy, inputx, hang, lie, control);
		//cct_gotoxy(0, 3 * hang + 4);
		//cout << special;
		cct_gotoxy(0, 3 * hang + 3);
		cout << "[��ǰ���] ";
		if (inputx == -1 || inputy == -1)
		{
			cct_gotoxy(11, 3 * hang + 3);
			cout << "λ�÷Ƿ�";
		}
		else if (inputx == TEMP_NO_MOVE || inputy == TEMP_NO_MOVE)
			;
		else
		{
			cout << "                        ";
			cct_gotoxy(11, 3 * hang + 3);
			transfer_input(inputy, 'y');
			cout << "��";
			transfer_input(inputx, 'x');
			cout << "��";
		}
		if (control == 9 && special == ' ')
		{
			cct_gotoxy(0, 0);
			cct_setcolor(COLOR_BLACK, COLOR_HYELLOW);
			cout << "                              ";
			cct_gotoxy(0, 0);
			cout << "��ǰʱ�䣺" << (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC << "�룬";
			cout << "ESC�˳����ո���ʾʱ��";
			cct_setcolor();
		}
		if (inputx >= 0 && inputy >= 0 && special == 'q' && control < 8)
			break;
		else if (inputx >= 0 && inputy >= 0 && special == 'q' && display[inputy][inputx] != 2 && control >= 8)
			break;
		else if ((special == '!' || special == VK_ESCAPE) && control >= 8)
			break;
	}
	return;
}
