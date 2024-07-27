#include "myutils.h"
#include "rtsp_task.h"


namespace FLOW_COUNT
{
    RtspTask::RtspTask(std::string task_name)
    {
        exit_flag = false;
        logger_ = gLoggerInstance->clone("rtsp_task_"+task_name);
        task_name_ = task_name;
    }

    RtspTask::~RtspTask()
    {

    }

    bool RtspTask::run(void *ptr)
    {
        logger_->info("into rtsp task");
        OPEN_TASK_PARAM * param = (OPEN_TASK_PARAM *)ptr;
        std::string rtsp_url = param->rtsp_url;
        cv::VideoCapture stream;
        stream = cv::VideoCapture(rtsp_url, cv::CAP_FFMPEG);

        if (!stream.isOpened())
        {
            logger_->error("open rtsp:{} error", rtsp_url);
            exit_flag = true;
            app_task_status_.task_status = TASK_ERROR;
            return false;
        }
        
        logger_->info("rtsp url:[{}]", rtsp_url);
        

        int interval = 200;
        int64_t last_ms_ts = 0;
        int64_t start_ts_point = 0;
        int count_fps = 0;
        unsigned long frame_id_count = 1;
        app_task_status_.task_status = TASK_RUNING;
        int retry_count = 20;
        while (!exit_flag)
        {
            //auto time1 = FLOW_COUNT::utility::now();
            auto start_times = std::chrono::system_clock::now().time_since_epoch();
            auto start_timestamp = std::chrono::duration_cast<std::chrono::seconds>(start_times).count();
            cv::Mat frame;
            
            if (!stream.read(frame))
            {
                std::cout << "no video read" << std::endl;
                if (retry_count < 0)
                {
                    app_task_status_.task_status = TASK_ERROR;
                    exit_flag = true;
                }
                retry_count--;
                continue;
            }
            int g_height = frame.rows;
            int g_width = frame.cols;
            int size = g_width * g_height * 3;

            interval = 1000 / 5;
            auto sys_now = utility::system_now();
            auto current_ms_ts = utility::timestamp_ms(sys_now);
            
            if (current_ms_ts - last_ms_ts < interval)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            last_ms_ts = current_ms_ts;
            {
                if (start_ts_point == 0)
                {
                    auto sys_now = utility::system_now();
                    start_ts_point = utility::timestamp_ms(sys_now);
                }

                count_fps++;

                auto sys_now1 = utility::system_now();
                auto end_ts_point = utility::timestamp_ms(sys_now1);
                
                if (end_ts_point - start_ts_point > 1000)
                {
                    logger_->info("-------> Task Id: {} [[[ count fps: {} ]]] <---------", rtsp_url, count_fps);
                    start_ts_point = end_ts_point;
                    count_fps = 0;
                }
            }

            auto time2 = utility::now();
            uint8_t *p_rawdata = nullptr;
            //int size = frame.rows * frame.cols * 3;
            p_rawdata = (uint8_t *) malloc(size);
            memset(p_rawdata, 0 , size);
            memcpy(p_rawdata, frame.data, size);
           
            DT_DATA bgr_data;
            bgr_data.url = rtsp_url;
            bgr_data.timestamp = start_timestamp;
            //yuv_data.openTaskParam = param.openTaskParam;
            bgr_data.frame_id = frame_id_count;
            frame_id_count++;
            bgr_data.bgr_data = std::make_shared<uint8_t>();
            bgr_data.bgr_data.reset(p_rawdata, [](uint8_t *data)
            {
                if (data) free(data);
            });
            bgr_data.bgr_length = size;
            bgr_data.width = g_width;
            bgr_data.height = g_height;
            bgr_data.openTaskParam = *param;
            bgr_data.task_name = task_name_;
            //MASK_POINTS ptLine_g;
            int nums = (param->region_size > 4)?4:param->region_size;
            for(int i = 0; i < nums ; i++)
            {
                bgr_data.mask_points.tpoints[i].points[0][0] = param->regions[i][0];
                bgr_data.mask_points.tpoints[i].points[0][1] = param->regions[i][1];
                bgr_data.mask_points.tpoints[i].points[1][0] = param->regions[i][2];
                bgr_data.mask_points.tpoints[i].points[1][1] = param->regions[i][3];
                bgr_data.mask_points.tpoints[i].points[2][0] = param->regions[i][4];
                bgr_data.mask_points.tpoints[i].points[2][1] = param->regions[i][5];
                bgr_data.mask_points.tpoints[i].points[3][0] = param->regions[i][6];
                bgr_data.mask_points.tpoints[i].points[3][1] = param->regions[i][7];
                logger_->info("--------------mask:[{},{}],[{},{}],[{},{}],[{},{}]--------", 
                    bgr_data.mask_points.tpoints[i].points[0][0],
                    bgr_data.mask_points.tpoints[i].points[0][1],
                    bgr_data.mask_points.tpoints[i].points[1][0],
                    bgr_data.mask_points.tpoints[i].points[1][1],
                    bgr_data.mask_points.tpoints[i].points[2][0],
                    bgr_data.mask_points.tpoints[i].points[2][1],
                    bgr_data.mask_points.tpoints[i].points[3][0],
                    bgr_data.mask_points.tpoints[i].points[3][1]);
                
            }

            bgr_data.mask_points.nums = nums;
            dt_queue_0.Push(bgr_data);
            /*if((param->task_id % 2) == 0)
            {
                dt_queue_0.Push(bgr_data);
            }else
            {
                dt_queue_1.Push(bgr_data);
            }*/
            
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        
        logger_->info("rtsp close");
    }

    bool RtspTask::stop()
    {
        logger_->info("{} recv stop singal", task_name_);
        exit_flag = true;
        logger_->info("{} exit successful", task_name_);
        return true;
    }

    APP_TASK_STATUS RtspTask::status()
    {
        return app_task_status_;
    }
}