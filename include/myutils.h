#ifndef MYUTILS_H
#define MYUTILS_H

#include <chrono>

namespace utility
    {
        /*
        * @brief: 获取单调的时钟 (不依赖系统时钟)
        * @param: 无
        * @ret:   时间信息
        * */
        inline std::chrono::steady_clock::time_point now()
        {
            return std::chrono::steady_clock::now();
        }

        /*
         * @brief: 获取系统时钟
         * @param: 无
         * @ret:   时间信息
         * */
        inline std::chrono::system_clock::time_point system_now()
        {
            return std::chrono::system_clock::now();
        }

        /*
         * @brief: 获取单调时钟间隔
         * @param: [in] begin : 单调时钟起始点
         * @param: [in] end : 单调时钟结束点
         * @ret:   时间间隔, 单位: ms
         * */
        inline int64_t duration_ms(std::chrono::steady_clock::time_point &begin, std::chrono::steady_clock::time_point &end)
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        }

        /*
         * @brief: 获取单调时钟间隔
         * @param: [in] begin : 单调时钟起始点
         * @param: [in] end : 单调时钟结束点
         * @ret:   时间间隔, 单位: us
         * */
        inline int64_t duration_us(std::chrono::steady_clock::time_point &begin, std::chrono::steady_clock::time_point &end)
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        }

        /*
         * @brief: 转化为int64_t格式的时间戳
         * @param: [in] now : 系统时钟信息
         * @ret:   int64_t 时间戳, 单位: ms
         * */
        inline int64_t timestamp_ms(std::chrono::system_clock::time_point &now)
        {
            return std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        }

        /*
         * @brief: 转化为int64_t格式的时间戳
         * @param: [in] now : 系统时钟信息
         * @ret:   int64_t 时间戳, 单位: s
         * */
        inline int64_t timestamp_sec(std::chrono::system_clock::time_point &now)
        {
            return std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();
        }
        inline int inpoly(unsigned int poly[][2], int npoints, unsigned int xt, unsigned int yt)
        {
            unsigned int xnew,ynew;
            unsigned int xold,yold;
            unsigned int x1,y1;
            unsigned int x2,y2;
            int i;
            int inside = 0;

            if (npoints < 3)
            {
                return(0);
            }
            xold = poly[npoints-1][0];
            yold = poly[npoints-1][1];
            for (i=0 ; i < npoints ; i++) 
            {
                xnew = poly[i][0];
                ynew = poly[i][1];
                if (xnew > xold) 
                {
                    x1 = xold;
                    x2 = xnew;
                    y1 = yold;
                    y2 = ynew;
                }
                else
                {
                    x1 = xnew;
                    x2 = xold;
                    y1 = ynew;
                    y2 = yold;
                }
                if((xnew < xt) == (xt <= xold) && ((long)yt-(long)y1)*(long)(x2-x1) < ((long)y2-(long)y1)*(long)(xt-x1)) 
                {
                    inside = !inside;
                }
                xold = xnew;
                yold = ynew;
            }

            return (inside);
        }
}

#endif