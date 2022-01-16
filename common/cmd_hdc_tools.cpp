#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include "../include/cmd_hdc_tools.h"
using namespace std;

#if !HDC_SERIES_BY_TEACHER	//未定义的情况下才打开条件编译

/* 允许添加需要的头文件 */
#include<cmath>
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow(); //VS中此处会有波浪线，不影响正常使用

/* 已定义的静态全局变量 */
static HWND hWnd = GetConsoleWindow();
static HDC hdc = NULL;
static const double PI = 3.14159;
static int _BgColor_, _FgColor_, _Width_, _High;
static int _Delay_ms;

/* 允许适度使用静态全局变量，但仅限static，即外部不可见 */
struct position {
	int x;
	int y;
};
/* 此处允许添加自定义函数，但仅限static，即外部不可见 */
static double get_k(int xa, int ya, int xb, int yb)
{
	if (xa != xb)
		return (double(ya) - double(yb)) / (double(xa) - double(xb));
	else
		return 1e31;
}

static double get_b(int xa, int ya, int xb, int yb)
{
	if (xa != xb)
		return (double(ya) * xb - double(yb) * xa) / (double(xb) - double(xa));
	else
		return -1e31;
}

/*使得数组按照升序排序*/
static void sort_by(position pos[], int len, char axis)
{
	for (int i = 0; i < len; i++)
	{
		int min = i;
		for (int j = i + 1; j < len; j++)
		{
			if (axis == 'x')
				if (pos[j].x < pos[min].x)
					min = j;
			if (axis == 'y')
				if (pos[j].y < pos[min].y)
					min = j;
		}
		position temp;
		temp = pos[i];
		pos[i] = pos[min];
		pos[min] = temp;
	}
}
/*比较小的在前面*/
static void select_sort(int y[], int len)
{
	for (int i = 0; i < len; i++)
	{
		int min = i;
		for (int j = i + 1; j < len; j++)
		{
			if (y[j] < y[min])
				min = j;
		}
		int temp;
		temp = y[i];
		y[i] = y[min];
		y[min] = temp;
	}
}
template<typename T>
static inline double convert(T angle_degree)
{
	return (PI * angle_degree) / 180;
}

/*后续没有调用此函数，以将x轴正向代表改为0度，逆时针为正为例*/
static int convert_sys_angle(int angle_degree)
{
	return -angle_degree;
}

/* 下面给出了几个基本函数的完整实现，不要改动 */
/***************************************************************************
  函数名称：
  功    能：初始化
  输入参数：const int bgcolor：背景色
			const int fgcolor：前景色
			const int width  ：屏幕宽度（点阵）
			const int high   ：屏幕高度（点阵）
  返 回 值：
  说    明：
***************************************************************************/
void hdc_init(const int bgcolor, const int fgcolor, const int width, const int high)
{
	/* 先释放，防止不release而再次init（hdc_release可重入） */
	hdc_release();

	/* 窗口init后，用一个静态全局量记录，后续hdc_cls()会用到 */
	_BgColor_ = bgcolor;
	_FgColor_ = fgcolor;
	_Width_ = width;
	_High = high;

	hdc = GetDC(hWnd);

	cct_setcursor(CURSOR_INVISIBLE);
	cct_setcolor(bgcolor, fgcolor);
	cct_setfontsize("点阵字体", 16);
	cct_setconsoleborder(width / 8 + !!(width % 8), high / 16 + !!(high % 16)); //将点阵的宽度及高度转换为特定字体的行列数，!!的含义：如果不是8/16的倍数，行列多+1
	cct_cls();
}

/***************************************************************************
  函数名称：
  功    能：释放画图资源
  输入参数：
  返 回 值：
  说    明：可重入
***************************************************************************/
void hdc_release()
{
	if (hdc) {
		ReleaseDC(hWnd, hdc);
		hdc = NULL;
		cct_setcursor(CURSOR_VISIBLE_NORMAL);
	}
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void hdc_set_delay(int ms)
{
	if (ms > 0)
		_Delay_ms = ms;
	else
		_Delay_ms = 0;
}

/***************************************************************************
  函数名称：
  功    能：设置画笔颜色，传入RGB值
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void hdc_set_pencolor(const int RGB_value)
{
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, RGB_value);
}

/***************************************************************************
  函数名称：
  功    能：设置画笔颜色，传入RGB三色，值0-255
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void hdc_set_pencolor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
	hdc_set_pencolor(RGB(red, green, blue));
}

/***************************************************************************
  函数名称：
  功    能：清除屏幕上现有的图形
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void hdc_cls()
{
	/* 发现一定要换一种颜色初始化才能清除像素点，找到更简便方法的同学可以通知我 */
	hdc_init(_BgColor_, (_FgColor_ + 1) % 16, _Width_, _High);

	hdc_init(_BgColor_, _FgColor_, _Width_, _High);

	/* 部分机器上运行demo时，会出现hdc_cls()后第一根线有缺线的情况，加延时即可
	   如果部分机器运行还有问题，调高此延时值 */
	Sleep(30);
}

