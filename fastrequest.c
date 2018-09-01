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
  char *url, *user_agent, *exception_msg;
  uint_fast8_t fast_get_retval;

  ChunkMem mem;

  PyObject *final_unicode;

  if (!PyArg_ParseTuple(args, "s|s", &url, &user_agent)) {
    return NULL;
  }

  // Initialize mem
  //mem.memory = malloc(1);
  mem.memory = PyMem_Malloc(1);
  mem.size = 0;

  fast_get_retval = FastGet_libcurl(url, user_agent, &mem);

  // An error occured
  if (fast_get_retval < 0) {
    FastGet_libcurl_format_exception(exception_msg, fast_get_retval);
    PyErr_SetString(PyExc_RuntimeError, exception_msg);
    return NULL;
  }

  final_unicode = PyUnicode_FromString(mem.memory);

  // Free heap
  free(mem.memory);

  return final_unicode;
}

static PyMethodDef FastRequestMethods[] = {
  {"get__libcurl", fastrequest__fast_get_libcurl, METH_VARARGS,
   "Fast GET request using libcurl4 library."},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef fastrequestmodule = {
  PyModuleDef_HEAD_INIT,
  "fastrequest",
  NULL, // No module docs,
  FastRequestMethods
};

PyMODINIT_FUNC PyInit_fastrequest(void)
{
  return PyModule_Create(&fastrequestmodule);
}
