#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringbuffer.h"
#include "utils.h"
#include "errors.h"

StringBuffer *StringBuffer_Create(void) {
    FastRequest_FuncDebug("StringBuffer_Create", "==> Function enter");

    StringBuffer *buf = (StringBuffer *) PyMem_RawMalloc(sizeof(StringBuffer));

    buf->buf = (char *) PyMem_RawMalloc(sizeof(char) * STRINGBUFFER_INITIAL_SIZE);
    buf->head_index = -1;
    buf->index = -1;
    buf->size = STRINGBUFFER_INITIAL_SIZE;

    FastRequest_FuncDebug("StringBuffer_Create", "<== Function exit");

    return buf;
}

StringBuffer *StringBuffer_FromString(const char *str) {
    StringBuffer *buf = StringBuffer_Create();

    if (str == NULL)
        return buf;

    if (strlen(str) >= (size_t)(buf->size))
        buf->buf = (char *) PyMem_RawRealloc(buf->buf, sizeof(char) * (strlen(str) + 4));

    for (buf->index = 0; (size_t)(buf->index) < strlen(str); buf->index++)
        buf->buf[buf->index] = str[buf->index];

    buf->index--;

    return buf;
}

void StringBuffer_Free(StringBuffer *buf) {
    FastRequest_FuncDebug("StringBuffer_Free", "==> Function enter");

    if (buf == NULL)
        return;

    PyMem_RawFree(buf->buf);
    PyMem_RawFree(buf);

    FastRequest_FuncDebug("StringBuffer_Free", "<== Function exit");
}

int_fast64_t StringBuffer_Extend(StringBuffer *buf) {
    //FastRequest_FuncDebug("StringBuffer_Extend", "==> Function enter");

    if (buf == NULL)
        return -FR_ERR_NULL_ARG;

    buf->buf = PyMem_RawRealloc(buf->buf, sizeof(char) * (buf->size + STRINGBUFFER_EXTEND_SIZE));

    // Memory error
    if (buf->buf == NULL)
        return -FR_ERR_MEMORY;

    buf->size += STRINGBUFFER_EXTEND_SIZE;

    //FastRequest_FuncDebug("StringBuffer_Extend", "<== Function exit");

    return buf->size;
}

int_fast64_t StringBuffer_PushChar(StringBuffer *buf, char ch) {
    int_fast64_t extend_ret;

    //FastRequest_FuncDebug("StringBuffer_PushChar", "==> Function enter");

    if (buf == NULL)
        return -FR_ERR_NULL_ARG;

    if (buf->index >= (buf->size - 3))
        if ((extend_ret = StringBuffer_Extend(buf)) < 0)
            return extend_ret;

    buf->index++;
    buf->buf[buf->index] = ch;

    //FastRequest_FuncDebug("StringBuffer_PushChar", "<== Function exit");

    return buf->index;
}

int_fast64_t StringBuffer_PushSequence(StringBuffer *buf, char *seq, size_t size) {
    int_fast64_t push_char_ret;

    if (buf == NULL)
        return -FR_ERR_NULL_ARG;

    buf->buf = PyMem_RawRealloc(buf->buf, sizeof(char) * (buf->size + size + 1));

    // Memory error
    if (buf->buf == NULL)
        return -FR_ERR_MEMORY;

    for (size_t i = 0; i < size; i++)
        if ((push_char_ret = StringBuffer_PushChar(buf, seq[i])) < 0)
            return push_char_ret;

    return buf->size;
}

char *StringBuffer_GetSequenceRef(StringBuffer *buf, size_t size, int_fast64_t *size_read) {
    char *retpos;

    if (buf == NULL)
        return NULL;

    if ((size_t)(buf->index - buf->head_index) < size)
        size = buf->index - buf->head_index;

    retpos = buf->buf + buf->head_index + 1;
    buf->head_index += size;
    *size_read = size;

    return retpos;
}

void StringBuffer_Dump(StringBuffer *buf) {
    if (buf == NULL) {
        FastRequest_FuncDebug("StringBuffer_Dump", "==== DUMP <NULL> ====");
        return;
    }

    FastRequest_FuncDebug("StringBuffer_Dump", "==== DUMP ====");
    printf("Address: %ld\n", (unsigned long) buf);
    printf("Head Index: %lld\n", buf->head_index);
    printf("Index: %lld\n", buf->index);
    printf("Size: %lld\n", buf->size);
    printf("String: ");

    for (int i = 0; i <= buf->index; i++)
        printf("%c", buf->buf[i]);

    printf("\n\n");
}