/***************************************************************************
  函数名称：
  功    能：在(x,y)位置处用指定颜色画出一个像素点
  输入参数：const int x：x坐标，左上角为(0,0)
			const int y：y坐标，左上角为(0,0)
  返 回 值：
  说    明：颜色直接用当前设定，可以在调用处指定
***************************************************************************/
static inline void hdc_base_point(const int x, const int y)
{
	MoveToEx(hdc, x - 1, y - 1, NULL);
	LineTo(hdc, x, y);
	if (_Delay_ms > 0)
		Sleep(_Delay_ms);
}

/***************************************************************************
  函数名称：
  功    能：在(x1,y1)-(x2,y2)之间画出一个像素点的连线
  输入参数：const int x1：起点x坐标，左上角为(0,0)
			const int y1：起点y坐标，左上角为(0,0)
			const int x2：终点y坐标，左上角为(0,0)
			const int y2：终点y坐标，左上角为(0,0)
  返 回 值：
  说    明：颜色直接用当前设定
***************************************************************************/
static inline void hdc_base_line(const int x1, const int y1, const int x2, const int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
	if (_Delay_ms > 0)
		Sleep(_Delay_ms);
}

/***************************************************************************
  函数名称：
  功    能：在(x,y)位置处用指定颜色画一个指定粗细的点(用画实心圆来模拟)
  输入参数：const int x			：x坐标，左上角为(0,0)
			const int y			：y坐标，左上角为(0,0)
			const int thickness	：点的粗细，下限位1，上限不限(如过大会导致不完全填充)（有缺省值）
			const int RGB_value	：点的颜色（有缺省值）
  返 回 值：
  说    明：改进运行效率
***************************************************************************/
void hdc_point(const int x, const int y, const int thickness, const int RGB_value)
{
	int tn = thickness;
	if (tn < 1)
		tn = 1;		//处理下限
	/* 不卡点直径的上限，但是单上限超过一定大小是，画出的圆部分位置未实心 */

	const int tn_end = (tn <= 1) ? 1 : tn / 2;

	int angle, level;
	int old_x1 = INT_MAX, old_y1 = INT_MAX, x1, y1;
	int count = 0;

	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);

	/* 用画圆的方式模拟粗点 */
	for (level = 1; level <= tn_end; level++) {
		for (angle = 0; angle <= 360; angle++) {
			/* 注意，系统的坐标轴，0°在圆的最下方 */
			x1 = x + (int)(level * sin(angle * PI / 180));
			y1 = y - (int)(level * cos(angle * PI / 180));

			/* 当半径很小时，角度变化不会引起int型的x1/y1变化，因此加判断语句，避免重复画同一像素点 */
			if (x1 != old_x1 || y1 != old_y1) {
				old_x1 = x1;
				old_y1 = y1;
				hdc_base_point(x1, y1);
				++count;
			}
		}
	}
#if 0
	/* 放开此注释，可以看到一个粗点用了几个像素点组成 */
	cct_gotoxy(0, 41);
	printf("count=%d           ", count);
	getchar();
#endif
}

/* -------------------------------------------------------------------------
	给出下列函数的实现，函数名及参数表不准改动
	1、不需要调用系统的其他有关图形操作的函数
	2、下列函数的实现都基于hdc_base_point/hdc_base_line函数的组合
	3、想一想，是不是所有函数都需要给出独立的实现过程？应该先实现哪些函数？
	4、填充是实现中最复杂的部分
	5、系统的角度与函数参数中的角度含义相差180°
   ------------------------------------------------------------------------- */

   /***************************************************************************
	 函数名称：
	 功    能：画线段
	 输入参数：const int x1		：起点的x
			   const int y1		：起点的y
			   const int x2		：终点的x
			   const int y2		：终点的y
			   const int thickness	：线段的粗细（有缺省值）
			   const int RGB_value	：线段的颜色（有缺省值）
	 返 回 值：
	 说    明：本函数可以用基于hdc_point的组合（速度慢）
					 也可以用hdc_base_point/hdc_base_line的组合（速度快）
   ***************************************************************************/
