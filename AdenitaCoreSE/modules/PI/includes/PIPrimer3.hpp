#pragma once

#include <QProcess>
#include "SEConfig.hpp"
#include "ADNNanorobot.hpp"
#include "ADNArray.hpp"
#include "PIBindingRegion.hpp"


class PIPrimer3 {
public:
  PIPrimer3() = default;
  ~PIPrimer3() = default;

  void CreateBindingRegions(ADNPointer<ADNPart> p);
  CollectionMap<PIBindingRegion> GetBindingRegions();
  void Calculate(ADNPointer<ADNPart> p, int oligo_conc, int mv, int dv);

private:
  double GetMinGibbsFreeEnergy();
  double GetMaxGibbsFreeEnergy();
  double GetMinMeltingTemperature();
  double GetMaxMeltingTemperature();

  ThermParam ExecuteNtthal(std::string leftSequence, std::string rightSequence, int oligo_conc, int mv, int dv);

  CollectionMap<PIBindingRegion> regions_;
};