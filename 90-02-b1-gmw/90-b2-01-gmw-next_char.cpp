#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"90-02-b1-gmw-tetris.h"
#include"../include/cmd_gmw_tools.h"
#endif
using namespace std;


/*在指定位置画出一个小方格
* @param x 左上角横坐标
* @param y 左上角纵坐标
*/
static void paint_square(int x, int y)
{
	cct_showstr(x, y + 0, "╔═╗", COLOR_HYELLOW, COLOR_HBLUE);
	cct_showstr(x, y + 1, "║★║", COLOR_HYELLOW, COLOR_HBLUE);
	cct_showstr(x, y + 2, "╚═╝", COLOR_HYELLOW, COLOR_HBLUE);
}
/*在指定位置擦除一个小方格
* @param x 左上角横坐标
* @param y 左上角纵坐标
*/
static void erase_square(int x, int y)
{
	cct_setcolor();
	cct_showstr(x, y + 0, "      ");
	cct_showstr(x, y + 1, "      ");
	cct_showstr(x, y + 2, "      ");
}

/*
* 画出下一个数字
* @param block 数字和方向
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