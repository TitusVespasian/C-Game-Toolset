/*2053628 大数据 许芷瑜*/
#include<iostream>
#include <cstdio>
#include <conio.h>
#include<iomanip>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"90-01-b2-mine_sweeper.h"
#endif
using namespace std;
void basiceins(char bomb[20][34], int hang, int lie)
{
	int i, j = 0;
	cout << "  |1 2 3 4 5 6 7 8 9 ";
	for (j = 0; j <= lie - 10; j++)
		cout << (char)('a' + j) << ' ';
	cout << endl;
	cout << "--+--------------------";
	for (j = 0; j <= lie - 10; j++)
		cout << "--";
	cout << endl;
	for (i = 0; i <= hang - 1; i++)
	{
		cout << (char)('A' + i) << " |";
		for (j = 0; j <= lie - 1; j++)
		{
			cout << bomb[i][j] << ' ';
		}
		cout << endl;
	}
	return;
}
void basiczwei(int display[20][34],char bomb[20][34],int hang,int lie)
{
	int i, j = 0;
	cout << "  |1 2 3 4 5 6 7 8 9 ";
	for (j = 0; j <= lie - 10; j++)
		cout << (char)('a' + j) << ' ';
	cout << endl;
	cout << "--+--------------------";
	for (j = 0; j <= lie - 10; j++)
		cout << "--";
	cout << endl;
	for (i = 0; i <= hang - 1; i++)
	{
		cout << (char)('A' + i) << " |";
		for (j = 0; j <= lie - 1; j++)
		{
			if (display[i][j] == 0)
				cout << 'X' << ' ';
			else if(display[i][j]==1)
			{
				if (bomb[i][j] != '*')
					cct_setcolor(COLOR_HYELLOW, bomb[i][j] - '0');
				cout << bomb[i][j];
				cct_setcolor(COLOR_BLACK, COLOR_WHITE);
				cout<< ' ';
			}
			else if (display[i][j] == 2)
			{
				cct_setcolor(COLOR_RED, COLOR_WHITE);
				cout << 'X';
				cct_setcolor(COLOR_BLACK, COLOR_WHITE);
				cout << ' ';
			}
		}
		cout << endl;
	}
	cout << endl;
	/*for (i = 0; i <= hang - 1; i++)
	{
		cout << (char)('A' + i) << " |";
		for (j = 0; j <= lie - 1; j++)
		{
			cout << display[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;*/
	return;
}
void print_square(char bomb_ch, int sitex, int sitey)
{
	int color;
	if (bomb_ch == '9')
		color = COLOR_YELLOW;
	else if (bomb_ch == '#')
		color = COLOR_RED;
	else
		color = COLOR_WHITE;
	cct_showstr(6 * sitex + 4, 3 * sitey + 3, "    ", color, COLOR_BLACK);
	cct_showstr(6 * sitex + 4, 3 * sitey + 4, " ", color, COLOR_BLACK);
	if (bomb_ch > '0' && bomb_ch < '9')
		cct_showch(6 * sitex + 5, 3 * sitey + 4, bomb_ch, color, bomb_ch - '0');
	else if (bomb_ch == '*')
		cct_showch(6 * sitex + 5, 3 * sitey + 4, bomb_ch, color, COLOR_BLACK);
	else if (bomb_ch == '0')
		cct_showch(6 * sitex + 5, 3 * sitey + 4, ' ', color, COLOR_BLACK);
	else if (bomb_ch == '#')
		cct_showch(6 * sitex + 5, 3 * sitey + 4, '#', COLOR_RED, COLOR_WHITE);
	if (bomb_ch == '9')
		cct_showch(6 * sitex + 5, 3 * sitey + 4, ' ', COLOR_YELLOW, COLOR_BLACK);
	cct_showstr(6 * sitex + 6, 3 * sitey + 4, "  ", color, COLOR_BLACK);
}
void paint_board(int display[20][34],char bomb[20][34],int hang,int lie,int border_col,int border_line)
{
	cct_setfontsize("点阵字体", 16, 8);
	cct_setconsoleborder(border_col, border_line);
	cout << endl;
	int i, j;
	/*输出边框*/
	cct_gotoxy(2, 3);
	for (i = 0; i <= hang - 1; i++)
	{
		cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
		for (j = 0; j <= lie - 1; j++)
			cout << "║"<< "    ";
		cout << "║" << endl;
		cct_setcolor();
		cout << "  ";
		cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
		for (j = 0; j <= lie -1; j++)
			cout << "║" << "    ";
		cout << "║" << endl;
		cct_setcolor();
		cout << "  ";
		cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
		cout << "╠";
		for (j = 0; j <= lie-2; j++)
			cout<< "══╬";
		cout << "══╣" << endl;
		cct_setcolor();
		cout<< "  ";
	}
	for (j = 0; j <= lie - 1; j++)
	{
		for (i = 0; i <= hang - 1; i++)
		{
			if (display[i][j] == 1)
				print_square(bomb[i][j], j, i);
			else if (display[i][j] == 2)
				print_square('#', j, i);
			else if (display[i][j] == 0)
				print_square('9', j, i);
		}
	}
	/*输出边框字符*/
	cct_setcolor();
	cct_gotoxy(0, 1);
	for (i = 0; i <= lie - 1; i++)
	{
		cout << setw(6) << i;
	}
	cct_gotoxy(0, 0);
	for (i = 0; i <= hang - 1; i++)
	{
		cout << endl << endl << endl << (char)('A' + i);
	}
	cct_setcolor(COLOR_HWHITE, COLOR_BLACK);
	cct_gotoxy(2, 2);
	cout << "╔";
	cct_showstr(4, 2, "══╦", COLOR_HWHITE, COLOR_BLACK, lie - 1);
	cout << "══╗" << endl;
	cct_gotoxy(2, 3 *hang+2);
	cout << "╚";
	cct_showstr(4, 3*hang+2, "══╩", COLOR_HWHITE, COLOR_BLACK, lie - 1);
	cout << "══╝" << endl;
	cct_setcolor(COLOR_BLACK, COLOR_WHITE);
	cct_gotoxy(0, 30);
	return ;
}
void paint_for89(int display[20][34], char bomb[20][34], int hang, int lie)
{
	int i, j;
	for (j = 0; j <= lie - 1; j++)
	{
		for (i = 0; i <= hang - 1; i++)
		{
			if (display[i][j] == 2)
				print_square('#', j, i);
			if (display[i][j] == 0)
				print_square('9', j, i);
			if (display[i][j] == 1)
				print_square(bomb[i][j], j, i);
		}
	}
	cct_setcolor();
}
