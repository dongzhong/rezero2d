// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_BASE_LOGGING_H_
#define REZERO_BASE_LOGGING_H_

#include <cstring>
#include <sstream>
#include <string>

#include "rezero2d/base/macros.h"

namespace rezero {

typedef int LogSeverity;

// Default log levels. Negative values can be used for verbose log levels.
constexpr LogSeverity REZERO_LOG_INFO = 0;
constexpr LogSeverity REZERO_LOG_WARNING = 1;
constexpr LogSeverity REZERO_LOG_ERROR = 2;
constexpr LogSeverity REZERO_LOG_FATAL = 3;
constexpr LogSeverity REZERO_LOG_NUM_SEVERITIES = 4;

void Log(LogSeverity severity, const std::string& message);

class LogMessageVoidify {
 public:
  void operator&(std::ostream&) {}
};

class LogMessage {
 public:
  LogMessage(LogSeverity severity,
             const char* file,
             int line,
             const char* condition);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  std::ostringstream stream_;
  const LogSeverity severity_;
  const char* file_;
  const int line_;

  REZERO_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

} // namespace rezero

#define REZERO_LOG_STREAM(severity)                                                  \
  ::rezero::LogMessage(::rezero::REZERO_LOG_##severity, __FILE__, __LINE__, nullptr) \
      .stream()

#define REZERO_LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::rezero::LogMessageVoidify() & (stream)

#define REZERO_LOG(severity)                                                         \
  REZERO_LAZY_STREAM(REZERO_LOG_STREAM(severity), true)

#define REZERO_CHECK(condition)                                                 \
  REZERO_LAZY_STREAM(::rezero::LogMessage(::rezero::REZERO_LOG_FATAL, __FILE__, \
                                          __LINE__, #condition)                 \
                       .stream(),                                               \
                    !(condition))

#define REZERO_EAT_STREAM_PARAMETERS(ignored) \
  true || (ignored)                           \
      ? (void)0                               \
      : ::rezero::LogMessageVoidify() &       \
            ::rezero::LogMessage(::rezero::REZERO_LOG_FATAL, 0, 0, nullptr).stream()

#ifndef NDEBUG
#define REZERO_DCHECK(condition) REZERO_CHECK(condition)
#else
#define REZERO_DCHECK(condition) REZERO_EAT_STREAM_PARAMETERS(condition)
#endif

#endif // REZERO_BASE_LOGGING_H_
