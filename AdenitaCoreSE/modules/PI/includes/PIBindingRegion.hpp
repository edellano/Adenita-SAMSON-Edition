#pragma once

#include <QProcess>
#include "SEConfig.hpp"
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
  PIBindingRegion(const PIBindingRegion& other) : SBNodeGroup(other) {};
  ~PIBindingRegion() = default;

  PIBindingRegion& operator=(const PIBindingRegion& other);

  void SetLastNt(ADNPointer<ADNNucleotide> nt);
  void SetThermParam(ThermParam res);

  void RegisterBindingRegion();
  void UnregisterBindingRegion();
  std::pair<std::string, std::string> GetSequences();

private:
  ThermParam thermParam_;
  ADNPointer<ADNNucleotide> lastNt_;
};
SB_REGISTER_TARGET_TYPE(PIBindingRegion, "PIBindingRegion", "171D8E12-FCB6-4770-A026-1AADC94BE325");
SB_DECLARE_BASE_TYPE(PIBindingRegion, SBNodeGroup);