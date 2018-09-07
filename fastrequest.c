#include <Python.h>

// stdio.h and stdlib.h are need to be included in Python.h by default.
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

#include "fastrequest.h"
#include "libfrext.h"
#include "frmodconfig.h"

static PyObject *
fastrequest__fast_get_libcurl(PyObject *self, PyObject *args)
{
  __debug("[CALL] fastrequest__fast_get_libcurl");

  const char *url, *user_agent;
  char exception_msg[EXCEPTION_MSG_MAX_SIZE];
  char unicode_encoding[] = "utf8";
  char unicode_decode_err_hnd[] = "ignore";

  int_fast8_t fast_get_retval;

  ChunkMem mem;

  PyObject *final_unicode = _PyObject_New(&PyUnicode_Type);

  __debug("[PRECALL] PyArg_ParseTuple");
  if (!PyArg_ParseTuple(args, "s|s", &url, &user_agent)) {
    return NULL;
  }

  __debug("[PRECALL] PyMem_Malloc");
  // Initialize mem
  mem.memory = PyMem_Malloc(1);
  mem.size = 0;

  __debug("[PRECALL] FastGet_libcurl");
  fast_get_retval = FastGet_libcurl(url, user_agent, &mem);

  // An error occured
  if (fast_get_retval < 0) {
    if (fast_get_retval != -4) {
      __debug("[PRECALL] FastGet_libcurl_format_exception");
      FastGet_libcurl_format_exception(exception_msg, fast_get_retval);
      __debug("[PRECALL] PyErr_SetString");
      PyErr_SetString(PyExc_RuntimeError, exception_msg);
    }
    return NULL;
  }

  printf("~~memory: %s\n", mem.memory);

  __debug("[PRECALL] PyUnicode_Decode");
  final_unicode = PyUnicode_Decode(mem.memory, mem.size * sizeof(char), unicode_encoding, unicode_decode_err_hnd);

  __debug("[PRECALL] PyMem_Free");
  // Free heap
  PyMem_Free(mem.memory);


  return final_unicode;
}

static PyObject *
fastrequest__fast_get_sock(PyObject *self, PyObject *args)
{
  __debug("[CALL] fastrequest__fast_get_sock");

  const char *host, *path;

  int_fast32_t retval;

  ChunkMem mem;

  __debug("[PRECALL] _PyObject_New");
  PyObject *final_unicode = _PyObject_New(&PyUnicode_Type);

  __debug("[PRECALL] PyArg_ParseTuple");
  if (!PyArg_ParseTuple(args, "ss", &host, &path)) {
    return NULL;
  }

  __debug("[PRECALL] PyMem_Malloc");
  mem.memory = PyMem_Malloc(1);
  mem.size = 0;

  __debug("[PRECALL] FastGet_sock");
  retval = FastGet_sock(host, path, &mem);

  printf("~~FastGet_sock returned %ld\n", retval);

  // If retval is negative that means there is an error raised.
  // PyErr_SetString must have already been called, so we will just
  // return NULL here. And that will raise the error specified.
  if (retval < 0) {
    printf("[!] retval: %ld\n", retval);
    return NULL;
  }

  __debug("[PRECALL] PyUnicode_FromString");
  final_unicode = PyUnicode_FromString(mem.memory);

  __debug("[PRECALL] PyMem_Free");
  PyMem_Free(mem.memory);

  return final_unicode;

}

static PyObject *
fastrequest__get(PyObject *self, PyObject *args)
{
  return fastrequest__fast_get_libcurl(self, args);
}

static PyMethodDef FastRequestMethods[] = {
  {"get__libcurl", fastrequest__fast_get_libcurl, METH_VARARGS,
   "Fast GET request using libcurl4 library."},
  {"get__socket", fastrequest__fast_get_sock, METH_VARARGS,
   "Fast GET request using UNIX sockets."},
  {"get", fastrequest__get, METH_VARARGS, "Base GET request bind"},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef fastrequestmodule = {
  PyModuleDef_HEAD_INIT,
  "fastrequest",
  "A Python C extension module for fast HTTP requests with SSL/TLS support",
  -1,
  FastRequestMethods
};

PyMODINIT_FUNC PyInit_fastrequest(void)
{
  __debug("[CALL] PyInit_fastrequest");
  __debug("[PRECALL] PyModule_Create");
  return PyModule_Create(&fastrequestmodule);
}
