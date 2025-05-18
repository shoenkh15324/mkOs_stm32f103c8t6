#pragma once

class MessageQueue{
protected:
     uint8_t _messageQueue[MESSAGE_QUEUE_SIZE]{};
     int _queueSize = 0;
     int _readIdx = 0, _writeIdx = 0;
public:
     int totalMessageSize = 0;

     MessageQueue();
     ~MessageQueue();
     int open(int);
     void close();
     void reset();
     int canPush() const;
     int push(const uint8_t *, int);
     int pull(uint8_t *, int);
};

class RxBuffer{
public:
     uint8_t rxBuf[PROJECT_RXBUFFER_SIZE] = {0};
     uint16_t head = 0, tail = 0;

     RxBuffer() = default;
     bool open();
     void close();
     void reset();
     bool available();
     bool read(uint8_t*, uint16_t);
     bool write(const uint8_t*, uint16_t);
};