#pragma once

#include "ADNNanorobot.hpp"
#include "ADNConstants.hpp"


enum RoutingType {
  None = 0,
  Seamless = 1,
  Tiles = 2,
  NonSeamless = 3
};

class DASRouter {
public:
  DASRouter() = default;
  ~DASRouter() = default;

  static DASRouter* GetRouter(RoutingType t);
  virtual void Route(ADNPointer<ADNPart> part) = 0;
};

class DASRouterNone: public DASRouter {
  void Route(ADNPointer<ADNPart> part) {};
};

class DASRouterNanotubeTiles: public DASRouter {
  void Route(ADNPointer<ADNPart> part);
};

class DASRouterNanotubeSeamless : public DASRouter {
  void Route(ADNPointer<ADNPart> part);
};

class DASRouterNanotubeNonSeamless : public DASRouter {
  void Route(ADNPointer<ADNPart> part);
};