#include "file.h"

bool FileManager::ReadFile(std::string fileName, std::string& fullFile)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
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
        return false;
    }
    file << fullFile;
    file.close();
    return true;
}