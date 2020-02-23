#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "http.h"
#include "stringbuffer.h"
#include "api.h"
#include "utils.h"
#include "httpresponse.h"

extern PyTypeObject HTTPResponseType;
extern PyObject *json_module;

PyObject *FastRequest_HttpGet(PyObject *self, PyObject *args) {
    char *url;

    StringBuffer *strbuf;

    PyObject *headers = NULL;
    PyObject *response_args, *response, *res_data, *res_size, *req_url;

    if (!PyArg_ParseTuple(args, "s|O", &url, &headers)) {
        return NULL;
    }

    if (headers != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpGet", "<ARG> Headers is non-null");

        if (!PyDict_Check(headers)) {
            PyErr_SetString(PyExc_TypeError, "\'headers\' argument must be type of dict.");
            return NULL;
        }

        Py_INCREF(headers);
    } else {
        FastRequest_FuncDebug("FastRequest_HttpGet", "<ARG> Headers is null");
    }

    strbuf = FastRequestAPI_LibcurlHttpGet(url, headers);

    if (strbuf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "FastRequestAPI_LibcurlHttpGet response strbuf got NULL");
        return NULL;
    }

    StringBuffer_PushChar(strbuf, '\0');

    res_data = PyBytes_FromString(strbuf->buf);    
    res_size = PyLong_FromLongLong((long long)(strbuf->index));
    req_url = PyUnicode_FromString(url);

    if (res_data == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_data PyObject got NULL");
        return NULL;
    }

    if (res_size == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_size PyObject got NULL");
        return NULL;
    }

    if (req_url == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "req_url PyObject got NULL");
        return NULL;
    }

    response_args = PyTuple_Pack(3, res_data, res_size, req_url);

    if (response_args == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response_args PyObject got NULL");
        return NULL;
    }

    response = PyObject_CallObject((PyObject *) &HTTPResponseType, response_args);

    Py_DECREF(response_args);
    Py_DECREF(res_data);
    Py_DECREF(res_size);
    Py_DECREF(req_url);

    StringBuffer_Free(strbuf);

    if (response == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response PyObject got NULL");
        return NULL;
    }

    return response;
}

PyObject *FastRequest_HttpPost(PyObject *self, PyObject *args) {
    char *url;
    const char *payload_str;

    int_fast8_t payload_encoding = FR_HTTP_POST_URLENCODED;

    StringBuffer *strbuf;

    PyObject *headers = NULL, *payload = NULL, *payload_json_dump, *json_dumps;
    PyObject *json_dumps_call_args, *payload_repr;
    PyObject *response_args, *response, *res_data, *res_size, *req_url;

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Initializing arguments");

    if (!PyArg_ParseTuple(args, "s|OO", &url, &payload, &headers)) {
        return NULL;
    }

    if (payload != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is given");

        if (PyDict_Check(payload)) {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is dictionary");

            json_dumps = PyObject_GetAttrString(json_module, "dumps");

            if (json_dumps == NULL) {
                PyErr_SetString(PyExc_AttributeError, "json.dumps");
                return NULL;
            }

            json_dumps_call_args = PyTuple_Pack(1, payload);
            payload_json_dump = PyObject_Call(json_dumps, json_dumps_call_args, NULL);

            payload_str = PyUnicode_AsUTF8(payload_json_dump);
            payload_encoding = FR_HTTP_POST_JSON;

            Py_DECREF(json_dumps_call_args);
            Py_DECREF(json_dumps);
            Py_DECREF(payload_json_dump);
        } else if (PyUnicode_Check(payload)) {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is string");

            payload_str = PyUnicode_AsUTF8(payload);
        } else {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is not dictionary nor string, will use repr");

            payload_repr = PyObject_Repr(payload);
            payload_str = PyUnicode_AsUTF8(payload_repr);

            Py_DECREF(payload_repr);
        }
    } else {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is NULL");

        payload_str = NULL;
    }

    if (headers != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Headers is given");

        if (!PyDict_Check(headers)) {
            PyErr_SetString(PyExc_TypeError, "\'headers\' argument must be type of dict.");
            return NULL;
        }

        Py_INCREF(headers);
    } else {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Headers is NULL");
    }

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Making the POST request");
    strbuf = FastRequestAPI_LibcurlHttpPost(url, payload_str, payload_encoding, headers);

    if (strbuf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "FastRequestAPI_LibcurlHttpGet response strbuf got NULL");
        return NULL;
    }

    StringBuffer_PushChar(strbuf, '\0');

    res_data = PyBytes_FromString(strbuf->buf);    
    res_size = PyLong_FromLongLong((long long)(strbuf->index));
    req_url = PyUnicode_FromString(url);

    if (res_data == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_data PyObject got NULL");
        return NULL;
    }

    if (res_size == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_size PyObject got NULL");
        return NULL;
    }

    if (req_url == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "req_url PyObject got NULL");
        return NULL;
    }

    response_args = PyTuple_Pack(3, res_data, res_size, req_url);

    if (response_args == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response_args PyObject got NULL");
        return NULL;
    }

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Creating response object");
    response = PyObject_CallObject((PyObject *) &HTTPResponseType, response_args);

    Py_DECREF(response_args);
    Py_DECREF(res_data);
    Py_DECREF(res_size);
    Py_DECREF(req_url);

    StringBuffer_Free(strbuf);

    if (response == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response PyObject got NULL");
        return NULL;
    }

    return response;
}