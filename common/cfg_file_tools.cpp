#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include<sstream>
#include <io.h>
#include "../include/cfg_file_tools.h"
using namespace std;
//待解决windows linux 
//结尾没有换行问题
/* ---------------------------------------------------------------
	 允许加入其它需要static函数（内部工具用）
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
		//cout << "文件格式无法识别" << endl;
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
/*size是元素的个数*/
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
  函数名称：get_groupname
  功    能：处理process_line 返回的行
  输入参数：processed_str 读入行 **=** ，ret_key ret_value 返回处理后的键值对
  返 回 值：是否为有效键值对 NOVALID 无效 FINGOOD 有效
  说    明：如有必要，按需加入其它内容
***************************************************************************/
static int get_key_value(const string processed_str, string& ret_key, string& ret_value)
{
	size_t pos = processed_str.find('=');
	if (pos == processed_str.npos)
		return NOVALID;
	ret_key = processed_str.substr(0, pos);
	ret_value = processed_str.substr(pos + 1);
	//ret_key处理后面的空格\t
	size_t len = ret_key.length();
	for (int i = (int)len - 1; i >= 0; i--)
	{
		if (ret_key[i] == ' ' || ret_key[i] == '\t')
			ret_key.erase(i);
		else //if (ret_key[i] != ' ' && ret_key[i] != '\t')
			break;
	}
	//ret_value处理前面的空格
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
  函数名称：get_groupname
  功    能：处理process_line 返回的行
  输入参数：processed_str 读入行[***]，ret_gpname 返回处理后的串
  返 回 值：是否为有效组名 NOVALID 无效 FINGOOD 有效
  说    明：如有必要，按需加入其它内容
***************************************************************************/
static int get_groupname(const string processed_str, string& ret_gpname)
{
	if (processed_str[0] != '[' || processed_str[processed_str.length() - 1] != ']')
		return NOVALID;
	ret_gpname = processed_str;
	//删除[]
	ret_gpname.erase(0, 1);
	ret_gpname.erase(ret_gpname.length() - 1, 1);
	//处理前面的空格
	size_t len = ret_gpname.length();
	if (len <= 0)//什么也没有时
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
	//去除后空格
	len = ret_gpname.length();
	if (len <= 0)//只有空格时
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
  函数名称：process_line
  功    能：处理getline 读入的行
  输入参数：raw_str 读入行，processed_str 返回处理后的串
  返 回 值：无
  说    明：有效内容：可能为空串
***************************************************************************/
void CFT::process_line(string &raw_str, string& processed_str)
{
	
	//去掉\r
	if (int(raw_str.length()) - 1 >= 0)
	{
		if (this->ENDL == "\r\n" && raw_str[raw_str.length() - 1] == '\r')
		{
			raw_str.erase(raw_str.length() - 1);
		}
	}
	processed_str = raw_str;
	//去掉注释
	size_t pos = processed_str.find('#');
	if (pos != processed_str.npos)
		processed_str.erase(pos);
	pos = processed_str.find(';');
	if (pos != processed_str.npos)
		processed_str.erase(pos);

	//去掉前面空格\t
	size_t len = processed_str.length();
	if (len <= 0)//什么也没有时
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

	//去除后空格
	len = processed_str.length();
	if (len <= 0)//只能取到注释时
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：如有必要，按需加入其它内容
***************************************************************************/
CFT::CFT()
{
	filename = "";
	this->ENDL = "\r\n";
	this->rw = READ;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：如有必要，按需加入其它内容
***************************************************************************/
CFT::~CFT()
{
	if (fp.is_open())
		fp.close();
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
		this->fp.open(cfgname, ios::in | ios::binary);//不写不用check
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
			this->fp.open(cfgname, ios::in | ios::out | ios::trunc | ios::binary);//新建不用check
	}
	return;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void CFT::open(const char* cfgname, int opt)
{
	this->filename = cfgname;
	if (this->fp.is_open())
		this->fp.close();
	if (opt == OPEN_OPT_RDONLY)
	{
		this->rw = READ;
		this->fp.open(cfgname, ios::in | ios::binary);//不写不用check
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
			this->fp.open(cfgname, ios::in | ios::out | ios::trunc | ios::binary);//新建不用check
	}
	return;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：打开成功返回true，否则返回false
***************************************************************************/
bool CFT::is_open(void)
{
	if (this->fp.is_open())
		return true;
	else
		return false;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void CFT::close()
{
	if (fp.is_open())
		fp.close();
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：C++17中有filesystem::resize_file直接可以修改文件大小，之前的版本无
				   因此本函数扔沿用C的旧方法
		已实现，不要动
***************************************************************************/
int CFT::file_resize(int newsize)
{
	/* 如果大小是负数则直接返回-1 */
	if (newsize < 0)
		return -1;

	if (!fp.good())
		fp.clear();

	/* 取fp当前指针位置 */
	int now_pos = int(fp.tellg());

	/* 首先：C++方式关闭 */
	this->close();

	/* 其次：C方式打开并改变文件大小 */
	FILE* fp_cstyle;
	fp_cstyle = fopen(filename.c_str(), "rb+"); //rw方式打开
	if (fp_cstyle) {
		_chsize(_fileno(fp_cstyle), newsize);
		fclose(fp_cstyle);
	}
	else
		cout << "open [" << filename << "] for resize failed" << endl;

	/* 再次：C++方式重新打开 */
	fp.open(filename, ios::in | ios::out | ios::binary);
	if (fp.is_open() == 0) {
		cout << "reopen [" << filename << "] after resize failed" << endl;
		return -1;
	}

	/* 文件指针移动到原位置 */
	fp.seekg(now_pos < newsize ? now_pos : newsize, ios::beg);

	return 0;
}

/* ---------------------------------------------------------------
	 允许AAT中自定义成员函数的实现（protected）
   ---------------------------------------------------------------- */

   /***************************************************************************
	 函数名称：
	 功    能：
	 输入参数：
	 返 回 值：
	 说    明：
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
		if (this->fp.fail() && raw_str == "")//最后没有回车不考虑了
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
		if (status == NOVALID)//行信息无效,不为组名
			continue;
		if (ret_groupname == group_name)
			return FAILURE;
	}
	//一定是fail退出
	this->fp.clear();
	int start = 0;
	if (this->fp.tellg() == (streampos)0)
		start = 1;
	this->fp.seekp(0, ios::end);
	if (start == 0)
		this->fp << this->ENDL;
	this->fp << '[' << group_name << ']'<< this->ENDL;
	if (this->fp.good())
		return SUCCESS; //此句根据需要修改
	else
		return FAILURE;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
		cout << "内存申请失败" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	string old_ret_gpname = "#";
	//存入数组
	while (1)
	{
		string temp_string;
		getline(this->fp, temp_string);
		if (this->fp.fail() && temp_string == "")//最后没有回车不考虑了
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
				gpnum++;//只有在有效的时候才是删了一个组
			continue;//不赋值就是删除
		}
		all_file[i] = temp_string;
		i++;
		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "内存申请失败" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
	}
	//一定是fail退出
	this->fp.clear();
	//文件输出
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	return gpnum; //此句根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name, const char* item_value)
{
	int ret = this->item_add(group_name, item_name, string(item_value));
	return ret;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
		cout << "内存申请失败" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	bool flag_change_line = false;
	//有组名
	if (group_name != NULL)
	{
		bool find_group = false;//是否找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
					raw_str.append(this->ENDL);
					raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//末尾退出，未找到
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
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}

		if (find_group == false)//退出，没找到组名
		{
			delete[] all_file;
			return FAILURE;
		}

		streampos pos_file = this->fp.tellg();//保存文件中位值

		while (1)//找到了组名
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

			//判断到了下一组
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
			{
				delete[] all_file;
				return FAILURE;
			}
		}
		this->fp.clear();
		this->fp.seekg(pos_file, ios::beg);

		//写入添加信息
		string tmp;
		stringstream stmp;
		stmp << item_name << "=" << item_value;//前面不用回车
		tmp = stmp.str();

		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "内存申请失败" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
		all_file[i] = tmp;
		i++;

		while (1)
		{
			//写入数组
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}

			//去掉\r
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
					cout << "内存申请失败" << endl;
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
	//简单配置文件+无组名
	if (group_name == NULL)
	{
		bool find_group = false;//找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str == "")//不考虑最后没有回车
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
			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (status == FINGOOD)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == true)//退出，找到组名
		{
			delete[] all_file;
			return FAILURE;
		}
		this->fp.clear();//一定是fail退出的
		//不用保存位置
		//简单配置
		this->fp.seekg(0, ios::beg);//从头再读
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//不考虑没有回车
				break;

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
			{
				delete[]all_file;
				return FAILURE;
			}
		}
		//一定是fail退出
		this->fp.clear();

		//写入添加信息
		string tmp;
		stringstream stmp;
		stmp << item_name << "=" << item_value;//前面不用回车
		tmp = stmp.str();

		//写入数组
		if (all_file_actual_len <= i)
		{
			string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
			if (temp == NULL)
			{
				cout << "内存申请失败" << endl;
				return FAILURE;
			}
			all_file_actual_len += SIZE_INCREMENT;
			all_file = temp;
		}
		all_file[i] = tmp;
		i++;

		//return SUCCESS;
	}
	//一定是fail退出
	this->fp.clear();
	//文件输出
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	if (this->fp.good())
		return SUCCESS; //此句根据需要修改
	else
		return FAILURE;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：单字符不能是空格和回车
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_add(const char* group_name, const char* item_name)
{
	string item_empty = "";
	int ret = this->item_add(group_name, item_name, item_empty);
	return ret;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
		cout << "内存申请失败" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	//有组名
	if (group_name != NULL)
	{
		string old_ret_gpname = "#";
		//存入数组
		while (1)
		{
			string temp_string;
			getline(this->fp, temp_string);
			if (this->fp.fail() && temp_string == "")//最后没有回车不考虑了
				break;
			if (this->fp.fail() && temp_string != "")
			{
				temp_string.append(this->ENDL);
				temp_string.erase(temp_string.length() - 1);
			}

			string processed_str;
			process_line(temp_string, processed_str);
			string ret_gpname, ret_key, ret_value;
			//检查是否为组名
			int status = get_groupname(processed_str, ret_gpname);
			//检查是否为键值对
			int is_key_value = get_key_value(processed_str, ret_key, ret_value);
			//检查组名是否更改
			if (status == NOVALID)
				ret_gpname = old_ret_gpname;
			else//VALID
				old_ret_gpname = ret_gpname;
			//检查键值对是否符合要求
			if (group_name == ret_gpname && is_key_value == FINGOOD && ret_key == item_name)
			{
				gpnum++;
				continue;//组名，得到键，键，三项相同，删除
			}
			all_file[i] = temp_string;
			i++;
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
		}
	}
	else//group_name == NULL
	{
		//存入数组
		while (1)
		{
			string temp_string;
			getline(this->fp, temp_string);
			if (this->fp.fail() && temp_string == "")//最后没有回车不考虑了
				break;
			if (this->fp.fail() && temp_string != "")
			{
				temp_string.append(this->ENDL);
				temp_string.erase(temp_string.length() - 1);
			}

			string processed_str;
			process_line(temp_string, processed_str);
			string ret_key, ret_value;
			//检查是否为键值对
			int is_key_value = get_key_value(processed_str, ret_key, ret_value);
			//检查键值对是否符合要求
			if (is_key_value == FINGOOD && ret_key == item_name)
			{
				gpnum++;
				continue;//得到键，键，两项相同，删除
			}
			all_file[i] = temp_string;
			i++;
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
		}
	}
	//一定是fail退出
	this->fp.clear();
	//文件输出
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file;
	return gpnum; //此句根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name, const char* item_value)
{
	int ret = this->item_update(group_name, item_name, string(item_value));
	return ret;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
	stmp << item_name << "=" << item_value;//前面不用回车
	tmp=stmp.str();

	string* all_file = new(nothrow) string[INIT_SIZE]{};
	if (all_file == NULL)
	{
		cout << "内存申请失败" << endl;
		return FAILURE;
	}
	int all_file_actual_len = INIT_SIZE;
	int i = 0;
	bool flag_change_line = false;
	//有组名
	if (group_name != NULL)
	{
		bool find_group = false;//是否找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);

			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			if (this->fp.fail() && raw_str == "")//末尾退出，未找到
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
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}

		if (find_group == false)//退出，没找到组名
		{
			delete[] all_file;
			return FAILURE;
		}

		streampos pos_file = this->fp.tellg();//保存文件中位值
		bool find_key = false;
		int same_key_in_arr = 0;
		while (1)//找到了组名
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

			//判断到了下一组
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
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
					cout << "内存申请失败" << endl;
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
			//写入数组
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
					cout << "内存申请失败" << endl;
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
				//判断是否有重复的键名
				int status = get_key_value(processed_str, ret_key, ret_value);
				if (status == NOVALID)//不是键值对
					continue;
				if (ret_key == string(item_name) && find_key)//有重复的键名
				{
					i--;
					find_key = false;
					if (all_file_actual_len <= i)
					{
						string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
						if (temp == NULL)
						{
							cout << "内存申请失败" << endl;
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
	//简单配置文件+无组名
	if (group_name == NULL)
	{
		bool find_group = false;//找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			
			if (this->fp.fail() && raw_str == "")//不考虑最后没有回车
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
			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (status == FINGOOD)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == true)//退出，找到组名
		{
			delete[]all_file;
			return FAILURE;
		}
		this->fp.clear();//一定是fail退出的
		//不用保存位置
		//简单配置
		this->fp.seekg(0, ios::beg);//从头再读
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
			if (this->fp.fail() && raw_str == "")//不考虑没有回车
				break;

			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = raw_str;
			i++;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
			{
				i--;
				if (all_file_actual_len <= i)
				{
					string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
					if (temp == NULL)
					{
						cout << "内存申请失败" << endl;
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
		//一定是fail退出
		this->fp.clear();

		//写入数组
		if (!find_key)
		{
			if (all_file_actual_len <= i)
			{
				string* temp = renew(all_file, all_file_actual_len, all_file_actual_len + SIZE_INCREMENT);
				if (temp == NULL)
				{
					cout << "内存申请失败" << endl;
					return FAILURE;
				}
				all_file_actual_len += SIZE_INCREMENT;
				all_file = temp;
			}
			all_file[i] = tmp;
			i++;
		}
	}
	//一定是fail退出
	this->fp.clear();
	//文件输出
	this->file_resize(0);
	this->fp.seekp(0, ios::beg);
	for (int j = 0; j < i; j++)
	{
		this->fp << all_file[j] << this->ENDL;
	}
	delete[]all_file; 
	if (this->fp.good())
		return SUCCESS; //此句根据需要修改
	else
		return FAILURE; return SUCCESS;//不可能达到
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_update(const char* group_name, const char* item_name)
{
	string item_empty = "";
	int ret = this->item_update(group_name, item_name, item_empty);
	return ret;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name, string& item_value)
{
	if (!this->fp.is_open())
		return FAILURE;
	this->fp.clear();
	this->fp.seekg(0, ios::beg);
	//有组名
	if (group_name != NULL)
	{
		bool find_group = false;//是否找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//末尾退出，未找到
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
			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (ret_groupname == group_name)
			{
				find_group = true;
				break;
			}
		}
		if (find_group == false)//退出，没找到组名
			return FAILURE;
		while (1)//找到了组名
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

			//判断到了下一组
			int is_next_group = get_groupname(processed_str, next_gpname);
			if (is_next_group == FINGOOD)
				break;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
			{
				item_value = ret_value;
				return SUCCESS;
			}
		}
		return FAILURE;
	}
	//简单配置文件+无组名
	if (group_name == NULL)
	{
		bool find_group = false;//找到组名
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//不考虑最后没有回车
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
			if (status == NOVALID)//行信息无效,不为组名
				continue;
			if (status == FINGOOD)//只要是组名都不行
				find_group = true;
		}
		if (find_group == true)//退出，找到组名
			return FAILURE;
		this->fp.clear();//一定是fail退出的
		//不用保存位置
		//简单配置
		this->fp.seekg(0, ios::beg);//从头再读
		while (1)
		{
			string raw_str;
			getline(this->fp, raw_str);
			if (this->fp.fail() && raw_str == "")//不考虑没有回车
				break;
			if (this->fp.fail() && raw_str != "")
			{
				raw_str.append(this->ENDL);
				raw_str.erase(raw_str.length() - 1);
			}
			string processed_str;
			process_line(raw_str, processed_str);
			string ret_key, ret_value, next_gpname;

			//判断是否有重复的键名
			int status = get_key_value(processed_str, ret_key, ret_value);
			if (status == NOVALID)//不是键值对
				continue;
			if (ret_key == string(item_name))//有重复的键名
			{
				item_value = ret_value;
				return SUCCESS;
			}
		}
		//一定是fail退出
		return FAILURE;
	}
	return FAILURE;//不可能达到
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
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
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int CFT::item_get_value(const char* group_name, const char* item_name)
{
	string buffer;
	int ret = this->item_get_value(group_name, item_name, buffer);
	return ret;
}
