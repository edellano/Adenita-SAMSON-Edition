#include "ADNLogger.hpp"


void ADNLogger::LogError(std::string value)
{
  SB_ERROR("Adenita error: " + value);
}

void ADNLogger::LogError(QString value)
{
  LogError(value.toStdString());
}

void ADNLogger::LogDebug(std::string value)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == SEConfigMode::DEBUG_LOG) {
    SB_WARNING("Adenita debug: " + value);
  }
}

void ADNLogger::LogDebug(QString value)
{
  LogDebug(value.toStdString());
}

void ADNLogger::Log(std::string value)
{
  SB_INFORMATION("Adenita info: " + value);
}

void ADNLogger::Log(QString value)
{
  Log(value.toStdString());
}