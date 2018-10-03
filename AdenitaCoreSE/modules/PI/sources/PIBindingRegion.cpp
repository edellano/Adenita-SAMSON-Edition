#include "PIBindingRegion.hpp"

PIBindingRegion & PIBindingRegion::operator=(const PIBindingRegion & other)
{
  SBNodeGroup::operator =(other);
  return *this;
}

double PIBindingRegion::getEntropy() const
{
  return thermParam_.dS_;
}

double PIBindingRegion::getEntalpy() const
{
  return thermParam_.dH_;
}

double PIBindingRegion::getGibbs() const
{
  return thermParam_.dG_;
}

double PIBindingRegion::getTemp() const
{
  return thermParam_.T_;
}

void PIBindingRegion::SetLastNt(ADNPointer<ADNNucleotide> nt)
{
  lastNt_ = nt;
}

ADNPointer<ADNPart> PIBindingRegion::GetPart()
{
  return part_;
}

void PIBindingRegion::SetPart(ADNPointer<ADNPart> part)
{
  part_ = part;
}

void PIBindingRegion::SetThermParam(ThermParam res)
{
  thermParam_ = res;
}

void PIBindingRegion::RegisterBindingRegion()
{
  create();
  SAMSON::getActiveDocument()->addChild(this);
}

void PIBindingRegion::UnregisterBindingRegion()
{
  auto parent = this->getParent();
  parent->removeChild(this);
}

std::pair<std::string, std::string> PIBindingRegion::GetSequences()
{
  std::string leftSeq;
  std::string rightSeq;

  auto ntLeft = lastNt_;
  while (ntLeft != nullptr) {
    char left = ADNModel::GetResidueName(ntLeft->GetType());
    leftSeq.insert(0, std::string(1, left));
    auto pair = ntLeft->GetPair();
    if (pair != nullptr) {
      char right = ADNModel::GetResidueName(pair->GetType());
      rightSeq.append(std::string(1, right));
    }
    ntLeft = ntLeft->GetPrev();
  }

  return std::make_pair(leftSeq, rightSeq);
}