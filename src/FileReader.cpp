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
    result.facesPerFace.reserve(3000);
    result.vertices.reserve(3000);
    result.verticesPerFace.reserve(3000);
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
            std::vector<unsigned int> faceIndices;
            faceIndices.reserve(3000);
            unsigned int id;
            std::stringstream ss(line);
            std::string token;
            ss >> token;
            while (ss >> id)
                faceIndices.emplace_back(id - 1);

            for (std::size_t i = 1; i < faceIndices.size() - 1; ++i)
            {
                result.faces.emplace_back(faceIndices[0]);
                result.faces.emplace_back(faceIndices[i]);
                result.faces.emplace_back(faceIndices[i + 1]);
            }
        }
    }

    for (std::size_t i = 0; i < result.faces.size(); i += 3)
    {
        for (int j = 0; j < 3; ++j)
        {
            unsigned int vertexIndex = result.faces[i + j];
            s_vec3 vertex = result.vertices[vertexIndex];

            result.verticesPerFace.emplace_back(vertex);

            result.facesPerFace.emplace_back(static_cast<unsigned int>(result.facesPerFace.size()));
        }
    }

    if (0 == result.vertices.size() || 0 == result.faces.size())
        throw std::runtime_error("no vertices or faces found in file");

    return result;
}