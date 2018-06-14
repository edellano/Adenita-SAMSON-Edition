#include "MSVBindingRegion.hpp"	
#include <Windows.h>
#include <QProcess>

MSVBindingRegion::MSVBindingRegion()
{

}

MSVBindingRegion::MSVBindingRegion(unsigned int index)
{
	index_ = index;
}

void MSVBindingRegion::initializeSequences()
{
  
  for (auto cur : nucleotides_)
  {
    char leftSymbol = cur->GetName();
    leftSequence_ += leftSymbol;

    ANTNucleotide * pair = cur->pair_;

    if (pair != nullptr) {
      char rightSymbol = pair->GetName();
      rightSequence_ += rightSymbol;
    }
  }
	
  //ANTAuxiliary::log(QString(leftSequence_.c_str()));
  //ANTAuxiliary::log(QString(rightSequence_.c_str()));

	reverse(rightSequence_.begin(), rightSequence_.end());

}


void MSVBindingRegion::executeNtthal(int oligo_conc, int mv, int dv)
{
  QString workingDirection = QString::fromStdString(SB_ELEMENT_PATH + "/Resource/externals/primer3-win-bin-2.3.6/release-2.3.6/");
	QString program = QString(workingDirection + "ntthal.exe");
	QStringList arguments;
  //ANTAuxiliary::log(program);

	arguments << "-s1" << leftSequence_.c_str();
	arguments << "-s2" << rightSequence_.c_str();
	arguments << "-mv" << to_string(mv).c_str();
	arguments << "-dv" << to_string(dv).c_str();
	arguments << "-dna_conc" << to_string(oligo_conc).c_str();
	
	QProcess *myProcess = new QProcess();
	myProcess->setWorkingDirectory(workingDirection);
	myProcess->start(program, arguments);

	myProcess->waitForFinished();

	QByteArray standardOutput = myProcess->readAllStandardOutput();

	QStringList strLines = QString(standardOutput).split("\n");
	QString firstLine = strLines[0];

  //for (auto line : strLines) {
  //  ANTAuxiliary::log(line);
  //}
  //ANTAuxiliary::log(string("strLines:"), false);
  //ANTAuxiliary::log(QString::number(strLines.size()));

  if (strLines.size() != 6) { //if there the region is unbound
    dS_ = FLT_MAX;
    dH_ = FLT_MAX;
    dG_ = FLT_MAX;
    t_ = FLT_MAX;

    return;
  }

	QString dS = "dS =";
	QString dH = "dH =";
	QString dG = "dG =";
	QString t = "t =";

	int idS = firstLine.indexOf(dS);
	int idSEnd = firstLine.indexOf(dH, idS);
	int idH = firstLine.indexOf(dH);
	int idHEnd = firstLine.indexOf(dG, idH);
	int idG = firstLine.indexOf(dG);
	int idGEnd = firstLine.indexOf(t, idG);
	int it = firstLine.indexOf(t);

	QString dSVal = firstLine.mid(idS + 5, idSEnd - idS - 6);
	QString dHVal = firstLine.mid(idH + 5, idHEnd - idH - 5);
	QString dGVal = firstLine.mid(idG + 5, idGEnd - idG - 6);
	QString tVal = firstLine.mid(it + 4, firstLine.size() - it);

	dS_ = dSVal.toFloat();
	dH_ = dHVal.toFloat();
	dG_ = dGVal.toFloat();
	t_ = tVal.toFloat();

  //ANTAuxiliary::log(string("dG:"), false);
  //ANTAuxiliary::log(dGVal);
  //ANTAuxiliary::log(string("t:"), false);
  //ANTAuxiliary::log(tVal);

}