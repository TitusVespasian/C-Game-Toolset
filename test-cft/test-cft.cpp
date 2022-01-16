#if 1
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <conio.h>
using namespace std;

#include "../include/cfg_file_tools.h"
#define read_mode 1
#define write_good 2
#define write_rpt 3
#define WINDOWS 4
#define LINUX 5
#define FILE_SIMPLE 6
#define FILE_NORMAL 7
const bool teacher_test = false;
const bool xzy_test = true;
// 本测试文件的开关。


union cfg_value {
	int ivalue;
	double dvalue;
	char svalue[128];
	char cvalue;
	void* value;
};

struct cfg_item {
	const char* cfg_item_name;
	enum ITEM_TYPE cfg_item_type;
	union cfg_value cfg_item_value;
};

/***************************************************************************
  函数名称：
  功    能：显示提示信息
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
static void to_be_continued(const char* prompt)
{
	if (prompt)
		cout << prompt << "，按回车键继续...";
	else
		cout << "按回车键继续...";

	/* 忽略除回车键外的所有输入（注意：_getch的回车是\r，而getchar是\n）*/
	while (_getch() != '\r')
		;
	cout << endl;

	return;
}

/***************************************************************************
  函数名称：
  功    能：因为cfg_item中含有共用体，共用体只能初始化第一项，因此用函数初始化
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
static void set_item(struct cfg_item* item)
{
	item[0].cfg_item_value.ivalue = 123;
	item[1].cfg_item_value.dvalue = 123.45;
	strcpy(item[2].cfg_item_value.svalue, "Hello");
	item[3].cfg_item_value.cvalue = 'Y';
	//	item[4].cfg_item_value.ivalue = 0;
	item[5].cfg_item_value.ivalue = 99999;
	strcpy(item[6].cfg_item_value.svalue, "Hello<String>");
	//	item[7].cfg_item_value.ivalue = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int test_readconfig_cpp(const char* cfgname, const char** group_name, struct cfg_item* item)
{
	CFT fcfg;
	int ret, i;
	char tmp[256];

	/* 第一次模拟，添加若干组 */
	if (1) {
		remove(cfgname); //删除文件

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << cfgname << ")." << endl;
			return -1;
		}
		for (i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "增加[" << group_name[i] << "]失败." << endl;
			else
				cout << "增加[" << group_name[i] << "]成功." << endl;
		}
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否仅有8个空组", cfgname);
		to_be_continued(tmp);
	}

	/* 第二次模拟，每组添加若干项 */
	if (1) {
		int j;

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << cfgname << ")." << endl;
			return -1;
		}

		for (i = 0; group_name[i]; i++) {
			for (j = 0; item[j].cfg_item_name; j++) {
				sprintf(tmp, "%s-%s", group_name[i], item[j].cfg_item_name);
				/* 说明：item_add的第3个参数是void*，因此不管什么类型，均传入union第一个成员的地址，即其它成员地址 */
				switch (item[j].cfg_item_type) {
				case ITEM_TYPE::Int:
					ret = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.ivalue);
					break;
				case ITEM_TYPE::Double:
					ret = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.dvalue);
					break;
				case ITEM_TYPE::String:
					if (j == 2)
						ret = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.svalue);
					else
						ret = fcfg.item_add(group_name[i], tmp, string(item[j].cfg_item_value.svalue));
					break;
				case ITEM_TYPE::Character:
					ret = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.cvalue);
					break;
				case ITEM_TYPE::Null:
					ret = fcfg.item_add(group_name[i], tmp);
					break;
				default:
					break;
				}
				cout << "增加[" << group_name[i] << "]/" << tmp << (ret > 0 ? "成功." : "失败.") << endl;
			}
		}
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否8个组均有6个配置项", cfgname);
		to_be_continued(tmp);
	}

	/* 第三次模拟，删除组及项 */
	if (1) {
		sprintf(tmp, "请打开配置文件%s，将[配置组3]复制若干份，配置组2-设置项B复制若干份，且后面的值类型不同", cfgname);
		to_be_continued(tmp);

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << cfgname << ")." << endl;
			return -1;
		}

		cout << "删除[配置组3]共" << fcfg.group_del("配置组3") << "组" << endl;
		cout << "删除[配置组5]共" << fcfg.group_del("配置组5") << "组" << endl;

		cout << "删除<配置组2-设置项B>共" << fcfg.item_del("配置组2", "配置组2-设置项B") << "项" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察[配置组3]/[配置组5]/<配置组2-设置项B>是否已被删除", cfgname);
		to_be_continued(tmp);
	}

	/* 第四次模拟，修改项 */
	if (1) {
		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << cfgname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* 已存在项，更新 */
		fcfg.item_update("配置组4", "配置组4-设置项B", a);
		fcfg.item_update("配置组1", "配置组1-设置项D", "helloworld");
		fcfg.item_update("配置组8", "配置组8-设置项E", b);
		fcfg.item_update("配置组6", "配置组6-设置项F");
		fcfg.item_update("配置组7", "配置组7-设置项C", c);
		fcfg.item_update("配置组2", "配置组2-设置项D", s);

		/* 不存在项，新增 */
		fcfg.item_update("配置组6", "配置组6-设置项U");
		fcfg.item_update("配置组7", "配置组7-设置项V", c);
		fcfg.item_update("配置组7", "配置组6-设置项W");
		fcfg.item_update("配置组4", "配置组4-设置项X", a);
		fcfg.item_update("配置组1", "配置组1-设置项Y", "helloworld");
		fcfg.item_update("配置组8", "配置组8-设置项Z", b);
		fcfg.item_update("配置组2", "配置组2-设置项Q", s);
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置是否已修改", cfgname);
		to_be_continued(tmp);
	}

	/* 第五次模拟，读 */
	if (1) {
		fcfg.open(cfgname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << cfgname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* 已存在项，更新 */
		if (fcfg.item_get_value("配置组4", "配置组4-设置项B", a) > 0)
			cout << "[配置组4]/配置组4-设置项B=" << a << endl;
		if (fcfg.item_get_value("配置组1", "配置组1-设置项F", a) > 0)
			cout << "[配置组1]/配置组1-设置项F=" << a << endl;
		if (fcfg.item_get_value("配置组6", "配置组6-设置项D", c) > 0)
			cout << "[配置组6]/配置组6-设置项D=" << c << endl;
		if (fcfg.item_get_value("配置组8", "配置组8-设置项C", s) > 0)
			cout << "[配置组8]/配置组8-设置项C=" << c << endl;
		if (fcfg.item_get_value("配置组2", "配置组2-设置项Q", ss) > 0)
			cout << "[配置组2]/配置组2-设置项Q=" << ss << endl;
		if (fcfg.item_get_value("配置组7", "配置组7-设置项E") > 0)
			cout << "[配置组7]/配置组7-设置项E=<NULL>" << endl;
		if (fcfg.item_get_value("配置组6", "配置组6-设置项B", b) > 0)
			cout << "[配置组6]/配置组6-设置项B=" <<b<< endl;
		if (fcfg.item_get_value("配置组1", "设置项100") > 0)
			cout << "[配置组1]/设置项100=<NULL>" << endl;
		else
			cout << "[配置组1]/设置项100 不存在" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置读取是否正确", cfgname);
		to_be_continued(tmp);
	}

	return 0;
}


