#pragma once

#include <stdlib.h>

#include "stringbuffer.h"

#define FR_LIBCURL_DEFAULT_UAGENT   "libcurl-agent/1.0"
#define FR_LIBCURL_HEADER_ITEM_SIZE 200
#define FR_GENERIC_ERRSTR_SIZE  100

#define FR_HTTP_POST_URLENCODED 0
#define FR_HTTP_POST_JSON   1

StringBuffer *FastRequestAPI_LibcurlHttpGet(char *url, PyObject *headers);
StringBuffer *FastRequestAPI_LibcurlHttpPost(char *url, const char *payload, int_fast8_t payload_encoding, PyObject *headers);

size_t _FastRequestAPI_LibcurlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata);
size_t _FastRequestAPI_LibcurlReadCallback(char *ptr, size_t size, size_t nmemb, void *userdata);