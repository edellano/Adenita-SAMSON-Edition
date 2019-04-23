#include "MSVColors.hpp"


MSVColors::MSVColors()
{
  SetStandardAtomColorScheme();
  SetStandardNucleotideColorScheme();
  SetStandardSingleStrandColorScheme();
  SetStandardDoubleStrandColorScheme();

}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNAtom> a)
{
  auto res = GetColor(a(), atomColors_);
  auto color = res.second;
  if (res.first == false) {
    SEConfig& config = SEConfig::GetInstance();

    color = ADNArray<float>(4);
    if (a->getElementSymbol() == "O") {
      color(0) = atomColorScheme_(0, 0);
      color(1) = atomColorScheme_(0, 1);
      color(2) = atomColorScheme_(0, 2);
      color(3) = atomColorScheme_(0, 3);
    }
    else if (a->getElementSymbol() == "C") {
      color(0) = atomColorScheme_(1, 0);
      color(1) = atomColorScheme_(1, 1);
      color(2) = atomColorScheme_(1, 2);
      color(3) = atomColorScheme_(1, 3);
    }
    else if (a->getElementSymbol() == "N") {
      color(0) = atomColorScheme_(2, 0);
      color(1) = atomColorScheme_(2, 1);
      color(2) = atomColorScheme_(2, 2);
      color(3) = atomColorScheme_(2, 3);
    }
    else if (a->getElementSymbol() == "P") {
      color(0) = atomColorScheme_(3, 0);
      color(1) = atomColorScheme_(3, 1);
      color(2) = atomColorScheme_(3, 2);
      color(3) = atomColorScheme_(3, 3);
    }
    else if (a->getElementSymbol() == "Fm") {
      color(0) = atomColorScheme_(4, 0);
      color(1) = atomColorScheme_(4, 1);
      color(2) = atomColorScheme_(4, 2);
      color(3) = atomColorScheme_(4, 3);
    }
    else if (a->getElementSymbol() == "Es") {
      color(0) = atomColorScheme_(5, 0);
      color(1) = atomColorScheme_(5, 1);
      color(2) = atomColorScheme_(5, 2);
      color(3) = atomColorScheme_(5, 3);
    }
    else {
      color(0) = 0.3;
      color(1) = 0.3;
      color(2) = 0.3;
      color(3) = 1.0;
    }

    SBNodeMaterial* material = a()->getMaterial();
    if (material) color = GetMaterialColor(a());
  }

  return color;
}


ADNArray<float> MSVColors::GetColor(ADNPointer<ADNNucleotide> nt)
{
  auto res = GetColor(nt(), ntsColors_);
  auto color = res.second;
  if (res.first == false) {
    SEConfig& config = SEConfig::GetInstance();

    color = ADNArray<float>(4);
    if (nt->GetType() == DNABlocks::DA) {
      color(0) = nucleotideColorScheme_(0, 0);
      color(1) = nucleotideColorScheme_(0, 1);
      color(2) = nucleotideColorScheme_(0, 2);
      color(3) = nucleotideColorScheme_(0, 3);
    }
    else if (nt->GetType() == DNABlocks::DT) {
      color(0) = nucleotideColorScheme_(1, 0);
      color(1) = nucleotideColorScheme_(1, 1);
      color(2) = nucleotideColorScheme_(1, 2);
      color(3) = nucleotideColorScheme_(1, 3);
    }
    else if (nt->GetType() == DNABlocks::DC) {
      color(0) = nucleotideColorScheme_(2, 0);
      color(1) = nucleotideColorScheme_(2, 1);
      color(2) = nucleotideColorScheme_(2, 2);
      color(3) = nucleotideColorScheme_(2, 3);
    }
    else if (nt->GetType() == DNABlocks::DG) {
      color(0) = nucleotideColorScheme_(3, 0);
      color(1) = nucleotideColorScheme_(3, 1);
      color(2) = nucleotideColorScheme_(3, 2);
      color(3) = nucleotideColorScheme_(3, 3);
    }
    else {
      color(0) = config.nucleotide_E_Color[0];
      color(1) = config.nucleotide_E_Color[1];
      color(2) = config.nucleotide_E_Color[2];
      color(3) = config.nucleotide_E_Color[3];
    }

    SBNodeMaterial* material = nt()->getMaterial();
    if (material) color = GetMaterialColor(nt());
  }

  return color;
}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNSingleStrand> ss)
{
  auto res = GetColor(ss(), sssColors_);
  auto color = res.second;
  if (res.first == false) {

    if (ss->IsScaffold())
    {
      auto scaffoldColorNum = singleStrandColorScheme_.GetNumElements() - 1;
      color(0) = singleStrandColorScheme_(scaffoldColorNum, 0);
      color(1) = singleStrandColorScheme_(scaffoldColorNum, 1);
      color(2) = singleStrandColorScheme_(scaffoldColorNum, 2);
      color(3) = singleStrandColorScheme_(scaffoldColorNum, 3);
    }
    else {
      int stapleColorNum = ss->getNodeIndex() % (singleStrandColorScheme_.GetNumElements() - 1);
      color(0) = singleStrandColorScheme_(stapleColorNum, 0);
      color(1) = singleStrandColorScheme_(stapleColorNum, 1);
      color(2) = singleStrandColorScheme_(stapleColorNum, 2);
      color(3) = singleStrandColorScheme_(stapleColorNum, 3);
    }
    SBNodeMaterial* material = ss()->getMaterial();
    if (material) color = GetMaterialColor(ss());
  }

  

  return color;
}

