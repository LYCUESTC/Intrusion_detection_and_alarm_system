#include "det_task.h"
// #include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "yolov8_onnx.hpp"
#include "myutils.h"
namespace FLOW_COUNT
{

    DetTask::DetTask(std::string task_name)
    {
        exit_flag = false;
        logger_ = gLoggerInstance->clone(task_name);
        task_name_ = task_name;
    }
    DetTask::~DetTask()
    {
    }

    bool DetTask::run(void *ptr)
    {
        FLOW_COUNT::ThreadSafeQueue<DT_DATA> *dt_queue_q = (FLOW_COUNT::ThreadSafeQueue<DT_DATA> *)ptr;
        app_task_status_.task_status = TASK_RUNING;
        auto model = Yolov8Onnx::create(std::string("yolov8_model.onnx"), 0.3);
        if (!model)
        {
            logger_->error(" load model fail");
            return false;
        }
        unsigned int count = 0;

        while (!exit_flag)
        {

            if (dt_queue_q->Size() > 0)
            {
                // logger_->info("{}:dt_data size:[ {} ]", task_name_, dt_queue_q->Size());
                DT_DATA dt_data;
                if (dt_queue_q->WaitAndTryPop(dt_data, std::chrono::microseconds(100)))
                {
                    auto start_times = std::chrono::system_clock::now().time_since_epoch();
                    auto start_timestamp = std::chrono::duration_cast<std::chrono::seconds>(start_times).count();
                    /*memset(data_frame, 0 , 2560 * 1440 * 3);*/
                    char *p_data = (char *)(dt_data.bgr_data.get());
                    cv::Mat frame(dt_data.height, dt_data.width, CV_8UC3, p_data);
                    /*std::string save_name = std::to_string(dt_data.frame_id) + "_data.jpg";
                    cv::imwrite(save_name.c_str(), frame);*/
                    auto results = model->run(frame);
                    logger_->info("detect nums={}", results.bboxes.size());
                    std::vector<Detection> dets;
                    bool is_next = false;
                    for (auto it : results.bboxes)
                    {
                        Detection det;
                        if (it.label == 0)
                        {

                            logger_->info("[{},{},{},{}]->{}", it.box[0], it.box[1], it.box[2], it.box[3], it.score);

                            if (it.score > 0.8)
                            {
                                int s32X;
                                int s32Y;

                                s32X = (int)((it.box[0] + it.box[2]) * 0.5);
                                int h = (int)(it.box[3]);
                                s32Y = (h > dt_data.height) ? (dt_data.height - 1) : h;

                                for (int ix = 0; ix < dt_data.mask_points.nums; ix++)
                                {
                                    int mask_p = utility::inpoly(dt_data.mask_points.tpoints[ix].points, 4, s32X, s32Y);
                                    logger_->info("s32X={},s32Y={}, mask_p:{}", s32X, s32Y, mask_p);
                                    if (mask_p == 1)
                                    {
                                        is_next = true;
                                        det.bbox[0] = it.box[0];
                                        det.bbox[1] = it.box[1];
                                        det.bbox[2] = it.box[2] - it.box[0];
                                        det.bbox[3] = it.box[3] - it.box[1];
                                        det.class_id = 0;
                                        det.conf = it.score;
                                        dets.push_back(det);
                                    }
                                }
                            }
                        }
                    }

                    // bool is_next = false;
                    // for(auto idx: dets)//x0,y0,w,h
                    //{
                    //     logger_->info("[{},{},{},{}]->{}", idx.bbox[0], idx.bbox[1], idx.bbox[2], idx.bbox[3], idx.conf);
                    //
                    //     if(idx.conf > 0.8)
                    //     {
                    //         int s32X;
                    //         int s32Y;
                    //
                    //         s32X = (int)(idx.bbox[0] + idx.bbox[2] * 0.5);
                    //         int h = (int)(idx.bbox[3] + idx.bbox[1]);
                    //         s32Y =  (h > dt_data.height) ? (dt_data.height-1):h;
                    //
                    //         for(int ix = 0; ix < dt_data.mask_points.nums; ix++)
                    //         {
                    //             int mask_p = utility::inpoly(dt_data.mask_points.tpoints[ix].points, 4, s32X, s32Y);
                    //             logger_->info("s32X={},s32Y={}, mask_p:{}",s32X, s32Y, mask_p);
                    //             if(mask_p == 1)
                    //             {
                    //                 is_next = true;
                    //             }
                    //         }
                    //     }
                    // }
                    if (is_next)
                    {

                        /*std::vector<unsigned char> data;
                        cv::imencode(".jpg", frame, data);*/

                        unsigned long outbuf_size = dt_data.bgr_length;
                        uint8_t *p_rawdata = nullptr;
                        p_rawdata = (uint8_t *)malloc(outbuf_size);
                        unsigned char *out_buf = (unsigned char *)p_rawdata;
                        memcpy(out_buf, frame.data, outbuf_size);
                        // std::string save_name = dt_data.openTaskParam.task_name + "_" + std::to_string(dt_data.openTaskParam.task_id)+ "_" + std::to_string(dt_data.timestamp)+ "_" + std::to_string(count) + ".jpg";
                        // logger_->info("======save_name={}=====", save_name);

                        // std::ofstream ofs(save_name, std::ofstream::binary);
                        // ofs.write((const char*)p_rawdata, outbuf_size);
                        // ofs.close();
                        TRACK_DATA post_data;
                        // POST_DATA post_data;
                        post_data.frame_id = dt_data.frame_id;
                        post_data.url = dt_data.url;
                        // post_data.type            = dt_data.type; //2-结束信号，1-其他
                        post_data.start_timestamp = dt_data.timestamp;
                        post_data.timestamp = dt_data.timestamp;
                        post_data.openTaskParam = dt_data.openTaskParam;
                        post_data.task_name = dt_data.task_name;
                        post_data.data.reset(p_rawdata, [](uint8_t *data)
                                             {
                            if (data) free(data); });
                        post_data.data_length = outbuf_size;
                        post_data.width = dt_data.width;
                        post_data.height = dt_data.height;
                        post_data.boxs = dets;
                        track_queue.Push(post_data);

                        count++;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

        logger_->info("detection task:{} exit", task_name_);
        app_task_status_.task_status = TASK_STOP;
    }
    bool DetTask::stop()
    {
        exit_flag = true;
        return true;
    }
    APP_TASK_STATUS DetTask::status()
    {
        return app_task_status_;
    }
}