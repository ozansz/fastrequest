#include <Python.h>

#include "httpresponse.h"
#include "structmember.h"

extern PyObject *json_module;

static void HTTPResponse_dealloc(HTTPResponseObject *self) {
    Py_XDECREF(self->response_data);
    Py_XDECREF(self->response_size);
    Py_XDECREF(self->request_url);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *HTTPResponse_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HTTPResponseObject *self;
    self = (HTTPResponseObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->response_data = PyBytes_FromString("");

        if (self->response_data == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        
        self->response_size = PyLong_FromLong(0);
        
        if (self->response_size == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        
        self->request_url = PyUnicode_FromString("");
        
        if (self->request_url == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }

    return (PyObject *) self;
}

static int HTTPResponse_init(HTTPResponseObject *self, PyObject *args, PyObject *kwds) {
    PyObject *data = NULL, *size = NULL, *url = NULL, *tmp;

    if (!PyArg_ParseTuple(args, "OO|O", &data, &size, &url))
        return -1;

    if (data) {
        tmp = self->response_data;
        Py_INCREF(data);
        self->response_data = data;
        Py_XDECREF(tmp);
    }
    
    if (size) {
        tmp = self->response_size;
        Py_INCREF(size);
        self->response_size = size;
        Py_XDECREF(tmp);
    }

    if (url) {
        tmp = self->request_url;
        Py_INCREF(url);
        self->request_url = url;
        Py_XDECREF(tmp);
    }

    return 0;
}

static PyObject *HTTPResponse_text(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->response_data == NULL) {
        PyErr_SetString(PyExc_AttributeError, "data");
        return NULL;
    }

    return PyUnicode_FromEncodedObject(self->response_data, "utf-8", NULL);
}

static PyObject *HTTPResponse_json(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    PyObject *json_loads, *call_ret, *call_args;

    if (self->response_data == NULL) {
        PyErr_SetString(PyExc_AttributeError, "data");
        return NULL;
    }

    if (json_module == NULL) {
        PyErr_SetString(PyExc_ImportError, "JSON module was not imported");
        return NULL;
    }

    json_loads = PyObject_GetAttrString(json_module, "loads");

    if (json_loads == NULL) {
        PyErr_SetString(PyExc_AttributeError, "json.loads");
        return NULL;
    }

    call_args = PyTuple_Pack(1, self->response_data);
    call_ret = PyObject_Call(json_loads, call_args, NULL);

    Py_DECREF(call_args);
    Py_DECREF(json_loads);

    return call_ret;
}

static PyObject *HTTPResponse_get_default_encoding(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    return PyUnicode_FromString("utf-8");
}

static PyObject *HTTPResponse_repr(HTTPResponseObject *self) {
    return PyUnicode_FromFormat("<fastrequest.HTTPResponse (%S)>", self->request_url);
}

static PyObject *HTTPResponse_str(HTTPResponseObject *self) {
    return HTTPResponse_text(self, NULL);
}

static PyMethodDef HTTPResponse_methods[] = {
    {"text", (PyCFunction) HTTPResponse_text, METH_NOARGS,
     "Get the string value of the HTTP response"
    },
    {"json", (PyCFunction) HTTPResponse_json, METH_NOARGS,
     "Get the string value of the HTTP response"
    },
    {"get_default_encoding", (PyCFunction) HTTPResponse_get_default_encoding,
    METH_CLASS | METH_NOARGS, "Get the default encoding type"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef HTTPResponse_members[] = {
    {"size", T_OBJECT_EX, offsetof(HTTPResponseObject, response_size), 0,
     "response size"},
    {"data", T_OBJECT_EX, offsetof(HTTPResponseObject, response_data), 0,
     "response data"},
    {"url", T_OBJECT_EX, offsetof(HTTPResponseObject, request_url), 0,
     "request url"},
    {NULL}  /* Sentinel */
};

PyTypeObject HTTPResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fastrequest.http.HTTPResponse",
    .tp_doc = "HTTP response object",
    .tp_basicsize = sizeof(HTTPResponseObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = HTTPResponse_new,
    .tp_init = (initproc) HTTPResponse_init,
    .tp_dealloc = (destructor) HTTPResponse_dealloc,
    .tp_members = HTTPResponse_members,
    .tp_methods = HTTPResponse_methods,
    .tp_repr = (reprfunc) HTTPResponse_repr,
    .tp_str = (reprfunc) HTTPResponse_str
};