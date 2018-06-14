/** \file
 *  \brief    Implementation of the DAEDALUS algorithm.
 *  \details  DAEDALUS algorithm designs an origami figure with two helices
 *            per edge. Minimum edge length has to be at least 31bp, for which
 *            it uses single crossovers. Otherwise a DX-motif is employed.
 *  \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
 */

#ifndef DASDAEDALUS_H
#define DASDAEDALUS_H

#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphviz.hpp>
#include "ANTPolyhedron.hpp"
#include "ANTNanorobot.hpp"
#include "ANTVectorMath.hpp"
#include "ANTConstants.hpp"


const double BP_RISE = ANTConstants::BP_RISE * 10;  // angstroms
const double DH_DIST = ANTConstants::DH_DIST + ANTConstants::DH_DIAMETER * 10;  // angstroms
const double SS_PADDING = 10;  //angstroms


struct DOTNode {
  DOTNode() = default;
  ~DOTNode() = default;
  DOTNode(const DOTNode& other);
  DOTNode& operator=(const DOTNode& other);

  std::vector<double> coordinates_;
  int id_;

  ANTVertex* vertex_; // if it corresponds with a vertex
  ANTEdge* edge_; // if it lies in an edge

  bool pseudo_ = false; // if it is a pseudo-node
};

struct DOTLink {
  DOTLink() = default;
  DOTLink(const DOTLink& other);
  ~DOTLink();
  DOTLink& operator=(const DOTLink& other);

  int id_;
  DOTNode* source_;
  DOTNode* target_;

  ANTHalfEdge* halfEdge_;
  bool directed_;
  bool split_ = false;

  ANTBaseSegment* firstBase_;
  ANTBaseSegment* lastBase_;

  int bp_ = 0; // number of base pairs
};

typedef std::vector<DOTLink*> LinkList;
typedef std::map<int, DOTNode*> NodeList;
typedef std::pair<SBPosition3, SBPosition3> PositionPair;
typedef std::map<ANTBaseSegment*, PositionPair> BSPositions;

struct VertexProperty {
  /** Structure to store vertex properties.
   *  \param node object associated with this vertex.
   *  \param index assigned to the vertex when created.
   *  \param original index (for split edges).
   *  \param x coordinate in 2D space (for Schlegel diagrams).
   *  \param y coordinate in 2D space (for Schlegel diagrams).
   */
  DOTNode* node_;
  int index;
  //int pseudoindex;
  //int original_index;
  float x;
  float y;
};

struct EdgeProperty {
  DOTLink* link_;
  int weight_ = 1;
};

/** \enum Daedalus::CrossoverType
 *  \brief The possible crossover types in DASDaedalus.
 */
enum CrossoverType {
  kSingleXO,
  kDoubleXO,
};

//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
//  VertexProperty, boost::property<boost::edge_weight_t, int>> UndirectedGraph;
typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> UndirectedGraph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty, EdgeProperty> DirectedGraph;
template<typename T>
using Edge = typename boost::graph_traits<T>::edge_descriptor;
template<typename T>
using Node = typename boost::graph_traits<T>::vertex_descriptor;
template<typename T>
using NodeIt = typename boost::graph_traits<T>::vertex_iterator;
template<typename T>
using EdgeIt = typename boost::graph_traits<T>::edge_iterator;
typedef std::vector<Edge<UndirectedGraph>> Path;

