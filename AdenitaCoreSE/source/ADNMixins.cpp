#include "ADNMixins.hpp"

Nameable::Nameable(const Nameable & other)
{
  *this = other;
}

Nameable & Nameable::operator=(const Nameable & other)
{
  if (&other == this) {
    return *this;
  }

  SetName(other.GetName());

  return *this;
}

void Nameable::SetName(std::string name)
{
  name_ = name;
}

std::string Nameable::GetName() const 
{
  return name_;
}

Positionable::Positionable(const Positionable & other)
{
  *this = other;
}

Positionable & Positionable::operator=(const Positionable & other)
{
  if (&other == this) {
    return *this;
  }

  SetPosition(other.GetPosition());

  return *this;
}

void Positionable::SetPosition(ublas::vector<double> pos)
{
  position_ = pos;
}

ublas::vector<double> Positionable::GetPosition() const
{
  return position_;
}

Identifiable::Identifiable(const Identifiable & other)
{
  *this = other;
}

Identifiable & Identifiable::operator=(const Identifiable & other)
{
  if (&other == this) {
    return *this;
  }

  SetId(other.GetId());

  return *this;
}

void Identifiable::SetId(int id)
{
  id_ = id;
}

int Identifiable::GetId() const
{
  return id_;
}

Orientable::Orientable()
{
  e1_ = ublas::vector<double>(3, 0.0);
  e2_ = ublas::vector<double>(3, 0.0);
  e3_ = ublas::vector<double>(3, 0.0);
}

Orientable::Orientable(const Orientable & other)
{
  *this = other;
}

Orientable & Orientable::operator=(const Orientable & other)
{
  if (&other == this) {
    return *this;
  }

  SetE1(other.GetE1());
  SetE2(other.GetE2());
  SetE3(other.GetE3());

  return *this;
}

void Orientable::SetE1(ublas::vector<double> e1)
{
  e1_ = e1;
}

void Orientable::SetE2(ublas::vector<double> e2)
{
  e2_ = e2;
}

void Orientable::SetE3(ublas::vector<double> e3)
{
  e3_ = e3;
}

ublas::vector<double> Orientable::GetE1() const
{
  return e1_;
}

ublas::vector<double> Orientable::GetE2() const
{
  return e2_;
}

ublas::vector<double> Orientable::GetE3() const
{
  return e3_;
}
