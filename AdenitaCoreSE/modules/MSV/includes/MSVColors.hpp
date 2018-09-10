#pragma once

#include "ADNNanorobot.hpp"
#include "ADNArray.hpp"


class MSVColors {
public:
  MSVColors() = default;
  ~MSVColors() = default;

  ADNArray<float> GetColor(ADNPointer<ADNNucleotide> nt);
  ADNArray<float> GetColor(ADNPointer<ADNSingleStrand> ss);
  ADNArray<float> GetColor(ADNPointer<ADNBaseSegment> bs);
  ADNArray<float> GetColor(ADNPointer<ADNDoubleStrand> ds);
  
  void SetColor(ADNArray<float> color, ADNPointer<ADNNucleotide> nt);
  void SetColor(ADNArray<float> color, ADNPointer<ADNSingleStrand> ss);
  void SetColor(ADNArray<float> color, ADNPointer<ADNBaseSegment> bs);
  void SetColor(ADNArray<float> color, ADNPointer<ADNDoubleStrand> ds);

private:

  template<typename T>
  std::pair<bool, ADNArray<float>> GetColor(T el, std::map<T, ADNArray<float>> searchMap) {
    ADNArray<float> color = ADNArray<float>(4);
    bool found = false;
    if (searchMap.find(el) != searchMap.end()) {
      color = searchMap.at(el);
      found = true;
    }

    return std::make_pair(found, color);
  }

  template<typename T>
  void SetColor(ADNArray<float> color, T el, std::map<T, ADNArray<float>> searchMap) {
    searchMap[el] = color;
  }

  ADNArray<float> GenerateRandomColor();

  ADNArray<float> defaultbssColor_;

  std::map<ADNNucleotide*, ADNArray<float>> ntsColors_;
  std::map<ADNSingleStrand*, ADNArray<float>> sssColors_;
  std::map<ADNBaseSegment*, ADNArray<float>> bssColors_;
  std::map<ADNDoubleStrand*, ADNArray<float>> dssColors_;
};