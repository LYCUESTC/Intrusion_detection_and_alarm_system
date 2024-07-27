#include "app_server.h"
#include "track_task.h"
#include "output_task.h"
#include <windows.h> 
namespace FLOW_COUNT
{
    AppServer::AppServer()
    {
        logger_ = gLoggerInstance->clone("app_server");
    }
    AppServer::~AppServer()
    {

    }

    bool AppServer::add_task(void * param)
    {
        TASK_PARAMS *task_param = (TASK_PARAMS *)param;
        int task_type = task_param->task_type;
        std::string task_name = task_param->task_name;
        TASK_SERVER task_server;
        if(0 == task_type) //rtsp
        {
            task_lists[task_name].task_ptr = std::make_shared<RtspTask>(task_name);
        }else if(1 == task_type)//det
        {
            task_lists[task_name].task_ptr = std::make_shared<DetTask>(task_name);
        }else if(2 == task_type)//http
        {
            task_lists[task_name].task_ptr = std::make_shared<HttpServer>(task_name);
        }else if(3 == task_type)//output
        {
            task_lists[task_name].task_ptr = std::make_shared<OutputTask>(task_name);
        }else if(4 == task_type)//track_task
        {
            task_lists[task_name].task_ptr = std::make_shared<TrackTask>(task_name);
        }
        else{
            logger_->info("no task add");
            return false;
        }
        
        task_threads.insert(std::make_pair(task_name, std::thread(&task_run, std::ref(task_lists[task_name].task_ptr), task_param->ptr)));
    }

    bool AppServer::stop_task(void * param)
    {
        TASK_PARAMS* task_param = (TASK_PARAMS*)param;
        logger_->info("task_lists:{}", task_lists.size());
        if (task_lists.find(task_param->task_name) != task_lists.end())
        {
            for (auto it : task_lists)
            {
                logger_->info("task_name:{}", it.first);
            }
            logger_->info("stop task_name:{}.", task_param->task_name);
            task_lists[task_param->task_name].task_ptr->stop();
            task_threads[task_param->task_name].join();

            task_lists.erase(task_param->task_name);
            task_threads.erase(task_param->task_name);
        }
        logger_->info("stop {} successful!", task_param->task_name);
        return true;
    }

    bool AppServer::stop_server()
    {
        for(auto &it: task_threads)
        {
            logger_->info("**************************************");
            std::string task_name = it.first;
            logger_->info("*****stop {} task...", task_name);
            task_lists[task_name].task_ptr->stop();
            it.second.join();
            logger_->info("*****{} exit successful", task_name);
            logger_->info("**************************************\n");
        }
        logger_->info("stop all server successful");
        return true;
    }
    bool AppServer::watch_task_status()
    {
        return true;
    }
    int AppServer::run_server(bool &exit_flag)
    {
        TASK_PARAMS task_param;
        task_param.task_name = "http_server";
        task_param.task_type = 2;
        HTTP_PARAM http_param;
        http_param.ip = "192.168.10.121";
        http_param.port = 6789;
        task_param.ptr = &http_param;
        add_task(&task_param);

        TASK_PARAMS det_task_param1;
        det_task_param1.task_name = "det_task1";
        det_task_param1.task_type = 1;
        det_task_param1.ptr = &dt_queue_0;
        add_task(&det_task_param1);

        /*TASK_PARAMS det_task_param2;
        det_task_param2.task_name = "det_task2";
        det_task_param2.task_type = 1;
        det_task_param2.ptr = &dt_queue_1;
        add_task(&det_task_param2);*/

        TASK_PARAMS track_task_param;
        track_task_param.task_name = "track_task";
        track_task_param.task_type = 4;
        track_task_param.ptr = nullptr;
        add_task(&track_task_param);

        TASK_PARAMS output_task_param;
        output_task_param.task_name = "output_task";
        output_task_param.task_type = 3;
        output_task_param.ptr = nullptr;
        add_task(&output_task_param);

        while(!exit_flag)
        {
            OPEN_TASK_PARAM open_task_param;
            bool ret = open_task_queue_.WaitAndTryPop(open_task_param, std::chrono::microseconds(100));
            if(ret)
            {
                TASK_PARAMS rtsp_task_param;
                rtsp_task_param.task_name = open_task_param.task_name;
                rtsp_task_param.task_type = 0;
                open_task_param.task_id = open_task_param.task_id;
                rtsp_task_param.ptr = &open_task_param;
                if (open_task_param.type == 0)
                {
                    add_task(&rtsp_task_param);
                }
                else if(open_task_param.type == 1)
                {
                    stop_task(&rtsp_task_param);
                }
                    
            }
            
            Sleep(1);
        }
        logger_->info("app server exit");
        return true;
    }
    int AppServer::task_run(std::shared_ptr<AppTaskBase> &c_ptr, void * param)
    {
        c_ptr->run(param);
        return 0;
    }
}