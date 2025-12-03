#ifndef UZMQ_APPLICATION_H
#define UZMQ_APPLICATION_H

#include <iostream>
#include "platform.h"
#include "processsingleton.h"

namespace uzmq
{

class UZMQAPI Application
{
public:
    Application(int argc, char* argv[]);

    ~Application();

    void init(std::istringstream& stream);

    int exec();

private:
    ProcessSingleton mProcess;
};
}

#endif // UZMQ_APPLICATION_H
