#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include "../include/cmd_hdc_tools.h"
using namespace std;

#if !HDC_SERIES_BY_TEACHER	//δ���������²Ŵ���������

/* ���������Ҫ��ͷ�ļ� */
#include<cmath>
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow(); //VS�д˴����в����ߣ���Ӱ������ʹ��

/* �Ѷ���ľ�̬ȫ�ֱ��� */
static HWND hWnd = GetConsoleWindow();
static HDC hdc = NULL;
static const double PI = 3.14159;
static int _BgColor_, _FgColor_, _Width_, _High;
static int _Delay_ms;

/* �����ʶ�ʹ�þ�̬ȫ�ֱ�����������static�����ⲿ���ɼ� */
struct position {
	int x;
	int y;
};
/* �˴���������Զ��庯����������static�����ⲿ���ɼ� */
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

/*ʹ�����鰴����������*/
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
/*�Ƚ�С����ǰ��*/
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

/*����û�е��ô˺������Խ�x����������Ϊ0�ȣ���ʱ��Ϊ��Ϊ��*/
static int convert_sys_angle(int angle_degree)
{
	return -angle_degree;
}

/* ��������˼�����������������ʵ�֣���Ҫ�Ķ� */
/***************************************************************************
  �������ƣ�
  ��    �ܣ���ʼ��
  ���������const int bgcolor������ɫ
			const int fgcolor��ǰ��ɫ
			const int width  ����Ļ��ȣ�����
			const int high   ����Ļ�߶ȣ�����
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_init(const int bgcolor, const int fgcolor, const int width, const int high)
{
	/* ���ͷţ���ֹ��release���ٴ�init��hdc_release�����룩 */
	hdc_release();

	/* ����init����һ����̬ȫ������¼������hdc_cls()���õ� */
	_BgColor_ = bgcolor;
	_FgColor_ = fgcolor;
	_Width_ = width;
	_High = high;

	hdc = GetDC(hWnd);

	cct_setcursor(CURSOR_INVISIBLE);
	cct_setcolor(bgcolor, fgcolor);
	cct_setfontsize("��������", 16);
	cct_setconsoleborder(width / 8 + !!(width % 8), high / 16 + !!(high % 16)); //������Ŀ�ȼ��߶�ת��Ϊ�ض��������������!!�ĺ��壺�������8/16�ı��������ж�+1
	cct_cls();
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ��ͷŻ�ͼ��Դ
  ���������
  �� �� ֵ��
  ˵    ����������
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
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_set_delay(int ms)
{
	if (ms > 0)
		_Delay_ms = ms;
	else
		_Delay_ms = 0;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ����û�����ɫ������RGBֵ
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_set_pencolor(const int RGB_value)
{
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, RGB_value);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ����û�����ɫ������RGB��ɫ��ֵ0-255
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_set_pencolor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
	hdc_set_pencolor(RGB(red, green, blue));
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������Ļ�����е�ͼ��
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_cls()
{
	/* ����һ��Ҫ��һ����ɫ��ʼ������������ص㣬�ҵ�����㷽����ͬѧ����֪ͨ�� */
	hdc_init(_BgColor_, (_FgColor_ + 1) % 16, _Width_, _High);

	hdc_init(_BgColor_, _FgColor_, _Width_, _High);

	/* ���ֻ���������demoʱ�������hdc_cls()���һ������ȱ�ߵ����������ʱ����
	   ������ֻ������л������⣬���ߴ���ʱֵ */
	Sleep(30);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���(x,y)λ�ô���ָ����ɫ����һ�����ص�
  ���������const int x��x���꣬���Ͻ�Ϊ(0,0)
			const int y��y���꣬���Ͻ�Ϊ(0,0)
  �� �� ֵ��
  ˵    ������ɫֱ���õ�ǰ�趨�������ڵ��ô�ָ��
***************************************************************************/
static inline void hdc_base_point(const int x, const int y)
{
	MoveToEx(hdc, x - 1, y - 1, NULL);
	LineTo(hdc, x, y);
	if (_Delay_ms > 0)
		Sleep(_Delay_ms);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���(x1,y1)-(x2,y2)֮�仭��һ�����ص������
  ���������const int x1�����x���꣬���Ͻ�Ϊ(0,0)
			const int y1�����y���꣬���Ͻ�Ϊ(0,0)
			const int x2���յ�y���꣬���Ͻ�Ϊ(0,0)
			const int y2���յ�y���꣬���Ͻ�Ϊ(0,0)
  �� �� ֵ��
  ˵    ������ɫֱ���õ�ǰ�趨
***************************************************************************/
static inline void hdc_base_line(const int x1, const int y1, const int x2, const int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
	if (_Delay_ms > 0)
		Sleep(_Delay_ms);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���(x,y)λ�ô���ָ����ɫ��һ��ָ����ϸ�ĵ�(�û�ʵ��Բ��ģ��)
  ���������const int x			��x���꣬���Ͻ�Ϊ(0,0)
			const int y			��y���꣬���Ͻ�Ϊ(0,0)
			const int thickness	����Ĵ�ϸ������λ1�����޲���(�����ᵼ�²���ȫ���)����ȱʡֵ��
			const int RGB_value	�������ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    �����Ľ�����Ч��
***************************************************************************/
void hdc_point(const int x, const int y, const int thickness, const int RGB_value)
{
	int tn = thickness;
	if (tn < 1)
		tn = 1;		//��������
	/* ������ֱ�������ޣ����ǵ����޳���һ����С�ǣ�������Բ����λ��δʵ�� */

	const int tn_end = (tn <= 1) ? 1 : tn / 2;

	int angle, level;
	int old_x1 = INT_MAX, old_y1 = INT_MAX, x1, y1;
	int count = 0;

	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);

	/* �û�Բ�ķ�ʽģ��ֵ� */
	for (level = 1; level <= tn_end; level++) {
		for (angle = 0; angle <= 360; angle++) {
			/* ע�⣬ϵͳ�������ᣬ0����Բ�����·� */
			x1 = x + (int)(level * sin(angle * PI / 180));
			y1 = y - (int)(level * cos(angle * PI / 180));

			/* ���뾶��Сʱ���Ƕȱ仯��������int�͵�x1/y1�仯����˼��ж���䣬�����ظ���ͬһ���ص� */
			if (x1 != old_x1 || y1 != old_y1) {
				old_x1 = x1;
				old_y1 = y1;
				hdc_base_point(x1, y1);
				++count;
			}
		}
	}
#if 0
	/* �ſ���ע�ͣ����Կ���һ���ֵ����˼������ص���� */
	cct_gotoxy(0, 41);
	printf("count=%d           ", count);
	getchar();
#endif
}

/* -------------------------------------------------------------------------
	�������к�����ʵ�֣���������������׼�Ķ�
	1������Ҫ����ϵͳ�������й�ͼ�β����ĺ���
	2�����к�����ʵ�ֶ�����hdc_base_point/hdc_base_line���������
	3����һ�룬�ǲ������к�������Ҫ����������ʵ�ֹ��̣�Ӧ����ʵ����Щ������
	4�������ʵ������ӵĲ���
	5��ϵͳ�ĽǶ��뺯�������еĽǶȺ������180��
   ------------------------------------------------------------------------- */

   /***************************************************************************
	 �������ƣ�
	 ��    �ܣ����߶�
	 ���������const int x1		������x
			   const int y1		������y
			   const int x2		���յ��x
			   const int y2		���յ��y
			   const int thickness	���߶εĴ�ϸ����ȱʡֵ��
			   const int RGB_value	���߶ε���ɫ����ȱʡֵ��
	 �� �� ֵ��
	 ˵    ���������������û���hdc_point����ϣ��ٶ�����
					 Ҳ������hdc_base_point/hdc_base_line����ϣ��ٶȿ죩
   ***************************************************************************/
void hdc_line(const int x1, const int y1, const int x2, const int y2, const int thickness, const int RGB_value)
{
	if (RGB_value != INVALID_RGB)
		hdc_set_pencolor(RGB_value);
	int tn = thickness;
	if (tn < 1)
		tn = 1;		//��������
	for (int i = -tn / 2 + 1 - (tn % 2 == 1); i <= tn / 2; i++)
		hdc_base_line(x1 + i, y1, x2 + i, y2);
	for (int i = -tn / 2 + 1 - (tn % 2 == 1); i <= tn / 2; i++)
		hdc_base_line(x1, y1 + i, x2, y2 + i);
}
/***************************************************************************
  �������ƣ�
  ��    �ܣ�������������꣬��һ��������
  ���������const int x1		����1�����x
			const int y1		����1�����y
			const int x2		����2�����x
			const int y2		����2�����y
			const int x3		����3�����x
			const int y3		����3�����y
			bool filled			���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness	���ߵĴ�ϸ����ȱʡֵ��
			const int RGB_value	����ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    �������ж������Ƿ��ߣ�������ߣ�����һ��ֱ�߼���
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
  �������ƣ�
  ��    �ܣ��������Ͻ����꼰��ȡ��߶ȣ�����һ��������
  ���������const int left_up_x			�����Ͻ�x
			const int left_up_y			�����Ͻ�y
			const int width				�����
			const int high				���߶�
			const int rotation_angles	�������Ͻ�Ϊ֧�㣬��x�����ת��ǣ�����תΪ����
			bool filled					���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness			���ߵĴ�ϸ����ȱʡֵ��
			const int RGB_value			����ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    ����
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
  �������ƣ�
  ��    �ܣ��������Ͻ����꼰�߳�������һ��������
  ���������const int left_up_x			�����Ͻ�x
			const int left_up_y			�����Ͻ�y
			const int length			���߳�
			const int rotation_angles	�������Ͻ�Ϊ֧�㣬��x�����ת��ǣ�����תΪ��������ȱʡֵ��
			const bool filled			���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness			���ߵĴ�ϸ����ȱʡֵ��
			const int RGB_value			����ɫ����ȱʡֵ��
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_square(const int left_up_x, const int left_up_y, const int length, const int rotation_angles, const bool filled, const int thickness, const int RGB_value)
{
	hdc_rectangle(left_up_x, left_up_y, length, length, rotation_angles, filled, thickness, RGB_value);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ����ո����Ĳ�����һ��Բ��
  ���������const int point_x		��Բ�ĵ�x
			const int point_y		��Բ�ĵ�y
			const int radius		���뾶
			const int angle_begin	����ʼ�Ƕȣ���Բ�����Ϸ�Ϊ0�㣬˳ʱ��Ϊ����ע�⣺��ϵͳ�������180��!!!������ȱʡֵ��
			const int angle_end		�������Ƕȣ�ͬ�ϣ�����ȱʡֵ��
			const int thickness		����ϸ����ȱʡֵ��
			const int RGB_value		����ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    ����
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
  �������ƣ�
  ��    �ܣ����ո����Ĳ�����һ������
  ���������const int point_x		��Բ�ĵ�x
			const int point_y		��Բ�ĵ�y
			const int radius		���뾶
			const int angle_begin	����ʼ�Ƕȣ���Բ�����Ϸ�Ϊ0�㣬˳ʱ��Ϊ����ע�⣺��ϵͳ�������180��!!!������ȱʡֵ��
			const int angle_end		�������Ƕȣ�ͬ�ϣ�����ȱʡֵ��
			const bool filled		���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness		����ϸ����ȱʡֵ��
			const int RGB_value		����ɫ����ȱʡֵ��
  ���������
  �� �� ֵ��
  ˵    ��������ʼ/�����ǶȲ�ֵΪ360�ı���ʱ������������
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

	//����ס�ľ���
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
			//���ǶȺ�Բ�� (point_x,0) (i,j)
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
  �������ƣ�
  ��    �ܣ����ո����Ĳ�����һ��Բ
  ���������const int point_x		��Բ�ĵ�x
			const int point_y		��Բ�ĵ�y
			const int radius		���뾶
			const bool filled		���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness		����ϸ����ȱʡֵ��
			const int RGB_value		����ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void hdc_circle(const int point_x, const int point_y, const int radius, const bool filled, const int thickness, const int RGB_value)
{
	hdc_sector(point_x, point_y, radius, 0, 0, filled, thickness, RGB_value);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ����ո����Ĳ�����һ����Բ
  ���������const int point_x			��Բ�ĵ�x
			const int point_y			��Բ�ĵ�y
			const int radius_a			��ƽ����X��İ뾶
			const int radius_b			��ƽ����Y��İ뾶
			const int rotation_angles	����Բ��Ϊ֧�㣬��x�����ת��ǣ�����תΪ��������ȱʡֵ��
			const bool filled			���Ƿ���Ҫ��䣨��ȱʡֵ��
			const int thickness			����ϸ����ȱʡֵ��
			const int RGB_value			����ɫ����ȱʡֵ��
  �� �� ֵ��
  ˵    ����
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
