#include "PIBindingRegion.hpp"

PIBindingRegion & PIBindingRegion::operator=(const PIBindingRegion & other)
{
  SBNodeGroup::operator =(other);
  return *this;
}

void PIBindingRegion::SetLastNt(ADNPointer<ADNNucleotide> nt)
{
  lastNt_ = nt;
}

void PIBindingRegion::SetThermParam(ThermParam res)
{
  thermParam_ = res;
}

void PIBindingRegion::RegisterBindingRegion()
{
  SAMSON::getActiveLayer()->addChild(this);
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
    leftSeq.insert(0, std::to_string(left));
    auto pair = ntLeft->GetPair();
    if (pair != nullptr) {
      char right = ADNModel::GetResidueName(pair->GetType());
      rightSeq.append(std::to_string(right));
    }
    ntLeft = ntLeft->GetPrev();
  }

  return std::make_pair(leftSeq, rightSeq);
}