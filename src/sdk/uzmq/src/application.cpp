#include "uzmq/application.h"
#include "appinst.h"

namespace uzmq
{
Application::Application(int argc, char *argv[]) : mProcess(argv[0])
{
    if (!mProcess.acquireApplicationLock())
    {
        std::cerr << "The same program has already been launched." << std::endl;
        exit(EXIT_SUCCESS);
    }
    AppInst::setArgument(argc, argv);
}

Application::~Application()
{
    mProcess.releaseApplicationLock();
}

void Application::init(std::istringstream& stream)
{
    AppInst::initEnv(stream);
}

int Application::exec()
{
    return AppInst::exec();
}
}
