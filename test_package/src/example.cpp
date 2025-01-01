

#ifdef _WIN32
#include <cpputils2/win/shm/shm.hpp>
#else
#include <cpputils2/linux/shm/shm.hpp>
#endif

#include <iostream>
#include <vector>
#include <string>

int main(int , char**) {
    std::cout << "cpputils2 installed!\n";
    return 0;
}
