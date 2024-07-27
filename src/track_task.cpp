#include "track_task.h"
#include "BYTETracker.h"

namespace FLOW_COUNT
{
    TrackTask::TrackTask(std::string task_name)
    {
        task_name_ = task_name;
        exit_flag = false;
        logger_ = gLoggerInstance->clone("track_task_"+task_name);
    }

    TrackTask::~TrackTask()
    {

    }

    bool TrackTask::run(void *ptr)
    {
        int fps=20;
        //BYTETracker bytetracker(fps, 30);
        std::map<std::string, BYTETracker> track_lists;
        while(!exit_flag)
        {
            if(track_queue.Size() > 0)
            {
                TRACK_DATA track_data;
                if(track_queue.WaitAndTryPop(track_data, std::chrono::microseconds(100)))
                {
                    std::string task_name = track_data.task_name;
                    auto it = track_lists.find(task_name);
                    if(it == track_lists.end())
                    {
                        track_lists.insert(std::make_pair(task_name, BYTETracker(fps, 30)));
                    }
                    std::vector<Detection> objects;
                    std::vector<Detection> results = track_data.boxs;
                    for (Detection dr : results)
                    {
                        int classId = (int)(dr.class_id);
                        //std::cout<<"conf="<<dr.conf<<"\n";
                        if(classId == 0) //person
                        {
                            objects.push_back(dr);
                        }
                    }
                    
                    std::vector<STrack> output_stracks = track_lists[task_name].update(objects);
                    //std::cout<<"output_stracks="<<output_stracks.size()<<"\n";
                    for (unsigned long i = 0; i < output_stracks.size(); i++)
                    {
                        std::vector<float> tlwh = output_stracks[i].tlwh;

                        bool vertical = tlwh[2] / tlwh[3] > 1.6;
                        //std::cout<<"tlwh=["<<tlwh[0]<<","<<tlwh[1]<<","<<tlwh[2]<<","<<tlwh[3]<<",vertical="<<vertical<<"\n";
                        if (tlwh[2] * tlwh[3] > 20 && !vertical)
                        {
                            logger_->info("track_id:{}", output_stracks[i].track_id);
                            OUTPUT_DATA output_data;
                            output_data.task_name = task_name;
                            output_data.timestamp = track_data.timestamp;
                            output_data.data_length = track_data.data_length;
                            output_data.data = track_data.data;
                            TRACK_INFO track_info;
                            track_info.track_id = output_stracks[i].track_id;
                            track_info.xywh[0] = tlwh[0];
                            track_info.xywh[1] = tlwh[1];
                            track_info.xywh[2] = tlwh[2];
                            track_info.xywh[3] = tlwh[3];
                            output_data.boxs = track_data.boxs;
                            output_data.tracks.push_back(track_info);
                            output_data.width = track_data.width;
                            output_data.height = track_data.height;
                            output_queue.Push(output_data);
                            // cv::Scalar s = tracker.get_color(output_stracks[i].track_id);
                            // cv::putText(frame, cv::format("%d", output_stracks[i].track_id), cv::Point(tlwh[0], tlwh[1] - 5),
                            //         0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
                            // cv::rectangle(frame, cv::Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3]), s, 2);
                        }
                    }
                }
            }
        }
        return true;
    }

    bool TrackTask::stop()
    {
        exit_flag = true;
        return true;
    }

    APP_TASK_STATUS TrackTask::status()
    {
        return app_task_status_;
    }
}