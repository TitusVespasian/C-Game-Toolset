#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include<sstream>
#include <io.h>
#include "../include/cfg_file_tools.h"
using namespace std;
//�����windows linux 
//��βû�л�������
/* ---------------------------------------------------------------
	 �������������Ҫstatic�������ڲ������ã�
   ---------------------------------------------------------------- */
int CFT::check_file()
{
	bool flag_windows = false;
	bool flag_linux = false;
	while (!this->fp.eof())
	{
		char u = this->fp.get();
		if (this->fp.fail())
			break;
		if (u == 0x0A)
		{
			this->fp.seekg(-2, ios::cur);
			char v = this->fp.get();
			//  cout << "v " << int(v);
			if (v == 0x0D)
				flag_windows = true;
			else
				flag_linux = true;
			this->fp.seekg(1, ios::cur);
		}
	}
	this->ENDL = "\r\n";
	if ((flag_linux == true && flag_windows == true) ||
		(flag_linux == false && flag_windows == false))
	{
		//cout << "�ļ���ʽ�޷�ʶ��" << endl;
		this->ENDL = "\r\n";
	}
	else if (flag_linux == true && flag_windows == false)
	{
		this->ENDL = "\n";
	}
	else if (flag_windows == true && flag_linux == false)
	{
		this->ENDL = "\r\n";
	}
	this->fp.clear();
	this->fp.seekg(0, ios::beg);
	return 0;
}
/*size��Ԫ�صĸ���*/
template<typename Elem>
static Elem* renew(Elem* p, int oldsize, int newsize)
{
	Elem* pnew;
	pnew = new(nothrow) Elem[newsize];
	if (pnew == NULL)
		return NULL;
	if (p != NULL)
	{
		//strncpy(pnew, p, min(oldsize, newsize));
		for (int i = 0; i < min(oldsize, newsize); i++)
			pnew[i] = p[i];
	}
	delete[]p;
	return pnew;
}

