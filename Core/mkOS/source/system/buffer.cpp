#include "../../include/application/project/stm32f103/application.h"

RequestQueue::RequestQueue() = default;

RequestQueue::~RequestQueue(){
    close();
}

int RequestQueue::open(int bufferSize){
    close();
    if(bufferSize < 1 || bufferSize > REQUEST_BUFFER_SIZE) {
        LOG("bufferSize(%d) invalid", bufferSize);
        return -1;
    }
    _bufferSize = bufferSize;
    return 0;
}

void RequestQueue::close(){
    reset();
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
    if(!_requestBuffer || !request || requestSize <= 0 || requestSize > canPush()) {
        LOG("push failed: buffer=%p, request=%p, requestSize=%d, canPush=%d",
            _requestBuffer, request, requestSize, canPush());
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
    if(!_requestBuffer || !request || requestSize <= 0 || requestSize > totalRequestSize) {
        LOG("pull failed: buffer=%p, requestSize=%d, total=%d",
            _requestBuffer, requestSize, totalRequestSize);
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

// RxBuffer
bool RxBuffer::open() {
    reset();
    return true;
}

void RxBuffer::close() {
    reset();
}

void RxBuffer::reset() {
    head = 0;
    tail = 0;
}

bool RxBuffer::available() {
    return head != tail;
}

bool RxBuffer::read(uint8_t* buf, uint16_t len) {
    uint16_t count = 0;
    while (count < len && available()) {
        buf[count++] = rxBuf[tail];
        tail = (tail + 1) % PROJECT_RXBUFFER_SIZE;
    }
    return count > 0;
}

bool RxBuffer::write(const uint8_t* buf, uint16_t len) {
    uint16_t count = 0;
    while (count < len) {
        uint16_t nextHead = (head + 1) % PROJECT_RXBUFFER_SIZE;
        if (nextHead == tail) {
            break;
        }
        rxBuf[head] = buf[count++];
        head = nextHead;
    }
    return count > 0;
}