ADNArray<float> MSVColors::GetColor(ADNPointer<ADNBaseSegment> bs)
{
  auto res = GetColor(bs(), bssColors_);
  auto color = res.second;
  if (res.first == false) {
    int doubleStrandColorNum = bs->GetDoubleStrand()->getNodeIndex() % doubleStrandColorScheme_.GetNumElements();

    color(0) = doubleStrandColorScheme_(doubleStrandColorNum, 0);
    color(1) = doubleStrandColorScheme_(doubleStrandColorNum, 1);
    color(2) = doubleStrandColorScheme_(doubleStrandColorNum, 2);
    color(3) = doubleStrandColorScheme_(doubleStrandColorNum, 3);

    SBNodeMaterial* material = bs()->getMaterial();
    if (material) color = GetMaterialColor(bs());
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

void MSVColors::SetColor(ADNArray<float> color, ADNPointer<ADNAtom> a)
{
  SetColor(color, a(), atomColors_);
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



void MSVColors::SetStandardDoubleStrandColorScheme()
{
  SEConfig& config = SEConfig::GetInstance();

  doubleStrandColorScheme_ = ADNArray<float>(4, 1);
  doubleStrandColorScheme_(0, 0) = config.double_helix_V_color[0];
  doubleStrandColorScheme_(0, 1) = config.double_helix_V_color[1];
  doubleStrandColorScheme_(0, 2) = config.double_helix_V_color[2];
  doubleStrandColorScheme_(0, 3) = config.double_helix_V_color[3];

}

void MSVColors::SetAtomColorScheme(ADNArray<float> colorScheme)
{
  atomColorScheme_ = colorScheme;
}

void MSVColors::SetStandardSingleStrandColorScheme()
{
  SEConfig& config = SEConfig::GetInstance();

  singleStrandColorScheme_ = ADNArray<float>(4, config.num_staple_colors + 1);

  for (unsigned int num = 0; num < config.num_staple_colors; num++) {
    singleStrandColorScheme_(num, 0) = config.staple_colors[num * 4 + 0];
    singleStrandColorScheme_(num, 1) = config.staple_colors[num * 4 + 1];
    singleStrandColorScheme_(num, 2) = config.staple_colors[num * 4 + 2];
    singleStrandColorScheme_(num, 3) = config.staple_colors[num * 4 + 3];
  }

  //scaffold strand color
  int scaffoldColorNum = config.num_staple_colors;
  singleStrandColorScheme_(scaffoldColorNum, 0) = config.nucleotide_E_Color[0];
  singleStrandColorScheme_(scaffoldColorNum, 1) = config.nucleotide_E_Color[1];
  singleStrandColorScheme_(scaffoldColorNum, 2) = config.nucleotide_E_Color[2];
  singleStrandColorScheme_(scaffoldColorNum, 3) = config.nucleotide_E_Color[3];
}

void MSVColors::SetStandardAtomColorScheme()
{
  SEConfig& config = SEConfig::GetInstance();

  int numAtomTypes = 6;

  atomColorScheme_ = ADNArray<float>(4, numAtomTypes);

  //O
  atomColorScheme_(0, 0) = 0.6f;
  atomColorScheme_(0, 1) = 0.1f;
  atomColorScheme_(0, 2) = 0.0f;
  atomColorScheme_(0, 3) = 1.0f;

  //C
  atomColorScheme_(1, 0) = 0.35f;
  atomColorScheme_(1, 1) = 0.35f;
  atomColorScheme_(1, 2) = 0.35f;
  atomColorScheme_(1, 3) = 1.0f;

  //N
  atomColorScheme_(2, 0) = 0.1f;
  atomColorScheme_(2, 1) = 0.1f;
  atomColorScheme_(2, 2) = 0.6f;
  atomColorScheme_(2, 3) = 1.0f;

  //P
  atomColorScheme_(3, 0) = 0.7f;
  atomColorScheme_(3, 1) = 0.3f;
  atomColorScheme_(3, 2) = 0.0f;
  atomColorScheme_(3, 3) = 1.0f;

  //Fm
  atomColorScheme_(4, 0) = 0.7f;
  atomColorScheme_(4, 1) = 0.5f;
  atomColorScheme_(4, 2) = 0.3f;
  atomColorScheme_(4, 3) = 1.0f;

  //Es
  atomColorScheme_(5, 0) = 0.3f;
  atomColorScheme_(5, 1) = 0.5f;
  atomColorScheme_(5, 2) = 0.7f;
  atomColorScheme_(5, 3) = 1.0f;

  
}

void MSVColors::SetStandardNucleotideColorScheme()
{
  SEConfig& config = SEConfig::GetInstance();

  int numNucleotideTypes = 4;

  nucleotideColorScheme_ = ADNArray<float>(4, numNucleotideTypes);

  nucleotideColorScheme_(0, 0) = config.adenine_color[0];
  nucleotideColorScheme_(0, 1) = config.adenine_color[1];
  nucleotideColorScheme_(0, 2) = config.adenine_color[2];
  nucleotideColorScheme_(0, 3) = config.adenine_color[3];

  nucleotideColorScheme_(1, 0) = config.thymine_color[0];
  nucleotideColorScheme_(1, 1) = config.thymine_color[1];
  nucleotideColorScheme_(1, 2) = config.thymine_color[2];
  nucleotideColorScheme_(1, 3) = config.thymine_color[3];

  nucleotideColorScheme_(2, 0) = config.cytosine_color[0];
  nucleotideColorScheme_(2, 1) = config.cytosine_color[1];
  nucleotideColorScheme_(2, 2) = config.cytosine_color[2];
  nucleotideColorScheme_(2, 3) = config.cytosine_color[3];

  nucleotideColorScheme_(3, 0) = config.guanine_color[0];
  nucleotideColorScheme_(3, 1) = config.guanine_color[1];
  nucleotideColorScheme_(3, 2) = config.guanine_color[2];
  nucleotideColorScheme_(3, 3) = config.guanine_color[3];


}

void MSVColors::SetSingleStrandColorScheme(ADNArray<float> colorScheme)
{
  singleStrandColorScheme_ = colorScheme;
}

void MSVColors::SetDoubleStrandColorScheme(ADNArray<float> colorScheme)
{
  doubleStrandColorScheme_ = colorScheme;

}

void MSVColors::SetNucleotideColorScheme(ADNArray<float> colorScheme)
{
  nucleotideColorScheme_ = colorScheme;
}

ADNArray<float> MSVColors::GetMaterialColor(SBNode* node)
{
  ADNArray<float> color = ADNArray<float>(4);

  SBNodeMaterial* material = node->getMaterial();
  if (material) {
    float *matColor = new float[4];
    material->getColorScheme()->getColor(matColor, node);
    color(0) = matColor[0];
    color(1) = matColor[1];
    color(2) = matColor[2];
    color(3) = matColor[3];
  }

  return color;
}

