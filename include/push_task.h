#ifndef PUSH_TASK_H
#define PUSH_TASK_H

#include <map>
#include "app_task_base.h"

namespace FLOW_COUNT
{
    class PushTask : public AppTaskBase
    {
    public:
        PushTask(std::string task_name);
        ~PushTask();

        bool run(void* ptr) override;
        bool stop() override;
        APP_TASK_STATUS status() override;
    private:
        //std::map<int,TRACK_TASK_INFO> track_tmp;
        std::map<std::string, std::map<int, TRACK_TASK_INFO>> task_tracks;
        std::map<std::string, IN_OUT_COUNT> task_count;
    };
}

#endif