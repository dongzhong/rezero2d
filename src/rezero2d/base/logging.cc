// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/base/logging.h"

namespace rezero {

namespace {

const char* const kLogTag = "Rezero";

const char* const kLogSeverityNames[REZERO_LOG_NUM_SEVERITIES] = {
    "INFO", "WARNING", "ERROR", "FATAL"};

const char* GetNameForLogSeverity(LogSeverity severity) {
  if (severity >= REZERO_LOG_INFO && severity < REZERO_LOG_NUM_SEVERITIES) {
    return kLogSeverityNames[severity];
  }
  return "UNKNOWN";
}

const char* StripDots(const char* path) {
  while (strncmp(path, "../", 3) == 0) {
    path += 3;
  }
  return path;
}

const char* StripPath(const char* path) {
  auto* p = strrchr(path, '/');
  if (p) {
    return p + 1;
  } else {
    return path;
  }
}

} // namespace

void Log(LogSeverity severity, const std::string& message) {
  std::fprintf(stderr, "[%s] %s", kLogTag, message.c_str());
  std::fflush(stderr);
}

LogMessage::LogMessage(LogSeverity severity,
                       const char* file,
                       int line,
                       const char* condition)
    : severity_(severity), file_(file), line_(line) {
  stream_ << "[";
  if (severity >= REZERO_LOG_INFO) {
    stream_ << GetNameForLogSeverity(severity);
  } else {
    stream_ << "VERBOSE" << -severity;
  }
  stream_ << ":"
          << (severity > REZERO_LOG_INFO ? StripDots(file_) : StripPath(file_))
          << "(" << line_ << ")] ";

  if (condition) {
    stream_ << "Check failed: " << condition << ". ";
  }
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
  Log(severity_, stream_.str());
  if (severity_ >= REZERO_LOG_FATAL) {
    abort();
  }
}

} // namespace rezero
