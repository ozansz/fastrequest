#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "http.h"
#include "stringbuffer.h"
#include "api.h"
#include "utils.h"
#include "httpresponse.h"
#include "funcwrappers.h"

#include "structmember.h"

PyObject *json_module = NULL;

extern PyTypeObject HTTPResponseType;
extern PyTypeObject HTTPRequestType;

static PyMethodDef FastRequestHTTPMethods[] = {
    {"get", FastRequest_HttpGet, METH_VARARGS, "Fast GET request using libcurl4 library."},
    {"post", FastRequest_HttpPost, METH_VARARGS, "Fast POST request using libcurl4 library."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef fastrequesthttpmodule = {
    PyModuleDef_HEAD_INIT,
    "fastrequest",
    "A Python C extension module for fast HTTP requests with SSL/TLS support",
    -1,
    FastRequestHTTPMethods
};

PyMODINIT_FUNC PyInit_http(void) {
    PyObject *m;

    if (PyType_Ready(&HTTPResponseType) < 0)
        return NULL;

    if (PyType_Ready(&HTTPRequestType) < 0)
        return NULL;

    m = PyModule_Create(&fastrequesthttpmodule);
    
    if (m == NULL)
        return NULL;

    Py_INCREF(&HTTPResponseType);
    if (PyModule_AddObject(m, "HTTPResponse", (PyObject *) &HTTPResponseType) < 0) {
        Py_DECREF(m);
        Py_DECREF(&HTTPResponseType);
        return NULL;
    }

    Py_INCREF(&HTTPRequestType);
    if (PyModule_AddObject(m, "HTTPRequest", (PyObject *) &HTTPRequestType) < 0) {
        Py_DECREF(m);
        Py_DECREF(&HTTPRequestType);
        return NULL;
    }

    json_module = PyImport_ImportModule("json");

    if (json_module == NULL)
        return NULL;

    Py_INCREF(json_module);

    return m;
}
