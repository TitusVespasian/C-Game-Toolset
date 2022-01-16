#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"90-02-b1-gmw-tetris.h"
#include"../include/cmd_gmw_tools.h"
#endif
using namespace std;


/*��ָ��λ�û���һ��С����
* @param x ���ϽǺ�����
* @param y ���Ͻ�������
*/
static void paint_square(int x, int y)
{
	cct_showstr(x, y + 0, "�X�T�[", COLOR_HYELLOW, COLOR_HBLUE);
	cct_showstr(x, y + 1, "�U��U", COLOR_HYELLOW, COLOR_HBLUE);
	cct_showstr(x, y + 2, "�^�T�a", COLOR_HYELLOW, COLOR_HBLUE);
}
/*��ָ��λ�ò���һ��С����
* @param x ���ϽǺ�����
* @param y ���Ͻ�������
*/
static void erase_square(int x, int y)
{
	cct_setcolor();
	cct_showstr(x, y + 0, "      ");
	cct_showstr(x, y + 1, "      ");
	cct_showstr(x, y + 2, "      ");
}

/*
* ������һ������
* @param block ���ֺͷ���
*/
void show_next_char(CONSOLE_GRAPHICS_INFO* pCGI, falling block)
{
	int temp[5][5] = {}, clean_up[5][5];
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			clean_up[i][j] = 1;
	get_present_char(temp, block.num, block.direction);
	for (int j = 0; j < 5; j++)
		for (int i = 0; i < 5; i++)
			if (clean_up[j][i] == 1)
				erase_square(i * square_width + 4, j * square_height + 6 * square_height);
	for (int j = 0; j < 5; j++)
		for (int i = 0; i < 5; i++)
			if (temp[j][i] == 1)
				paint_square(i * square_width + 4, j * square_height + 6 * square_height);
}