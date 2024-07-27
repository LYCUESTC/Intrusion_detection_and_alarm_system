#pragma once

#include "thread_safe_queue.h"
//#include "myai_type.h"

// return value
struct alignas(float) Detection {
    //center_x center_y w h
    float bbox[4];
    float conf;  // bbox_conf * cls_conf
    float class_id;
};
enum TASK_STATUS
{
    TASK_NORUNING = 0,
    TASK_RUNING   = 1,
    TASK_ERROR    = 2,
    TASK_STOP     = 3,
};
typedef struct _APP_TASK_STATUS_
{
    TASK_STATUS task_status;

}APP_TASK_STATUS;

typedef struct _TASK_PARAMS_
{
    std::string task_name;
    int task_type; //0-rtsp,1-det,2-http,3-output
    void *ptr;
}TASK_PARAMS;

struct OPEN_TASK_PARAM
{
    int task_id;
    int type;//0-创建，1-关闭
    std::string task_name;
    std::string rtsp_url;
    int region_size;
    float regions[4][8];
};
struct TPOINTS
{
    unsigned int points[4][2];
};

struct MASK_POINTS
{
    int nums;
    TPOINTS tpoints[4];
};
struct DT_DATA
{
    std::string task_name;
    unsigned long frame_id;
    std::string url;
    long timestamp;
    MASK_POINTS mask_points;
    OPEN_TASK_PARAM openTaskParam;
    // int region_size;
    // float regions[4][8];
    std::shared_ptr<uint8_t> bgr_data;
    long bgr_length;
    int width;
    int height;
};
// struct TRACK_INFO
// {
//     int track_id;
//     float rect[4];
// };
struct TRACK_DATA
{
    std::string task_name;
    unsigned long frame_id;
    std::string url;
    int type; //2-结束信号，1-其他
    long start_timestamp;
    long timestamp;
    OPEN_TASK_PARAM openTaskParam;
    std::shared_ptr<uint8_t> data; //jpg
    std::vector<Detection> boxs;
    long data_length;
    int width;
    int height;
};
struct TRACK_INFO
{
    int track_id;
    float xywh[4];
};
struct OUTPUT_DATA
{
    std::string task_name;
    long timestamp;
    std::shared_ptr<uint8_t> data; //jpg
    long data_length;
    std::vector<TRACK_INFO> tracks;
    std::vector<Detection> boxs;
    int width;
    int height;
};
extern FLOW_COUNT::ThreadSafeQueue<OPEN_TASK_PARAM> open_task_queue_;
extern FLOW_COUNT::ThreadSafeQueue<DT_DATA> dt_queue_0;
extern FLOW_COUNT::ThreadSafeQueue<DT_DATA> dt_queue_1;
extern FLOW_COUNT::ThreadSafeQueue<TRACK_DATA> track_queue;
extern FLOW_COUNT::ThreadSafeQueue<OUTPUT_DATA> output_queue;