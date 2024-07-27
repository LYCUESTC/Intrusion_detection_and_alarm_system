#ifndef APP_TASK_BASE_H
#define APP_TASK_BASE_H

#include <iostream>
#include "app.h"
#include "mylogger.h"

namespace FLOW_COUNT
{
    
    class AppTaskBase
    {
        public:
            AppTaskBase();
            virtual ~AppTaskBase();
            virtual bool run(void *ptr) = 0;  
            virtual bool stop() = 0;
            virtual APP_TASK_STATUS status() = 0;
        protected:
            bool exit_flag;
            APP_TASK_STATUS app_task_status_;
            std::string task_name_;
            std::shared_ptr<spdlog::logger> logger_;
    };
}

#endif