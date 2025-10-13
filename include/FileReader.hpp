#ifndef FILEREADER_HPP
# define FILEREADER_HPP

# include <vector>
# include <iostream>

extern int g_sVecCopyCount;
extern int g_sVecMoveCount;

void addToCopy();
void addToMove();

int getCopyCount();
int getMoveCount();

struct s_vec3
{
    float x, y, z;
    s_vec3(): x(0), y(0), z(0) {};
    s_vec3(const s_vec3& other);
    s_vec3(s_vec3&& other);

    s_vec3& operator=(const s_vec3& other);
    s_vec3& operator=(s_vec3&& other);
};

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