void hdc_line(const int x1, const int y1, const int x2, const int y2, const int thickness, const int RGB_value)
{
	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);
	int tn = thickness;
	if (tn < 1)
		tn = 1;		//处理下限
	for (int i = -tn / 2 + 1 - (tn % 2 == 1); i <= tn / 2; i++)
		hdc_base_line(x1 + i, y1, x2 + i, y2);
	for (int i = -tn / 2 + 1 - (tn % 2 == 1); i <= tn / 2; i++)
		hdc_base_line(x1, y1 + i, x2, y2 + i);
}
/***************************************************************************
  函数名称：
  功    能：给出三点的坐标，画一个三角形
  输入参数：const int x1		：第1个点的x
			const int y1		：第1个点的y
			const int x2		：第2个点的x
			const int y2		：第2个点的y
			const int x3		：第3个点的x
			const int y3		：第3个点的y
			bool filled			：是否需要填充（有缺省值）
			const int thickness	：边的粗细（有缺省值）
			const int RGB_value	：颜色（有缺省值）
  返 回 值：
  说    明：不判断三点是否共线，如果共线，划出一条直线即可
***************************************************************************/
void hdc_triangle(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3, bool filled, const int thickness, const int RGB_value)
{
	hdc_line(x1, y1, x2, y2, thickness, RGB_value);
	hdc_line(x1, y1, x3, y3, thickness, RGB_value);
	hdc_line(x2, y2, x3, y3, thickness, RGB_value);

	position pos[5];
	pos[0].x = x1;
	pos[1].x = x2;
	pos[2].x = x3;

	pos[0].y = y1;
	pos[1].y = y2;
	pos[2].y = y3;

	sort_by(pos, 3, 'x');

	double k0 = get_k(pos[0].x, pos[0].y, pos[1].x, pos[1].y);
	double b0 = get_b(pos[0].x, pos[0].y, pos[1].x, pos[1].y);

	double k1 = get_k(pos[1].x, pos[1].y, pos[2].x, pos[2].y);
	double b1 = get_b(pos[1].x, pos[1].y, pos[2].x, pos[2].y);

	double k2 = get_k(pos[2].x, pos[2].y, pos[0].x, pos[0].y);
	double b2 = get_b(pos[2].x, pos[2].y, pos[0].x, pos[0].y);

	int y_temp_up, y_temp_down;
	int x_temp;
	if (filled)
	{
		for (int i = 0; i <= pos[1].x - pos[0].x - (pos[0].x == pos[1].x); i++)
		{
			x_temp = pos[0].x + i;
			y_temp_up = (int)round(k2 * x_temp + b2);
			y_temp_down = (int)round(k0 * x_temp + b0);
			hdc_base_line(x_temp, y_temp_down, x_temp, y_temp_up);
		}
		for (int i = 0; i <= pos[2].x - pos[1].x - (pos[2].x == pos[1].x); i++)
		{
			x_temp = pos[1].x + i;
			y_temp_up = (int)round(k2 * x_temp + b2);
			y_temp_down = (int)round(k1 * x_temp + b1);
			hdc_base_line(x_temp, y_temp_down, x_temp, y_temp_up);
		}
	}
}

