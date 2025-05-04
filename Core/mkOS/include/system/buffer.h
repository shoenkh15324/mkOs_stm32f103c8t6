#pragma once

class RequestQueue{
protected:
     uint8_t _requestBuffer[REQUEST_BUFFER_SIZE]{};
     int _bufferSize = 0;
     int _readIdx = 0, _writeIdx = 0;
public:
     int totalRequestSize = 0;

     RequestQueue();
     ~RequestQueue();
     int open(int);
     void close();
     void reset();
     int canPush() const;
     int push(const uint8_t *request, int requestSize);
     int pull(uint8_t *request, int requestSize);
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