#include "output_task.h"
#include "httplib.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
namespace FLOW_COUNT
{
    OutputTask::OutputTask(std::string task_name)
    {
        task_name_ = task_name;
        exit_flag = false;
        logger_ = gLoggerInstance->clone("output_task_" + task_name);
    }

    OutputTask::~OutputTask()
    {

    }
    std::vector<int> b = { 144, 89, 30, 3, 16, 69, 237, 54, 4, 89, 15, 141, 87, 65, 118, 150, 117, 119, 19, 90, 33, 53, 39, 11, 228, 93, 40, 164, 46, 228, 48, 163, 114, 182, 232, 103, 21, 49, 116, 54, 62, 160, 159, 163, 212, 117, 237, 169, 94, 16, 79, 124, 68, 154, 190, 70, 203, 178, 64, 55, 206, 79, 25, 230, 43, 52, 255, 230, 116, 3, 135, 175, 78, 158, 254, 50, 161, 223, 204, 108, 63 };
    std::vector<int> g = { 246, 80, 103, 0, 134, 12, 197, 233, 7, 31, 118, 88, 161, 221, 236, 228, 71, 81, 26, 143, 188, 5, 154, 6, 152, 224, 39, 126, 196, 216, 177, 149, 161, 65, 192, 133, 5, 254, 151, 66, 158, 117, 193, 173, 56, 252, 5, 197, 37, 143, 131, 220, 229, 73, 176, 60, 124, 46, 36, 44, 200, 92, 126, 216, 248, 151, 189, 162, 135, 145, 244, 158, 135, 188, 34, 33, 99, 10, 146, 107, 139 };
    std::vector<int> r = { 100, 9, 243, 216, 240, 65, 11, 32, 124, 164, 27, 14, 83, 143, 49, 27, 59, 131, 138, 184, 178, 176, 73, 16, 10, 226, 189, 30, 150, 31, 126, 95, 144, 13, 205, 128, 226, 39, 158, 12, 202, 155, 210, 255, 250, 106, 93, 25, 56, 36, 51, 20, 134, 108, 120, 41, 118, 163, 162, 55, 122, 160, 89, 173, 240, 218, 187, 150, 231, 78, 177, 184, 160, 246, 36, 11, 152, 221, 108, 249, 216 };

