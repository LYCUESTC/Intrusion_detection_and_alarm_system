#include <iostream>
#include <memory>
#include<signal.h>
#include "app_server.h"
#include "mylogger.h"

using namespace FLOW_COUNT;

FLOW_COUNT::ThreadSafeQueue<OPEN_TASK_PARAM> open_task_queue_;
FLOW_COUNT::ThreadSafeQueue<DT_DATA> dt_queue_0;
FLOW_COUNT::ThreadSafeQueue<DT_DATA> dt_queue_1;
FLOW_COUNT::ThreadSafeQueue<TRACK_DATA> track_queue;
FLOW_COUNT::ThreadSafeQueue<OUTPUT_DATA> output_queue;

std::shared_ptr<FLOW_COUNT::Logger> gLoggerInstance;

bool signal_recieved_master = false;

void sig_handler_master(int signo)
{
    if (signo == SIGINT)
    {
        gLoggerInstance->info("sig_handler_master received SIGINT");
        signal_recieved_master = true;
    }
}

int main(int argc, char * argv[])
{
    gLoggerInstance = std::make_shared<FLOW_COUNT::Logger>("ai_app", "debug");
    gLoggerInstance->info("*****************************************************");
    gLoggerInstance->info("*********************app runing**********************");
    gLoggerInstance->info("*****************************************************");
    
    if (signal(SIGINT, sig_handler_master) == SIG_ERR)
    {
        gLoggerInstance->error("can't catch SIGINT");
    }

    AppServer app_server;
    app_server.run_server(signal_recieved_master);

    app_server.stop_server();

    gLoggerInstance->info("*****************************************************");
    gLoggerInstance->info("**********************app exit***********************");
    gLoggerInstance->info("*****************************************************");
    return 0;
}