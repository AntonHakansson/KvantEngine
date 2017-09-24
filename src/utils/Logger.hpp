#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <chrono>
#define UNUSED(x) (void)(x)

/**
 *
 * A stream logger you can use just like std::cout, but with severity 
 * indication/filtering added.
 *
 *      LOG_DEBUG << "debug message";
 *      LOG_INFO << "info message";
 *      LOG_WARNING << "warning message";
 *      LOG_ERROR << "error message";
 *
 */

namespace Kvant {
    struct LogRecord; // Forward declaration
    struct LogRecorder {
       void operator <<(LogRecord&& t) {
           UNUSED(t);
       }
    };

    struct Logger {
        enum LogType {
            DEBUG = 0,
            INFO,
            WARNING,
            ERROR
        };
    
        static LogRecorder recorder;
        static LogType log_type;
        static bool check_level(LogType type);
        static std::string ts();
    };

    class LogRecord {
        public:
            LogRecord() {
                std::cout << std::chrono::high_resolution_clock::now().time_since_epoch().count() << " ";
            }

            virtual ~LogRecord() {
                std::cout << str() << std::endl;
            }

            operator std::stringstream&() {
                return _stream;
            }

            template<typename T>
            std::stringstream& operator <<(T&& t) {
                _stream << t;
                return _stream;
            }

            std::string str() const { return _stream.str(); }

        private:
            std::stringstream _stream;
    };
}

#ifdef DEBUG_LOGS
#define LOG_DEBUG \
    Kvant::LogRecord() << "[DEBUG] (" << __FILE__ << ":" << __LINE__ << ") "
#else
#define LOG_DEBUG  if (0) Kvant::LogRecord()
#endif

#ifdef INFO_LOGS
#define LOG_INFO \
    Kvant::LogRecord() << "[INFO] (" << __FILE__ << ":" << __LINE__ << ") "
#else
#define LOG_INFO  if (0) Kvant::LogRecord()
#endif

#ifdef WARNING_LOGS
#define LOG_WARNING \
    Kvant::LogRecord() << "[WARNING] (" << __FILE__ << ":" << __LINE__ << ") "
#else
#define LOG_WARNING  if (0) Kvant::LogRecord()
#endif

#ifdef ERROR_LOGS
#define LOG_ERROR \
    Kvant::LogRecord() << "[ERROR] (" << __FILE__ << ":" << __LINE__ << ") "
#else
#define LOG_ERROR  if (0) Kvant::LogRecord()
#endif

#endif // LOGGER_HPP_INCLUDED