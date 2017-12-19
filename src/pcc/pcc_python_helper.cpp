
#include "pcc_python_helper.h"

PccPythonHelper::PccPythonHelper(const std::string& python_filename) {
    Py_Initialize();
    PySys_SetArgv(Options::argc, Options::argv);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/users/njay2\")");
    PyRun_SimpleString("sys.path.append(\"/home/njay2/PCC/clean_ppc2/pcc/src\")");
    PyObject* module_name = PyString_FromString(python_filename.c_str());
    module = PyImport_Import(module_name);
    if (module == NULL) {
        std::cerr << "ERROR: Could not load python module: " << python_filename << std::endl;
        PyErr_Print();
        exit(-1);
    }
    Py_DECREF(module_name);
    give_sample_func = PyObject_GetAttrString(module, "give_sample");
    if (give_sample_func == NULL) {
        std::cerr << "ERROR: Could not load python function: give_sample" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    get_rate_func = PyObject_GetAttrString(module, "get_rate");
    if (get_rate_func == NULL) {
        std::cerr << "ERROR: Could not load python function: get_rate" << std::endl;
        PyErr_Print();
        exit(-1);
    }
}

PccPythonHelper::~PccPythonHelper() {
    PyObject* save_model_func = PyObject_GetAttrString(module, "save_model");
    if (save_model_func == NULL) {
        std::cerr << "ERROR: Could not load python function: save_model" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    PyObject_CallObject(save_model_func, NULL);
    Py_DECREF(save_model_func);
    Py_DECREF(give_sample_func);
    Py_DECREF(get_rate_func);
    Py_DECREF(module);
    Py_Finalize();
}

void PccPythonHelper::GiveSample(double sending_rate, double latency, double loss_rate, double latency_inflation, double utility) {
    PyObject* args = PyTuple_New(5);
    PyObject* sending_rate_value = PyFloat_FromDouble(sending_rate);
    PyObject* latency_value = PyFloat_FromDouble(latency);
    PyObject* loss_rate_value = PyFloat_FromDouble(loss_rate);
    PyObject* latency_inflation_value = PyFloat_FromDouble(latency_inflation);
    PyObject* utility_value = PyFloat_FromDouble(utility);
    PyTuple_SetItem(args, 0, sending_rate_value);
    PyTuple_SetItem(args, 1, latency_value);
    PyTuple_SetItem(args, 2, loss_rate_value);
    PyTuple_SetItem(args, 3, latency_inflation_value);
    PyTuple_SetItem(args, 4, utility_value);
    PyObject* result = PyObject_CallObject(give_sample_func, args);
    Py_DECREF(sending_rate_value);
    Py_DECREF(latency_value);
    Py_DECREF(loss_rate_value);
    Py_DECREF(latency_inflation_value);
    Py_DECREF(utility_value);
    Py_DECREF(args);
    Py_DECREF(result);
}

double PccPythonHelper::GetRate() {
    PyObject* result = PyObject_CallObject(get_rate_func, NULL);
    double result_double = PyFloat_AS_DOUBLE(result);
    Py_DECREF(result);
    return result_double;
}
