#include "stdafx.h"
#include "PyEngine.h"

namespace DotNetPy {
	PyEngine::PyEngine(void) {}
	PyEngine::~PyEngine(void) {}

	string PyEngine::String2string(String ^str) {
		string stdString = marshal_as<string>(str);
		return stdString;
	}

	String^ PyEngine::Char2String(char *ch) {
		String ^string = marshal_as<String^>(ch);
		return string;
	}

	bool PyEngine::PyInitialize(void) {
		Py_Initialize();
		if (Py_IsInitialized() == 0)
			return false;
		return true;
	}

	bool PyEngine::PyIsInitialized(void) {
		if (Py_IsInitialized() == 0)
			return false;
		return true;
	}

	void PyEngine::PyFinalize(void) {
		Py_Finalize();
	}

	void PyEngine::PySetModulePath(String ^path) {
		PyRun_SimpleString("import sys,os");
		if (path) {
			string pathCode = "sys.path.append(r'";
			string pathString = String2string(path);
			pathCode += pathString + "')";
			PyRun_SimpleString(pathCode.data());
		}
		else
		{
			PyRun_SimpleString("sys.path.append(os.getcwd())");
		}
	}

	void PyEngine::PyExecuteString(String ^code) {
		string codeString = String2string(code);
		PyRun_SimpleString(codeString.data());
	}

	bool PyEngine::PyImportModule(String ^moduleName) {
		string moduleNameString = String2string(moduleName);
		PyObject *pyModule = PyImport_ImportModule(moduleNameString.data());
		if (!pyModule)
			return false;
		_PyObjects->Add(moduleNameString, pyModule);
		return true;
	}

	bool PyEngine::PyImportFunc(String ^moduleName, String ^funcName) {
		string moduleNameString = String2string(moduleName);
		string funcNameString = String2string(funcName);
		PyObject *pyModule = PyImport_ImportModule(moduleNameString.data());
		PyObject *_PyFunc = PyObject_GetAttrString(pyModule, funcNameString.data());
		if (!_PyFunc)
			return false;
		_PyObjects->Add(funcNameString, pyModule);
		return true;
	}

	bool PyEngine::PyCallFunc(String ^funcName) {
		string funcNameString = String2string(funcName);
		PyObject *_PyFunc = _PyObjects->Get(funcNameString);
		if (_PyFunc == NULL)
			return false;
		return true;
	}

	Object^ PyEngine::PyCallFuncFromModule(String ^moduleName, String ^funcName, Array ^args, String ^resType) {
		string moduleNameString = String2string(moduleName);
		string funcNameString = String2string(funcName);
		PyObject *pyModule = _PyObjects->Get(moduleNameString);
		if (pyModule == NULL)
			return NULL;
		PyObject *pyDict = PyModule_GetDict(pyModule);
		if (!pyDict)
			return NULL;
		PyObject *pyFunc = PyDict_GetItemString(pyDict, funcNameString.data());
		if (!pyFunc)
			return NULL;
		PyObject *pyArgs;
		if (args) {

			int count = args->GetLength(0);
			pyArgs = PyTuple_New(count);
			for (int i = 0;i != count;i++) {
				Object ^argSource = args->GetValue(i);
				Type ^type = argSource->GetType();
				PyObject *pyArg;
				string strArg = String2string(argSource->ToString());
				if (type->ToString() == "System.String") {
					pyArg = Py_BuildValue("s", strArg.data());
				}
				else if (type->ToString() == "System.Int32")
				{
					pyArg = Py_BuildValue("i", stoi(strArg));
				}
				else if (type->ToString() == "System.Double")
				{
					pyArg = Py_BuildValue("d", stod(strArg));
				}
				PyTuple_SetItem(pyArgs, i, pyArg);
			}
		}
		else
		{
			pyArgs = NULL;
		}
		PyObject *pyRes = PyEval_CallObject(pyFunc, pyArgs);
		if (!pyRes)
			return NULL;
		string resTypeString = String2string(resType);
		if (resTypeString == "l" || resTypeString == "L") {
			long value = PyLong_AsLong(pyRes);
			return value;
		}
		else if (resTypeString == "s" || resTypeString == "S") {
			char *value = PyUnicode_AsUTF8(pyRes);
			return Char2String(value);
		}
		else if (resTypeString == "d" || resTypeString == "D") {
			double value = PyFloat_AsDouble(pyRes);
			return value;
		}
		return NULL;
	}
}