    std::vector<std::string> classes{ "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant",
    "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
    "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog",
      "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
      "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush" };
    std::string Stamp2Time(long long timestamp)
    {
        auto milli = (timestamp + static_cast<long long>(8) * 60 * 60) * 1000; // 转化为东八区北京时间
        auto mTime = std::chrono::milliseconds(milli);
        auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
        auto tt = std::chrono::system_clock::to_time_t(tp);
        std::tm* now = gmtime(&tt);
        /*printf("%4d年%02d月%02d日 %02d:%02d:%02d\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);*/
        char s[20];
        strftime(s, sizeof(s), "%Y-%m-%d %H/%M/%S", now);
        std::string str(s);

        return str;
    }
    bool OutputTask::run(void *ptr)
    {
        httplib::Client cli("localhost", 6008);
        while(!exit_flag)
        {
            auto tmp_track = task_tracks;
            for(auto it:tmp_track)//task_name:map
            {
                for(auto item:it.second)//map
                {
                    if(item.second.loss > 20)
                    {
                        auto idx = task_tracks[it.first].find(item.first);
                        task_tracks[it.first].erase(idx);
                    }
                } 
            }
            if(output_queue.Size() > 0)
            {
                OUTPUT_DATA output_data;
                if(output_queue.WaitAndTryPop(output_data, std::chrono::microseconds(100)))
                {
                    std::string task_name = output_data.task_name;
                    
                    auto it = task_tracks.find(task_name);
                    if(it == task_tracks.end())
                    {
                        std::map<int,TRACK_TASK_INFO> item;
                        for(auto idx:output_data.tracks)
                        {
                            TRACK_TASK_INFO track_task_info;
                            track_task_info.track_id = idx.track_id;
                            track_task_info.loss = 0;
                           
                            item[idx.track_id] = track_task_info;
                        }
                        task_tracks[task_name] = item;
                        continue;
                    }
                    auto output_data_item = output_data.tracks;
                    for(auto item:task_tracks[task_name])
                    {
                        bool is_next = false;
                        int count = 0;
                        int y_center = 0;
                        for(auto idx:output_data_item)
                        {
                            if(idx.track_id == item.first)
                            {
                                is_next = true;
                                output_data_item.erase(output_data_item.begin()+count);
                                y_center = idx.xywh[1] + idx.xywh[3] / 2;
                                break;
                            }
                            count += 1;
                        }
                        if(!is_next)
                        {
                            task_tracks[task_name][item.second.track_id].loss += 1;
                            
                        }else{
                            task_tracks[task_name][item.second.track_id].loss = 0;
                        }
                    }
                    for(auto idx:output_data_item)//新得track_id,加到track_tmp
                    {
                        TRACK_TASK_INFO track_task_info;
                        track_task_info.track_id = idx.track_id;
                        track_task_info.loss = 0;
                       
                        task_tracks[task_name][idx.track_id] = track_task_info;
                        logger_->info("new track_id:{}", idx.track_id);
                        std::string save_name = std::to_string(idx.track_id) + "_data.jpg";
                        logger_->info("======save_name={}=====", save_name);
                        logger_->info("output_data.data_length={}", output_data.data_length);
                        cv::Mat frame(output_data.height, output_data.width, CV_8UC3, output_data.data.get());
                        for (auto& res : output_data.boxs) {
                            int label = res.class_id;
                            auto& box = res.bbox;
                            /*std::cout << "result: " << label << "\t" << classes[label] << "\t" << std::fixed << std::setprecision(5)
                                << box[0] << "\t" << box[1] << "\t" << box[2] << "\t" << box[3] << "\t"
                                << std::setprecision(6) << res.conf << "\n";*/
                            cv::rectangle(frame, cv::Point(box[0], box[1]), cv::Point(box[2] + box[0], box[3]+box[1]),
                                cv::Scalar(b[label], g[label], r[label]), 3, 1, 0);
                            cv::putText(frame, classes[label] + " " + std::to_string(res.conf),
                                cv::Point(box[0] + 5, box[1] + 20), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                cv::Scalar(b[label], g[label], r[label]), 2, 4);
                        }
                        std::vector<unsigned char> data;
                        cv::imencode(".jpg", frame, data);
                        /*std::ofstream ofs(save_name, std::ofstream::binary);
                        ofs.write((const char*)data.data(), data.size() *sizeof(unsigned char));
                        ofs.close();*/
                        
                       
                        std::string body(data.size() * sizeof(unsigned char), 0);
                        memcpy((char*)(body.c_str()), data.data(), data.size() * sizeof(unsigned char));
                     
                        std::cout << "timestamp=" << output_data.timestamp << "\n";
                        std::string time_str = Stamp2Time(output_data.timestamp);
                        std::string apt_str = "/alarm/" + output_data.task_name + "/" + std::to_string(output_data.timestamp) + "?name=alarm&alarm_time=" + time_str;
                        ////cli.Post("/alarm/test/test.jpg?name=tes2&alarm_time=2022-1-23 22\:08\:00", body, "image/jpeg");
                        cli.Post(apt_str.c_str(), body, "image/jpeg");
                    }

                    //std::cout<<"**************************in_count="<<task_count[task_name].in_d<<"\n**************************out_count="<<task_count[task_name].out_d<<"\n";
                }
            }
        }
        return true;
    }

    bool OutputTask::stop()
    {
        exit_flag = true;
        logger_->info("stop output_task successful");
        return true;
    }

    APP_TASK_STATUS OutputTask::status()
    {
        return app_task_status_;
    }
}