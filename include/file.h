#ifndef _FILE_H
#define _FILE_H

#include <iostream>
#include <fstream>
#include <string>
/**
 * @brief 文件管理器，执行文本文件的读写
 */
class FileManager
{
public:
    /**
     * @brief ReadFile
     * @param  fileName             文件名
     * @param  fullFile             引用类型，存储读取到的全部数据
     * @return true                 读取成功
     * @return false                读取失败
     * @brief 读取整个文本文件          
     */
    bool ReadFile(std::string fileName, std::string& fullFile);

    /**
     * @brief WriteFile
     * @param  fileName             文件名
     * @param  fullFile             存储要写入的全部数据
     * @return true                 写入成功
     * @return false                写入失败
     * @brief 写入整个文本文件          
     */
    bool WriteFile(std::string fileName, std::string fullFile);    
};

#endif