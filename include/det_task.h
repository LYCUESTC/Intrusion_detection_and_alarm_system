#ifndef DET_TASK_H
#define DET_TASK_H

#include "app_task_base.h"


namespace FLOW_COUNT
{
    
    class DetTask : public AppTaskBase
    {
        public:
            DetTask(std::string task_name);
            ~DetTask();

            bool run(void *ptr) override;  
            bool stop() override;
            APP_TASK_STATUS status() override;
        private:
            
            //FLOW_COUNT::ThreadSafeQueue<DT_DATA> dt_queue_q;
    };
}
#endif
