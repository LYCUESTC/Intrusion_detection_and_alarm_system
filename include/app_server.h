#ifndef APP_SERVER_H
#define APP_SERVER_H

#include <map>
#include "app.h"
#include "app_task_base.h"
#include "det_task.h"
#include "rtsp_task.h"
#include "thread_safe_queue.h"
#include "http_server.h"
#include "mylogger.h"

namespace FLOW_COUNT
{
    struct TASK_SERVER
    {
        TASK_PARAMS param;
        std::shared_ptr<AppTaskBase> task_ptr;
    };
    

    class AppServer
    {
        public:
            AppServer();
            ~AppServer();

            bool add_task(void * param);
            bool stop_task(void *param);
            bool stop_server();
            bool watch_task_status();
            int run_server(bool &exit_flag);
            static int task_run(std::shared_ptr<AppTaskBase> &c_ptr, void * param);
        private:
            std::map<std::string, TASK_SERVER> task_lists;
            std::map<std::string, std::thread> task_threads;
            std::shared_ptr<spdlog::logger> logger_;
    };
}
#endif