/***************************************************************************
  函数名称：
  功    能：给出左上角坐标及宽度、高度，画出一个长方形
  输入参数：const int left_up_x			：左上角x
			const int left_up_y			：左上角y
			const int width				：宽度
			const int high				：高度
			const int rotation_angles	：以左上角为支点，与x轴的旋转倾角（向下转为正）
			bool filled					：是否需要填充（有缺省值）
			const int thickness			：边的粗细（有缺省值）
			const int RGB_value			：颜色（有缺省值）
  返 回 值：
  说    明：
***************************************************************************/
void hdc_rectangle(const int left_up_x, const int left_up_y, const int width, const int high, const int rotation_angles, const bool filled, const int thickness, const int RGB_value)
{
	int tn = thickness;
	int wd = width;
	int hg = high;
	if (tn < 1)
		tn = 1;
	if (wd < 1)
		wd = 2;
	if (hg < 1)
		hg = 2;
	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);

	double degree_angle = convert(rotation_angles);
	int right_up_x = int(left_up_x + wd * cos(degree_angle));
	int right_up_y = int(left_up_y + wd * sin(degree_angle));

	int right_down_x = int(right_up_x - hg * sin(degree_angle));
	int right_down_y = int(right_up_y + hg * cos(degree_angle));

	int left_down_x = int(left_up_x - hg * sin(degree_angle));
	int left_down_y = int(left_up_y + hg * cos(degree_angle));

	hdc_line(left_up_x, left_up_y, right_up_x, right_up_y, thickness, RGB_value);
	hdc_line(right_up_x, right_up_y, right_down_x, right_down_y, thickness, RGB_value);
	hdc_line(right_down_x, right_down_y, left_down_x, left_down_y, thickness, RGB_value);
	hdc_line(left_down_x, left_down_y, left_up_x, left_up_y, thickness, RGB_value);
	if (filled)
	{
		position pos[8];
		pos[0].x = left_up_x;
		pos[0].y = left_up_y;

		pos[1].x = left_down_x;
		pos[1].y = left_down_y;

		pos[2].x = right_up_x;
		pos[2].y = right_up_y;

		pos[3].x = right_down_x;
		pos[3].y = right_down_y;

		sort_by(pos, 4, 'x');
		position pos_xmin = pos[0];
		position pos_xmax = pos[3];

		pos[0].x = left_up_x;
		pos[0].y = left_up_y;

		pos[1].x = left_down_x;
		pos[1].y = left_down_y;

		pos[2].x = right_up_x;
		pos[2].y = right_up_y;

		pos[3].x = right_down_x;
		pos[3].y = right_down_y;

		if (pos[0].x == pos[1].x)
		{
			int x_min = pos[0].x < pos[2].x ? pos[0].x : pos[2].x;
			for (int i = 0; i <= width; i++)
				hdc_base_line(x_min + i, pos[0].y, x_min + i, pos[1].y);
			return;
		}

		else if (pos[0].y == pos[1].y)
		{
			int x_min = pos[0].x < pos[1].x ? pos[0].x : pos[1].x;
			for (int i = 0; i <= high; i++)
				hdc_base_line(x_min + i, pos[0].y, x_min + i, pos[2].y);
			return;
		}

		else
		{
			for (int i = pos_xmin.x; i <= pos_xmax.x; i++)
			{
				int y[8];
				y[0] = int(get_k(pos[2].x, pos[2].y, pos[0].x, pos[0].y) * i + get_b(pos[2].x, pos[2].y, pos[0].x, pos[0].y));
				y[1] = int(get_k(pos[1].x, pos[1].y, pos[0].x, pos[0].y) * i + get_b(pos[1].x, pos[1].y, pos[0].x, pos[0].y));
				y[2] = int(get_k(pos[2].x, pos[2].y, pos[3].x, pos[3].y) * i + get_b(pos[2].x, pos[2].y, pos[3].x, pos[3].y));
				y[3] = int(get_k(pos[1].x, pos[1].y, pos[3].x, pos[3].y) * i + get_b(pos[1].x, pos[1].y, pos[3].x, pos[3].y));

				select_sort(y, 4);
				hdc_line(i, y[1], i, y[2], tn, RGB_value);
			}
		}
	}

}

