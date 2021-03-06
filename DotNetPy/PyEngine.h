#pragma once
#include "Python.h"
#include <vector>
#include <algorithm>
#include <msclr/marshal_cppstd.h>

using namespace std;
using namespace System;
using namespace msclr::interop;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace DotNetPy {
	class Dict
	{
	public:
		Dict(void) {
			_Key = vector<string>();
			_Value = vector<PyObject*>();
		}
		~Dict(void) {}
		void Add(string key, PyObject *value) {
			_Key.push_back(key);
			_Value.push_back(value);
		}
		PyObject* Get(string key) {
			vector<string>::iterator iter = find(_Key.begin(), _Key.end(), key);
			if (iter != _Key.end()) {
				return _Value[iter - _Key.begin()];
			}
			else {
				return NULL;
			}
		}
	private:
		vector<string> _Key;
		vector<PyObject*> _Value;
	};

	public ref class PyEngine
	{
	public:
		PyEngine(void);
		~PyEngine(void);

		// 初始化
		bool PyInitialize(void);
		// 检查状态
		bool PyIsInitialized(void);
		// 释放
		void PyFinalize(void);
		// 设置模块路径
		void PySetModulePath(String ^path);
		// 执行代码
		void PyExecuteString(String ^code);
		// 导入模块
		bool PyImportModule(String ^moduleName);
		// 导入函数
		bool PyImportFunc(String ^moduleName, String ^funcName);
		// 调用函数
		bool PyCallFunc(String ^funcName);
		// 调用函数
		Object^ PyCallFuncFromModule(String ^moduleName, String ^funcName, Array ^args, String ^resType);
	private:
		Dict * _PyObjects = new Dict();
		// 参数类型转换 String^转std::string
		string String2string(String ^str);
		// 参数类型转换 char*转String^
		String^ Char2String(char *ch);
	};
}
