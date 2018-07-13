#include "ADNLogger.hpp"

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
  QString fileName = QString::fromStdString(logPath_);
  QFile::copy(fileName, fileName + ".bak");
  QFile file(fileName);
  file.resize(0);
  file.close();
}

void ADNLogger::Log(int value)
{
  Log(to_string(value));
}

void ADNLogger::Log(std::string value)
{
  QString qs = QString::fromStdString(value);
  Log(qs);
}

void ADNLogger::Log(QString value)
{
  QIODevice::OpenModeFlag mode = QIODevice::Append;
  QString fileName = QString::fromStdString(logPath_);
  QFile file(fileName);
  file.open(QIODevice::ReadWrite | mode);
  QTextStream out(&file);
  out << value;
  out << QString("\n");
  file.close();
}

void ADNLogger::LogVector(ublas::vector<double> v)
{
  std::string line = ADNAuxiliary::UblasVectorToString(v);
  Log(line);
}

void ADNLogger::LogVector(std::string name, ublas::vector<double> v)
{
  std::string line = name + ": " + ADNAuxiliary::UblasVectorToString(v);
  Log(line);
}

void ADNLogger::LogMatrix(ublas::matrix<double> m)
{
  std::string startLine = "BEGIN MATRIX";
  Log(startLine);
  for (auto rit = 0; rit != m.size1(); ++rit) {
    LogVector(ublas::row(m, rit));
  }
  std::string endLine = "END MATRIX";
  Log(endLine);
}

time_t ADNLogger::LogPassedTime(time_t time1, std::string text)
{
  double seconds_since_start = difftime(time(0), time1);
  Log(QString("\t *** ") + QString(text.c_str()) + QString(" ") + QString::number(seconds_since_start) + QString(" seconds"));
  return time(0);
}

void ADNLogger::LogPtr(quintptr ptr)
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