/***************************************************************************
  函数名称：
  功    能：给出左上角坐标及边长，画出一个长方形
  输入参数：const int left_up_x			：左上角x
			const int left_up_y			：左上角y
			const int length			：边长
			const int rotation_angles	：以左上角为支点，与x轴的旋转倾角（向下转为正）（有缺省值）
			const bool filled			：是否需要填充（有缺省值）
			const int thickness			：边的粗细（有缺省值）
			const int RGB_value			：颜色（有缺省值）
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void hdc_square(const int left_up_x, const int left_up_y, const int length, const int rotation_angles, const bool filled, const int thickness, const int RGB_value)
{
	hdc_rectangle(left_up_x, left_up_y, length, length, rotation_angles, filled, thickness, RGB_value);
}

/***************************************************************************
  函数名称：
  功    能：按照给出的参数画一段圆弧
  输入参数：const int point_x		：圆心的x
			const int point_y		：圆心的y
			const int radius		：半径
			const int angle_begin	：起始角度（以圆的最上方为0°，顺时针为正，注意：与系统坐标差了180°!!!）（有缺省值）
			const int angle_end		：结束角度（同上）（有缺省值）
			const int thickness		：粗细（有缺省值）
			const int RGB_value		：颜色（有缺省值）
  返 回 值：
  说    明：
***************************************************************************/
void hdc_arc(const int point_x, const int point_y, const int radius, const int angle_begin, const int angle_end, const int thickness, const int RGB_value)
{
	int an_start = (angle_begin + 360) % 360;
	int an_end = (angle_end + 360) % 360;
	if (an_start == an_end)
	{
		an_start = 0;
		an_end = 360;
	}
	if (an_start > an_end)
		an_end += 360;
	int tn = thickness;
	if (tn < 1)
		tn = 1;
	hdc_point(int(point_x + radius * sin(convert(an_start))), int(point_y - radius * cos(convert(an_start))),
		tn, RGB_value);
	for (double i = an_start; i <= an_end - 1.0; i++)
		hdc_line(int(point_x + radius * sin(convert(i))), int(point_y - radius * cos(convert(i))),
			int(point_x + radius * sin(convert(i + 1))), int(point_y - radius * cos(convert(i + 1))),
			tn, RGB_value);
	if (an_end - an_start == 360)
		hdc_line(int(point_x + radius * sin(convert(359))), int(point_y - radius * cos(convert(359))),
			int(point_x + radius * sin(convert(361))), int(point_y - radius * cos(convert(361))),
			tn, RGB_value);
}

