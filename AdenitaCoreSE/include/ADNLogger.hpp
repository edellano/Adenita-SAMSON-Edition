#pragma once

#include <ctime>
#include <string>
#include <boost/numeric/ublas/vector.hpp>
#include <QString>
#include <QTextStream>
#include <QFile>
#include "ADNAuxiliary.hpp"
#include "SEConfig.hpp"


namespace ublas = boost::numeric::ublas;

namespace ADNLogger
{
  // error logging
  void LogError(std::string value);
  void LogError(QString value);

  // debug logging
  void LogDebug(std::string value);
  void LogDebug(QString value);

  // normal logging
  void Log(std::string value);
  void Log(QString value);
};

