#pragma once

class RequestQueue{
protected:
     uint8_t *_requestBuffer = nullptr;
     int _bufferSize = 0;
     int _readIdx = 0, _writeIdx = 0;
public:
     int totalRequestSize = 0;
     explicit RequestQueue(int bufferSize = 0);
     ~RequestQueue();
     int open(int bufferSize);
     void close();
     void reset();
     int canPush() const;
     int push(const uint8_t *request, int requestSize);
     int pull(uint8_t *event, int requestSize);
};
