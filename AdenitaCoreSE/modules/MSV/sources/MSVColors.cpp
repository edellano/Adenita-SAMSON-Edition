#include "MSVColors.hpp"


ADNArray<float> MSVColors::GetColor(ADNPointer<ADNNucleotide> nt)
{
  auto res = GetColor(nt(), ntsColors_);
  auto color = res.second;
  if (res.first == false) {
    SEConfig& config = SEConfig::GetInstance();

    color = ADNArray<float>(4);
    if (nt->GetType() == DNABlocks::DA) {
      color(0) = config.adenine_color[0];
      color(1) = config.adenine_color[1];
      color(2) = config.adenine_color[2];
      color(3) = config.adenine_color[3];
    }
    else if (nt->GetType() == DNABlocks::DT) {
      color(0) = config.thymine_color[0];
      color(1) = config.thymine_color[1];
      color(2) = config.thymine_color[2];
      color(3) = config.thymine_color[3];
    }
    else if (nt->GetType() == DNABlocks::DC) {
      color(0) = config.cytosine_color[0];
      color(1) = config.cytosine_color[1];
      color(2) = config.cytosine_color[2];
      color(3) = config.cytosine_color[3];
    }
    else if (nt->GetType() == DNABlocks::DG) {
      color(0) = config.guanine_color[0];
      color(1) = config.guanine_color[1];
      color(2) = config.guanine_color[2];
      color(3) = config.guanine_color[3];
    }
    else {
      color(0) = config.nucleotide_E_Color[0];
      color(1) = config.nucleotide_E_Color[1];
      color(2) = config.nucleotide_E_Color[2];
      color(3) = config.nucleotide_E_Color[3];
    }    
  }

  return color;
}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNSingleStrand> ss)
{
  auto res = GetColor(ss(), sssColors_);
  auto color = res.second;
  if (res.first == false) {
    SEConfig& config = SEConfig::GetInstance();

    int stapleColorNum = ss->getNodeIndex() % config.num_staple_colors;
    color(0) = config.staple_colors[stapleColorNum * 4 + 0];
    color(1) = config.staple_colors[stapleColorNum * 4 + 1];
    color(2) = config.staple_colors[stapleColorNum * 4 + 2];
    color(3) = config.staple_colors[stapleColorNum * 4 + 3];
  }

  return color;
}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNBaseSegment> bs)
{
  auto res = GetColor(bs(), bssColors_);
  auto color = res.second;
  if (res.first == false) {
    // fetch default or random color
  }

  return color;
}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNDoubleStrand> ds)
{
  auto res = GetColor(ds(), dssColors_);
  auto color = res.second;
  if (res.first == false) {
    // fetch default or random color
  }

  return color;
}

void MSVColors::SetColor(ADNArray<float> color, ADNPointer<ADNNucleotide> nt)
{
  SetColor(color, nt(), ntsColors_);
}

void MSVColors::SetColor(ADNArray<float> color, ADNPointer<ADNSingleStrand> ss)
{
  SetColor(color, ss(), sssColors_);
}

void MSVColors::SetColor(ADNArray<float> color, ADNPointer<ADNBaseSegment> bs)
{
  SetColor(color, bs(), bssColors_);
}

void MSVColors::SetColor(ADNArray<float> color, ADNPointer<ADNDoubleStrand> ds)
{
  SetColor(color, ds(), dssColors_);
}

