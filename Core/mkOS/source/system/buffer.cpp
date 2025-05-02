#include "../../include/application/application.h"

RequestQueue::RequestQueue(int bufferSize){
    if(bufferSize > 0){
        open(bufferSize);
    }
}

RequestQueue::~RequestQueue(){
    close();
}

int RequestQueue::open(int bufferSize){
    close();
    if(bufferSize < 1) { LOG("bufferSize(%d) < 1", bufferSize);
        return -1;
    }
    _requestBuffer = new uint8_t[bufferSize];
    _bufferSize = bufferSize;
    if(!_requestBuffer){ LOG("new requestQueue fail");
        return -1;
    }
    return 0;
}

void RequestQueue::close(){
    reset();
    if(_requestBuffer){
        delete[] _requestBuffer;
        _requestBuffer = nullptr;
    }
    _bufferSize = 0;
}

void RequestQueue::reset(){
    _readIdx = _writeIdx = totalRequestSize = 0;
}

int RequestQueue::canPush() const {
    //LOG("_bufferSize:%d, totalRequestSize: %d", _bufferSize, totalRequestSize);
    return _bufferSize - totalRequestSize;
}

int RequestQueue::push(const uint8_t *request, int requestSize){
    if (!request || requestSize <= 0) {
        return 0;
    }
    if (requestSize > canPush()) {
        LOG("requestSize(%d) > canPush()", requestSize);
        return 0;
    }

    int firstChunk = std::min(_bufferSize - _writeIdx, requestSize);
    std::copy_n(request, firstChunk, &_requestBuffer[_writeIdx]);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(request + firstChunk, remaining, &_requestBuffer[0]);
    }
    _writeIdx = (_writeIdx + requestSize) % _bufferSize;
    totalRequestSize += requestSize;
    return requestSize;
}

int RequestQueue::pull(uint8_t *request, int requestSize){
    if (!request || requestSize <= 0) {
        return 0;
    }
    if (requestSize > totalRequestSize) {
        LOG("requestSize(%d) > totalRequestSize(%d)", requestSize, totalRequestSize);
        return 0;
    }

    int firstChunk = std::min(_bufferSize - _readIdx, requestSize);
    std::copy_n(&_requestBuffer[_readIdx], firstChunk, request);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(&_requestBuffer[0], remaining, request + firstChunk);
    }
    _readIdx = (_readIdx + requestSize) % _bufferSize;
    totalRequestSize -= requestSize;
    return requestSize;
}

#if 0
RequestQueue::RequestQueue(int bufferSize) noexcept {
    if (bufferSize > 0) {
        open(bufferSize);
    }
}

int RequestQueue::open(int bufferSize) noexcept {
    close();
    if (bufferSize < 1) {
        LOG("bufferSize(%d) < 1", bufferSize);
        return -1;
    }
    _requestBuffer = std::make_unique<uint8_t[]>(bufferSize);
    if (!_requestBuffer) {
        LOG("Failed to allocate request buffer");
        return -1;
    }
    _bufferSize = bufferSize;
    return 0;
}

void RequestQueue::close() noexcept {
    reset();
    _requestBuffer.reset();
    _bufferSize = 0;
}

void RequestQueue::reset() noexcept {
    _readIdx = _writeIdx = totalRequestSize = 0;
}

int RequestQueue::canPush() const noexcept {
    return _bufferSize - totalRequestSize;
}

int RequestQueue::push(const uint8_t* request, int requestSize) noexcept {
    if (!request || requestSize <= 0) {
        return 0;
    }
    if (requestSize > canPush()) {
        LOG("requestSize(%d) > canPush()", requestSize);
        return 0;
    }

    int firstChunk = std::min(_bufferSize - _writeIdx, requestSize);
    std::copy_n(request, firstChunk, &_requestBuffer[_writeIdx]);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(request + firstChunk, remaining, &_requestBuffer[0]);
    }

    _writeIdx = (_writeIdx + requestSize) % _bufferSize;
    totalRequestSize += requestSize;
    return requestSize;
}

int RequestQueue::pull(uint8_t* request, int requestSize) noexcept {
    if (!request || requestSize <= 0) {
        return 0;
    }
    if (requestSize > totalRequestSize) {
        LOG("requestSize(%d) > totalRequestSize(%d)", requestSize, totalRequestSize);
        return 0;
    }

    int firstChunk = std::min(_bufferSize - _readIdx, requestSize);
    std::copy_n(&_requestBuffer[_readIdx], firstChunk, request);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(&_requestBuffer[0], remaining, request + firstChunk);
    }

    _readIdx = (_readIdx + requestSize) % _bufferSize;
    totalRequestSize -= requestSize;
    return requestSize;
}
#endif