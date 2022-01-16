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
// �������ļ��Ŀ��ء�


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
  �������ƣ�
  ��    �ܣ���ʾ��ʾ��Ϣ
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
static void to_be_continued(const char* prompt)
{
	if (prompt)
		cout << prompt << "�����س�������...";
	else
		cout << "���س�������...";

	/* ���Գ��س�������������루ע�⣺_getch�Ļس���\r����getchar��\n��*/
	while (_getch() != '\r')
		;
	cout << endl;

	return;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���Ϊcfg_item�к��й����壬������ֻ�ܳ�ʼ����һ�����ú�����ʼ��
  ���������
  �� �� ֵ��
  ˵    ����
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
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int test_readconfig_cpp(const char* cfgname, const char** group_name, struct cfg_item* item)
{
	CFT fcfg;
	int ret, i;
	char tmp[256];

	/* ��һ��ģ�⣬��������� */
	if (1) {
		remove(cfgname); //ɾ���ļ�

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << cfgname << ")." << endl;
			return -1;
		}
		for (i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "����[" << group_name[i] << "]ʧ��." << endl;
			else
				cout << "����[" << group_name[i] << "]�ɹ�." << endl;
		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲��Ƿ����8������", cfgname);
		to_be_continued(tmp);
	}

	/* �ڶ���ģ�⣬ÿ����������� */
	if (1) {
		int j;

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << cfgname << ")." << endl;
			return -1;
		}

		for (i = 0; group_name[i]; i++) {
			for (j = 0; item[j].cfg_item_name; j++) {
				sprintf(tmp, "%s-%s", group_name[i], item[j].cfg_item_name);
				/* ˵����item_add�ĵ�3��������void*����˲���ʲô���ͣ�������union��һ����Ա�ĵ�ַ����������Ա��ַ */
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
				cout << "����[" << group_name[i] << "]/" << tmp << (ret > 0 ? "�ɹ�." : "ʧ��.") << endl;
			}
		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲��Ƿ�8�������6��������", cfgname);
		to_be_continued(tmp);
	}

	/* ������ģ�⣬ɾ���鼰�� */
	if (1) {
		sprintf(tmp, "��������ļ�%s����[������3]�������ɷݣ�������2-������B�������ɷݣ��Һ����ֵ���Ͳ�ͬ", cfgname);
		to_be_continued(tmp);

		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << cfgname << ")." << endl;
			return -1;
		}

		cout << "ɾ��[������3]��" << fcfg.group_del("������3") << "��" << endl;
		cout << "ɾ��[������5]��" << fcfg.group_del("������5") << "��" << endl;

		cout << "ɾ��<������2-������B>��" << fcfg.item_del("������2", "������2-������B") << "��" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲�[������3]/[������5]/<������2-������B>�Ƿ��ѱ�ɾ��", cfgname);
		to_be_continued(tmp);
	}

	/* ���Ĵ�ģ�⣬�޸��� */
	if (1) {
		fcfg.open(cfgname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << cfgname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* �Ѵ�������� */
		fcfg.item_update("������4", "������4-������B", a);
		fcfg.item_update("������1", "������1-������D", "helloworld");
		fcfg.item_update("������8", "������8-������E", b);
		fcfg.item_update("������6", "������6-������F");
		fcfg.item_update("������7", "������7-������C", c);
		fcfg.item_update("������2", "������2-������D", s);

		/* ����������� */
		fcfg.item_update("������6", "������6-������U");
		fcfg.item_update("������7", "������7-������V", c);
		fcfg.item_update("������7", "������6-������W");
		fcfg.item_update("������4", "������4-������X", a);
		fcfg.item_update("������1", "������1-������Y", "helloworld");
		fcfg.item_update("������8", "������8-������Z", b);
		fcfg.item_update("������2", "������2-������Q", s);
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲����������Ƿ����޸�", cfgname);
		to_be_continued(tmp);
	}

	/* �����ģ�⣬�� */
	if (1) {
		fcfg.open(cfgname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << cfgname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* �Ѵ�������� */
		if (fcfg.item_get_value("������4", "������4-������B", a) > 0)
			cout << "[������4]/������4-������B=" << a << endl;
		if (fcfg.item_get_value("������1", "������1-������F", a) > 0)
			cout << "[������1]/������1-������F=" << a << endl;
		if (fcfg.item_get_value("������6", "������6-������D", c) > 0)
			cout << "[������6]/������6-������D=" << c << endl;
		if (fcfg.item_get_value("������8", "������8-������C", s) > 0)
			cout << "[������8]/������8-������C=" << c << endl;
		if (fcfg.item_get_value("������2", "������2-������Q", ss) > 0)
			cout << "[������2]/������2-������Q=" << ss << endl;
		if (fcfg.item_get_value("������7", "������7-������E") > 0)
			cout << "[������7]/������7-������E=<NULL>" << endl;
		if (fcfg.item_get_value("������6", "������6-������B", b) > 0)
			cout << "[������6]/������6-������B=" <<b<< endl;
		if (fcfg.item_get_value("������1", "������100") > 0)
			cout << "[������1]/������100=<NULL>" << endl;
		else
			cout << "[������1]/������100 ������" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲��������ö�ȡ�Ƿ���ȷ", cfgname);
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
	fout << "[ ������1    \t \t ]" << end_of_line;
	fout << " ������1-������G  \t \t=   Hello<String>#Regulus" << end_of_line;
	fout << "    [ ������2    \t \t ]  ;Sirius" << end_of_line;
	fout << "������7-������F=\t99999" << end_of_line;
	fout << "    [ ������3    \t \t ]  Sirius" << end_of_line;
	fout << "[������   4]" << end_of_line;
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
			cout << "1.д����Ŀ��������ģʽ���´�����ӣ�����Ӧ��ȫΪʧ��" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDONLY);
		}
		if (mode == write_good)
		{
			cout << "2.д����Ŀ����дģʽ����������ӣ�����Ӧ����ǰ�����ɹ���������ʧ��" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}
		if (mode == write_rpt)
		{
			cout << "3.д����Ŀ����дģʽ�����ظ���ӣ�����Ӧ��ȫΪʧ��" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}

		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}
		cout << "����[" << "NULL" << "]/" << "Astronomy" << (fcfg.item_add(NULL, "Astronomy") > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Muggle Study" << (fcfg.item_add(NULL, "Muggle Study",123.72) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "care of magical ceatures" << (fcfg.item_add(NULL, "care of magical ceatures",'O') > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Herbology" << (fcfg.item_add(NULL, "Herbology", string("Outstanding")) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Potions" << (fcfg.item_add(NULL, "Potions", "Outstanding") > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "History" << (fcfg.item_add(NULL, "History", 120) > 0 ? "�ɹ�." : "ʧ��.") << endl;

		
		cout << "����[" << "NULL" << "]/" << "DADA" << (fcfg.item_add(NULL, "DADA") > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Transfiguration" << (fcfg.item_add(NULL, "Transfiguration", 100) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Charms" << (fcfg.item_add(NULL, "Charms", 100.0F) > 0 ? "�ɹ�." : "ʧ��.") << endl;


		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������
		if (mode != read_mode)
			sprintf(tmp, "��������ļ�%s���۲��Ƿ�8�������6��������", xzyname);
		else
			sprintf(tmp, "��������ļ�%s���۲��Ƿ�8�������0��������", xzyname);
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;

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
			cout << "3.д����Ŀ��������ģʽ���´�����ӣ�����Ӧ��ȫΪʧ��" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDONLY);
		}
		if (mode == write_good)
		{
			cout << "4.д����Ŀ����дģʽ����������ӣ�����Ӧ��\"����1-������G\"ʧ�ܣ�����ɹ�" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}
		if (mode == write_rpt)
		{
			cout << "5.д����Ŀ����дģʽ�����ظ���ӣ�����Ӧ��ȫΪʧ��" << endl;
			fcfg.open(xzyname, OPEN_OPT_RDWR);
		}

		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		for (int i = 0; group_name[i]; i++) {
			for (j = 0; item[j].cfg_item_name; j++) {
				sprintf(tmp, "%s-%s", group_name[i], item[j].cfg_item_name);
				/* ˵����item_add�ĵ�3��������void*����˲���ʲô���ͣ�������union��һ����Ա�ĵ�ַ����������Ա��ַ */
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
				cout << "����[" << group_name[i] << "]/" << tmp << (ret_sum > 0 ? "�ɹ�." : "ʧ��.") << endl;
			}
		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������
		if(mode!=read_mode)
			sprintf(tmp, "��������ļ�%s���۲��Ƿ�8�������6��������", xzyname);
		else
			sprintf(tmp, "��������ļ�%s���۲��Ƿ�8�������0��������", xzyname);
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;

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

	//����д��������
	if (1)
	{
		//����д�룬�ɹ�
		check_add_key_value_simple(xzyname, group_name,fcfg, read_mode);
		check_add_key_value_simple(xzyname, group_name,fcfg, write_good);
		check_add_key_value_simple(xzyname, group_name,fcfg, write_rpt);

		cout << endl << "6.������ͼд�벻���ڵ�����,Ӧ��Ϊʧ��" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		int ret = 0;
		cout << "����[" << "������9" << "]/" << "������9-������Z" << (ret > 0 ? "�ɹ�." : "ʧ��.") << endl;
		fcfg.close();
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;
	}

	/* ������ģ�⣬ɾ���鼰�� */
	if (1) {
		sprintf(tmp, "7.��������ļ�%s����\"Tranfiguration\"����3�ݣ��Һ����ֵ���Ͳ�ͬ", xzyname);
		to_be_continued(tmp);

		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		cout << "Ӧ��Ϊ1�飬ʵ��" << "ɾ��\"Potions\"��" << fcfg.item_del(NULL,"Potions") << "��" << endl;
		cout << "Ӧ��Ϊ3�飬ʵ��" << "ɾ��\"Transfiguration\"��" << fcfg.item_del(NULL, "Transfiguration") << "��" << endl;
		cout << "Ӧ��Ϊ0�ʵ��" << "ɾ��\"DADA\"��" << fcfg.item_del("������2", "DADA") << "��" << endl;
		cout << "Ӧ��Ϊ0�ʵ��" << "ɾ��[������100]��" << fcfg.group_del("������100") << "��" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲�[������3]/[������5]/<������2-������B>�Ƿ��ѱ�ɾ��", xzyname);
		to_be_continued(tmp);
	}

	/* ���Ĵ�ģ�⣬�޸��� */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDWR);  
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* �Ѵ�������� */
		cout << "����[" << "NULL" << "]/" << "Astronomy" << (fcfg.item_update(NULL, "Astronomy",123.72) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Muggle Study" << (fcfg.item_update(NULL, "Muggle Study", 'O') > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "care of magical ceatures" << (fcfg.item_update(NULL, "care of magical ceatures") > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Herbology" << (fcfg.item_update(NULL, "Herbology", 120) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Charms" << (fcfg.item_update(NULL, "Charms", string("Outstanding")) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "History" << (fcfg.item_update(NULL, "History", "Outstanding") > 0 ? "�ɹ�." : "ʧ��.") << endl;

		/* ����������� */
		cout << "����[" << "NULL" << "]/" << "Potions" << (fcfg.item_update(NULL, "Potions") > 0 ? "�ɹ�." : "ʧ��.") << endl;
		cout << "����[" << "NULL" << "]/" << "Transifiguration" << (fcfg.item_update(NULL, "Transifiguration", 100) > 0 ? "�ɹ�." : "ʧ��.") << endl;
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲����������Ƿ����޸�", xzyname);
		to_be_continued(tmp);
	}

	/* �����ģ�⣬�� */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* �Ѵ�������� */
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
			cout << "flying ������" << endl;
		if (fcfg.item_get_value("Hogwarts", "flying") > 0)
			cout << "[Hogwarts]/flying=<NULL>" << endl;
		else
			cout << "[Hogwarts]/flying ������" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������
		cout << endl;
		cout << "���Ӧ��Ϊ\n"
			"Astronomy=123.72\n"
			"Muggle Study = O\n"
			"Herbology = 120\n"
			"care of magical ceatures =\n"
			"Charms = Outstanding\n"
			"Potions = <NULL>\n"
			"flying ������\n"
			"[Hogwarts] / flying ������" << endl << endl;
		
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
		cout << endl << "1.����д������������Ӧ��������1��2Ϊʧ�ܣ������Ϊ�ɹ�" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}
		for (int i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "����[" << group_name[i] << "]ʧ��." << endl;
			else
				cout << "����[" << group_name[i] << "]�ɹ�." << endl;
		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲��Ƿ�����8������", xzyname);
		to_be_continued(tmp);
		fcfg.close();
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;
	}

	if (1)
	{
		int ret;
		cout << endl << "2.����д������������Ӧ��ȫΪʧ��" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}
		for (int i = 0; group_name[i]; i++) {
			if ((ret = fcfg.group_add(group_name[i])) <= 0)
				cout << "����[" << group_name[i] << "]ʧ��." << endl;
			else
				cout << "����[" << group_name[i] << "]�ɹ�." << endl;
		}
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲��Ƿ����9������", xzyname);
		to_be_continued(tmp);
		fcfg.close();
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;
	}

	//����д��������
	if (1)
	{
		//����д�룬�ɹ�
		check_add_key_value(xzyname,group_name, item,fcfg,read_mode);
		check_add_key_value(xzyname, group_name, item, fcfg, write_good);
		check_add_key_value(xzyname, group_name, item, fcfg, write_rpt);

		cout << endl << "6.������ͼд�벻���ڵ�����,Ӧ��Ϊʧ��" << endl;
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		int ret = 0;
		cout << "����[" << "������9" << "]/" << "������9-������Z" << (ret > 0 ? "�ɹ�." : "ʧ��.") << endl;
		fcfg.close();
		if (!fcfg.is_open())
			cout << "�ļ��ѹر�" << endl;
		else
			cout << "�ļ�δ�ɹ��رգ����˳�����鿴ԭ��" << endl;
	}

	/* ������ģ�⣬ɾ���鼰�� */
	if (1) {
		sprintf(tmp, "7.��������ļ�%s����[������3]����3�ݣ�������2-������B����2�ݣ��Һ����ֵ���Ͳ�ͬ", xzyname);
		to_be_continued(tmp);

		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		cout << "Ӧ��Ϊ1�飬ʵ��" << "ɾ��[������3]��" << fcfg.group_del("������3") << "��" << endl;
		cout << "Ӧ��Ϊ3�飬ʵ��" << "ɾ��[������5]��" << fcfg.group_del("������5") << "��" << endl;
		cout<<"Ӧ��Ϊ2�ʵ��" << "ɾ��<������2-������B>��" << fcfg.item_del("������2", "������2-������B") << "��" << endl;
		cout << "Ӧ��Ϊ0�ʵ��" << "ɾ��[������100]��" << fcfg.group_del("������100") << "��" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲�[������3]/[������5]/<������2-������B>�Ƿ��ѱ�ɾ��", xzyname);
		to_be_continued(tmp);
	}

	/* ���Ĵ�ģ�⣬�޸��� */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDWR);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';
		string s = "helloworld<string>";

		/* �Ѵ�������� */
		fcfg.item_update("������4", "������4-������B", a);
		fcfg.item_update("������1", "������1-������D", "helloworld");
		fcfg.item_update("������8", "������8-������E", b);
		fcfg.item_update("������6", "������6-������F");
		fcfg.item_update("������7", "������7-������C", c);
		fcfg.item_update("������2", "������2-������D", s);

		/* ����������� */
		fcfg.item_update("������6", "������6-������U");
		fcfg.item_update("������7", "������7-������V", c);
		fcfg.item_update("������7", "������6-������W");
		fcfg.item_update("������4", "������4-������X", a);
		fcfg.item_update("������1", "������1-������Y", "helloworld");
		fcfg.item_update("������8", "������8-������Z", b);
		fcfg.item_update("������2", "������2-������Q", s);
		fcfg.close(); //�ر��ļ���ģ�Ȿ���������

		sprintf(tmp, "��������ļ�%s���۲����������Ƿ����޸�", xzyname);
		to_be_continued(tmp);
	}

	/* �����ģ�⣬�� */
	if (1) {
		fcfg.open(xzyname, OPEN_OPT_RDONLY);
		if (fcfg.is_open() == false) {
			cout << "�޷��������ļ�(" << xzyname << ")." << endl;
			return -1;
		}

		int a;
		double b;
		char c, s[80];
		string ss;

		/* �Ѵ�������� */
		if (fcfg.item_get_value("������4", "������4-������B", a) > 0)
			cout << "[������4]/������4-������B=" << a << endl;
		if (fcfg.item_get_value("������1", "������1-������F", a) > 0)
			cout << "[������1]/������1-������F=" << a << endl;
		if (fcfg.item_get_value("������6", "������6-������D", c) > 0)
			cout << "[������6]/������6-������D=" << c << endl;
		if (fcfg.item_get_value("������8", "������8-������C", s) > 0)
			cout << "[������8]/������8-������C=" << s << endl;
		if (fcfg.item_get_value("������2", "������2-������Q", ss) > 0)
			cout << "[������2]/������2-������Q=" << ss << endl;
		if (fcfg.item_get_value("������7", "������7-������E") > 0)
			cout << "[������7]/������7-������E=<NULL>" << endl;
		if (fcfg.item_get_value("������6", "������6-������B", b) > 0)
			cout << "[������6]/������6-������B=" << b << endl;
		if (fcfg.item_get_value("������1", "������100") > 0)
			cout << "[������1]/������100=<NULL>" << endl;
		else
			cout << "[������1]/������100 ������" << endl;

		fcfg.close(); //�ر��ļ���ģ�Ȿ���������
		cout << endl;
		cout << "���Ӧ��Ϊ\n"
			"[������4]/������4-������B=54321\n"
			"[������1] / ������1 - ������F = 99999\n"
			"[������6] / ������6 - ������D = Y\n"
			"[������8] / ������8 - ������C = Hello\n"
			"[������2] / ������2 - ������Q = helloworld<string>\n"
			"[������7] / ������7 - ������E = <NULL>\n"
			"[������6] / ������6 - ������B = 123.45\n"
			"[������1] / ������100 ������" << endl<<endl;
		to_be_continued(tmp);
	}
	return 0;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int main()
{
	const char* cfgname = "c1.cfg";
	const char* group_name[] = {
		"������1",
		"������2",
		"������3",
		"������4",
		"������5",
		"������6",
		"������7",
		"������8",
		NULL
	};

	struct cfg_item item[] =
	{
		{ "������A",ITEM_TYPE::Int, 0 },		//������union�ĳ�ʼ����ֻ�ܳ�ʼ����һ��Ԫ��
		{ "������B", ITEM_TYPE::Double, 0 },
		{ "������C",ITEM_TYPE::String, 0 },
		{ "������D",ITEM_TYPE::Character, 0 },
		{ "������E", ITEM_TYPE::Null, 0 },
		{ "������F",ITEM_TYPE::Int, 0 },
		{ "������G",ITEM_TYPE::String, 0 },
		{ NULL,ITEM_TYPE::Null, 0 }
	};

	set_item(item); //��union����ֵ
	if (teacher_test)
	{
		test_readconfig_cpp(cfgname, group_name, item);
	}
	if (xzy_test)
	{
		//cmp_file();
		//��������win
		cout << "xzy�Ĳ��ԡ�win�ļ���ʽ����ͨ���á�" << endl;
		test_normal_file("u2053628_g.cfg", group_name,item,WINDOWS);

		// ��������win
		cout << "��һ�β����ļ�����ɾ����xzy�Ĳ��ԡ�win�ļ���ʽ�������á�" << endl;
		test_simple_file("u2053628_s.cfg" , group_name, item, WINDOWS);

		//��������linux
		cout << "��һ�β����ļ�����ɾ����xzy�Ĳ��ԡ�linux�ļ���ʽ����ͨ���á�" << endl;
		test_normal_file("u2053628_g.cfg", group_name, item, LINUX);

		//��������linux
		cout << "��һ�β����ļ�����ɾ����xzy�Ĳ��ԡ�linux�ļ���ʽ�������á�" << endl;
		test_simple_file("u2053628_s.cfg", group_name, item, LINUX);
		cout << "���һ��û��ɾ�ļ�" << endl;
	}
	return 0;
}
#endif