class DASDaedalus {
public:
  /** Destructor
   */
  ~DASDaedalus();
  /** 
   *  \param ANTNanorobot object with a mesh.
   *  \output Origami object.
   */
  ANTPart* ApplyAlgorithm(std::string seq, std::string filename, bool center = true);
  /**
  *  \param ANTNanorobot object with a mesh as ANTPolyhedron.
  *  \output Origami object.
  */
  ANTPart* ApplyAlgorithm(std::string seq, ANTPolyhedron & polyhedron, bool center = true);
  /** Setters
   */
  void SetMinEdgeLength(int l);
  static void SetVerticesPositions(ANTPart &origami, bool center = true);
  /** Returns the complimentary base
  *  \param the base nucleotide
  *  \return the pair of the input base
  */
  static DNABlocks GetComplementaryBase(DNABlocks type);
  /**
  * Calculates the size in integer multiples of 10.5bp
  */
  static int CalculateEdgeSize(SBQuantity::length nmLength);
protected:
private:
  /** Minimum edge length
   */
  int min_edge_length_ = 42; // default 42 bp
  /** Edge staple spanning size
   */
  int edge_staple_span_ = 21; // bp
  /** Vertex staple span at the beginning of an edge
   */
  int vertex_staple_span_start_= 10;
  /** Vertex staple span at the end of an edge
   */
  int vertex_staple_span_end_= 11;
  /** Size of the single crossover staples
   */
  int crossover_staple_span_ = 11;
  /** Crossover staple split
   */
  int crossover_split_ = 16;
  /** Number of Poly-T in vertex staples
   */
  int num_poly_t_ = 5;
  /** Map ids with Node objects
  */
  std::map<int, DOTNode*> nodes_;
  /** List with links
  */
  LinkList linkGraph_;
  std::map<int, ANTSingleStrand*> chains_;
  std::map<ANTHalfEdge*, ANTBaseSegment*> firstBasesHe_;
  BSPositions positionsBBSC_;
  /** Generates an undirected graph from figure from current nodes and links
   *  \return an UndirectedGraph object
   */
  UndirectedGraph GenerateUndirectedGraph();
  /** Create directed graph
   *  \param undirectedGraph
   *  \param map with position of bases
   *  \return DirectedGraph, direction is chosen randomly.
   */
  DirectedGraph GenerateDirectedGraph(const UndirectedGraph &u_graph, ANTPart& origami);
  /** Computes the MST from an undirected graph
   *  \param UndirectedGraph object
   *  \return a vector with the edges in the order they are accessed
   */
  std::vector<DOTLink*> MinimumSpanningTree(const UndirectedGraph &u_graph);
  /** Get the Eulerian Circuit from a cyclic graph
   *  \param the cyclic graph
   *  \return a vector with the nodes in order of visitation
   */
  std::vector<DOTNode*> GetEulerianCircuit(UndirectedGraph g);
  /** Get the neighbors from a node in a graph
   *  \param the node.
   *  \param the graph.
   *  \return a vector with the nodes connected to the input node.
   */
  std::vector<Node<UndirectedGraph>> GetNeighbours(Node<UndirectedGraph> v, UndirectedGraph g);
  /** Split edges not in MST.
   *  Edges not belonging to MST are split in half to allow scaffold crossovers.
   *  \param vector with MST
   *  \param target object
   */
  void DASDaedalus::SplitEdges(const std::vector<DOTLink*> &mst, ANTPart &origami);
  /** Adds Pseudo-nodes so the A-trail can be routed.
   *  \param the target polyhedron
   */
  void AddPseudoNodes(ANTPolyhedron &figure);
  /** Length of the routing
   *  \param Length of every edge
   *  \return the total length of the routing in base pairs.
   */
  int RoutingLength(EdgeBps &lengths);

  void CreateEdgeStaples(ANTPart &origami);
  void CreateVertexStaples(ANTPart &origami);
  /** Wrapper to add an edge to a graph if it doesn't exist
   *  \param id of the source node
   *  \param id of the target node
   *  \param graph either directed or undirected
   */
  template<typename T>
  void AddEdge(int id_source, int id_target, DOTLink* link, T &graph);
  /** Returns the crossover type.
   *  \param the length of the edge with the crossover.
   *  \return the type of crossover.
   *
   *  If min_edge_length_ = 31, the crossover may fall on a 31bp edge and using
   *  two 21bp staples is not possible.
   */
  CrossoverType GetCrossoverType(int edge_length);
  /** Initializes the edge map object.
   *
   *  We generate now the base objects to make sure we don't forget any position.
   *  \param origami ANTNanorobot object
   */
  void InitEdgeMap(ANTPart &origami);
  /** Get the length of an edge pair. Includes split edges.
   *  \param the edge pair.
   *  \param the edge map.
   */
  void CreateLinkGraphFromMesh(ANTPart &p);
  void AddNode(DOTNode* node);
  void RemoveNode(DOTNode* node);
  void RemoveLink(DOTLink* link);
  DOTLink* AddLink(std::pair<DOTNode*, DOTNode*> ab, int bp, ANTPolyhedron &fig);
  DOTNode* GetNodeById(int id) const;
  DOTLink* GetLinkByNodes(DOTNode* v, DOTNode* w) const;
  void SetEdgeBps(int min_edge_bp, ANTPart &part);
  void RouteScaffold(ANTSingleStrand* scaff, std::string seq, int routing_length);
  DNABlocks ParseBase(char c); //deprecated
  std::string GetChainSequence(int c_id);
  ANTBaseSegment* AdvanceBaseSegment(ANTBaseSegment* bs, int pos);
  ANTBaseSegment* MoveBackBaseSegment(ANTBaseSegment* bs, int pos);
  ANTSingleStrand* CreateEdgeChain(int c_id, ANTBaseSegment* bs, int pos_span, int neg_span);
  ANTSingleStrand* CreateVertexChain(int c_id, std::vector<ANTHalfEdge*>ps, EdgeBps &lengths);
  static void LogEdgeMap(ANTPart &origami);
  static SBVector3 SBCrossProduct(SBVector3 v, SBVector3 w);
  double SBInnerProduct(SBVector3 v, SBVector3 w);
  void LogLinkGraph();
  template <typename T>
  static void OutputGraph(T g, std::string filename);
  void Set2DPositions(ANTPart& origami);
  void Set1DPositions(ANTPart& origami);
};

#endif
