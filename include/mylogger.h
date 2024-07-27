#ifndef _LOGGER_H__
#define _LOGGER_H__

#include <memory>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging

namespace FLOW_COUNT {
    using RawLogger = spdlog::logger;

    class Logger {
        public:
            Logger(const std::string name, const std::string level = "info", const std::string file = "") {

                logger_ = spdlog::get(name);
                if (logger_) {
                    return;
                }

                if (file.size() == 0) {
                    shared_file_sink_ = nullptr;
                    logger_ = spdlog::stdout_color_mt(name);
                } else {
                    shared_file_sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file, default_file_max_size_, default_max_files_);
                    logger_ = std::make_shared<spdlog::logger>(name, shared_file_sink_);
                }

                if (level == "release")
                {
                    logger_->set_level(spdlog::level::warn);
                }
                else
                {
                    logger_->set_level(spdlog::level::info);
                    logger_->flush_on(spdlog::level::info);
                }
            }

            Logger(const std::string name, std::shared_ptr<spdlog::sinks::sink> file_sink) {
                if (file_sink) {
                    shared_file_sink_ = file_sink;
                    logger_ = std::make_shared<spdlog::logger>(name, file_sink);
                } else {
                    shared_file_sink_ = nullptr;
                    logger_ = spdlog::stdout_color_mt(name);
                }

                logger_->flush_on(spdlog::level::err);
            }

            std::shared_ptr<spdlog::sinks::sink> get_filesink(const std::shared_ptr<Logger> logger) const {
                return logger->shared_file_sink_;
            }

            std::shared_ptr<spdlog::logger> clone(const std::string name) {
                return logger_->clone(name);
            }

            template<typename... Args>
            void info(const char *fmt, const Args &... args)
            {
                logger_->log(spdlog::level::info, fmt, args...);
            }

            template<typename... Args>
            void debug(const char *fmt, const Args &... args)
            {
                logger_->log(spdlog::level::debug, fmt, args...);
            }

            template<typename... Args>
            void warn(const char *fmt, const Args &... args)
            {
                logger_->log(spdlog::level::warn, fmt, args...);
            }

            template<typename... Args>
            void error(const char *fmt, const Args &... args)
            {
                logger_->log(spdlog::level::err, fmt, args...);
            }

            ~Logger() {
                logger_->flush();
                spdlog::drop(logger_->name());
            }

        private:
            std::shared_ptr<spdlog::logger> logger_;
            std::shared_ptr<spdlog::sinks::sink> shared_file_sink_;
            // 最大文件大小
            int default_file_max_size_ {50*1024*1024};
            // 文件分段个数
            int default_max_files_ {10};
    };
}

extern std::shared_ptr<FLOW_COUNT::Logger> gLoggerInstance;

#endif

