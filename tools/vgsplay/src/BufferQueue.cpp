#include "BufferQueue.h"
#include <string.h>
#include <stdlib.h>

BufferQueue::BufferQueue(size_t iSize) {
    size = ((iSize / BUFFER_BLOCK_SIZE) + 1) * BUFFER_BLOCK_SIZE;
    cursor = 0;
    buffer = malloc(size);
    if (!buffer) {
        size = 0;
    }
    resultBuffer = malloc(size);
    if (!resultBuffer) {
        free(buffer);
        buffer = NULL;
        size = 0;
    }
}

BufferQueue::~BufferQueue() {
    if (buffer) free(buffer);
    if (resultBuffer) free(resultBuffer);
}

bool BufferQueue::enqueue(const void *aBuffer, const size_t aSize) {
    if (size - cursor < aSize) {
        size_t newSize = ((size + aSize) / BUFFER_BLOCK_SIZE + 1) * BUFFER_BLOCK_SIZE;
        void *newBuffer = malloc(newSize);
        if (!newBuffer) return false;
        void *newResultBuffer = malloc(newSize);
        if (!newResultBuffer) {
            free(newBuffer);
            return false;
        }
        memcpy(newBuffer, buffer, cursor);
        free(buffer);
        buffer = newBuffer;
        free(resultBuffer);
        resultBuffer = newResultBuffer;
        size = newSize;
    }
    memcpy(((char *) this->buffer) + cursor, aBuffer, aSize);
    cursor += aSize;
    return true;
}

void BufferQueue::dequeue(void **oBuffer, size_t *oSize, size_t limit) {
    if (cursor) {
        *oBuffer = resultBuffer;
        if (limit < cursor) {
            memcpy(resultBuffer, buffer, limit);
            memmove(buffer, ((char*)buffer) + limit, cursor - limit);
            cursor -= limit;
            *oSize = limit;
        } else {
            memcpy(resultBuffer, buffer, cursor);
            *oSize = cursor;
            cursor = 0;
        }
    } else {
        *oBuffer = NULL;
        *oSize = 0;
    }
}
