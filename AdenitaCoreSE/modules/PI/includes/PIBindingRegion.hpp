#pragma once

#include <QProcess>
#include "ADNConfig.hpp"
#include "ADNNanorobot.hpp"
#include "ADNArray.hpp"


struct ThermParam {
  double dS_;
  double dH_;
  double T_;
  double dG_;
};

class PIBindingRegion : public SBNodeGroup {
  SB_CLASS
public:
  PIBindingRegion() : SBNodeGroup() {};
  PIBindingRegion(std::string name, SBNodeIndexer indexer) : SBNodeGroup(name, indexer) {};
  PIBindingRegion(const PIBindingRegion& other) : SBNodeGroup(other) {};
  ~PIBindingRegion() = default;

  PIBindingRegion& operator=(const PIBindingRegion& other);

  double getEntropy() const;
  double getEntalpy() const;
  double getGibbs() const;
  double getTemp() const;

  void SetLastNt(ADNPointer<ADNNucleotide> nt);
  void SetFirstNt(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNPart> GetPart();
  void SetPart(ADNPointer<ADNPart> part);
  void SetThermParam(ThermParam res);

  void RegisterBindingRegion();
  void UnregisterBindingRegion();
  std::pair<std::string, std::string> GetSequences();

private:
  ThermParam thermParam_;
  ADNPointer<ADNNucleotide> firstNt_;
  ADNPointer<ADNNucleotide> lastNt_;
  ADNPointer<ADNPart> part_;
};
SB_REGISTER_TARGET_TYPE(PIBindingRegion, "PIBindingRegion", "171D8E12-FCB6-4770-A026-1AADC94BE325");
SB_DECLARE_BASE_TYPE(PIBindingRegion, SBNodeGroup);