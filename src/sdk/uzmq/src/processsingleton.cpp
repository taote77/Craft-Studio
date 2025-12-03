#include "uzmq/processsingleton.h"
#include <boost/filesystem.hpp>

ProcessSingleton::ProcessSingleton(const std::string appName)
{
    boost::filesystem::path programPath(appName);
    mAppName = programPath.filename().string();
    if(mAppName.empty())
    {
        mAppName = "ucore";
    }
}

ProcessSingleton::~ProcessSingleton()
{

}

boost::filesystem::path ProcessSingleton::getLockFilePath()
{
    // 可根据需要修改锁文件路径，确保有读写权限
    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path();
    if (tmp_dir.empty())
    {
        tmp_dir = "/tmp";
    }
    return tmp_dir / (mAppName + "_process.lock");
}

bool ProcessSingleton::acquireApplicationLock()
{
    const auto lock_path = getLockFilePath();

    std::cout << "getLockFilePath :" << lock_path << std::endl;

    std::ofstream lockFile(lock_path.string(), std::ios::out | std::ios::binary);
    if (!lockFile)
    {
        std::cerr << "无法创建锁文件: " << lock_path << std::endl;
        return false;
    }
    lockFile.close();    // 关闭文件流，仅需文件存在
    // 创建文件锁对象并尝试加锁
    g_file_lock = new boost::interprocess::file_lock(lock_path.string().c_str());
    if (!g_file_lock->try_lock())
    {
        delete g_file_lock;
        g_file_lock = nullptr;
        return false;
    }
    return true;
}

void ProcessSingleton::releaseApplicationLock()
{
    if (g_file_lock)
    {
        g_file_lock->unlock();
        delete g_file_lock;
        g_file_lock = nullptr;
    }
}
