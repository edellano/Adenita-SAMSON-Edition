#include "ADNLogger.hpp"

ADNLogger::ADNLogger()
{
  QIODevice::OpenModeFlag mode = QIODevice::Append;
  QString fileName = QString::fromStdString(logPath_);
  file_.setFileName(fileName);
  file_.open(QIODevice::ReadWrite | mode);
}

ADNLogger::~ADNLogger()
{
  file_.close();
}

ADNLogger & ADNLogger::GetLogger()
{
  static ADNLogger instance;
  return instance;
}

void ADNLogger::SetLogPath(std::string path)
{
  logPath_ = path;
}

void ADNLogger::ClearLog()
{
  file_.resize(0);
  file_.flush();
}

void ADNLogger::LogDebug(float value)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(value);
  }
}

void ADNLogger::LogDebug(int value)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(value);
  }
}

void ADNLogger::LogDebug(std::string value)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    SB_WARNING("Adenita debug: " + value);
    Log(value);
  }
}

void ADNLogger::LogDebug(QString value)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(value);
  }
}

void ADNLogger::LogDebug(ublas::vector<double> v)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(v);
  }
}

void ADNLogger::LogDebug(std::string name, ublas::vector<double> v)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(name, v);
  }
}

void ADNLogger::LogDebug(ublas::matrix<double> m)
{
  SEConfig& config = SEConfig::GetInstance();
  if (config.mode == DEBUG_LOG) {
    Log(m);
  }
}

void ADNLogger::Log(float value)
{
  Log(std::to_string(value));
}

void ADNLogger::Log(int value)
{
  Log(std::to_string(value));
}

void ADNLogger::Log(std::string value)
{
  QString qs = QString::fromStdString(value);
  SB_INFORMATION("Adenita info: " + value);
  Log(qs);
}

void ADNLogger::Log(QString value)
{
  QTextStream out(&file_);
  out << value;
  out << QString("\n");
  file_.flush();
}

void ADNLogger::Log(ublas::vector<double> v)
{
  std::string line = ADNAuxiliary::UblasVectorToString(v);
  Log(line);
}

void ADNLogger::Log(std::string name, ublas::vector<double> v)
{
  std::string line = name + ": " + ADNAuxiliary::UblasVectorToString(v);
  Log(line);
}

void ADNLogger::Log(ublas::matrix<double> m)
{
  std::string startLine = "BEGIN MATRIX";
  Log(startLine);
  for (auto rit = 0; rit != m.size1(); ++rit) {
    Log(ublas::row(m, rit));
  }
  std::string endLine = "END MATRIX";
  Log(endLine);
}

void ADNLogger::Log(SBPosition3 pos)
{
  auto x = pos[0].getValue();
  auto y = pos[1].getValue();
  auto z = pos[2].getValue();

  Log(QString("\t pos ") + QString::number(x) + QString(", ") + QString::number(y) + QString(", ") + QString::number(z));

}

void ADNLogger::Log(quintptr ptr)
{
  QString ptrStr = QString("0x%1").arg(ptr,
    QT_POINTER_SIZE * 2, 16, QChar('0'));
  Log(ptrStr);
}

//void ADNAuxiliary::logPosition(string name, SBPosition3 pos)
//{
//  if (abs(pos[0].getValue()) < 0.0001) pos[0].setValue(0);
//  if (abs(pos[1].getValue()) < 0.0001) pos[1].setValue(0);
//  if (abs(pos[2].getValue()) < 0.0001) pos[2].setValue(0);
//
//  ADNAuxiliary::log(QString(name.c_str()));
//  ADNAuxiliary::log(QString::number(pos[0].getValue()), false);
//  ADNAuxiliary::log(QString::number(pos[1].getValue()), false);
//  ADNAuxiliary::log(QString::number(pos[2].getValue()));
//}
//
//void ADNAuxiliary::logVector(string name, SBVector3 vec)
//{
//  if (abs(vec[0].getValue()) < 0.0001) vec[0].setValue(0);
//  if (abs(vec[1].getValue()) < 0.0001) vec[1].setValue(0);
//  if (abs(vec[2].getValue()) < 0.0001) vec[2].setValue(0);
//
//  ADNAuxiliary::log(QString(name.c_str()));
//  ADNAuxiliary::log(QString::number(vec[0].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[1].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[2].getValue()));
//}
//
//void ADNAuxiliary::logVector(string name, SBPicometerPerSecond3 vec)
//{
//  if (abs(vec[0].getValue()) < 0.0001) vec[0].setValue(0);
//  if (abs(vec[1].getValue()) < 0.0001) vec[1].setValue(0);
//  if (abs(vec[2].getValue()) < 0.0001) vec[2].setValue(0);
//
//  ADNAuxiliary::log(QString(name.c_str()));
//  ADNAuxiliary::log(QString::number(vec[0].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[1].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[2].getValue()));
//}
//
//void ADNAuxiliary::logVector(string name, SBRadianPerSecond3 vec)
//{
//  if (abs(vec[0].getValue()) < 0.0001) vec[0].setValue(0);
//  if (abs(vec[1].getValue()) < 0.0001) vec[1].setValue(0);
//  if (abs(vec[2].getValue()) < 0.0001) vec[2].setValue(0);
//
//  ADNAuxiliary::log(QString(name.c_str()));
//  ADNAuxiliary::log(QString::number(vec[0].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[1].getValue()), false);
//  ADNAuxiliary::log(QString::number(vec[2].getValue()));
//}