
#include <assert.h>
#include <unistd.h>

namespace EasyEvent{

// 文件描述符的 RAII 封装，在析构时会自动关闭 fd
class FileDescriptor{
public:

    explicit FileDescriptor(int fd)
        : _fd(fd) {}

    ~FileDescriptor(){
        int returnVal = ::close(_fd);
        assert(returnVal >= 0);
    }

    int getFd() const { return _fd; }

private:
    int _fd;
};

}
