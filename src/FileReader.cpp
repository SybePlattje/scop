#include "FileReader.hpp"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

s_InputFileLines FileReader::sParseInput(const char* path)
{
    if (!path)
        throw std::runtime_error("path cannot be empty");

    std::filesystem::path filePath = path;
    if (".obj" != filePath.extension())
        throw std::runtime_error("file needs to be a .obj");

    std::ifstream fstream(path);
    if (!fstream)
    {
        std::string fileString = "resources/" + static_cast<std::string>(path);
        fstream.open(fileString);
        if (!fstream)
            throw std::runtime_error("File not found");
    }

    s_InputFileLines result;

    std::string line;
    result.faces.reserve(3000);
    result.vertices.reserve(3000);
    while (std::getline(fstream, line))
    {
        if (0 == line.rfind("v ", 0))
        {
            float x, y, z;
            std::sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
            result.vertices.emplace_back(x, y, z);
        }
        else if (0 == line.rfind("f ", 0))
        {
            unsigned int a, b, c;
            if (std::sscanf(line.c_str(), "f %u %u %u", &a, &b, &c) == 3)
            {
                result.faces.emplace_back(a - 1);
                result.faces.emplace_back(b - 1);
                result.faces.emplace_back(c - 1);
            }
        }
    }

    if (0 == result.vertices.size() || 0 == result.faces.size())
        throw std::runtime_error("no vertices or faces found in file");

    return result;
}