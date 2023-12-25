/*
 * FCS80 - Simple Buffer Queue
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */
#include "BufferQueue.h"
#include <stdlib.h>
#include <string.h>

BufferQueue::BufferQueue(size_t iSize)
{
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

BufferQueue::~BufferQueue()
{
    if (buffer) free(buffer);
    if (resultBuffer) free(resultBuffer);
}

bool BufferQueue::enqueue(const void* aBuffer, const size_t aSize)
{
    if (size - cursor < aSize) {
        size_t newSize = ((size + aSize) / BUFFER_BLOCK_SIZE + 1) * BUFFER_BLOCK_SIZE;
        void* newBuffer = malloc(newSize);
        if (!newBuffer) return false;
        void* newResultBuffer = malloc(newSize);
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
    memcpy(((char*)this->buffer) + cursor, aBuffer, aSize);
    cursor += aSize;
    return true;
}

void BufferQueue::dequeue(void** oBuffer, size_t* oSize, size_t limit)
{
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
