#include<iostream>
#include <Windows.h>
#include"../include/cmd_hdc_tools.h"
using namespace std;
static const double PI = 3.1415926;
static const int t_len = 50;
static void xzy_lines(const int point_x, const int point_y, const int a, const int thickness, const int RGB_value)
{
	double deta_theta = (double(thickness) / 100) / 180 * PI;
	for (double theta = 0; theta <2*PI; theta += deta_theta)
	{
		int x =int( a *(1 + 3*cos(3*theta) + 3*sin(3*theta)* sin(3 * theta))*cos(theta));
		int y = int(a * (1 + 3 * cos(3 * theta) + 3 * sin(3 * theta) * sin(3 * theta)) * sin(theta));
		hdc_point(point_x + x, point_y + y, thickness, RGB_value);
	}
}

void hdc_draw_Mathematical_curve_2053628()
{
 	hdc_cls();
	hdc_set_pencolor(0);
	cout << "这是3条三叶草曲线" << endl;
	xzy_lines(350, 400, 30, 5, RGB(75, 205, 45));
	xzy_lines(350, 400, 50, 5, RGB(75, 220, 250));
	xzy_lines(350, 400, 65, 5, RGB(250, 245, 5));
}