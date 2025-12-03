#ifndef _GLOBAL_ENV_HPP_
#define _GLOBAL_ENV_HPP_

#include "uzmq/env.h"
#include <fstream>
#include <string>
#include <sstream>

namespace heygears
{
    namespace sys
    {
        enum PrinterType
        {
            TYPE_UNKNOW,
            TYPE_REFLEX_1_0,
            TYPE_REFLEX_RS,
            TYPE_REFLEX_PRO,
            TYPE_REFLEX_2_0,
        };

        inline heygears::sys::PrinterType readSysPrinterType()
        {
            try {
                std::ifstream file("/usr/bin/sysversion");
                if (!file.is_open()) {
                    return heygears::sys::TYPE_UNKNOW;
                }
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string output = buffer.str();
                
                if (output.find("reflex-pro") != std::string::npos) {
                    return heygears::sys::TYPE_REFLEX_PRO;
                }
                return heygears::sys::TYPE_REFLEX_2_0;
            } catch (const std::exception& e) {
                return heygears::sys::TYPE_REFLEX_2_0;
            }
        }

        inline bool isReflexPro()
        {
            return (readSysPrinterType() == heygears::sys::TYPE_REFLEX_PRO);
        }
    }
}

#endif