#ifndef TRACK_TASK_H
#define TRACK_TASK_H

#include "app_task_base.h"

namespace FLOW_COUNT
{
    class TrackTask : public AppTaskBase
    {
        public:
            TrackTask(std::string task_name);
            ~TrackTask();

            bool run(void *ptr) override;
            bool stop() override;
            APP_TASK_STATUS status() override;
    };
}

#endif