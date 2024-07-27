#ifndef OUTPUT_TASK_H
#define OUTPUT_TASK_H

#include <map>
#include "app_task_base.h"

namespace FLOW_COUNT
{
    struct TRACK_TASK_INFO
    {
        int track_id;
        int in_line; //1,2 [当1先出现就是进]
        int out_line;//1,2
        int loss;
    };
    struct IN_OUT_COUNT
    {
        int in_d;
        int out_d;
    };
    class OutputTask : public AppTaskBase
    {
        public:
            OutputTask(std::string task_name);
            ~OutputTask();

            bool run(void *ptr) override;  
            bool stop() override;
            APP_TASK_STATUS status() override;
        private:
            //std::map<int,TRACK_TASK_INFO> track_tmp;
            std::map<std::string, std::map<int, TRACK_TASK_INFO>> task_tracks;
            std::map<std::string, IN_OUT_COUNT> task_count;
    };
}

#endif