#include "PIPrimer3.hpp"

CollectionMap<PIBindingRegion> PIPrimer3::GetBindingRegions()
{
  return regions_;
}

ThermParam PIPrimer3::ExecuteNtthal(std::string leftSequence, std::string rightSequence, int oligo_conc, int mv, int dv)
{
  QString workingDirection = QString::fromStdString(SB_ELEMENT_PATH + "/Resource/externals/primer3-win-bin-2.3.6/release-2.3.6/");
  QString program = QString(workingDirection + "ntthal.exe");
  QStringList arguments;

  arguments << "-s1" << leftSequence.c_str();
  arguments << "-s2" << rightSequence.c_str();
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

  ThermParam res;
  if (strLines.size() != 6) { //if there the region is unbound
    res.dS_ = FLT_MAX;
    res.dH_ = FLT_MAX;
    res.dG_ = FLT_MAX;
    res.T_ = FLT_MAX;
    return res;
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

  res.dS_ = dSVal.toFloat();
  res.dH_ = dHVal.toFloat();
  res.dG_ = dGVal.toFloat();
  res.T_ = tVal.toFloat();

  return res;
}

void PIPrimer3::Calculate(ADNPointer<ADNPart> p, int oligo_conc, int mv, int dv)
{
  CreateBindingRegions(p);
  auto regions = GetBindingRegions();

  SB_FOR(ADNPointer<PIBindingRegion> r, regions) {
    auto seqs = r->GetSequences();
    ThermParam res = ExecuteNtthal(seqs.first, seqs.second, oligo_conc, mv, dv);
    r->SetThermParam(res);
  }
}

void PIPrimer3::CreateBindingRegions(ADNPointer<ADNPart> p)
{
  auto singleStrands = p->GetSingleStrands();

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

    int regionSize = 0;
    unsigned int numRegions = 0;

    ADNPointer<PIBindingRegion> region;
    while (nt != nullptr) {
      if (regionSize == 0) {
        region = new PIBindingRegion();
        //region->RegisterBindingRegion();
        //regions_.addReferenceTarget(region());
        ++numRegions;
      }

      bool endOfRegion = true;

      auto st_cur = nt->GetPair();
      auto sc_next = nt->GetNext();

      if (sc_next != nullptr && st_cur != nullptr && st_cur->GetPrev() != nullptr) {
        if (sc_next->GetPair() == st_cur->GetPrev()) {
          endOfRegion = false;
        }
      }

      //region->addChild(nt());
      //region->SetLastNt(nt);

      ++regionSize;

      if (endOfRegion) {
        regionSize = 0;
      }
      nt = nt->GetNext();
    }
  }
}