/***************************************************************************
  函数名称：
  功    能：按照给出的参数画一个扇形
  输入参数：const int point_x		：圆心的x
			const int point_y		：圆心的y
			const int radius		：半径
			const int angle_begin	：起始角度（以圆的最上方为0°，顺时针为正，注意：与系统坐标差了180°!!!）（有缺省值）
			const int angle_end		：结束角度（同上）（有缺省值）
			const bool filled		：是否需要填充（有缺省值）
			const int thickness		：粗细（有缺省值）
			const int RGB_value		：颜色（有缺省值）
  输入参数：
  返 回 值：
  说    明：当起始/结束角度差值为360的倍数时，不画两条边
***************************************************************************/
void hdc_sector(const int point_x, const int point_y, const int radius, const int angle_begin, const int angle_end, const bool filled, const int thickness, const int RGB_value)
{
	int draw_r = 1;
	int an_start = (angle_begin + 360) % 360;
	int an_end = (angle_end + 360) % 360;
	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);
	if (an_start == an_end)
	{
		an_start = 0;
		an_end = 360;
		draw_r = 0;
	}
	if (an_start > an_end)
		an_end += 360;
	int tn = thickness;
	if (tn < 1)
		tn = 1;

	//包裹住的矩形
	int x_max = point_x, y_max = point_y, x_min = point_x, y_min = point_y;
	for (int i = an_start; i < an_end; i++)
	{
		hdc_line(int(point_x + radius * sin(convert(i))), int(point_y - radius * cos(convert(i))),
			int(point_x + radius * sin(convert(i + 1))), int(point_y - radius * cos(convert(i + 1))),
			tn, RGB_value);
		/*if (int(point_x + radius * sin(convert(i))) > x_max)
			x_max = int(point_x + radius * sin(convert(i)));
		if (int(point_x + radius * sin(convert(i))) < x_min)
			x_min = int(point_x + radius * sin(convert(i)));
		if (int(point_y - radius * cos(convert(i))) < y_min)
			y_min = int(point_y - radius * cos(convert(i)));
		if (int(point_y - radius * cos(convert(i))) > y_max)
			y_max = int(point_y - radius * cos(convert(i)));*/
	}
	if (an_end - an_start == 360)
		hdc_line(int(point_x + radius * sin(convert(359))), int(point_y - radius * cos(convert(359))),
			int(point_x + radius * sin(convert(361))), int(point_y - radius * cos(convert(361))),
			tn, RGB_value);

	if (draw_r)
	{
		hdc_line(int(point_x + radius * sin(convert(an_start))), int(point_y - radius * cos(convert(an_start))),
			point_x, point_y,
			tn, RGB_value);
		hdc_line(point_x, point_y,
			int(point_x + radius * sin(convert(an_end))), int(point_y - radius * cos(convert(an_end))),
			tn, RGB_value);
	}

	if (!filled)
		return;
	for (double i = an_start; i <= an_end - 1.0; i += 0.8)
		hdc_triangle(point_x, point_y, int(point_x + radius * sin(convert(i))), int(point_y - radius * cos(convert(i))),
			int(point_x + radius * sin(convert(i + 1))), int(point_y - radius * cos(convert(i + 1))), filled,
			tn, RGB_value);
	if (an_end - an_start == 360)
		hdc_triangle(point_x, point_y, int(point_x + radius * sin(convert(359))), int(point_y - radius * cos(convert(359))),
			int(point_x + radius * sin(convert(361))), int(point_y - radius * cos(convert(361))), filled,
			tn, RGB_value);
	/*hdc_line(x_max, y_max, x_min, y_max, 5, RGB_value);
	hdc_line(x_max, y_min, x_min, y_min, 5, RGB_value);
	hdc_line(x_min, y_min, x_min, y_max, 5, RGB_value);
	hdc_line(x_max, y_min, x_max, y_max, 5, RGB_value);
	for (int i = x_min; i <= x_max; i++)
	{
		int start_y = -1, end_y = -1;
		for (int j = y_min; j <= y_max; j++)
		{
			double angle_j;

			//(point_x,point_y-b)
			int b = 50;
			//(i,j)
			double a_2 = (i - point_x) * (i - point_x) + (j - point_y) * (j - point_y);
			int a = sqrt((i - point_x) * (i - point_x) + (j - point_y) * (j - point_y));
			//(point_x,point_y-b) (i,j)
			int c_2 = (i - point_x) * (i - point_x) + (j - point_y + b) * (j - point_y + b);
			if (i > point_x)
			{
				angle_j = acos((a_2 + b * b - c_2) / (2 * a * b));
				//cout << "(a_2 + b * b - c_2) / (2 * a * b)" << (a_2 + b * b - c_2) / (2 * a * b) << endl;
			}
			else if (i == point_x && j > point_y)
				angle_j = PI;
			else if (i == point_x && j <= point_y)
				angle_j = 0;
			else if (i < point_x)
				angle_j = 2 * PI - acos((a_2 + b * b - c_2) / (2 * a * b));
			/*cct_gotoxy(1, 5);
			cout << angle_j << endl;
			cout << "convert(an_start)" << convert(an_start) << endl;
			cout << "convert(an_end)" << convert(an_end) << endl;
			cout << "a_2*" << a_2 << " b*" << b << "*c_2*" << c_2 << "*" << endl;
			cout << "(" << i-x_min << ", " << j-y_min << ")" << endl;
			cout << (i - point_x) * (i - point_x) + (j - point_y) * (j - point_y) << endl;
			cout << radius * radius << endl;*/
			//检查角度和圆周 (point_x,0) (i,j)
			/*if (angle_j >= convert(an_start)
				&& ((i - point_x) * (i - point_x) + (j - point_y) * (j - point_y) <= radius * radius))
			{
				if(j< start_y||start_y<0)
					start_y = j;
			}
			if ((angle_j <= convert(an_end))
				&& ((i - point_x) * (i - point_x) + (j - point_y) * (j - point_y) <= radius * radius))
			{
				if (j > end_y || end_y < 0)
					end_y = j;
			}
		}*/
		//cout << "start_y" << start_y << "end_y" << end_y << endl;
		/*if (start_y != -1 && end_y != -1)
			hdc_base_line(i, start_y, i, end_y);}*/


}

/***************************************************************************
  函数名称：
  功    能：按照给出的参数画一个圆
  输入参数：const int point_x		：圆心的x
			const int point_y		：圆心的y
			const int radius		：半径
			const bool filled		：是否需要填充（有缺省值）
			const int thickness		：粗细（有缺省值）
			const int RGB_value		：颜色（有缺省值）
  返 回 值：
  说    明：
***************************************************************************/
void hdc_circle(const int point_x, const int point_y, const int radius, const bool filled, const int thickness, const int RGB_value)
{
	hdc_sector(point_x, point_y, radius, 0, 0, filled, thickness, RGB_value);
}

