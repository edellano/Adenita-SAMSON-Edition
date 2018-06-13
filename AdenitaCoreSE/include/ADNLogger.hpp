#pragma once

#include <time.h>
#include <string>
#include <boost/numeric/ublas/vector.hpp>
#include <QString>
#include <QTextStream>
#include <QFile>
#include "ADNAuxiliary.hpp"

namespace ublas = boost::numeric::ublas;

class ADNLogger
{
public:
  // singleton pattern
  ~ADNLogger() = default;
  // deleted functions should be public for better error message handling
  ADNLogger(ADNLogger const& l) = delete;
  void operator=(ADNLogger const& l) = delete;

  static ADNLogger& GetLogger();
  
  void SetLogPath(std::string path);

  // logging functions
  void ClearLog();
  void Log(std::string value);
  void Log(QString value);
  void LogVector(ublas::vector<double> v);
  void LogVector(std::string name, ublas::vector<double> v);
  void LogMatrix(ublas::matrix<double> m);
  time_t LogPassedTime(time_t time1, std::string text);
  void LogPtr(quintptr ptr);
  //void logPosition(std::string name, SBPosition3 pos);
  //void logVector(std::string name, SBPicometerPerSecond3 pos);
  //void logVector(std::string name, SBRadianPerSecond3 pos);
  //void logVector(std::string name, SBVector3 pos);

private:
  ADNLogger() = default;

  std::string logPath_ = "adenita_log.txt";
};

