#include "../../include/application/project/stm32f103/application.h"

MessageQueue::MessageQueue() = default;

MessageQueue::~MessageQueue(){
    close();
}

int MessageQueue::open(int queueSize){
    close();
    if(queueSize < 1 || queueSize > MESSAGE_QUEUE_SIZE) {
        LOG("queueSize(%d) invalid", queueSize);
        return -1;
    }
    _queueSize = queueSize;
    return 0;
}

void MessageQueue::close(){
    reset();
    _queueSize = 0;
}

void MessageQueue::reset(){
    _readIdx = _writeIdx = totalMessageSize = 0;
}

int MessageQueue::canPush() const {
    //LOG("_queueSize:%d, totalMessageSize: %d", _queueSize, totalMessageSize);
    return _queueSize - totalMessageSize;
}

int MessageQueue::push(const uint8_t *request, int requestSize){
    if(!_messageQueue || !request || requestSize <= 0 || requestSize > canPush()) {
        LOG("push failed: buffer=%p, request=%p, requestSize=%d, canPush=%d",
            _messageQueue, request, requestSize, canPush());
        return 0;
    }
    int firstChunk = std::min(_queueSize - _writeIdx, requestSize);
    std::copy_n(request, firstChunk, &_messageQueue[_writeIdx]);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(request + firstChunk, remaining, &_messageQueue[0]);
    }
    _writeIdx = (_writeIdx + requestSize) % _queueSize;
    totalMessageSize += requestSize;
    return requestSize;
}

int MessageQueue::pull(uint8_t *request, int requestSize){
    if(!_messageQueue || !request || requestSize <= 0 || requestSize > totalMessageSize) {
        LOG("pull failed: buffer=%p, requestSize=%d, total=%d",
            _messageQueue, requestSize, totalMessageSize);
        return 0;
    }
    int firstChunk = std::min(_queueSize - _readIdx, requestSize);
    std::copy_n(&_messageQueue[_readIdx], firstChunk, request);

    int remaining = requestSize - firstChunk;
    if (remaining > 0) {
        std::copy_n(&_messageQueue[0], remaining, request + firstChunk);
    }
    _readIdx = (_readIdx + requestSize) % _queueSize;
    totalMessageSize -= requestSize;
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

