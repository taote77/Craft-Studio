#ifndef PROCESSSINGLETON_H
#define PROCESSSINGLETON_H

#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>

class ProcessSingleton
{
private:
    boost::interprocess::file_lock* g_file_lock = nullptr;

public:
    ProcessSingleton(const std::string appName);

    ~ProcessSingleton();

    // 尝试获取文件锁
    bool acquireApplicationLock();

    // 释放文件锁
    void releaseApplicationLock();

private:
    boost::filesystem::path getLockFilePath();

    std::string mAppName;
};


#endif    // PROCESSSINGLETON_H
