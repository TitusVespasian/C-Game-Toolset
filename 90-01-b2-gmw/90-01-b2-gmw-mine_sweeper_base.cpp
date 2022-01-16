#include<iostream>
#include<ctime>
#include<cmath>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"  
#include"../include/cmd_gmw_tools.h"
#include"../include/cmd_input.h"  
#include"90-01-b2-gmw-mine_sweeper.h"
#endif
using namespace std;

/***************************************************************************
  函数名称：
  功    能：输入坐标产生内部数组
  输入参数：
  返 回 值：
  说    明：hang是行数，hang对应y,lie对应x,bomb[hang][lie],hang<=9,lie<=25
***************************************************************************/
void generatebomb(char bomb[20][34], const int hang, const int lie, const int bomb_num, const int inputy, const int inputx)
{
	srand((unsigned)time(NULL));
	int flag = 1, j, i, sitex[110] = {}, sitey[110] = {};
	for (i = 1; i <= bomb_num; i++)
	{
		sitey[i - 1] = rand() % hang;
		sitex[i - 1] = rand() % lie;
		for (j = i - 2; j >= 0; j--)
		{
			if ((sitex[i - 1] == sitex[j] && sitey[i - 1] == sitey[j]) || (abs(sitex[i - 1] - inputx) <= 1 && abs(sitey[i - 1] - inputy) <= 1))
			{
				flag = 0;
				break;
			}
			else
				flag = 1;
		}
		if (flag)
			bomb[sitey[i - 1]][sitex[i - 1]] = '*';
		else
			i--;
	}
	return;
}

/*将内部数组bomb值设好*/
void inarray(char bomb[20][34], int hang, int lie, int bomb_num)
{
	int i, j;
	for (i = 0; i <= hang - 1; i++)
	{
		for (j = 0; j <= lie - 1; j++)
		{
			if (bomb[i][j] != '*')
			{
				if (i - 1 >= 0)
					if (bomb[i - 1][j] == '*')//up
						bomb[i][j]++;
				if (i + 1 <= hang - 1)
					if (bomb[i + 1][j] == '*')//down
						bomb[i][j]++;
				if (j - 1 >= 0)
					if (bomb[i][j - 1] == '*')//left
						bomb[i][j]++;
				if (j + 1 <= lie - 1)
					if (bomb[i][j + 1] == '*')//right
						bomb[i][j]++;
				if (i - 1 >= 0 && j - 1 >= 0)
					if (bomb[i - 1][j - 1] == '*')//leftup
						bomb[i][j]++;
				if (i - 1 >= 0 && j + 1 <= lie - 1)
					if (bomb[i - 1][j + 1] == '*')//rightup
						bomb[i][j]++;
				if (i + 1 <= hang - 1 && j - 1 >= 0)
					if (bomb[i + 1][j - 1] == '*')//leftdown
						bomb[i][j]++;
				if (i + 1 <= hang - 1 && j + 1 <= lie - 1)
					if (bomb[i + 1][j + 1] == '*')//rightdown
						bomb[i][j]++;
			}
		}
	}
}

/***************************************************************************
  函数名称：
  功    能：对内部数组进行打开扩展到
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void BFS(char bomb[20][34], int display[20][34], int hang, int lie, int y, int x)
{
	int row[8] = { 1,1,1,0,0,-1,-1,-1 };
	int col[8] = { 1,-1,0,1,-1,0,1,-1 };
	int Next = 0;
	if (display[y][x] != DISPLAY_MARK)
	{
		display[y][x] = DISPLAY_ON;
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
						display[y + row[Next]][x + col[Next]] = DISPLAY_ON;
						if (bomb[y + row[Next]][x + col[Next]] == '0')
						{
							display[y + row[Next]][x + col[Next]] = DISPLAY_ON;
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


