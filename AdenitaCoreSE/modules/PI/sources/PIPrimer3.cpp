#include "PIPrimer3.hpp"
#include <cfloat>

CollectionMap<PIBindingRegion> PIPrimer3::GetBindingRegions()
{
  CollectionMap<PIBindingRegion> regions;

  for (auto it = regionsMap_.begin(); it != regionsMap_.end(); ++it) {
    CollectionMap<PIBindingRegion> regs = it->second;
    SB_FOR(ADNPointer<PIBindingRegion> r, regs) {
      regions.addReferenceTarget(r());
    }
  }

  return regions;
}

PIPrimer3 & PIPrimer3::GetInstance()
{
  static PIPrimer3 instance;
  return instance;
}

CollectionMap<PIBindingRegion> PIPrimer3::GetBindingRegions(ADNPointer<ADNPart> p)
{
  CollectionMap<PIBindingRegion> regions;
  if (regionsMap_.find(p()) != regionsMap_.end()) {
    regions = regionsMap_[p()];
  }

  return regions;
}

void PIPrimer3::DeleteBindingRegions(ADNPointer<ADNPart> p)
{
  auto regions = GetBindingRegions(p);
  SB_FOR(ADNPointer<PIBindingRegion> r, regions) {
    r->UnregisterBindingRegion();
  }
}

ThermParam PIPrimer3::ExecuteNtthal(std::string leftSequence, std::string rightSequence, int oligo_conc, int mv, int dv)
{
  SEConfig& c = SEConfig::GetInstance();
  QFileInfo ntthal(QString::fromStdString(c.ntthal));
  QString workingDirection = ntthal.absolutePath();
  QString program = ntthal.absoluteFilePath();
  std::string test1 = program.toStdString();
  std::string test2 = workingDirection.toStdString();
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
  if (strLines.size() != 6) {  //if there the region is unbound
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
  auto regions = GetBindingRegions(p);

  SB_FOR(ADNPointer<PIBindingRegion> r, regions) {
    auto seqs = r->GetSequences();
    ThermParam res = ExecuteNtthal(seqs.first, seqs.second, oligo_conc, mv, dv);
    r->SetThermParam(res);
  }
}

void PIPrimer3::UpdateBindingRegions(ADNPointer<ADNPart> p)
{
  if (regionsMap_.find(p()) != regionsMap_.end()) {
    regionsMap_[p()].clear();
  }
  else {
    regionsMap_.insert(std::make_pair(p(), CollectionMap<PIBindingRegion>()));
  }

  auto singleStrands = p->GetSingleStrands();

  std::vector<ADNPointer<ADNNucleotide>> added_nt;
  ADNPointer<ADNNucleotide> firstNt;
  unsigned int numRegions = 0;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

    int regionSize = 0;

    SBNodeIndexer nodeIndexer;
    while (nt != nullptr) {

      if (std::find(added_nt.begin(), added_nt.end(), nt) == added_nt.end()) {
        bool endOfRegion = true;

        auto st_cur = nt->GetPair();
        auto sc_next = nt->GetNext();

        if (sc_next != nullptr && st_cur != nullptr && st_cur->GetPrev() != nullptr) {
          if (sc_next->GetPair() == st_cur->GetPrev()) {
            endOfRegion = false;
          }
        }
        else if (st_cur == nullptr) {
          // group up in one binding region the contiguous unpaired nts
          if (sc_next->GetPair() == nullptr) {
            endOfRegion = false;
          }
        }

        nodeIndexer.addNode(nt());
        added_nt.push_back(nt);
        if (regionSize == 0) firstNt = nt;

        auto pair = nt->GetPair();
        if (pair != nullptr) {
          nodeIndexer.addNode(pair());
          added_nt.push_back(pair);
        }

        ++regionSize;

        if (endOfRegion) {
          regionSize = 0;
          std::string name = "Binding Region " + std::to_string(numRegions);
          ADNPointer<PIBindingRegion> region = new PIBindingRegion(name, nodeIndexer);
          region->SetPart(p);
          region->RegisterBindingRegion();
          regionsMap_[p()].addReferenceTarget(region());
          region->SetLastNt(nt);
          region->SetFirstNt(firstNt);
          ++numRegions;
          nodeIndexer.clear();
        }
      }

      nt = nt->GetNext();
    }
  }
}
