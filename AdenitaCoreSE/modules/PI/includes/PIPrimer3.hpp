#pragma once

#include <QProcess>
#include "ADNConfig.hpp"
#include "ADNNanorobot.hpp"
#include "ADNArray.hpp"
#include "PIBindingRegion.hpp"


class PIPrimer3 {
public:
  static PIPrimer3& GetInstance();
  ~PIPrimer3() = default;

  void UpdateBindingRegions(ADNPointer<ADNPart> p);
  CollectionMap<PIBindingRegion> GetBindingRegions();
  CollectionMap<PIBindingRegion> GetBindingRegions(ADNPointer<ADNPart> p);
  void Calculate(ADNPointer<ADNPart> p, int oligo_conc, int mv, int dv);
  void DeleteBindingRegions(ADNPointer<ADNPart> p);

private:
  PIPrimer3() = default;

  double GetMinGibbsFreeEnergy();
  double GetMaxGibbsFreeEnergy();
  double GetMinMeltingTemperature();
  double GetMaxMeltingTemperature();

  ThermParam ExecuteNtthal(std::string leftSequence, std::string rightSequence, int oligo_conc, int mv, int dv);

  std::map<ADNPart*, CollectionMap<PIBindingRegion>> regionsMap_;
};