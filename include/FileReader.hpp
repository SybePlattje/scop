#ifndef FILEREADER_HPP
# define FILEREADER_HPP

# include <vector>
# include <iostream>
# include "GLShader.hpp"

struct  s_InputFileLines
{
    std::vector<s_vec3> vertices;
    std::vector<s_vec3> verticesPerFace;
    std::vector<unsigned int> faces;
    std::vector<unsigned int> facesPerFace;
};


class FileReader
{
    public:
        static s_InputFileLines sParseInput(const char* path);
};


#endif