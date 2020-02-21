#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <sys/uio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

//using namespace std;

namespace EasyEvent{
class Buffer{
public:
    static const size_t initialSize = 10;
    
    Buffer()
        : _buffer(initialSize),
          _startIndex(0),
          _endIndex(0),
          _isFull(false)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
    }
    
    // Read data directly into buffer.
    size_t readFd(int fd, int* savedErrno){
        // FIX ME!!!  这里效率比较低，拷贝了两次才放到buffer中
        char extrabuf[65535];
        const ssize_t n = read(fd, extrabuf, sizeof extrabuf);
        if(n < 0){
            *savedErrno = errno;
        }else{
            append(extrabuf, n);
        }
        return n;
    }

    size_t writeFd(int fd){
        size_t n = 0;
        assert(!isEmpty());
        // FIX ME !!!  这里后边可以改为使用 writev，一次系统调用写入两部分数据
        if(_startIndex < _endIndex){
            n = write(fd, peek(), readableBytes());
        }else{
            n = write(fd, peek(), bufferSize()-_startIndex);
        }
    }
    
    size_t readableBytes() const{
        if(_isFull){
            return bufferSize();
        }
        if(_startIndex <= _endIndex){
            return _endIndex - _startIndex;
        }else{
            return bufferSize() - (_startIndex - _endIndex);
        }
    }
    
    size_t writableBytes() const{
        return bufferSize() - readableBytes();
    }

    size_t bytesAfterEnd() const{
        if(_isFull){
            return 0;
        }
        if(_endIndex < _startIndex){
            return _startIndex - _endIndex;
        }else{
            return bufferSize() - _endIndex;
        }
    }
    
    const char* peek() const{
        return begin() + _startIndex;
    }
    
    
    void retrieve(size_t len){
        assert(len <= readableBytes());
        if(_startIndex + len < bufferSize()){
            _startIndex += len;
        }else{
            _startIndex = _startIndex + len - bufferSize();
        }
        if(len > 0 && _startIndex == _endIndex){
            _isFull = false;
        }
    }
    
//    void retrieveUntil(const char* end){
//
//    }
    
    void retrieveAll(){
        _startIndex = _endIndex = 0;
        _isFull = false;
    }
    
    std::string retrieveAsString(){
        std::string str;
        
        if(_startIndex < _endIndex){
            str += std::string(peek(), readableBytes());
        }else if(_startIndex > _endIndex){
            str += std::string(peek(), bufferSize()-_startIndex);
            str += std::string(begin(), _endIndex);
        }else{
            if(_isFull){
                str += std::string(peek(), bufferSize()-_startIndex);
                str += std::string(begin(), _endIndex);
            }
        }
        
        retrieveAll();
        return str;
    }
    
    void append(const std::string& str){
        append(str.data(), str.length());
    }
    
    void append(const char* data, size_t len){
        if(writableBytes() < len){
            makeSpace(len);
        }
        writeAfterEnd(data, len);
    }
    
    void append(const void* data, size_t len){
        append(static_cast<const char*>(data), len);
    }
    
    void makeSpace(size_t len){
        size_t readable = readableBytes();
        
        size_t newSize = bufferSize() + (len-writableBytes());
        std::string data = retrieveAsString();
        _buffer.resize(newSize);
        append(data);
        
        assert(readable == readableBytes());
    }
    
    void writeAfterEnd(const char* data, size_t len){
        if(_endIndex + len <= bufferSize()){
            std::copy(data, data+len, begin()+_endIndex);
        }else{
            size_t firstPart = bufferSize() - _endIndex;
            size_t secondPart = len - firstPart;
            std::copy(data, data+firstPart, begin()+_endIndex);
            std::copy(data+firstPart, data+secondPart, begin());
        }
        hasWritten(len);
    }
    
    void hasWritten(size_t len){
        if(_endIndex + len < bufferSize()){
            _endIndex += len;
        }else{
            _endIndex = _endIndex + len - bufferSize();
        }
        if(len > 0 && _endIndex == _startIndex){
            _isFull = true;
        }
    }

    bool isEmpty(){
        return (_startIndex == _endIndex && !_isFull);
    }
    
        
    // void printBufInfo(){
    //     printf("buffer size: %d\n", bufferSize());
    //     printf("start index: %d\n", _startIndex);
    //     printf("end index: %d\n", _endIndex);
    //     printf("readable bytes: %d\n", readableBytes());
    //     printf("writable bytes: %d\n", writableBytes());
    //     printf("bytes after end: %d\n", bytesAfterEnd());
    //     printf("is full: %d\n", _isFull);
    //     printf("\n\n");
    // }
    
private:
    char* begin(){
        return &*_buffer.begin();
    }
    
    const char* begin() const{
        return &*_buffer.begin();
    }
    
    size_t bufferSize() const{
        return _buffer.size();
    }
    
    std::vector<char> _buffer;
    size_t _startIndex;
    size_t _endIndex;
    bool _isFull;
};

}