/***************************************************************************
  �������ƣ�get_groupname
  ��    �ܣ�����process_line ���ص���
  ���������processed_str ������ **=** ��ret_key ret_value ���ش����ļ�ֵ��
  �� �� ֵ���Ƿ�Ϊ��Ч��ֵ�� NOVALID ��Ч FINGOOD ��Ч
  ˵    �������б�Ҫ�����������������
***************************************************************************/
static int get_key_value(const string processed_str, string& ret_key, string& ret_value)
{
	size_t pos = processed_str.find('=');
	if (pos == processed_str.npos)
		return NOVALID;
	ret_key = processed_str.substr(0, pos);
	ret_value = processed_str.substr(pos + 1);
	//ret_key�������Ŀո�\t
	size_t len = ret_key.length();
	for (int i = (int)len - 1; i >= 0; i--)
	{
		if (ret_key[i] == ' ' || ret_key[i] == '\t')
			ret_key.erase(i);
		else //if (ret_key[i] != ' ' && ret_key[i] != '\t')
			break;
	}
	//ret_value����ǰ��Ŀո�
	len = ret_value.length();
	for (int i = 0; i < (int)len; i++)
	{
		if (ret_value[i] == ' ' || ret_value[i] == '\t')
		{
			ret_value.erase(i, 1);
			i--;
		}
		else
			break;
	}
	return FINGOOD;
}
/***************************************************************************
  �������ƣ�get_groupname
  ��    �ܣ�����process_line ���ص���
  ���������processed_str ������[***]��ret_gpname ���ش����Ĵ�
  �� �� ֵ���Ƿ�Ϊ��Ч���� NOVALID ��Ч FINGOOD ��Ч
  ˵    �������б�Ҫ�����������������
***************************************************************************/
static int get_groupname(const string processed_str, string& ret_gpname)
{
	if (processed_str[0] != '[' || processed_str[processed_str.length() - 1] != ']')
		return NOVALID;
	ret_gpname = processed_str;
	//ɾ��[]
	ret_gpname.erase(0, 1);
	ret_gpname.erase(ret_gpname.length() - 1, 1);
	//����ǰ��Ŀո�
	size_t len = ret_gpname.length();
	if (len <= 0)//ʲôҲû��ʱ
	{
		ret_gpname = "";
		return FINGOOD;
	}
	for (int i = 0; i < (int)len; i++)
	{
		if (ret_gpname[i] == ' ' || ret_gpname[i] == '\t')
		{
			ret_gpname.erase(i, 1);
			i--;
		}
		else
			break;
	}
	//ȥ����ո�
	len = ret_gpname.length();
	if (len <= 0)//ֻ�пո�ʱ
	{
		ret_gpname = "";
		return FINGOOD;
	}
	for (int i = (int)len - 1; i >= 0; i--)
	{
		if (ret_gpname[i] == ' ' || ret_gpname[i] == '\t')
		{
			ret_gpname.erase(i);
		}
		else
			break;
	}
	ret_gpname = ret_gpname;
	return FINGOOD;
}
/***************************************************************************
  �������ƣ�process_line
  ��    �ܣ�����getline �������
  ���������raw_str �����У�processed_str ���ش����Ĵ�
  �� �� ֵ����
  ˵    ������Ч���ݣ�����Ϊ�մ�
***************************************************************************/
void CFT::process_line(string &raw_str, string& processed_str)
{
	
	//ȥ��\r
	if (int(raw_str.length()) - 1 >= 0)
	{
		if (this->ENDL == "\r\n" && raw_str[raw_str.length() - 1] == '\r')
		{
			raw_str.erase(raw_str.length() - 1);
		}
	}
	processed_str = raw_str;
	//ȥ��ע��
	size_t pos = processed_str.find('#');
	if (pos != processed_str.npos)
		processed_str.erase(pos);
	pos = processed_str.find(';');
	if (pos != processed_str.npos)
		processed_str.erase(pos);

	//ȥ��ǰ��ո�\t
	size_t len = processed_str.length();
	if (len <= 0)//ʲôҲû��ʱ
	{
		processed_str = "";
		return;
	}
	for (int i = 0; i < (int)len; i++)
	{
		if (processed_str[i] == ' ' || processed_str[i] == '\t')
		{
			processed_str.erase(i, 1);
			i--;
		}
		else
			break;
	}

	//ȥ����ո�
	len = processed_str.length();
	if (len <= 0)//ֻ��ȡ��ע��ʱ
		return;
	for (int i = (int)len - 1; i >= 0; i--)
	{
		if (processed_str[i] == ' ' || processed_str[i] == '\t')
			processed_str.erase(i);
		else //if (processed_str[i] != ' ' && processed_str[i] != '\t')
			break;
	}
	return;
}
/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    �������б�Ҫ�����������������
***************************************************************************/
CFT::CFT()
{
	filename = "";
	this->ENDL = "\r\n";
	this->rw = READ;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    �������б�Ҫ�����������������
***************************************************************************/
CFT::~CFT()
{
	if (fp.is_open())
		fp.close();
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
CFT::CFT(const char* cfgname, int opt)
{
	this->filename = cfgname;
	this->ENDL = "\r\n";
	if (!this->fp.is_open())
	{
		this->fp.close();
	}
	if (opt == OPEN_OPT_RDONLY)
	{
		this->rw = READ;
		this->fp.open(cfgname, ios::in | ios::binary);//��д����check
		if (this->fp.is_open())
			this->check_file();
	}
	if (opt == OPEN_OPT_RDWR)
	{
		this->rw = WRITE;
		this->fp.open(cfgname, ios::in | ios::out | ios::binary);
		if (this->fp.is_open())
			this->check_file();
		else
			this->fp.open(cfgname, ios::in | ios::out | ios::trunc | ios::binary);//�½�����check
	}
	return;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void CFT::open(const char* cfgname, int opt)
{
	this->filename = cfgname;
	if (this->fp.is_open())
		this->fp.close();
	if (opt == OPEN_OPT_RDONLY)
	{
		this->rw = READ;
		this->fp.open(cfgname, ios::in | ios::binary);//��д����check
		if (this->fp.is_open())
			this->check_file();
	}
	if (opt == OPEN_OPT_RDWR)
	{
		this->rw = WRITE;
		this->fp.open(cfgname, ios::in | ios::out | ios::binary);
		if (this->fp.is_open())
			this->check_file();
		else
			this->fp.open(cfgname, ios::in | ios::out | ios::trunc | ios::binary);//�½�����check
	}
	return;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    �����򿪳ɹ�����true�����򷵻�false
***************************************************************************/
bool CFT::is_open(void)
{
	if (this->fp.is_open())
		return true;
	else
		return false;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void CFT::close()
{
	if (fp.is_open())
		fp.close();
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����C++17����filesystem::resize_fileֱ�ӿ����޸��ļ���С��֮ǰ�İ汾��
				   ��˱�����������C�ľɷ���
		��ʵ�֣���Ҫ��
***************************************************************************/
int CFT::file_resize(int newsize)
{
	/* �����С�Ǹ�����ֱ�ӷ���-1 */
	if (newsize < 0)
		return -1;

	if (!fp.good())
		fp.clear();

	/* ȡfp��ǰָ��λ�� */
	int now_pos = int(fp.tellg());

	/* ���ȣ�C++��ʽ�ر� */
	this->close();

	/* ��Σ�C��ʽ�򿪲��ı��ļ���С */
	FILE* fp_cstyle;
	fp_cstyle = fopen(filename.c_str(), "rb+"); //rw��ʽ��
	if (fp_cstyle) {
		_chsize(_fileno(fp_cstyle), newsize);
		fclose(fp_cstyle);
	}
	else
		cout << "open [" << filename << "] for resize failed" << endl;

	/* �ٴΣ�C++��ʽ���´� */
	fp.open(filename, ios::in | ios::out | ios::binary);
	if (fp.is_open() == 0) {
		cout << "reopen [" << filename << "] after resize failed" << endl;
		return -1;
	}

	/* �ļ�ָ���ƶ���ԭλ�� */
	fp.seekg(now_pos < newsize ? now_pos : newsize, ios::beg);

	return 0;
}

/* ---------------------------------------------------------------
	 ����AAT���Զ����Ա������ʵ�֣�protected��
   ---------------------------------------------------------------- */

   /***************************************************************************
	 �������ƣ�
	 ��    �ܣ�
	 ���������
	 �� �� ֵ��
	 ˵    ����
	***************************************************************************/
int CFT::group_add(const char* group_name)
{
	if (!this->fp.is_open())
		return FAILURE;
	if (this->rw == READ)
		return FAILURE;
	this->fp.clear();
	this->fp.seekp(0, ios::beg);
	while (1)
	{
		string raw_str;
		getline(this->fp, raw_str);
		if (this->fp.fail() && raw_str == "")//���û�лس���������
			break;
		if (this->fp.fail() && raw_str != "")
		{
			raw_str.append(this->ENDL);
			raw_str.erase(raw_str.length() - 1);
		}
		string processed_str;
		process_line(raw_str, processed_str);
		string ret_groupname;
		int status = get_groupname(processed_str, ret_groupname);
		if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
			continue;
		if (ret_groupname == group_name)
			return FAILURE;
	}
	//һ����fail�˳�
	this->fp.clear();
	int start = 0;
	if (this->fp.tellg() == (streampos)0)
		start = 1;
	this->fp.seekp(0, ios::end);
	if (start == 0)
		this->fp << this->ENDL;
	this->fp << '[' << group_name << ']'<< this->ENDL;
	if (this->fp.good())
		return SUCCESS; //�˾������Ҫ�޸�
	else
		return FAILURE;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::group_del(const char* group_name)
{
	if (!this->fp.is_open())
		return FAILURE;
	if (this->rw == READ)
		return FAILURE;
	this->fp.clear();
	this->fp.seekp(0, ios::beg);
	int gpnum = 0;
	string* all_file = new(nothrow) string[INIT_SIZE]{};
	if (all_file == NULL)
	{
		cout << "�ڴ�����ʧ��" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	string old_ret_gpname = "#";
	//��������
	while (1)
	{
		string temp_string;
		getline(this->fp, temp_string);
		if (this->fp.fail() && temp_string == "")//���û�лس���������
			break;
		if (this->fp.fail() && temp_string != "")
		{
			temp_string.append(this->ENDL);
			temp_string.erase(temp_string.length() - 1);
		}

		string processed_str;
		process_line(temp_string, processed_str);
		string ret_gpname;
		int status = get_groupname(processed_str, ret_gpname);
		if (status == NOVALID)
			ret_gpname = old_ret_gpname;
		else//VALID
			old_ret_gpname = ret_gpname;
		if (ret_gpname == group_name)
		{
			if (status == FINGOOD)
				gpnum++;//ֻ������Ч��ʱ�����ɾ��һ����
			continue;//����ֵ����ɾ��
		}
		all_file[i] = temp_string;
		i++;
		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "�ڴ�����ʧ��" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
	}
	//һ����fail�˳�
	this->fp.clear();
	//�ļ����
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	return gpnum; //�˾������Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const int item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value=ss.str();
	int ret = this->item_add(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const double item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value = ss.str();
	int ret = this->item_add(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const char* item_value)
{
	int ret = this->item_add(group_name, item_name, string(item_value));
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const string& item_value)
{
	if (!this->fp.is_open())
		return FAILURE;
	if (this->rw == READ)
		return FAILURE;
	this->fp.clear();
	this->fp.seekp(0, ios::beg);

	string* all_file = new(nothrow) string[INIT_SIZE]{};
	if (all_file == NULL)
	{
		cout << "�ڴ�����ʧ��" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	bool flag_change_line = false;
	//������
	if (group_name != NULL)
	{
		bool find_group = false;//�Ƿ��ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
					raw_str.append(this->ENDL);
					raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//ĩβ�˳���δ�ҵ�
				break;

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}

		if (find_group == false)//�˳���û�ҵ�����
		{
			delete[] all_file;
			return FAILURE;
		}

		streampos pos_file = this->fp.tellg();//�����ļ���λֵ

		while (1)//�ҵ�������
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			//�жϵ�����һ��
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
			{
				delete[] all_file;
				return FAILURE;
			}
		}
		this->fp.clear();
		this->fp.seekg(pos_file, ios::beg);

		//д�������Ϣ
		string tmp;
		stringstream stmp;
		stmp << item_name << "=" << item_value;//ǰ�治�ûس�
		tmp = stmp.str();

		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "�ڴ�����ʧ��" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
		all_file[i] = tmp;
		i++;

		while (1)
		{
			//д������
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			//ȥ��\r
			if (int(raw_str.length()) - 1 >= 0)
			{
				if (this->ENDL == "\r\n" && raw_str[raw_str.length() - 1] == '\r')
				{
					raw_str.erase(raw_str.length() - 1);
				}
			}

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;
		}
		//SUCCESS;
	}
	//�������ļ�+������
	if (group_name == NULL)
	{
		bool find_group = false;//�ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")//���������û�лس�
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);
			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (status == FINGOOD)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == true)//�˳����ҵ�����
		{
			delete[] all_file;
			return FAILURE;
		}
		this->fp.clear();//һ����fail�˳���
		//���ñ���λ��
		//������
		this->fp.seekg(0, ios::beg);//��ͷ�ٶ�
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//������û�лس�
				break;

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
			{
				delete[]all_file;
				return FAILURE;
			}
		}
		//һ����fail�˳�
		this->fp.clear();

		//д�������Ϣ
		string tmp;
		stringstream stmp;
		stmp << item_name << "=" << item_value;//ǰ�治�ûس�
		tmp = stmp.str();

		//д������
		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "�ڴ�����ʧ��" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
		all_file[i] = tmp;
		i++;

		//return SUCCESS;
	}
	//һ����fail�˳�
	this->fp.clear();
	//�ļ����
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	if (this->fp.good())
		return SUCCESS; //�˾������Ҫ�޸�
	else
		return FAILURE;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    �������ַ������ǿո�ͻس�
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const char item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value = ss.str();
	int ret = this->item_add(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name)
{
	string item_empty = "";
	int ret = this->item_add(group_name, item_name, item_empty);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_del(const char* group_name, const char* item_name)
{
	if (!this->fp.is_open())
		return FAILURE;
	this->fp.clear();
	this->fp.seekp(0, ios::beg);
	int gpnum = 0;
	string* all_file = new(nothrow) string[INIT_SIZE]{};
	if (all_file == NULL)
	{
		cout << "�ڴ�����ʧ��" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	//������
	if (group_name != NULL)
	{
		string old_ret_gpname = "#";
		//��������
		while (1)
		{
			string temp_string;
			getline(this->fp, temp_string);
			if (this->fp.fail() && temp_string == "")//���û�лس���������
				break;
			if (this->fp.fail() && temp_string != "")
			{
				temp_string.append(this->ENDL);
				temp_string.erase(temp_string.length() - 1);
			}

			string processed_str;
			process_line(temp_string, processed_str);
			string ret_gpname, ret_key, ret_value;
			//����Ƿ�Ϊ����
			int status = get_groupname(processed_str, ret_gpname);
			//����Ƿ�Ϊ��ֵ��
			int is_key_value = get_key_value(processed_str, ret_key, ret_value);
			//��������Ƿ����
			if (status == NOVALID)
				ret_gpname = old_ret_gpname;
			else//VALID
				old_ret_gpname = ret_gpname;
			//����ֵ���Ƿ����Ҫ��
			if (group_name == ret_gpname && is_key_value == FINGOOD && ret_key == item_name)
			{
				gpnum++;
				continue;//�������õ���������������ͬ��ɾ��
			}
			all_file[i] = temp_string;
			i++;
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
		}
	}
	else//group_name == NULL
	{
		//��������
		while (1)
		{
			string temp_string;
			getline(this->fp, temp_string);
			if (this->fp.fail() && temp_string == "")//���û�лس���������
				break;
			if (this->fp.fail() && temp_string != "")
			{
				temp_string.append(this->ENDL);
				temp_string.erase(temp_string.length() - 1);
			}

			string processed_str;
			process_line(temp_string, processed_str);
			string ret_key, ret_value;
			//����Ƿ�Ϊ��ֵ��
			int is_key_value = get_key_value(processed_str, ret_key, ret_value);
			//����ֵ���Ƿ����Ҫ��
			if (is_key_value == FINGOOD && ret_key == item_name)
			{
				gpnum++;
				continue;//�õ���������������ͬ��ɾ��
			}
			all_file[i] = temp_string;
			i++;
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
		}
	}
	//һ����fail�˳�
	this->fp.clear();
	//�ļ����
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	return gpnum; //�˾������Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const int item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value=ss.str();
	int ret = this->item_update(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const double item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value=ss.str();
	int ret = this->item_update(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const char* item_value)
{
	int ret = this->item_update(group_name, item_name, string(item_value));
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const string& item_value)
{
	if (!this->fp.is_open())
		return FAILURE;
	if (this->rw == READ)
		return FAILURE;
	this->fp.clear();
	this->fp.seekp(0, ios::beg);

	string tmp;
	stringstream stmp;
	stmp << item_name << "=" << item_value;//ǰ�治�ûس�
	tmp=stmp.str();

	string* all_file = new(nothrow) string[INIT_SIZE]{};
	if (all_file == NULL)
	{
		cout << "�ڴ�����ʧ��" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	bool flag_change_line = false;
	//������
	if (group_name != NULL)
	{
		bool find_group = false;//�Ƿ��ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//ĩβ�˳���δ�ҵ�
				break;


			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}

		if (find_group == false)//�˳���û�ҵ�����
		{
			delete[] all_file;
			return FAILURE;
		}

		streampos pos_file = this->fp.tellg();//�����ļ���λֵ
		bool find_key = false;
		int same_key_in_arr = 0;
		while (1)//�ҵ�������
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			//�жϵ�����һ��
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
				find_key = true;
		}
		this->fp.clear();
		this->fp.seekg(pos_file, ios::beg);
		if (!find_key)
		{
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = tmp;
			i++;
		}
		while (1)
		{
			//д������
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			string processed_str;
			process_line(raw_str, processed_str);

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			if (find_key)
			{
				string ret_key, ret_value, next_gpname;
				//�ж��Ƿ����ظ��ļ���
				int status = get_key_value(processed_str, ret_key, ret_value);
				if (status == NOVALID)//���Ǽ�ֵ��
					continue;
				if (ret_key == string(item_name) && find_key)//���ظ��ļ���
				{
					i--;
					find_key = false;
					if (all_file_actual_len <= i)
					{
						string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
						if (temp == NULL)
						{
							cout << "�ڴ�����ʧ��" << endl;
							return FAILURE;
						}
						all_file_actual_len += SIZE_INCREMENT;
						all_file = temp;
					}
					all_file[i] = tmp;
					i++;
				}
			}
		}
	}
	//�������ļ�+������
	if (group_name == NULL)
	{
		bool find_group = false;//�ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			
			if (this->fp.fail() && raw_str == "")//���������û�лس�
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);
			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (status == FINGOOD)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == true)//�˳����ҵ�����
		{
			delete[]all_file;
			return FAILURE;
		}
		this->fp.clear();//һ����fail�˳���
		//���ñ���λ��
		//������
		this->fp.seekg(0, ios::beg);//��ͷ�ٶ�
		int find_key = false;
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//������û�лس�
				break;

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
			{
				i--;
				if (all_file_actual_len <= i)
				{
					string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
					if (temp == NULL)
					{
						cout << "�ڴ�����ʧ��" << endl;
						return FAILURE;
					}
					all_file_actual_len += SIZE_INCREMENT;
					all_file = temp;
				}
				all_file[i] = tmp;
				i++;
				find_key = true;
			}
		}
		//һ����fail�˳�
		this->fp.clear();

		//д������
		if (!find_key)
		{
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "�ڴ�����ʧ��" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = tmp;
			i++;
		}
	}
	//һ����fail�˳�
	this->fp.clear();
	//�ļ����
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file; 
	if (this->fp.good())
		return SUCCESS; //�˾������Ҫ�޸�
	else
		return FAILURE; return SUCCESS;//�����ܴﵽ
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const char item_value)
{
	stringstream ss;
	ss << item_value;
	string string_item_value;
	string_item_value=ss.str();
	int ret = this->item_update(group_name, item_name, string_item_value);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name)
{
	string item_empty = "";
	int ret = this->item_update(group_name, item_name, item_empty);
	return ret;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, int& item_value)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	if (ret == FAILURE)
		return FAILURE;
	stringstream ss;
	ss << buffer;
	ss >> item_value;
	return SUCCESS;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, double& item_value)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	if (ret == FAILURE)
		return FAILURE;
	stringstream ss;
	ss << buffer;
	ss >> item_value;
	return SUCCESS;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, char* item_value)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	if (ret == FAILURE)
		return FAILURE;
	strcpy(item_value,buffer.c_str());
	return SUCCESS;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, string& item_value)
{
	if (!this->fp.is_open())
		return FAILURE;
	this->fp.clear();
	this->fp.seekg(0, ios::beg);
	//������
	if (group_name != NULL)
	{
		bool find_group = false;//�Ƿ��ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//ĩβ�˳���δ�ҵ�
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);
			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == false)//�˳���û�ҵ�����
			return FAILURE;
		while (1)//�ҵ�������
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			//�жϵ�����һ��
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
			{
				item_value = ret_value;
				return SUCCESS;
			}
		}
		return FAILURE;
	}
	//�������ļ�+������
	if (group_name == NULL)
	{
		bool find_group = false;//�ҵ�����
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//���������û�лس�
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_groupname;
			int status = get_groupname(processed_str, ret_groupname);
			if (status == NOVALID)//����Ϣ��Ч,��Ϊ����
				continue;
			if (status == FINGOOD)//ֻҪ������������
				find_group = true;
		}
		if (find_group == true)//�˳����ҵ�����
			return FAILURE;
		this->fp.clear();//һ����fail�˳���
		//���ñ���λ��
		//������
		this->fp.seekg(0, ios::beg);//��ͷ�ٶ�
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//������û�лس�
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			//�ж��Ƿ����ظ��ļ���
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//���Ǽ�ֵ��
				continue;
			if (ret_key == string(item_name))//���ظ��ļ���
			{
				item_value = ret_value;
				return SUCCESS;
			}
		}
		//һ����fail�˳�
		return FAILURE;
	}
	return FAILURE;//�����ܴﵽ
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, char& item_value)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	if (ret == FAILURE)
		return FAILURE;
	stringstream ss;
	ss << buffer;
	ss >> item_value;
	return SUCCESS;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	return ret;
}
