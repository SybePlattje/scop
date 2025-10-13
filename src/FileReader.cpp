#include "FileReader.hpp"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

int g_sVecCopyCount = 0;
int g_sVecMoveCount = 0;

void addToCopy() { g_sVecCopyCount += 1; }
void addToMove() { g_sVecMoveCount += 1; }
int getCopyCount() { return g_sVecCopyCount; }
int getMoveCount() { return g_sVecMoveCount; }

s_vec3::s_vec3(const s_vec3& other)
    : x(other.x), y(other.y), z(other.z)
{
    std::cout << "s_vec3 copied via constructor" << std::endl;
    addToCopy();
}

s_vec3::s_vec3(s_vec3&& other)
    : x(other.x), y(other.y), z(other.z)
{
    std::cout << "s_vec3 moved via constructor" << std::endl;
    addToMove();
    other.x = other.y = other.z = 0;
}

s_vec3& s_vec3::operator=(const s_vec3& other)
{
    std::cout << "s_vec3 copied via operator" << std::endl;
    addToCopy();
    if (this != &other)
        x = other.x, y = other.y, z = other.z;
    return *this;
}

s_vec3& s_vec3::operator=(s_vec3&& other)
{
    std::cout << "s_vec3 moved via operator" << std::endl;
    addToMove();
    if (this != &other)
    {
        x = other.x, y = other.y, z = other.z;
        other.x = other.y = other.z = 0;
    }
    return *this;
}

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
    while (std::getline(fstream, line))
    {
        std::size_t startv = line.find("v ");
        std::size_t startf = line.find("f ");

        if (std::string::npos != startv)
        {
            std::string token;
            s_vec3 pos;
            std::stringstream ss(line);
            ss >> token >> pos.x >> pos.y >> pos.z;
            result.vertices.push_back(pos);
        }

        if (std::string::npos != startf)
        {
            std::vector<unsigned int> faceIndices;
            unsigned int idx;
            std::stringstream ss(line);
            std::string token;

            ss >> token;
            while (ss >> idx)
                faceIndices.push_back(idx - 1);

            for (std::size_t i = 1; faceIndices.size() > i; ++i)
            {
                result.faces.push_back(faceIndices[0]);
                result.faces.push_back(faceIndices[i]);
                result.faces.push_back(faceIndices[i + 1]);
            }
        }
    }

    if (0 == result.vertices.size() || 0 == result.faces.size())
        throw std::runtime_error("no vertices or faces found in file");

    return result;
}