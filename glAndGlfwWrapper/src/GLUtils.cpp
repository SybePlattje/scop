#include "GLUtils.hpp"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @param path the string of the path to the shader file
 * @param buffer the buffer that will hold  the data of the shader file
 * @brief checks path, reads into it, and stores the data in buffer
 * @return true if storing the file data in buffer succeeds, else false and error message is printed
 */
bool GLUtils::sReadShaderFile(const char* path, std::vector<unsigned char>& buffer)
{
    if (!path)
    {
        std::cerr <<"path is empty" << std::endl;
        return false;
    }
    
    std::ifstream fstream(path, std::ios::binary | std::ios::ate);
    if (!fstream)
    {
        std::cerr << "file not found: " << path << std::endl;
        return false;
    }
    
    std::streamsize size = fstream.tellg();
    fstream.seekg(0, std::ios::beg);

    buffer.resize(size);
    if(!fstream.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        fstream.close();
        std::cerr << "reading file failed: " << path << std::endl;
        return false;
    }

    fstream.close();
    return true;
}

/**
 * @param path the path to the texture file
 * @param buffer the buffer that will hold the data from the texture file
 * @brief tries to read the texture file and put the data in the buffer
 * @return true if the data from the file is in buffer, else false and an error message is printed
 */
bool GLUtils::sReadTexture(const char* path, std::vector<unsigned char>& buffer)
{
    if (!path)
    {
        std::cerr << "path is empty" << std::endl;
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "failed not found: " << path << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.resize(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        file.close();
        std::cerr << "reading file failed " << path << std::endl;
        return false;
    }

    file.close();
    return true;
}