void before_simple_file(const char* xzyname, const int mode_file)
{
	string end_of_line;
	if (mode_file == WINDOWS)
		end_of_line = "\r\n";
	else
		end_of_line = "\n";
	ofstream fout;
	fout.open(xzyname, ios::out | ios::binary);
	fout << "Transfiguration=\t\tO;#$@^" << end_of_line;
	fout << "\tDADA\t=Outstanding" << end_of_line;
	fout << "\t\t  Charms=120  #spark" << end_of_line;
	fout << "asdjnjkbqwdui  wbcujewi" << end_of_line;
	fout << "sadnkawndk\t  \tjwndjc" << end_of_line;
	fout.close();
}



void before_normal_file(const char* xzyname,const int mode_file)
{
	string end_of_line;
	if (mode_file == WINDOWS)
		end_of_line = "\r\n";
	else
		end_of_line = "\n";
	ofstream fout;
	fout.open(xzyname, ios::out | ios::binary);
	fout << "[ 配置组1    \t \t ]" << end_of_line;
	fout << " 配置组1-设置项G  \t \t=   Hello<String>#Regulus" << end_of_line;
	fout << "    [ 配置组2    \t \t ]  ;Sirius" << end_of_line;
	fout << "配置组7-设置项F=\t99999" << end_of_line;
	fout << "    [ 配置组3    \t \t ]  Sirius" << end_of_line;
	fout << "[配置组   4]" << end_of_line;
	fout << "asdjnjkbqwdui  wbcujewi" << end_of_line;
	fout << "sadnkawndkjwndjc" << end_of_line;
	fout.close();
}
int check_add_key_value_simple(const char* xzyname, const char* group_name[],CFT& fcfg, const int mode)
{
	char tmp[256];
	if (1) {
		if (mode == read_mode)
		{
			fcfg.open(xzyname, OPEN_OPT_RDWR);
			cout << "1.写入项目，“读入模式”下错误添加，以下应当全为失败" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDONLY);
		}
		if (mode == write_good)
		{
			cout << "2.写入项目，“写模式”下正常添加，以下应当是前六条成功，后三条失败" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}
		if (mode == write_rpt)
		{
			cout << "3.写入项目，“写模式”下重复添加，以下应当全为失败" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}

		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}
		cout << "增加[" << "NULL" << "]/" << "Astronomy" << (fcfg.item_add(NULL, "Astronomy") > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "Muggle Study" << (fcfg.item_add(NULL, "Muggle Study",123.72) > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "care of magical ceatures" << (fcfg.item_add(NULL, "care of magical ceatures",'O') > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "Herbology" << (fcfg.item_add(NULL, "Herbology", string("Outstanding")) > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "Potions" << (fcfg.item_add(NULL, "Potions", "Outstanding") > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "History" << (fcfg.item_add(NULL, "History", 120) > 0 ? "成功." : "失败.") << endl;

		
		cout << "增加[" << "NULL" << "]/" << "DADA" << (fcfg.item_add(NULL, "DADA") > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "Transfiguration" << (fcfg.item_add(NULL, "Transfiguration", 100) > 0 ? "成功." : "失败.") << endl;
		cout << "增加[" << "NULL" << "]/" << "Charms" << (fcfg.item_add(NULL, "Charms", 100.0F) > 0 ? "成功." : "失败.") << endl;


		}
		fcfg.close(); //关闭文件，模拟本次设置完成
		if (mode != read_mode)
			sprintf(tmp, "请打开配置文件%s，观察是否8个组均有6个配置项", xzyname);
		else
			sprintf(tmp, "请打开配置文件%s，观察是否8个组均有0个配置项", xzyname);
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;

		to_be_continued(tmp);
		return 0;
}
int check_add_key_value(const char* xzyname, const char* group_name[], struct cfg_item item[],CFT &fcfg,const int mode)
{
	char tmp[256];
	if (1) {
		int ret_sum=0;
		int j;
		if (mode == read_mode)
		{
			fcfg.open(xzyname, OPEN_OPT_RDWR);
			cout << "3.写入项目，“读入模式”下错误添加，以下应当全为失败" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDONLY);
		}
		if (mode == write_good)
		{
			cout << "4.写入项目，“写模式”下正常添加，以下应当\"配置1-设置项G\"失败，其余成功" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}
		if (mode == write_rpt)
		{
			cout << "5.写入项目，“写模式”下重复添加，以下应当全为失败" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}

		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		for (int i = 0; group_name[i]; i++) {
			for (j = 0; item[j].cfg_item_name; j++) {
				sprintf(tmp, "%s-%s", group_name[i], item[j].cfg_item_name);
				/* 说明：item_add的第3个参数是void*，因此不管什么类型，均传入union第一个成员的地址，即其它成员地址 */
				switch (item[j].cfg_item_type) {
				case ITEM_TYPE::Int:
					ret_sum = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.ivalue);
					break;
				case ITEM_TYPE::Double:
					ret_sum = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.dvalue);
					break;
				case ITEM_TYPE::String:
					if (j == 2)
						ret_sum = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.svalue);
					else
						ret_sum = fcfg.item_add(group_name[i], tmp, string(item[j].cfg_item_value.svalue));
					break;
				case ITEM_TYPE::Character:
					ret_sum = fcfg.item_add(group_name[i], tmp, item[j].cfg_item_value.cvalue);
					break;
				case ITEM_TYPE::Null:
					ret_sum = fcfg.item_add(group_name[i], tmp);
					break;
				default:
					break;
				}
				cout << "增加[" << group_name[i] << "]/" << tmp << (ret_sum > 0 ? "成功." : "失败.") << endl;
			}
		}
		fcfg.close(); //关闭文件，模拟本次设置完成
		if(mode!=read_mode)
			sprintf(tmp, "请打开配置文件%s，观察是否8个组均有6个配置项", xzyname);
		else
			sprintf(tmp, "请打开配置文件%s，观察是否8个组均有0个配置项", xzyname);
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;

		to_be_continued(tmp);
	}
	return 0;
}

