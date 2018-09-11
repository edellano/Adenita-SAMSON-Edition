#include "DASRouter.hpp"

DASRouter * DASRouter::GetRouter(RoutingType t)
{
  DASRouter* router;

  if (t == RoutingType::None) {
    router = new DASRouterNone();
  }

  return router;
}
