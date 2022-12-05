#include "file.h"
//╤анд╪Ч
bool FileManager::ReadFile(std::string fileName, std::string& fullFile)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        printf("file.cpp:09 File Open Failed!\n");
        return false;
    }
    std::string line;
    while (getline(file, line))
    {
        fullFile += line + "\n";
    }
    file.close();
    return true;
}

bool FileManager::WriteFile(std::string fileName, std::string fullFile)
{
    std::ofstream file(fileName);
    if (!file.is_open())
    {
        printf("file.cpp:26 Ofstream File Open Failed!\n");
        return false;
    }
    file << fullFile;
    file.close();
    return true;
}