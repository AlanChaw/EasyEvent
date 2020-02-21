#include <iostream>


#include "base/Buffer.h"

int main(){
    Buffer buf;
    buf.append(std::string("hellohel"));
    buf.printBufInfo();

    buf.retrieve(5);

    buf.printBufInfo();

    buf.append(std::string("lohello"));
    buf.printBufInfo();
    buf.append(std::string("test"));
    std::cout << buf.retrieveAsString() << "\n\n";
    buf.printBufInfo();


}
