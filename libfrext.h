#pragma once

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  RETURN_SUCCESS = 0,
  GENERIC_ERROR = 1,
  FATAL_ERROR,
  OUT_OF_MEMORY,
  CURL_ERROR,
  ARG_URL_IS_NULL,
  ARG_UAGENT_IS_TOO_BIG
} FAST_GET_LIBCURL_RETURN_CODE;

typedef struct {
  char *memory;
  size_t size;
} ChunkMem;

size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

uint_fast8_t
FastGet_libcurl(const char *url, const char *user_agent, ChunkMem *mem);

uint_fast32_t
FastGet_libcurl_format_exception(char *str, uint_fast8_t err_code);

void __debug(const char *msg);

#ifdef __cplusplus
}
#endif
