#pragma once

#include <stdlib.h>

#include "stringbuffer.h"

#define FR_LIBCURL_DEFAULT_UAGENT   "libcurl-agent/1.0"
#define FR_LIBCURL_HEADER_ITEM_SIZE 200
#define FR_GENERIC_ERRSTR_SIZE  100

StringBuffer *FastRequestAPI_LibcurlHttpGet(char *url, PyObject *headers);
size_t _FastRequestAPI_LibcurlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata);