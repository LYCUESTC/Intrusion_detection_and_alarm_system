#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <iostream>
#include <thread>
#include <mutex>
#include "httplib.h"
#include "json/json.h"
#include "app.h"
#include "thread_safe_queue.h"
#include "app_task_base.h"

namespace FLOW_COUNT
{
    struct HTTP_PARAM
    {
        std::string ip;
        int port;
    };
    class HttpServer : public AppTaskBase
    {
        public:
            HttpServer(std::string task_name);
            ~HttpServer();

            bool run(void *ptr) override;  
            bool stop() override;
            APP_TASK_STATUS status() override;
        private:
            std::thread http_thread;
            httplib::Server server;
    };
}
#endif