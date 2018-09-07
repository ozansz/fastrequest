#include "libfrext.h"
#include "frmodconfig.h"

#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <curl/curl.h>

#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>


void __debug(const char *msg)
{
  if (DEBUG) {
    printf("\e[1;94m[DEBUG]\e[0m %s\n", msg);
  }
}

size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  __debug("[CALL] WriteMemoryCallback");

  size_t realsize = size * nmemb;
  ChunkMem *mem = (ChunkMem *)userp;

  printf("I  mem.size = %ld\n", mem->size);

  __debug("[PRECALL] PyMem_Realloc");
  mem->memory = PyMem_Realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    // Because we're going to use this library in a Python extension,
    // explicit printf calls are inappropriate.
    //printf("[!] Not enough memory (code: %d)\n", OUT_OF_MEMORY);
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  printf("O  mem.size = %ld\n", mem->size);

  return realsize;
}

int_fast8_t
FastGet_libcurl(const char *url, const char *user_agent, ChunkMem *mem)
{
  __debug("[CALL] FastGet");

  CURL *curl_handle;
  CURLcode res;

  char uagent[UAGENT_MAX_SIZE];
  char curl_err_string[EXCEPTION_MSG_MAX_SIZE];

  __debug("Checking if arg:url is NULL");
  if (url == NULL || strlen(url) == 0) {
    return -ARG_URL_IS_NULL;
  }

  bzero((char *) &uagent, UAGENT_MAX_SIZE);

  __debug("Checking if arg:user_agent is NULL or empty");
  if ((user_agent == NULL) || (user_agent == "") || (strlen(user_agent) == 0)) {
    __debug("arg:user_agent is NULL or empty, using DEFAULT_UAGENT instead");
    strcpy(uagent, DEFAULT_UAGENT);
  }
  else {
    __debug("Checking if arg:user_agent is too big");
    if (strlen(user_agent) >= UAGENT_MAX_SIZE) {
      return -ARG_UAGENT_IS_TOO_BIG;
    }
    __debug("arg:user_agent is not NULL nor empty");
    strcpy(uagent, user_agent);
  }

  __debug("CURL init");
  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();

  __debug("Setting CURL opts");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) mem);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, uagent);

  __debug("CURL hnd process is starting");
  res = curl_easy_perform(curl_handle);

  __debug("Checking if CURL has returned any error");
  if(res != CURLE_OK) {
    sprintf(curl_err_string, "Generic CURL error: %s", curl_easy_strerror(res));
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, curl_err_string);
    return -CURL_ERROR;
  }

  __debug("CURL cleanup");
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return RETURN_SUCCESS;
}

int_fast32_t
FastGet_libcurl_format_exception(char *str, int_fast8_t err_code)
{
  __debug("[CALL] FastGet_libcurl_format_exception");

  char err_str[ERR_STR_MAX_SIZE] = {0};

  switch (err_code) {
    case -OUT_OF_MEMORY:
      strcpy(err_str, EC_OUT_OF_MEMORY);
      break;
    case -CURL_ERROR:
      strcpy(err_str, EC_CURL_ERROR);
      break;
    case -ARG_URL_IS_NULL:
      strcpy(err_str, EC_ARG_URL_IS_NULL);
      break;
    case -ARG_UAGENT_IS_TOO_BIG:
      strcpy(err_str, EC_ARG_UAGENT_TOO_BIG);
      break;
    default:
      strcpy(err_str, "Unknown error");
  }

  __debug("[PRECALL] sprintf");
  return (int_fast32_t) sprintf(str, \
    "FastGet_libcurl returned with code: %d\n%s", err_code, err_str);
}

int_fast8_t FastReq_SockConnect(int_fast16_t *sock, const char *host)
{
  __debug("[CALL] FastReq_SockConnect");

  struct hostent *hp;
  struct sockaddr_in addr;
  int_fast8_t on = 1;

  __debug("[PRECALL] gethostbyname");
  if ((hp = gethostbyname(host)) == NULL) {
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, "System call \"gethostbyname\" failed");
    return -SYSCALL_GETHOSTBYNAME_FAILED;
  }

  __debug("[PRECALL] socket");
  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_port = htons(DEFAULT_HTTP_PORT);
  addr.sin_family = AF_INET;
  *sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  __debug("[PRECALL] setsockopt");
  setsockopt(*sock, IPPROTO_TCP, TCP_NODELAY, (const char *) &on, sizeof(int));

  if (*sock == -1) {
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, "System call \"setsockopt\" failed");
    return -SYSCALL_SETSOCKOPT_FAILED;
  }

  __debug("[PRECALL] connect");
  if(connect(*sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1){
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, "System call \"connect\" failed");
    return -SYSCALL_CONNECT_FAILED;
	}

	return RETURN_SUCCESS;
}

size_t FastReq_FillChunk(void *contents, size_t size, void *chunk)
{
  __debug("[CALL] FastReq_FillChunk");

  ChunkMem *mem = (ChunkMem *) chunk;

  printf("I  mem.size = %ld\n", mem->size);

  __debug("[PRECALL] PyMem_Realloc");
  mem->memory = PyMem_Realloc(mem->memory, mem->size + size + 1);
  if(mem->memory == NULL) {
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_MemoryError, "FastReq_FillChunk: Out of memory");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, size * sizeof(char));
  mem->size += size;
  mem->memory[mem->size] = 0;

  printf("O  mem.size = %ld\n", mem->size);

  return size;
}

int_fast32_t FastGet_sock(const char *host, const char *path, ChunkMem *mem)
{
  __debug("[CALL] FastGet_sock");

  int_fast16_t sock_dsc;
  int_fast8_t connect_ret;
  int_fast32_t data_receieved = 0;
  size_t mem_increased_size, read_size;

  char payload[SOCK_GET_MSG_MAX_SIZE] = {0};
  char buffer[CHUNK_SIZE] = {0};

  __debug("[PRECALL] FastReq_SockConnect");
  connect_ret = FastReq_SockConnect(&sock_dsc, host);

  if (connect_ret < 0) {
    return connect_ret;
  }

  __debug("[PRE] Payload creation sequence");
  strcpy(payload, "GET ");
  strcat(payload, path);
  strcat(payload, " HTTP/1.1\r\nHost: ");
  strcat(payload, host);
  strcat(payload, "\r\n\r\n");

  __debug("[PRECALL] write");
  if (write(sock_dsc, payload, strlen(payload)) <= 0) {
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, "System call \"write\" failed");
    return -SYSCALL_WRITE_FAILED;
  }

  do {
    __debug("[PRECALL] read");
    read_size = read(sock_dsc, buffer, CHUNK_SIZE - 1);

    __debug("[PRECALL] FastReq_FillChunk");
    mem_increased_size = FastReq_FillChunk(buffer, CHUNK_SIZE - 1, mem);

    if (mem_increased_size == 0) {
      return -OUT_OF_MEMORY;
    }

    data_receieved += mem_increased_size;

    __debug("[PRECALL] memset");
    memset(buffer, 0, CHUNK_SIZE - 1);
  } while (read_size == (CHUNK_SIZE - 1));

  __debug("[PRECALL] shutdown");
  shutdown(sock_dsc, SHUT_RDWR);

  __debug("[PRECALL] close");
	close(sock_dsc);

  return data_receieved;
}
