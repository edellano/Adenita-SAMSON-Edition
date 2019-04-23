#pragma once
#ifndef DASPOLYHEDRON_H
#define DASPOLYHEDRON_H

#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include "SBIFileReader.hpp"
#include "ADNVectorMath.hpp"
#include "ADNAuxiliary.hpp"


/* Half-edge implementation */
class DASVertex;
class DASEdge;
class DASPolygon;

using EdgeBps = std::map<DASEdge*, int>;

class DASHalfEdge {
public:
  DASHalfEdge() = default;
  /**
   * Half-edges don't need custom destructor because in Halfedge 
   * representation they don't make sense by themselves
   */
  ~DASHalfEdge() = default;
  DASHalfEdge(const DASHalfEdge& other);
  DASHalfEdge& operator=(const DASHalfEdge& other);

  DASHalfEdge* next_ = nullptr;
  DASHalfEdge* prev_ = nullptr;
  DASHalfEdge* pair_ = nullptr;

  DASVertex* source_ = nullptr;
  DASPolygon* left_ = nullptr;
  DASEdge* edge_ = nullptr;

  // we need an id_ for copying
  unsigned int id_;
};

class DASVertex {
public:
  DASVertex() = default;
  /**
   * Topologically vertex has to be removed from the polyhedron.
   * Hence default destructor.
   */
  ~DASVertex() = default;
  DASVertex(const DASVertex& w);
  DASVertex(DASVertex* w);

  DASVertex& operator=(const DASVertex& other);

  /* Members */
  DASHalfEdge* halfEdge_ = nullptr;
  int id_;
  SBPosition3 position_;

  /* Getters */
  std::vector<double> GetVectorCoordinates() const; // always returns picometers 10^-12
  SBPosition3 GetSBPosition() const;

  /* Setters */
  void SetCoordinates(SBPosition3 coordinates);
};

class DASEdge {
public:
  DASEdge() = default;
  ~DASEdge();
  DASEdge(const DASEdge& other);

  DASEdge& operator=(const DASEdge& other);

  DASHalfEdge* halfEdge_ = nullptr;
  int id_;
};

class DASPolygon {
public:
  DASPolygon() = default;
  DASPolygon(const DASPolygon& p);
  ~DASPolygon();

  DASPolygon& operator=(const DASPolygon& p);
  
  SBPosition3 GetCenter();

  int id_;
  DASHalfEdge* halfEdge_ = nullptr;
};

using Edges = std::vector<DASEdge*>;
using Faces = std::vector<DASPolygon*>;
using Vertices = std::map<int, DASVertex*>;

class DASPolyhedron {
public:
  /* Constructor */
  DASPolyhedron() = default;
  DASPolyhedron(const DASPolyhedron &p);
  DASPolyhedron(std::string filename);

  ~DASPolyhedron();

  DASPolyhedron& operator=(const DASPolyhedron& other);

  /* Getters */
  Vertices GetVertices() const;
  Vertices GetOriginalVertices() const;
  Faces GetFaces() const;
  Edges GetEdges() const;
  DASVertex* GetVertexById(int id);
  static DASEdge* GetEdgeByVertices(DASVertex* source, DASVertex* target);
  unsigned int * GetIndices();

  /* Setters */
  void SetVertices(Vertices vertices, bool isOriginal);
  void SetFaces(Faces faces);
  void SetEdges(Edges edges);

  /* Methods */
  size_t GetNumVertices() const;
  size_t GetNumFaces() const;
  void BuildPolyhedron(std::map<int, SBPosition3> vertices, std::map<int, std::vector<int>> faces);
  void LoadFromPLYFile(std::string filename);
  void AddVertex(int id, SBPosition3 coords);
  void AddFace(int id, std::vector<int> vertices);
  std::pair<DASEdge*, double> MinimumEdgeLength();
  std::pair<DASEdge*, double> MaximumEdgeLength();
  static double CalculateEdgeLength(DASEdge* edge);
  static int GetVertexDegree(DASVertex* v);
  static bool IsInFace(DASVertex* v, DASPolygon* f);
  DASHalfEdge * GetHalfEdge(unsigned int id);
  // static ANTPolygon* GetSharedFace(ANTVertex* v, ANTVertex* w);
  static DASHalfEdge* GetHalfEdge(DASVertex* v, DASVertex* w);
  void Scale(double scalingFactor);
  void Center(SBPosition3 center);
  SBPosition3 GetCenter();

protected:
  int FetchNumber(std::string st, std::string tok);
  template <typename T> std::vector<T> SplitString(std::string s, std::string type, T dummy);
private:
  // Map ids with Vertex objects
  Vertices vertices_;
  // Map ids with Face objects
  Faces faces_;
  // List with the edges
  Edges edges_;
  // Store original value for scaling
  Vertices originalVertices_;
  //store index list of faces
  unsigned int * indices_;
};

#endif
