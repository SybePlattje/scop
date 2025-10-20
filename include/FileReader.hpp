#ifndef FILEREADER_HPP
# define FILEREADER_HPP

# include <vector>
# include <iostream>
# include "GLShader.hpp"

extern int g_sVecCopyCount;
extern int g_sVecMoveCount;

void addToCopy();
void addToMove();

int getCopyCount();
int getMoveCount();

struct  s_InputFileLines
{
    std::vector<s_vec3> vertices;
    std::vector<unsigned int> faces;
};


class FileReader
{
    public:
        static s_InputFileLines sParseInput(const char* path);
};


#endif