/***************************************************************************
  函数名称：
  功    能：按照给出的参数画一个椭圆
  输入参数：const int point_x			：圆心的x
			const int point_y			：圆心的y
			const int radius_a			：平行于X轴的半径
			const int radius_b			：平行于Y轴的半径
			const int rotation_angles	：以圆心为支点，与x轴的旋转倾角（向下转为正）（有缺省值）
			const bool filled			：是否需要填充（有缺省值）
			const int thickness			：粗细（有缺省值）
			const int RGB_value			：颜色（有缺省值）
  返 回 值：
  说    明：
***************************************************************************/
void hdc_ellipse(const int point_x, const int point_y, const int radius_a, const int radius_b, const int rotation_angles, const bool filled, const int thickness, const int RGB_value)
{
	int tn = thickness;
	if (tn < 1)
		tn = 1;
	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);
	//int ra = rotation_angles;
	int ra = convert_sys_angle(rotation_angles);
	int x_max = point_x, y_max = point_y, x_min = point_x, y_min = point_y;
	for (int i = 0; i <= 360; i+=1)
	{
		double a_cos = radius_a * cos(convert(i));
		double b_sin = radius_b * sin(convert(i));

		double a_cos_next = radius_a * cos(convert(i + 1));
		double b_sin_next = radius_b * sin(convert(i + 1));
		hdc_line(point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra))),
			point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra))),
			point_x + int(a_cos_next * cos(convert(ra)) - b_sin_next * sin(convert(ra))),
			point_y - int(a_cos_next * sin(convert(ra)) + b_sin_next * cos(convert(ra))),
			tn, RGB_value);
		if (point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra))) > x_max)
			x_max = point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra)));
		if (point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra))) < x_min)
			x_min = point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra)));
		if (point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra))) > y_max)
			y_max = point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra)));
		if (point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra))) < y_min)
			y_min = point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra)));
	}
	double arc_ra = convert(ra);

	if (!filled)
		return;
	for (int i = 0; i <= 360; i++)
	{
		double a_cos = radius_a * cos(convert(i));
		double b_sin = radius_b * sin(convert(i));

		double a_cos_next = radius_a * cos(convert(i + 1));
		double b_sin_next = radius_b * sin(convert(i + 1));
		hdc_triangle(point_x,point_y,
			point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra))),
			point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra))),
			point_x + int(a_cos_next * cos(convert(ra)) - b_sin_next * sin(convert(ra))),
			point_y - int(a_cos_next * sin(convert(ra)) + b_sin_next * cos(convert(ra))),
			filled,
			tn, RGB_value);
	}
	/*hdc_triangle(point_x, point_y, point_x + int(a_cos * cos(convert(ra)) - b_sin * sin(convert(ra))),
		point_y - int(a_cos * sin(convert(ra)) + b_sin * cos(convert(ra))),
		point_x + int(a_cos_next * cos(convert(ra)) - b_sin_next * sin(convert(ra))),
		point_y - int(a_cos_next * sin(convert(ra)) + b_sin_next * cos(convert(ra))),
		tn, RGB_value);*/
	/*hdc_line(x_max, y_max, x_min, y_max, 5, RGB_value);
	hdc_line(x_max, y_min, x_min, y_min, 5, RGB_value);
	hdc_line(x_min, y_min, x_min, y_max, 5, RGB_value);
	hdc_line(x_max, y_min, x_max, y_max, 5, RGB_value);*/
	/*for (int i = x_min; i <= x_max; i++)
	{
		bool init = true;
		double start_y, end_y;
		for (int j = y_min; j <= y_max ; j++)
		{
			double org_x, org_y;
			org_x = cos(arc_ra) * (double(i) - point_x) + sin(arc_ra) * (point_y-double(j));
			org_y = (-sin(arc_ra)) * (double(i) - point_x) + cos(arc_ra) * (point_y-double(j));
			if ((org_x * org_x / (double(radius_a) * radius_a) + org_y * org_y / (double(radius_b) * radius_b)) <= 1)
			{
				if (init|| org_y <= start_y)
					start_y = org_y;
				if (init|| org_y >= end_y)
					end_y = org_y;
				init = false;
			}
		}
		if (!init)
		{
			start_y = point_y + start_y;
			end_y = point_y + end_y;
			hdc_base_line(i, start_y, i, end_y);
		}
	}*/
}

#endif !HDC_SERIES_BY_TEACHER