int test_simple_file(const char* xzyname, const char* group_name[], struct cfg_item item[], const int mode_file)
{
	remove(xzyname);
	before_simple_file(xzyname, mode_file);
	CFT fcfg;
	char tmp[256];

	//测试写入设置项
	if (1)
	{
		//测试写入，成功
		check_add_key_value_simple(xzyname, group_name,fcfg, read_mode);
		check_add_key_value_simple(xzyname, group_name,fcfg, write_good);
		check_add_key_value_simple(xzyname, group_name,fcfg, write_rpt);

		cout << endl << "6.下面试图写入不存在的组中,应当为失败" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		int ret = 0;
		cout << "增加[" << "配置组9" << "]/" << "配置组9-设置项Z" << (ret > 0 ? "成功." : "失败.") << endl;
		fcfg.close();
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;
	}

	/* 第三次模拟，删除组及项 */
	if (1) {
		sprintf(tmp, "7.请打开配置文件%s，将\"Tranfiguration\"复制3份，且后面的值类型不同", xzyname);
		to_be_continued(tmp);

		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		cout << "应当为1组，实际" << "删除\"Potions\"共" << fcfg.item_del(NULL,"Potions") << "组" << endl;
		cout << "应当为3组，实际" << "删除\"Transfiguration\"共" << fcfg.item_del(NULL, "Transfiguration") << "组" << endl;
		cout << "应当为0项，实际" << "删除\"DADA\"共" << fcfg.item_del("配置组2", "DADA") << "项" << endl;
		cout << "应当为0项，实际" << "删除[配置组100]共" << fcfg.group_del("配置组100") << "组" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察[配置组3]/[配置组5]/<配置组2-设置项B>是否已被删除", xzyname);
		to_be_continued(tmp);
	}

	/* 第四次模拟，修改项 */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDWR);  
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* 已存在项，更新 */
		cout << "更新[" << "NULL" << "]/" << "Astronomy" << (fcfg.item_update(NULL, "Astronomy",123.72) > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "Muggle Study" << (fcfg.item_update(NULL, "Muggle Study", 'O') > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "care of magical ceatures" << (fcfg.item_update(NULL, "care of magical ceatures") > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "Herbology" << (fcfg.item_update(NULL, "Herbology", 120) > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "Charms" << (fcfg.item_update(NULL, "Charms", string("Outstanding")) > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "History" << (fcfg.item_update(NULL, "History", "Outstanding") > 0 ? "成功." : "失败.") << endl;

		/* 不存在项，新增 */
		cout << "更新[" << "NULL" << "]/" << "Potions" << (fcfg.item_update(NULL, "Potions") > 0 ? "成功." : "失败.") << endl;
		cout << "更新[" << "NULL" << "]/" << "Transifiguration" << (fcfg.item_update(NULL, "Transifiguration", 100) > 0 ? "成功." : "失败.") << endl;
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置是否已修改", xzyname);
		to_be_continued(tmp);
	}

	/* 第五次模拟，读 */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* 已存在项，更新 */
		if (fcfg.item_get_value(NULL, "Astronomy", b) > 0)
			cout << "Astronomy=" << b << endl;
		if (fcfg.item_get_value(NULL, "Muggle Study", c) > 0)
			cout << "Muggle Study=" << c << endl;
		if (fcfg.item_get_value(NULL, "Herbology", a) > 0)
			cout << "Herbology=" << a << endl;
		if (fcfg.item_get_value(NULL, "care of magical ceatures", s) > 0)
			cout << "care of magical ceatures=" << s << endl;
		if (fcfg.item_get_value(NULL, "Charms", ss) > 0)
			cout << "Charms=" << ss << endl;
		if (fcfg.item_get_value(NULL, "Potions") > 0)
			cout << "Potions=<NULL>" << endl;
		if (fcfg.item_get_value(NULL, "flying") > 0)
			cout << "flying=<NULL>" << endl;
		else
			cout << "flying 不存在" << endl;
		if (fcfg.item_get_value("Hogwarts", "flying") > 0)
			cout << "[Hogwarts]/flying=<NULL>" << endl;
		else
			cout << "[Hogwarts]/flying 不存在" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成
		cout << endl;
		cout << "结果应当为\n"
			"Astronomy=123.72\n"
			"Muggle Study = O\n"
			"Herbology = 120\n"
			"care of magical ceatures =\n"
			"Charms = Outstanding\n"
			"Potions = <NULL>\n"
			"flying 不存在\n"
			"[Hogwarts] / flying 不存在" << endl << endl;
		
		to_be_continued(tmp);
	}
	return 0;
}

int test_normal_file(const char* xzyname, const char* group_name[], struct cfg_item item[],const int mode_file)
{
	remove(xzyname);
	before_normal_file(xzyname, mode_file);
	CFT fcfg;
	char tmp[256];
	if (1)
	{
		int ret;
		cout << endl << "1.测试写入组名，以下应当配置组1，2为失败，其余均为成功" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}
		for (int i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "增加[" << group_name[i] << "]失败." << endl;
			else
				cout << "增加[" << group_name[i] << "]成功." << endl;
		}
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否新增8个空组", xzyname);
		to_be_continued(tmp);
		fcfg.close();
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;
	}

	if (1)
	{
		int ret;
		cout << endl << "2.测试写入组名，以下应当全为失败" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}
		for (int i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "增加[" << group_name[i] << "]失败." << endl;
			else
				cout << "增加[" << group_name[i] << "]成功." << endl;
		}
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否仅有9个空组", xzyname);
		to_be_continued(tmp);
		fcfg.close();
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;
	}

	//测试写入设置项
	if (1)
	{
		//测试写入，成功
		check_add_key_value(xzyname,group_name, item,fcfg,read_mode);
		check_add_key_value(xzyname, group_name, item, fcfg, write_good);
		check_add_key_value(xzyname, group_name, item, fcfg, write_rpt);

		cout << endl << "6.下面试图写入不存在的组中,应当为失败" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		int ret = 0;
		cout << "增加[" << "配置组9" << "]/" << "配置组9-设置项Z" << (ret > 0 ? "成功." : "失败.") << endl;
		fcfg.close();
		if (!fcfg.is_open())
			cout << "文件已关闭" << endl;
		else
			cout << "文件未成功关闭，请退出程序查看原因" << endl;
	}

	/* 第三次模拟，删除组及项 */
	if (1) {
		sprintf(tmp, "7.请打开配置文件%s，将[配置组3]复制3份，配置组2-设置项B复制2份，且后面的值类型不同", xzyname);
		to_be_continued(tmp);

		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		cout << "应当为1组，实际" << "删除[配置组3]共" << fcfg.group_del("配置组3") << "组" << endl;
		cout << "应当为3组，实际" << "删除[配置组5]共" << fcfg.group_del("配置组5") << "组" << endl;
		cout<<"应当为2项，实际" << "删除<配置组2-设置项B>共" << fcfg.item_del("配置组2", "配置组2-设置项B") << "项" << endl;
		cout << "应当为0项，实际" << "删除[配置组100]共" << fcfg.group_del("配置组100") << "组" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察[配置组3]/[配置组5]/<配置组2-设置项B>是否已被删除", xzyname);
		to_be_continued(tmp);
	}

	/* 第四次模拟，修改项 */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* 已存在项，更新 */
		fcfg.item_update("配置组4", "配置组4-设置项B", a);
		fcfg.item_update("配置组1", "配置组1-设置项D", "helloworld");
		fcfg.item_update("配置组8", "配置组8-设置项E", b);
		fcfg.item_update("配置组6", "配置组6-设置项F");
		fcfg.item_update("配置组7", "配置组7-设置项C", c);
		fcfg.item_update("配置组2", "配置组2-设置项D", s);

		/* 不存在项，新增 */
		fcfg.item_update("配置组6", "配置组6-设置项U");
		fcfg.item_update("配置组7", "配置组7-设置项V", c);
		fcfg.item_update("配置组7", "配置组6-设置项W");
		fcfg.item_update("配置组4", "配置组4-设置项X", a);
		fcfg.item_update("配置组1", "配置组1-设置项Y", "helloworld");
		fcfg.item_update("配置组8", "配置组8-设置项Z", b);
		fcfg.item_update("配置组2", "配置组2-设置项Q", s);
		fcfg.close(); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置是否已修改", xzyname);
		to_be_continued(tmp);
	}

	/* 第五次模拟，读 */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "无法打开配置文件(" << xzyname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* 已存在项，更新 */
		if (fcfg.item_get_value("配置组4", "配置组4-设置项B", a) > 0)
			cout << "[配置组4]/配置组4-设置项B=" << a << endl;
		if (fcfg.item_get_value("配置组1", "配置组1-设置项F", a) > 0)
			cout << "[配置组1]/配置组1-设置项F=" << a << endl;
		if (fcfg.item_get_value("配置组6", "配置组6-设置项D", c) > 0)
			cout << "[配置组6]/配置组6-设置项D=" << c << endl;
		if (fcfg.item_get_value("配置组8", "配置组8-设置项C", s) > 0)
			cout << "[配置组8]/配置组8-设置项C=" << s << endl;
		if (fcfg.item_get_value("配置组2", "配置组2-设置项Q", ss) > 0)
			cout << "[配置组2]/配置组2-设置项Q=" << ss << endl;
		if (fcfg.item_get_value("配置组7", "配置组7-设置项E") > 0)
			cout << "[配置组7]/配置组7-设置项E=<NULL>" << endl;
		if (fcfg.item_get_value("配置组6", "配置组6-设置项B", b) > 0)
			cout << "[配置组6]/配置组6-设置项B=" << b << endl;
		if (fcfg.item_get_value("配置组1", "设置项100") > 0)
			cout << "[配置组1]/设置项100=<NULL>" << endl;
		else
			cout << "[配置组1]/设置项100 不存在" << endl;

		fcfg.close(); //关闭文件，模拟本次设置完成
		cout << endl;
		cout << "结果应当为\n"
			"[配置组4]/配置组4-设置项B=54321\n"
			"[配置组1] / 配置组1 - 设置项F = 99999\n"
			"[配置组6] / 配置组6 - 设置项D = Y\n"
			"[配置组8] / 配置组8 - 设置项C = Hello\n"
			"[配置组2] / 配置组2 - 设置项Q = helloworld<string>\n"
			"[配置组7] / 配置组7 - 设置项E = <NULL>\n"
			"[配置组6] / 配置组6 - 设置项B = 123.45\n"
			"[配置组1] / 设置项100 不存在" << endl<<endl;
		to_be_continued(tmp);
	}
	return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int main()
{
	const char* cfgname = "c1.cfg";
	const char* group_name[] = {
		"配置组1",
		"配置组2",
		"配置组3",
		"配置组4",
		"配置组5",
		"配置组6",
		"配置组7",
		"配置组8",
		NULL
	};

	struct cfg_item item[] =
	{
		{ "设置项A",ITEM_TYPE::Int, 0 },		//共用体union的初始化，只能初始化第一个元素
		{ "设置项B", ITEM_TYPE::Double, 0 },
		{ "设置项C",ITEM_TYPE::String, 0 },
		{ "设置项D",ITEM_TYPE::Character, 0 },
		{ "设置项E", ITEM_TYPE::Null, 0 },
		{ "设置项F",ITEM_TYPE::Int, 0 },
		{ "设置项G",ITEM_TYPE::String, 0 },
		{ NULL,ITEM_TYPE::Null, 0 }
	};

	set_item(item); //给union赋初值
	if (teacher_test)
	{
		test_readconfig_cpp(cfgname, group_name, item);
	}
	if (xzy_test)
	{
		//cmp_file();
		//有组名，win
		cout << "xzy的测试“win文件格式，普通配置”" << endl;
		test_normal_file("u2053628_g.cfg", group_name,item,WINDOWS);

		// 无组名，win
		cout << "上一次测试文件将被删除，xzy的测试“win文件格式，简单配置”" << endl;
		test_simple_file("u2053628_s.cfg" , group_name, item, WINDOWS);

		//有组名，linux
		cout << "上一次测试文件将被删除，xzy的测试“linux文件格式，普通配置”" << endl;
		test_normal_file("u2053628_g.cfg", group_name, item, LINUX);

		//无组名，linux
		cout << "上一次测试文件将被删除，xzy的测试“linux文件格式，简单配置”" << endl;
		test_simple_file("u2053628_s.cfg", group_name, item, LINUX);
		cout << "最后一次没有删文件" << endl;
	}
	return 0;
}
#endif