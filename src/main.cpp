#include "FileReader.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (2 != argc)
    {
        std::cout << "to start use program like this\n ./scop NAME.obj" << std::endl;
        return 1;
    }

    s_InputFileLines info = FileReader::sParseInput(argv[1]);
    std::cout << "amound of times s_vec3 got copied is [" << getCopyCount() <<"] amound of times s_vec3 got moved [" << getMoveCount() << "]" << std::endl;
    return 0;
}