#include <glad/glad.h>
#include "Scop.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (2 != argc)
    {
        std::cout << "to start use program like this\n ./scop NAME.obj" << std::endl;
        return 1;
    }

    try
    {
        Scop scop(argv[1]);
        scop.start();
        return 0;
    }
    catch( std::runtime_error& e)
    {
        std::cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }
    catch( std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}