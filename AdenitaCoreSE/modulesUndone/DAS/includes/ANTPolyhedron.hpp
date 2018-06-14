
/** \file
 *  \brief    Target 3D geometry is stored in this class.
 *  \details  Class for storing the 3D figure information.
 *  \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
 */
#pragma once
#ifndef ANTPOLYHEDRON_H
#define ANTPOLYHEDRON_H

#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
//#include "SAMSON.hpp"
//#include "SBIFileReader.hpp"
//#include "SBDDataGraphNode.hpp"
#include "ANTVectorMath.hpp"
#include "ANTAuxiliary.hpp"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"


using namespace rapidjson;

/* Half-edge implementation */
class ANTVertex;
class ANTEdge;
class ANTPolygon;

class ANTHalfEdge {
public:
  ANTHalfEdge();
  /**
   * Half-edges don't need custom destructor because in Halfedge 
   * representation they don't make sense by themselves
   */
  ~ANTHalfEdge() = default;
  ANTHalfEdge(const ANTHalfEdge& other);
  ANTHalfEdge& operator=(const ANTHalfEdge& other);

  ANTHalfEdge* next_ = nullptr;
  ANTHalfEdge* prev_ = nullptr;
  ANTHalfEdge* pair_ = nullptr;

  ANTVertex* source_ = nullptr;
  ANTPolygon* left_ = nullptr;
  ANTEdge* edge_ = nullptr;

  // we need an id_ for copying
  unsigned int id_;

  // Load and save methods
  void WriteToJson(Writer<StringBuffer>& writer);
};

class ANTVertex {
public:
  ANTVertex() = default;
  /**
   * Topologically vertex has to be removed from the polyhedron.
   * Hence default destructor.
   */
  ~ANTVertex() = default;
  ANTVertex(const ANTVertex& w);
  ANTVertex(ANTVertex* w);

  ANTVertex& operator=(const ANTVertex& other);

  /* Members */
  ANTHalfEdge* halfEdge_ = nullptr;
  int id_;
  SBPosition3 position_;

  /* Getters */
  std::vector<double> GetVectorCoordinates() const; // always returns picometers 10^-12
  SBPosition3 GetSBPosition() const;

  /* Setters */
  void SetCoordinates(SBPosition3 coordinates);

  // Load and save methods
  void WriteToJson(Writer<StringBuffer>& writer);
  static ANTVertex* LoadFromJson(Value::ConstMemberIterator itr, std::map<int, ANTHalfEdge*>& halfEdges);
};

class ANTEdge {
public:
  ANTEdge() = default;
  ~ANTEdge();
  ANTEdge(const ANTEdge& other);

  ANTEdge& operator=(const ANTEdge& other);

  ANTHalfEdge* halfEdge_ = nullptr;
  int id_;
};

class ANTPolygon {
public:
  ANTPolygon() = default;
  ANTPolygon(const ANTPolygon& p);
  ~ANTPolygon();

  ANTPolygon& operator=(const ANTPolygon& p);
  
  int id_;
  ANTHalfEdge* halfEdge_ = nullptr;
};

using Edges = std::vector<ANTEdge*>;
using Faces = std::vector<ANTPolygon*>;
using Vertices = std::map<int, ANTVertex*>;

class ANTPolyhedron {
public:
  /* Constructor */
  ANTPolyhedron() = default;
  ANTPolyhedron(const ANTPolyhedron &p);
  ANTPolyhedron(std::string filename);

  ~ANTPolyhedron();

  ANTPolyhedron& operator=(const ANTPolyhedron& other);

  /* Getters */
  Vertices GetVertices() const;
  Vertices GetOriginalVertices() const;
  Faces GetFaces() const;
  Edges GetEdges() const;
  ANTVertex* GetVertexById(int id);
  static ANTEdge* GetEdgeByVertices(ANTVertex* source, ANTVertex* target);
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
  std::pair<ANTEdge*, double> MinimumEdgeLength();
  std::pair<ANTEdge*, double> MaximumEdgeLength();
  static double CalculateEdgeLength(ANTEdge* edge);
  static int GetVertexDegree(ANTVertex* v);
  static bool IsInFace(ANTVertex* v, ANTPolygon* f);
  ANTHalfEdge * GetHalfEdge(unsigned int id);
  // static ANTPolygon* GetSharedFace(ANTVertex* v, ANTVertex* w);
  static ANTHalfEdge* GetHalfEdge(ANTVertex* v, ANTVertex* w);
  void Scale(double scalingFactor);
  void Center(SBPosition3 center);

  // Load and save methods
  void WriteToJson(Writer<StringBuffer>& writer);
  static ANTPolyhedron LoadFromJson(Value& value, std::map<int, ANTHalfEdge*> hes, std::map<int, ANTPolygon*> faces);

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
