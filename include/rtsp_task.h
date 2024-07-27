#ifndef RTSP_TASK_H
#define RTSP_TASK_H

#include "app_task_base.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace FLOW_COUNT
{
    class RtspTask : public AppTaskBase
    {
        public:
            RtspTask(std::string task_name);
            ~RtspTask();

            bool run(void *ptr) override;
            bool stop() override;
            APP_TASK_STATUS status() override;
        private:
            
    };
}
#endif