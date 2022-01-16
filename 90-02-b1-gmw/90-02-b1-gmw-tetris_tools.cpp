#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<cstring>
#include<windows.h>
#include<conio.h>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include "../include/cmd_console_tools.h"
#include"90-02-b1-gmw-tetris.h"
#include"../include/cmd_input.h"
#include"../include/cmd_gmw_tools.h"
#endif
using namespace std;

/*获取一个随机数*/
int get_next_num(const bool new_seed, const unsigned int seed)
{
	if (new_seed)
		srand(seed);

	return rand() % 10;
}
/*跟据指定数字获取一个present_char数组
* @param present_char 数组
* @param num 数字
* @param direction 方向
*/
void get_present_char(int present_char[5][5], int num, int direction)
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			present_char[i][j] = arr_of_charaters[num][direction][i][j];
		}
}


/*输出文字提示
* @param score 分数总和
* @param all_times 所有次数
* @param M 宽度
* @fin 是否游戏中止
*/
void show_score(CONSOLE_GRAPHICS_INFO* pCGI, int score, int all_times, int M, int fin)
{
	cct_setcolor();
	char temp[256] = {};
	sprintf(temp, "Your score : %d Total rows vanquished : %d", score, all_times);
	gmw_status_line(pCGI, LOWER_STATUS_LINE, temp);
	if (fin == T_ACCEPT)
		gmw_status_line(pCGI, TOP_STATUS_LINE, "Need a break? Press \'p\' OR Wanna quit? Press\'q\'");
	else if (fin == T_FAILED)
		gmw_status_line(pCGI, TOP_STATUS_LINE, "Bravo! Game over! Press\'q\' to quit");
	else if (fin == T_QUIT)
		gmw_status_line(pCGI, TOP_STATUS_LINE, "See you! Press any key to return.");
}

/*获取停顿时间
* @param all_score 成绩
*/
int get_pause(int all_score)
{
	if (all_score >= 120)
		return 300;
	else
		return(1000 - 100 * (all_score / 15));
}