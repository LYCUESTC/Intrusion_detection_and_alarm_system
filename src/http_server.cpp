#include "http_server.h"

namespace FLOW_COUNT
{
    /*
    * {
    *   "type":0,1  0 打开 1-关闭
    *   "task_id":1
        task_name;
        rtsp_url;
        region_size;
        regions[4][8];
    * }
    */
    void TaskResultCallback(const httplib::Request &req, httplib::Response &res)
    {
        std::cout << "recv json...............\n";
        Json::Value root;
        if(req.body.empty())
        {
            std::cout<<"recv body is empty\n";
            res.set_content("{\"code\":201,\"msg\":\"recv body error\"}", "application/json");
        }
        std::string output_data;
        Json::Reader reader;
        Json::Value value;
        if(!reader.parse(req.body, value))
        {
            root["statusCode"] = 202;
            root["message"] = "Invalid json";
            root["data"] = "{}";
            output_data = root.toStyledString();

            res.set_content(output_data, "application/json");
            return;
        }
        std::cout << "value=" << value.toStyledString() << "\n";
        
        int recv_type = value["type"].asInt();
        if(recv_type == 0)
        {
            OPEN_TASK_PARAM open_task;
            open_task.task_id = value["task_id"].asInt();
            open_task.task_name = value["task_name"].asString();
            open_task.rtsp_url = value["rtsp_url"].asString();
            auto regions = value["regions"];
            open_task.region_size = regions.size();
            open_task.type = recv_type;
            for (int j = 0; j < regions.size(); j++)
            {
                auto regions_item = regions[j];

                for (int inx = 0; inx < 8; inx++)
                {
                    open_task.regions[j][inx] = regions_item[inx].asFloat();
                }
            }
            open_task_queue_.Push(open_task);
 
            root["statusCode"] = 200;
            root["message"] = "success";
        }else if(recv_type == 1)
        {
            // std::lock_guard<std::mutex> lk(smartData.close_mutex);
            // smartData.close_flag = true;
            OPEN_TASK_PARAM open_task;
            //open_task.task_id = value["task_id"].asInt();
            open_task.task_name = value["task_name"].asString();
            open_task.rtsp_url = value["rtsp_url"].asString();
            //auto regions = value["regions"];
            //open_task.region_size = regions.size();
            open_task.type = recv_type;
            open_task_queue_.Push(open_task);
            root["statusCode"] = 200;
            root["message"] = "success";
        }else{
            root["statusCode"] = 202;
            root["message"] = "Invalid command";
        }

        root["data"] = "{}";
        output_data = root.toStyledString();
        //gLoggerInstance->info("output_data={}",output_data);
        res.set_content(output_data, "application/json");

        return ;
    }
    static void http_server_func(httplib::Server &server, std::string ip, int port)
    {
        std::cout << "ip=" << ip << ",port=" << port << "\n";
        server.Post("/unmanned/operate", TaskResultCallback);
        std::cout << "HttpServer Start running...\n";
        //server.listen("0.0.0.0", 1568);
        server.listen(ip.c_str(), port);
        std::cout << "HttpServer Exit\n";
    }
    HttpServer::HttpServer(std::string task_name)
    {
       
    }
    HttpServer::~HttpServer()
    {

    }

    bool HttpServer::run(void *ptr)
    {
        HTTP_PARAM * http_param = (HTTP_PARAM *)ptr;
        http_thread = std::thread(http_server_func, std::ref(server), http_param->ip, http_param->port);
        http_thread.detach();
        return true;
    } 
    bool HttpServer::stop()
    {
        server.stop();
        std::cout<<"http server exit\n";
        return true;
    }
    APP_TASK_STATUS HttpServer::status()
    {
        return app_task_status_;
    }
}