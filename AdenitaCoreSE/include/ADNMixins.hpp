#pragma once

#include <string>
#include <memory>
#include <map>
#include <boost/numeric/ublas/vector.hpp>
#include "SAMSON.hpp"
#include "SBCReferencePointer.hpp"
#include "SBCReferencePointerIndexer.hpp"


namespace ublas = boost::numeric::ublas;

/* Using smart pointers */
//template <typename T>
//using ADNPointer = std::shared_ptr<T>;

template <typename T>
using ADNPointer = SBCReferencePointer<T>;

//template <typename T>
//using ADNWeakPointer = std::weak_ptr<T>;

template <typename T>
using ADNWeakPointer = SBCReferencePointer<T>;

//template<typename T>
//using CollectionMap = std::map<int, ADNPointer<T>>;

template<typename T>
using CollectionMap = SBPointerIndexer<T>;

//using Position3D = ublas::vector<double>;
using Position3D = SBPosition3;

class Nameable {
public:
  Nameable() = default;
  ~Nameable() = default;
  Nameable(const Nameable& other);

  Nameable& operator=(const Nameable& other);

  void SetName(std::string name);
  std::string GetName() const;
private:
  std::string name_;
};

class Positionable {
public:
  Positionable() = default;
  ~Positionable() = default;
  Positionable(const Positionable& other);

  Positionable& operator=(const Positionable& other);

  void SetPosition(ublas::vector<double> pos);
  ublas::vector<double> GetPosition() const;
private:
  ublas::vector<double> position_;
};

class Identifiable {
public:
  Identifiable() = default;
  ~Identifiable() = default;
  Identifiable(const Identifiable& other);

  Identifiable& operator=(const Identifiable& other);

  void SetId(int id);
  int GetId() const;
private:
  int id_ = -1;
};

class Orientable {
public:
  Orientable();
  ~Orientable() = default;
  Orientable(const Orientable& other);

  Orientable& operator=(const Orientable& other);

  void SetE1(ublas::vector<double> e1);
  void SetE2(ublas::vector<double> e2);
  void SetE3(ublas::vector<double> e3);
  ublas::vector<double> GetE1() const;
  ublas::vector<double> GetE2() const;
  ublas::vector<double> GetE3() const;
private:
  ublas::vector<double> e1_;
  ublas::vector<double> e2_;
  ublas::vector<double> e3_;
};

template <class T>
class Collection {
public:
  Collection() = default;
  ~Collection() = default;
  Collection(const Collection<T>& other);

  Collection<T>& operator=(const Collection<T>& other);

  void AddElement(ADNPointer<T> elem, int id = -1);
  CollectionMap<T> GetCollection() const;
  void DeleteElement(int id);
  int GetLastKey();
  ADNPointer<T> GetElement(int id) const;
private:
  CollectionMap<T> collection_;
};

template<class T>
inline Collection<T>::Collection(const Collection<T>& other)
{
  *this = other;
}

template<class T>
inline Collection<T> & Collection<T>::operator=(const Collection<T>& other)
{
  if (&other == this) {
    return *this;
  }

  collection_ = other.GetCollection();

  return *this;
}

template<class T>
inline void Collection<T>::AddElement(ADNPointer<T> elem, int id = -1)
{
  if (id == -1) {
    id = 0;
    if (collection_.size() > 0) id = collection_.rbegin()->first + 1;
  }
  collection_.insert(std::make_pair(id,elem));
}

template<class T>
inline CollectionMap<T> Collection<T>::GetCollection() const
{
  return collection_;
}

template<class T>
inline void Collection<T>::DeleteElement(int id)
{
  collection_.erase(id);
}

template<class T>
inline int Collection<T>::GetLastKey()
{
  int id = -1;
  if (collection_.size() > 0) {
    id = collection_.rbegin()->first;
  }
  return id;
}

template<class T>
inline ADNPointer<T> Collection<T>::GetElement(int id) const
{
  return collection_.at(id);
}
