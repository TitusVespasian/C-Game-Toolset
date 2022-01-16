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

/*��ȡһ�������*/
int get_next_num(const bool new_seed, const unsigned int seed)
{
	if (new_seed)
		srand(seed);

	return rand() % 10;
}
/*����ָ�����ֻ�ȡһ��present_char����
* @param present_char ����
* @param num ����
* @param direction ����
*/
void get_present_char(int present_char[5][5], int num, int direction)
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			present_char[i][j] = arr_of_charaters[num][direction][i][j];
		}
}


/*���������ʾ
* @param score �����ܺ�
* @param all_times ���д���
* @param M ���
* @fin �Ƿ���Ϸ��ֹ
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

/*��ȡͣ��ʱ��
* @param all_score �ɼ�
*/
int get_pause(int all_score)
{
	if (all_score >= 120)
		return 300;
	else
		return(1000 - 100 * (all_score / 15));
}