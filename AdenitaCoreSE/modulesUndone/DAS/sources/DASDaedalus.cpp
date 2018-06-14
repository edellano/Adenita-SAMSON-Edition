/** \file
 *  \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
 */

#include "DASDaedalus.hpp"

DASDaedalus::~DASDaedalus() {
  for (auto &l : linkGraph_) {
    delete l;
  }
  linkGraph_.clear();
  nodes_.clear();
  firstBasesHe_.clear();
  chains_.clear();
  positionsBBSC_.clear();
}

ANTPart* DASDaedalus::ApplyAlgorithm(std::string seq, std::string filename, bool center) {

  // Create ANTPolyhedron
  ANTPolyhedron& p = ANTPolyhedron(filename);
  p.Center(SBPosition3());
  
  ANTPart* daedalus_part = ApplyAlgorithm(seq, p, center);

  return daedalus_part;
}

ANTPart* DASDaedalus::ApplyAlgorithm(std::string seq, ANTPolyhedron & p, bool center) {
  ANTPart* daedalus_part = new ANTPart();

  daedalus_part->SetPolyhedron(p);

  // Set edge lengths in base pairs and generate the scaffold
  SetEdgeBps(min_edge_length_, *daedalus_part);
  SetVerticesPositions(*daedalus_part, center);
  // origami.LogPolyhedron();
  CreateLinkGraphFromMesh(*daedalus_part);
  InitEdgeMap(*daedalus_part);
  auto fig = daedalus_part->GetPolyhedron();

  int r_length = RoutingLength(daedalus_part->bpLengths_);
  if (r_length > seq.size()) {
    // we should display here a message in SAMSON
    std::string msg = "Selected sequence too short for targeted figure.";
    ANTAuxiliary::log(msg);
    // fulfill with NNN bases (this should be optional)
    int d = r_length - boost::numeric_cast<int>(seq.size());
    std::string ap = "";
    std::string base = "N";
    for (int i = 0; i < d; ++i) {
      ap += base;
    }
    seq += ap;
  }

  ANTSingleStrand* scaff = new ANTSingleStrand();
  scaff->id_ = 0;
  scaff->chainName_ = "Scaffold";
  scaff->isScaffold_ = true;
  chains_.insert(std::make_pair(scaff->id_, scaff));

  // Create undirected graph from figure
  UndirectedGraph u_graph = GenerateUndirectedGraph();
  // Compute MST
  LinkList mst = MinimumSpanningTree(u_graph);
  // Split edges not in MST
  SplitEdges(mst, *daedalus_part);
  // Add Pseudo-nodes
  AddPseudoNodes(fig);
  // LogLinkGraph();
  UndirectedGraph u_graph_pseudo = GenerateUndirectedGraph();
  // Create directed graph
  DirectedGraph d_graph = GenerateDirectedGraph(u_graph_pseudo, *daedalus_part);
  //LogLinkGraph();
  // Route scaffold
  RouteScaffold(scaff, seq, r_length);
  //ANTAuxiliary::log(GetChainSequence(scaff->id_));
  // Create staple objects
  CreateEdgeStaples(*daedalus_part);
  CreateVertexStaples(*daedalus_part);

  daedalus_part->SetSingleStrands(chains_);
  daedalus_part->SetScaffold(scaff->id_);
  // Set 1D and 2D positions for nucleotides
  Set2DPositions(*daedalus_part);
  Set1DPositions(*daedalus_part);
  daedalus_part->SetNtSegmentMap();

  return daedalus_part;
}

UndirectedGraph DASDaedalus::GenerateUndirectedGraph() {
  UndirectedGraph u_graph;
  // int new_id = 0;
  for (auto v_it = nodes_.begin(); v_it != nodes_.end(); ++v_it) {
    Node<UndirectedGraph> u = boost::add_vertex(VertexProperty{ v_it->second, v_it->first, 0.0, 0.0 }, u_graph);
  }

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    DOTNode* source = (*lit)->source_;
    DOTNode* target = (*lit)->target_;
    AddEdge(source->id_, target->id_, *lit, u_graph);
  }

  return u_graph;
}

DirectedGraph DASDaedalus::GenerateDirectedGraph(const UndirectedGraph &u_graph, ANTPart& origami) {
  DirectedGraph d_graph;
  // Add all nodes with the same indices
  boost::graph_traits<UndirectedGraph>::vertex_iterator v_it, v_end;
  for (std::tie(v_it, v_end) = boost::vertices(u_graph); v_it != v_end; ++v_it) {
    Node<DirectedGraph> new_v = boost::add_vertex(d_graph);
    d_graph[new_v].node_ = u_graph[*v_it].node_;
    d_graph[new_v].index = u_graph[*v_it].index;
    d_graph[new_v].x = u_graph[*v_it].x;
    d_graph[new_v].y = u_graph[*v_it].y;
  }
  // Find Eulerian circuit in the undirected graph and add edges
  // while going through the circuit
  std::vector<DOTNode*> eu_circuit = GetEulerianCircuit(u_graph);
  LinkList link_graph;
  int new_id = 0;
  for (auto vit = eu_circuit.begin(); vit != eu_circuit.end(); ++vit) {
    auto vvit = std::next(vit);
    if (vvit == eu_circuit.end()) {
      // We are done
      break;
    }
    auto e = boost::add_edge((*vit)->id_, (*vvit)->id_, d_graph);
    DOTLink* link = new DOTLink();
    DOTLink* undir_link = GetLinkByNodes(*vit, *vvit);
    link->id_ = new_id;
    ++new_id;
    link->source_ = *vit;
    link->target_ = *vvit;
    link->directed_ = true;
    if (link->source_->vertex_ == undir_link->halfEdge_->source_) {
      link->halfEdge_ = undir_link->halfEdge_;
    }
    else {
      link->halfEdge_ = undir_link->halfEdge_->pair_;
    }
    link->bp_ = undir_link->bp_;
    link->split_ = undir_link->split_;
    link_graph.push_back(link);
    d_graph[e.first].link_ = link;

    ANTBaseSegment* bs = firstBasesHe_.at(link->halfEdge_);
    link->firstBase_ = bs;
    if (link->split_) {
      DOTNode* s = link->source_;
      if (s->pseudo_) {
        // we need to advance it the other part of the edge
        int adv = origami.bpLengths_.at(link->halfEdge_->edge_) - link->bp_;
        link->firstBase_ = AdvanceBaseSegment(bs, adv);
      }
    }
  }

  linkGraph_ = link_graph;
  return d_graph;
}

template<typename T>
void DASDaedalus::AddEdge(int id_source, int id_target, DOTLink* link, T &graph) {
  bool edge_there = boost::edge(id_source, id_target, graph).second;
  if (!edge_there) {
    auto edge = boost::add_edge(id_source, id_target, graph);
    graph[edge.first].link_ = link;
  }
}

//Path DASDaedalus::MinimumSpanningTree(const UndirectedGraph &u_graph) {
LinkList DASDaedalus::MinimumSpanningTree(const UndirectedGraph &u_graph) {
  //Path prim_path;
  LinkList prim_path;
  std::vector<Node<UndirectedGraph>> mst_path_(boost::num_vertices(u_graph));
  //auto weights = boost::weight_map(get(&EdgeProperty::weight_, u_graph));
  boost::prim_minimum_spanning_tree(u_graph, &mst_path_[0], boost::weight_map(get(&EdgeProperty::weight_, u_graph)));

  std::size_t plength = mst_path_.size();
  for (std::size_t i = 0; i < plength; ++i) {
    if (mst_path_[i] != i) {
      Node<UndirectedGraph> v = boost::vertex(i, u_graph);
      Edge<UndirectedGraph> e = boost::edge(v, mst_path_[i], u_graph).first;
      DOTLink* link = u_graph[e].link_;
      //prim_path.push_back(e);
      prim_path.push_back(link);
    }
  }
  return prim_path;
}

void DASDaedalus::SplitEdges(const LinkList &mst, ANTPart &origami) {

  ANTPolyhedron &figure = origami.GetPolyhedron();
  LinkList links_to_remove;
  std::map<std::pair<DOTNode*, DOTNode*>, int> links_to_add;
  int id = nodes_.rbegin()->first + 1;

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    if (std::find(mst.begin(), mst.end(), *lit) == mst.end()) {
      //links_to_remove.push_back(*lit);
      DOTNode* u = (*lit)->source_;
      DOTNode* v = (*lit)->target_;
      ANTEdge* edge = (*lit)->halfEdge_->edge_;
      // create new nodes
      std::vector<double> coords{ 0.0, 0.0, 0.0 };
      DOTNode* ug = new DOTNode();
      ug->id_ = id;
      ++id;
      ug->edge_ = edge;
      ug->vertex_ = v->vertex_;
      ug->coordinates_ = coords;
      ug->pseudo_ = true;
      DOTNode* vg = new DOTNode();
      vg->id_ = id;
      ++id;
      vg->edge_ = edge;
      vg->vertex_ = u->vertex_;
      vg->coordinates_ = coords;
      vg->pseudo_ = true;
      AddNode(ug);
      AddNode(vg);
      links_to_remove.push_back(*lit);
      int e_len = origami.bpLengths_.at(edge);
      // calculate sizes
      //int sz1 = std::floor(float(e_len)*0.5);
      //int sz2 = std::ceil(float(e_len)*0.5);
      int sz1 = 0;
      int sz2 = 0;
      if (e_len % 21 == 0) {
        if (e_len % 2 == 0) {
          sz1 = (e_len - 32) * 0.5;
          sz2 = sz1 + 11;
        }
        else {
          sz1 = (e_len - 31) * 0.5;
          sz2 = sz1 + 10;
        }
      }
      else {
        if (e_len % 2 == 0) {
          sz1 = (e_len - 22) * 0.5;
          sz2 = sz1 + 1;
        }
        else {
          sz1 = (e_len - 21) * 0.5;
          sz2 = sz1;
        }
      }
      int sz_lower = sz1 + vertex_staple_span_start_;
      int sz_higher = sz2 + vertex_staple_span_end_;
      if (u->id_ > v->id_) {
        sz_lower = sz2 + vertex_staple_span_start_;
        sz_higher = sz1 + vertex_staple_span_end_;
      }
      links_to_add.insert(std::make_pair(std::make_pair(u, ug), sz_lower));
      links_to_add.insert(std::make_pair(std::make_pair(v, vg), sz_higher));
    }
  }

  for (auto lit = links_to_remove.begin(); lit != links_to_remove.end(); ++lit) {
    RemoveLink(*lit);
  }

  for (auto lit = links_to_add.begin(); lit != links_to_add.end(); ++lit) {
    DOTLink* l = AddLink(lit->first, lit->second, figure);
    l->split_ = true;
  }
}

void DASDaedalus::AddPseudoNodes(ANTPolyhedron &figure) {

  std::map<int, ANTVertex*> vertices = figure.GetVertices();
  Faces faces = figure.GetFaces();

  int new_id = 0;
  int l_id = 0;
  NodeList new_pseudos;
  NodeList new_nodes;
  LinkList new_link_list;

  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
    // keep track of original DOTNode and old
    std::map<int, DOTNode*> node_relation;
    ANTHalfEdge* begin = (*fit)->halfEdge_;
    ANTHalfEdge* he = begin;
    do {
      DOTNode* source = GetNodeById(he->source_->id_);
      DOTNode* new_s = new DOTNode();
      new_s->id_ = new_id;
      ++new_id;
      new_s->coordinates_ = source->coordinates_;
      new_s->vertex_ = source->vertex_;
      new_nodes.insert(std::make_pair(new_s->id_, new_s));
      node_relation.insert(std::make_pair(source->id_, new_s));
      he = he->next_;
    } while (he != begin);
    // find split edges
    for (auto nit = nodes_.begin(); nit != nodes_.end(); ++nit) {
      DOTNode* n = nit->second;
      ANTVertex* v = n->vertex_;
      if (n->pseudo_ && ANTPolyhedron::IsInFace(v, *fit)) {
        DOTNode* new_s = new DOTNode();
        if (new_pseudos.find(n->id_) != new_pseudos.end()) {
          new_s = new_pseudos.at(n->id_);
        }
        else {
          new_s->id_ = new_id;
          ++new_id;
          new_s->coordinates_ = n->coordinates_;
          new_s->vertex_ = n->vertex_;
          new_s->pseudo_ = true;
          new_pseudos.insert(std::make_pair(n->id_, new_s));
        }
        new_nodes.insert(std::make_pair(new_s->id_, new_s));
        node_relation.insert(std::make_pair(n->id_, new_s));
      }
    }
    // Add links
    for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
      DOTNode* v = (*lit)->source_;
      DOTNode* w = (*lit)->target_;
      // check if link is in face
      if (ANTPolyhedron::IsInFace(v->vertex_, *fit) && ANTPolyhedron::IsInFace(w->vertex_, *fit)) {
        DOTNode* s = node_relation.at(v->id_);
        DOTNode* t = node_relation.at(w->id_);
        DOTLink* link = new DOTLink();
        link->id_ = l_id;
        ++l_id;
        link->source_ = s;
        link->target_ = t;
        link->halfEdge_ = figure.GetEdgeByVertices(s->vertex_, t->vertex_)->halfEdge_;
        link->directed_ = false;
        if ((new_pseudos.find(v->id_) != new_pseudos.end()) || (new_pseudos.find(w->id_) != new_pseudos.end())) {
          link->split_ = true;
        }
        link->bp_ = (*lit)->bp_;
        new_link_list.push_back(link);
      }
    }
  }
  nodes_ = new_nodes;
  linkGraph_ = new_link_list;
}

void DASDaedalus::SetMinEdgeLength(int l) {
  min_edge_length_ = l;
}

//template<typename T>
std::vector<DOTNode*> DASDaedalus::GetEulerianCircuit(UndirectedGraph g) {
  // Not really a generic algorithm since we work with a cycle undirected graph
  std::vector<DOTNode*> eu_circuit;
  Node<UndirectedGraph> start_v = *boost::vertices(g).first;
  // Future work: compute path going counter-clockwise to face
  Node<UndirectedGraph> actual_v = GetNeighbours(start_v, g)[0];
  Node<UndirectedGraph> prev_v = start_v;
  Node<UndirectedGraph> next_v = actual_v;
  eu_circuit.push_back(g[start_v].node_);
  eu_circuit.push_back(g[actual_v].node_);
  while (start_v != next_v) {
    std::vector<Node<UndirectedGraph>> neighbours = GetNeighbours(actual_v, g);
    // Choose one different from prev_v
    next_v = neighbours[0];
    if (next_v == prev_v) {
      next_v = neighbours[1];
    }
    eu_circuit.push_back(g[next_v].node_);
    prev_v = actual_v;
    actual_v = next_v;
  }
  return eu_circuit;
}

std::vector<Node<UndirectedGraph>> DASDaedalus::GetNeighbours(Node<UndirectedGraph> v, UndirectedGraph g) {
  std::vector<Node<UndirectedGraph>> neighbours;
  boost::graph_traits<UndirectedGraph>::adjacency_iterator v_it, v_end;
  for (boost::tie(v_it, v_end) = boost::adjacent_vertices(v, g); v_it != v_end; ++v_it) {
    neighbours.push_back(*v_it);
  }
  return neighbours;
}

int DASDaedalus::RoutingLength(EdgeBps &lengths) {
  int total_length = 0;
  for (auto l_it = lengths.begin(); l_it != lengths.end(); ++l_it) {
    int len = l_it->second;
    total_length += len;
  }
  // In DASDaedalus the scaffold goes through every edge twice!
  return 2 * total_length;
}

void DASDaedalus::CreateEdgeStaples(ANTPart &origami) {

  unsigned int c_id = chains_.rbegin()->first + 1;
  std::vector<ANTEdge*> stapled_edges;

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    ANTEdge* edge = (*lit)->halfEdge_->edge_;
    int length = origami.bpLengths_.at(edge);
    int aval_length = length - vertex_staple_span_start_ - vertex_staple_span_end_;
    CrossoverType c_type = GetCrossoverType(length);
    int staple_length = edge_staple_span_;
    int num_staples = floor(aval_length / edge_staple_span_);
    // dirty fix for 31bp
    if (c_type == kSingleXO) {
      num_staples = 1;
      staple_length = 10;
    }
    int length_extra_staple = aval_length - staple_length*num_staples;
    if (std::find(stapled_edges.begin(), stapled_edges.end(), edge) == stapled_edges.end()) {
      if (!(*lit)->split_) {
        int start = vertex_staple_span_start_;
        for (int i = 0; i < num_staples; ++i) {
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, staple_length, staple_length);
          start += staple_length;
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
        }
        if (length_extra_staple > 0) {
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + crossover_staple_span_);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, crossover_staple_span_, crossover_staple_span_);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
        }
      }
      else {
        // Edge is split, place crossover as much in the middle as possible
        int num_staples_first = int(floor(num_staples - 1)*0.5);
        int num_staples_second = num_staples - 1 - num_staples_first;
        // Divide staples in sides(place the scaffold crossover)
        int start = vertex_staple_span_start_;
        if (length_extra_staple > 0) {
          // First pair keeps the extra staple
          int span = crossover_staple_span_;
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + crossover_staple_span_);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, crossover_staple_span_, crossover_staple_span_);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          start += crossover_staple_span_;
        }
        for (int i = 0; i < num_staples_first; ++i) {
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, staple_length, staple_length);
          chains_.insert(std::make_pair(c_id, chain));
          start += staple_length;
          ++c_id;
        }
        // Place crossover now
        int span_pos;
        int span_neg;
        if (c_type == kDoubleXO) {
          span_pos = 16;
          span_neg = 5;
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + span_pos);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          bs_start = MoveBackBaseSegment((*lit)->firstBase_->pair_, start + span_neg);
          ANTSingleStrand* chain2 = CreateEdgeChain(c_id, bs_start, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain2));
          ++c_id;
          start += span_pos + span_neg;
        }
        else {
          span_pos = 10;
          span_neg = 10;
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + span_pos);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          start += span_pos;
        }
        // Remaining staples
        for (int i = 0; i < num_staples_second; ++i) {
          ANTBaseSegment* bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ANTSingleStrand* chain = CreateEdgeChain(c_id, bs_start, staple_length, staple_length);
          chains_.insert(std::make_pair(c_id, chain));
          start += staple_length;
          ++c_id;
        }
      }
      stapled_edges.push_back(edge);
    }
  }
}

CrossoverType DASDaedalus::GetCrossoverType(int edge_length) {
  CrossoverType t;
  if (edge_length < 42) {
    t = kSingleXO;
  }
  else {
    t = kDoubleXO;
  }
  return t;
}

void DASDaedalus::CreateVertexStaples(ANTPart &origami) {

  auto &figure = origami.GetPolyhedron();
  std::map<int, ANTVertex*> vertices = figure.GetVertices();
  int c_id = chains_.rbegin()->first + 1;

  for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
    int degree = figure.GetVertexDegree(vit->second);
    // number of 52nt staples => 2 pseudonodes
    int a;
    if (degree % 3 == 0) {
      a = 0;
    }
    else if (degree % 3 == 1) {
      a = 2;
    }
    else if (degree % 3 == 2) {
      a = 1;
    }
    // number of 78nt staples => 3 pseudonodes
    int b = (degree - 2 * a) / 3;

    std::vector<ANTHalfEdge*> edges;
    ANTHalfEdge* begin = vit->second->halfEdge_;
    ANTHalfEdge* he = begin;
    do {
      edges.push_back(he);
      edges.push_back(he->pair_);
      he = he->pair_->next_;
    } while (he != begin);

    for (int i = 0; i < a; ++i) {
      // 52nt span 2 edges = 4 half edges
      std::vector<ANTHalfEdge*> ps(edges.end() - 4, edges.end());
      ANTSingleStrand* chain = CreateVertexChain(c_id, ps, origami.bpLengths_);
      chains_.insert(std::make_pair(c_id, chain));
      ++c_id;
      edges.erase(edges.end() - 4, edges.end());
    }
    for (int i = 0; i < b; ++i) {
      // 78nt span 3 edges = 6 half edges
      std::vector<ANTHalfEdge*> ps(edges.end() - 6, edges.end());
      ANTSingleStrand* chain = CreateVertexChain(c_id, ps, origami.bpLengths_);
      chains_.insert(std::make_pair(c_id, chain));
      ++c_id;
      edges.erase(edges.end() - 6, edges.end());
    }
  }
}

DNABlocks DASDaedalus::GetComplementaryBase(DNABlocks type) {
  return ANTModel::GetComplementaryBase(type);
}

//void DASDaedalus::InitEdgeMap(ANTPart &origami) {
//  unsigned int b_id = 0;
//  int j_id = 0;
//  Faces faces = origami.GetFaces();
//
//  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
//    ANTHalfEdge* begin = (*fit)->halfEdge_;
//    ANTHalfEdge* he = begin;
//    SBVector3 norm = ANTPart::GetPolygonNorm(*fit);
//    do {
//      // iterate over face edges
//      ANTVertex* s = he->source_;
//      ANTVertex* t = he->pair_->source_;
//      SBPosition3 coords = s->GetSBPosition();
//      SBVector3 dir = (t->GetSBPosition() - coords).normalizedVersion();
//      // calculate v = -dir x normal to add padding
//      SBVector3 u = SBCrossProduct(dir, norm);
//      // calculate rise along the normal
//      ANTPolygon* adj_face = he->pair_->left_;
//      SBVector3 adj_norm = ANTPart::GetPolygonNorm(adj_face);
//      double cos_theta = SBInnerProduct(norm, adj_norm);
//      double theta = acos(cos_theta);
//      double sn = sin(theta * 0.5);
//      double separation = DH_DIST*0.5 / sn;
//      coords += SBQuantity::angstrom(separation)*norm;
//
//      int l = origami.bpLengths_.at(he->edge_) + 1; // +1 since we have apolyT region at the end
//
//      ANTJoint* source = new ANTJoint();
//      source->id_ = j_id;
//      ++j_id;
//      source->SetCoordinates(coords);
//      coords += SBQuantity::angstrom(SS_PADDING * 0.5)*u.normalizedVersion();
//      ANTBaseSegment* prev_bs = nullptr;
//      for (int i = 0; i < l; ++i) {
//        // every step is a ANTBaseSegment
//        ANTJoint* target = new ANTJoint();
//        target->id_ = j_id;
//        target->SetCoordinates(coords);
//        ANTBaseSegment* bs = new ANTBaseSegment(source, target);
//        target->edge_ = bs;
//
//        bs->id_ = b_id;
//        bs->normal_ = norm;
//        bs->direction_ = dir;
//        bs->u_ = u;
//        bs->number_ = i;
//
//        if (i == l - 1) {
//          // this region is polyT
//          ANTLoopPair* cell = new ANTLoopPair();
//          bs->SetCell(cell);
//        }
//        else {
//          // this region is dsDNA
//          ANTBasePair* cell = new ANTBasePair();
//          bs->SetCell(cell);
//        }
//        bs->previous_ = prev_bs;
//        if (prev_bs != nullptr) prev_bs->next_ = bs;
//
//        // Save objects
//        origami.AddJoint(target);
//        origami.AddBaseSegment(bs);
//
//        // match first segment with corresponding halfedge
//        if (i == 0) {
//          firstBasesHe_.insert(std::make_pair(he, bs));
//        }
//
//        // next cycle
//        prev_bs = bs;
//        source = target;
//        ++j_id;
//        ++b_id;
//        coords += SBQuantity::angstrom(BP_RISE)*dir;
//      }
//      he = he->next_;
//    } while (he != begin);
//  }
//  // Pair base segments and create double helices
//  Edges edges = origami.GetEdges();
//
//  // LogEdgeMap(e_map);
//}

void DASDaedalus::InitEdgeMap(ANTPart &origami) {
  unsigned int b_id = 0;
  int j_id = 0;
  Faces faces = origami.GetFaces();

  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
    ANTHalfEdge* begin = (*fit)->halfEdge_;
    ANTHalfEdge* he = begin;
    SBVector3 norm = ANTPart::GetPolygonNorm(*fit);
    do {
      // iterate over face edges
      ANTVertex* s = he->source_;
      ANTVertex* t = he->pair_->source_;
      SBPosition3 coords = s->GetSBPosition();
      SBVector3 dir = (t->GetSBPosition() - coords).normalizedVersion();
      // calculate v = -dir x normal to add padding
      SBVector3 u = SBCrossProduct(dir, norm);
      // calculate rise along the normal
      ANTPolygon* adj_face = he->pair_->left_;
      SBVector3 adj_norm = ANTPart::GetPolygonNorm(adj_face);
      double cos_theta = SBInnerProduct(norm, adj_norm);
      double theta = acos(cos_theta);
      double sn = sin(theta * 0.5);
      double separation = DH_DIST*0.5 / sn;
      coords += SBQuantity::angstrom(separation)*norm;

      int l = origami.bpLengths_.at(he->edge_) + 1; // +1 since we have apolyT region at the end

      ANTJoint* source = new ANTJoint();
      source->id_ = j_id;
      ++j_id;
      origami.AddJoint(source);
      coords += SBQuantity::angstrom(SS_PADDING * 0.5)*u.normalizedVersion();
      source->SetCoordinates(coords);
      coords += SBQuantity::angstrom(BP_RISE)*dir;
      ANTBaseSegment* prev_bs = nullptr;
      for (int i = 0; i < l; ++i) {
        // every step is a ANTBaseSegment
        ANTJoint* target = new ANTJoint();
        target->id_ = j_id;
        target->SetCoordinates(coords);
        ANTBaseSegment* bs = new ANTBaseSegment(source, target);
        target->edge_ = bs;

        bs->id_ = b_id;
        bs->normal_ = norm;
        bs->direction_ = dir;
        bs->u_ = u;
        bs->number_ = i;

        if (i == l - 1) {
          // this region is polyT
          ANTLoopPair* cell = new ANTLoopPair();
          bs->SetCell(cell);
        }
        else {
          // this region is dsDNA
          ANTBasePair* cell = new ANTBasePair();
          bs->SetCell(cell);
        }
        bs->previous_ = prev_bs;
        if (prev_bs != nullptr) prev_bs->next_ = bs;

        // Save objects
        origami.AddJoint(target);
        origami.AddBaseSegment(bs);

        // match first segment with corresponding halfedge
        if (i == 0) {
          firstBasesHe_.insert(std::make_pair(he, bs));
        }

        // next cycle
        prev_bs = bs;
        source = target;
        ++j_id;
        ++b_id;
        coords += SBQuantity::angstrom(BP_RISE)*dir;
      }
      he = he->next_;
    } while (he != begin);
  }
  // Pair base segments and create double helices

  for (auto it = firstBasesHe_.begin(); it != firstBasesHe_.end(); ++it) {
    ANTHalfEdge* fhe = it->first;
    ANTHalfEdge* she = fhe->pair_;
    ANTBaseSegment* begin = it->second;
    ANTBaseSegment* bs = begin;
    ANTEdge* e = fhe->edge_;
    int length = origami.bpLengths_.at(e) + 1;  // to include polyTs
    ANTBaseSegment* bs_pair = AdvanceBaseSegment(firstBasesHe_.at(she), length - 2); // since the last one is a PolyT

    ANTDoubleStrand* ds = new ANTDoubleStrand();
    ds->size_ = length;
    ds->start_ = bs;
    origami.RegisterDoubleStrand(ds);

    ANTDoubleStrand* ds_pair = new ANTDoubleStrand();
    ds_pair->size_ = length;
    ds_pair->start_ = firstBasesHe_.at(she);
    origami.RegisterDoubleStrand(ds_pair);

    for (int i = 0; i < length; ++i) {
      bs->doubleStrand_ = ds;
      if (bs->GetCell()->GetType() == CellType::LoopPair) {
        int here = 1;
      }
      if (bs->GetCell()->GetType() == CellType::BasePair) {
        // we only pair ANTBasePair
        bs_pair->doubleStrand_ = ds_pair;
        bs->pair_ = bs_pair;
        bs_pair->pair_ = bs;
        bs_pair = bs_pair->previous_;
      }
      bs = bs->next_;
    }
  }
}

void DASDaedalus::CreateLinkGraphFromMesh(ANTPart& origami) {
  ANTPolyhedron &figure = origami.GetPolyhedron();
  Vertices vertices = figure.GetVertices();
  Edges edges = figure.GetEdges();
  for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
    DOTNode* node = new DOTNode();
    ANTVertex* vertex = vit->second;
    node->id_ = vertex->id_;
    node->coordinates_ = vertex->GetVectorCoordinates();
    node->vertex_ = vertex;
    nodes_.insert(std::make_pair(node->id_, node));
  }
  int id = 0;
  for (auto eit = edges.begin(); eit != edges.end(); ++eit) {
    DOTLink* link = new DOTLink();
    ANTHalfEdge* he = (*eit)->halfEdge_;
    link->directed_ = false;
    link->halfEdge_ = he;
    link->id_ = id;
    ++id;
    link->source_ = nodes_[he->source_->id_];
    link->target_ = nodes_[he->pair_->source_->id_];
    link->bp_ = origami.bpLengths_.at(he->edge_);
    linkGraph_.push_back(link);
  }
}

void DASDaedalus::AddNode(DOTNode* node) {
  nodes_.insert(std::make_pair(node->id_, node));
}

void DASDaedalus::RemoveLink(DOTLink* link) {
  linkGraph_.erase(std::find(linkGraph_.begin(), linkGraph_.end(), link));
}

DOTLink* DASDaedalus::AddLink(std::pair<DOTNode*, DOTNode*> ab, int bp, ANTPolyhedron &fig) {
  DOTLink* link = new DOTLink();
  link->source_ = ab.first;
  link->target_ = ab.second;
  link->halfEdge_ = fig.GetEdgeByVertices(ab.first->vertex_, ab.second->vertex_)->halfEdge_;
  link->directed_ = false;
  int id = (*(linkGraph_.rbegin()))->id_ + 1;
  link->id_ = id;
  link->bp_ = bp;
  linkGraph_.push_back(link);
  return link;
}

void DASDaedalus::RemoveNode(DOTNode* node) {
  LinkList links_to_remove;
  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    DOTNode* s = (*lit)->source_;
    DOTNode* t = (*lit)->target_;
    if (s == node || t == node) {
      links_to_remove.push_back(*lit);
    }
  }

  for (auto lit = links_to_remove.begin(); lit != links_to_remove.end(); ++lit) {
    RemoveLink(*lit);
  }

  nodes_.erase(nodes_.find(node->id_));
}

DOTNode* DASDaedalus::GetNodeById(int id) const {
  return nodes_.at(id);
}

DOTLink* DASDaedalus::GetLinkByNodes(DOTNode* v, DOTNode* w) const {
  DOTLink* link = new DOTLink();
  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    if ((*lit)->source_ == v && (*lit)->target_ == w) {
      link = *lit;
      break;
    }
    if (!(*lit)->directed_) {
      if ((*lit)->source_ == w && (*lit)->target_ == v) {
        link = *lit;
        break;
      }
    }
  }
  return link;
}

void DASDaedalus::SetEdgeBps(int min_edge_bp, ANTPart &part) {
  float min_length = part.MinimumEdgeLength().second;
  float rel = min_edge_bp / min_length;

  Edges edges = part.GetEdges();

  for (auto it = edges.begin(); it != edges.end(); ++it) {
    double l = ANTPolyhedron::CalculateEdgeLength(*it);
    double bp_l = l*rel;
    int quot;
    remquo(bp_l, 10.5, &quot);
    int bp_length = int(std::floor(float(quot * 10.5)));
    part.bpLengths_.insert(std::make_pair(*it, bp_length));
  }
}

void DASDaedalus::RouteScaffold(ANTSingleStrand* scaff, std::string seq, int routing_length) {

  unsigned int nt_id = 0;

  std::string used_seq = seq.substr(0, routing_length);
  //std::reverse(used_seq.begin(), used_seq.end());
  scaff->direction_ = Direction::ThreeToFive;

  // std::string msg = "Logging nt coordinates...";
  // ANTAuxiliary::log(msg);
  ANTNucleotide* prev = nullptr;
  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    ANTBaseSegment* bs = (*lit)->firstBase_;
    for (int i = 0; i < (*lit)->bp_; ++i) {
      ANTNucleotide* nt = new ANTNucleotide();
      nt->id_ = nt_id;
      nt->type_ = ParseBase(used_seq[nt_id]);
      nt->SetPosition(bs->GetSBPosition());
      nt->SetBackboneCenter(bs->GetSBPosition());
      nt->SetSidechainCenter(bs->GetSBPosition());
      nt->strand_ = scaff;
      nt->SetBaseSegment(bs);
      // std::vector<double> c = nt->GetVectorPosition();
      // std::string coords = std::to_string(c[0]) + "\t" + std::to_string(c[1]) + "\t" + std::to_string(c[2]);
      // ANTAuxiliary::log(coords);
      ANTCell* cell = bs->GetCell();
      if (cell->GetType() == BasePair) {
        // scaffold only goes through dsDNA regions
        ANTBasePair* bp_cell = static_cast<ANTBasePair*>(cell);
        bp_cell->SetLeftNucleotide(nt);
      }
      scaff->nucleotides_.insert(std::make_pair(nt->id_, nt));
      nt->prev_ = prev;
      if (prev != nullptr) {
        prev->next_ = nt;
      }
      prev = nt;
      bs = bs->next_;
      ++nt_id;
    }
  }
  prev->next_ = nullptr;
  scaff->SetNucleotidesEnd();
  // msg = "Logging of nt coordinates finished.";
  // ANTAuxiliary::log(msg);
}

DNABlocks DASDaedalus::ParseBase(char c) {
  DNABlocks b = DNABlocks::DN_;
  if (c == 'A') {
    b = DNABlocks::DA_;
  }
  else if (c == 'T') {
    b = DNABlocks::DT_;
  }
  else if (c == 'C') {
    b = DNABlocks::DC_;
  }
  else if (c == 'G') {
    b = DNABlocks::DG_;
  }
  return b;
}

std::string DASDaedalus::GetChainSequence(int c_id) {
  ANTSingleStrand* scaff = chains_.at(c_id);
  std::string seq = scaff->GetSequence();

  return seq;
}

ANTBaseSegment* DASDaedalus::AdvanceBaseSegment(ANTBaseSegment* bs, int pos) {
  ANTBaseSegment* res = bs->next_;
  for (int i = 0; i < pos - 1; ++i) {
    res = res->next_;
  }
  return res;
}

ANTBaseSegment* DASDaedalus::MoveBackBaseSegment(ANTBaseSegment* bs, int pos) {
  ANTBaseSegment* res = bs->previous_;
  for (int i = 0; i < pos - 1; ++i) {
    res = res->previous_;
  }
  return res;
}

ANTSingleStrand* DASDaedalus::CreateEdgeChain(int c_id, ANTBaseSegment* bs, int pos_span, int neg_span) {

  ANTSingleStrand* chain = new ANTSingleStrand();
  chain->id_ = c_id;
  chain->chainName_ = "Edge Staple " + std::to_string(c_id);
  chain->direction_ = Direction::FiveToThree;
  // staples always go in opposite direction to bs->next_, since scaffold follows bs->next_
  // BaseSegment* bs = AdvanceBaseSegment(bs_start, start+pos_span);
  // BaseSegment* bs_pair = MoveBackBaseSegment(bs->pair_, neg_span);

  unsigned int nt_id = 0;
  bool fst = true;
  ANTNucleotide* prev_nt = new ANTNucleotide();

  for (int i = 0; i < pos_span; ++i) {
    ANTNucleotide* nt = new ANTNucleotide();
    ANTNucleotide* ntOld = bs->GetNucleotide();
    nt->id_ = nt_id;
    nt->type_ = GetComplementaryBase(ntOld->type_);
    nt->SetPosition(bs->GetSBPosition());
    nt->SetBackboneCenter(bs->GetSBPosition());
    nt->SetSidechainCenter(bs->GetSBPosition());
    nt->pair_ = ntOld;
    ntOld->pair_ = nt;
    nt->strand_ = chain;
    nt->SetBaseSegment(bs);
    ANTCell* cell = bs->GetCell();
    if (cell->GetType() == BasePair) {
      // edge staples only goes through dsDNA regions
      ANTBasePair* bp_cell = static_cast<ANTBasePair*>(cell);
      bp_cell->SetRightNucleotide(nt);
    }
    if (!fst) {
      nt->prev_ = prev_nt;
      prev_nt->next_ = nt;
    }
    else {
      nt->end_ = FivePrime;
      nt->prev_ = nullptr;
      fst = false;
    }
    chain->nucleotides_.insert(std::make_pair(nt_id, nt));
    prev_nt = nt;
    bs = bs->previous_;
    ++nt_id;
  }
  // move base segment to other edge
  bs = bs->pair_;

  for (int i = 0; i < neg_span; ++i) {
    ANTNucleotide* nt = new ANTNucleotide();
    ANTNucleotide* ntOld = bs->GetNucleotide();
    nt->id_ = nt_id;
    nt->type_ = GetComplementaryBase(ntOld->type_);
    nt->SetPosition(bs->GetSBPosition());
    nt->SetBackboneCenter(bs->GetSBPosition());
    nt->SetSidechainCenter(bs->GetSBPosition());
    nt->pair_ = ntOld;
    nt->SetBaseSegment(bs);
    ANTCell* cell = bs->GetCell();
    if (cell->GetType() == BasePair) {
      // edge staples only goes through dsDNA regions
      ANTBasePair* bp_cell = static_cast<ANTBasePair*>(cell);
      bp_cell->SetRightNucleotide(nt);
    }
    ntOld->pair_ = nt;
    nt->prev_ = prev_nt;
    nt->strand_ = chain;
    prev_nt->next_ = nt;
    chain->nucleotides_.insert(std::make_pair(nt_id, nt));
    prev_nt = nt;
    bs = bs->previous_;
    ++nt_id;
  }
  prev_nt->end_ = ThreePrime;
  prev_nt->next_ = nullptr;

  chain->SetNucleotidesEnd();
  return chain;
}

ANTSingleStrand* DASDaedalus::CreateVertexChain(int c_id, std::vector<ANTHalfEdge*>ps, EdgeBps &lengths) {
  ANTSingleStrand* chain = new ANTSingleStrand();
  chain->id_ = c_id;
  chain->chainName_ = "Vertex Staple " + std::to_string(c_id);
  //chain->direction_ = Direction::FiveToThree;
  bool beg = true;
  int len = 0;

  unsigned int nt_id = 0;
  bool fst = true; // we can change it for polyT to int = 0, 1, 2
  // we follow base segments but staples go in the opposite direction
  ANTNucleotide* next_nt = new ANTNucleotide();

  int count = 0;
  for (auto pit = ps.begin(); pit != ps.end(); ++pit) {
    ANTBaseSegment* bs = firstBasesHe_.at(*pit);
    // vertex of lower index gets end, vertex of higher index gets beginning
    if (beg) {
      len = vertex_staple_span_end_;
      beg = false;
      // we need first bs
    }
    else {
      len = vertex_staple_span_start_;
      bs = AdvanceBaseSegment(bs, lengths.at((*pit)->edge_) - len);
      beg = true;
    }
    for (int i = 0; i < len; ++i) {
      // this are the ANTBasePair nts
      ANTNucleotide* nt = new ANTNucleotide();
      ANTCell* cell = bs->GetCell();
      nt->id_ = nt_id;
      nt->SetPosition(bs->GetSBPosition());
      nt->SetBackboneCenter(bs->GetSBPosition());
      nt->SetSidechainCenter(bs->GetSBPosition());
      nt->strand_ = chain;
      ANTNucleotide* ntOld = bs->GetNucleotide();
      nt->type_ = GetComplementaryBase(ntOld->type_);
      nt->pair_ = ntOld;
      nt->SetBaseSegment(bs);
      ntOld->pair_ = nt;
      chain->nucleotides_.insert(std::make_pair(nt_id, nt));
      ANTBasePair* bp_cell = static_cast<ANTBasePair*>(cell);
      bp_cell->SetRightNucleotide(nt);
      if (!fst) {
        nt->next_ = next_nt;
        next_nt->prev_ = nt;
      }
      else fst = false;
      next_nt = nt;
      bs = bs->next_;
      ++nt_id;
    }
    if (count % 2 != 0) {
      ANTCell* cell = bs->GetCell();
      if (cell->GetType() != LoopPair) {
        std::string msg = "PolyT region not reached";
        ANTAuxiliary::log(msg);
      }
      else {
        ANTLoopPair* loop_cell = static_cast<ANTLoopPair*>(cell);
        ANTLoop* loop = ANTPart::AppendPolyTLoopThreeToFive(chain, bs, next_nt, num_poly_t_);
        loop_cell->SetRightLoop(loop);
        next_nt = loop->startNt_;
        nt_id = next_nt->id_ + 1;
        bs = bs->next_;
      }
    }
    ++count;
  }
  // set beginning of chain
  chain->SetNucleotidesEnd();
  chain->ShiftStart(chain->nucleotides_.at(11));
  return chain;
}

void DASDaedalus::SetVerticesPositions(ANTPart &origami, bool center) {
  std::vector<ANTVertex*> done_vertex;
  Vertices vertices = origami.GetVertices();
  Vertices originalVertices = origami.GetOriginalVertices();
  SBPosition3 cm = SBPosition3();
  ANTVertex* v = vertices.begin()->second;
  ANTVertex* orig_v = originalVertices.at(v->id_);
  done_vertex.push_back(orig_v);
  cm += v->GetSBPosition();
  for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
    v = vit->second;
    orig_v = originalVertices.at(v->id_);
    ANTHalfEdge* begin = v->halfEdge_;
    ANTHalfEdge* he = begin;
    do {
      SBPosition3 c_v = v->GetSBPosition();
      SBPosition3 orig_c_v = orig_v->GetSBPosition();
      ANTVertex* next_v = he->pair_->source_;
      ANTVertex* orig_next_v = originalVertices.at(next_v->id_);
      ANTEdge* edge = ANTPolyhedron::GetEdgeByVertices(v, next_v);
      int bp_length = origami.bpLengths_.at(edge);
      double length = bp_length * BP_RISE; // angstrom

      // modify next vertex
      if (std::find(done_vertex.begin(), done_vertex.end(), orig_next_v) == done_vertex.end()) {
        SBPosition3 orig_c_next = orig_next_v->GetSBPosition();
        SBVector3 dir = (orig_c_next - orig_c_v).normalizedVersion();
        SBQuantity::angstrom l(length);

        SBPosition3 new_c_next = c_v + l*dir;

        next_v->SetCoordinates(new_c_next);
        cm += new_c_next;
        done_vertex.push_back(orig_next_v);
      }

      he = he->pair_->next_;
      v = he->source_;
      orig_v = originalVertices.at(v->id_);
    } while (he != begin);
  }
  cm /= vertices.size();
  // center
  if (center) {
    for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
      SBPosition3 c = vit->second->GetSBPosition();
      c = c - cm;
      vit->second->SetCoordinates(c);
    }
  }
}

void DASDaedalus::LogEdgeMap(ANTPart& origami) {
  std::vector<ANTBaseSegment*> bases;
  auto edgeMap = origami.GetBaseSegments();
  // log the edge map as n x 3 matrix
  std::string msg = " == Recording edge map ==";
  ANTAuxiliary::log(msg);
  for (auto eit = edgeMap.begin(); eit != edgeMap.end(); ++eit) {
    ANTJoint* v = eit->second->source_;
    ANTBaseSegment* bs = eit->second;
    if (bs->pair_ == nullptr) {
      bases.push_back(bs);
    }
  }
  msg = " == Unpaired bases == ";
  ANTAuxiliary::log(msg);
  for (auto vit = bases.begin(); vit != bases.end(); ++vit) {
    msg = std::to_string((*vit)->id_);
    ANTAuxiliary::log(msg);
  }
  msg = " == End of unpaired bases ==";
  ANTAuxiliary::log(msg);
  msg = " == End of edge map == ";
  ANTAuxiliary::log(msg);
}

SBVector3 DASDaedalus::SBCrossProduct(SBVector3 v, SBVector3 w) {
  std::vector<double> v_std = { v[0].getValue(), v[1].getValue(), v[2].getValue() };
  std::vector<double> w_std = { w[0].getValue(), w[1].getValue(), w[2].getValue() };
  ublas::vector<double> v_boost = ANTVectorMath::CreateBoostVector(v_std);
  ublas::vector<double> w_boost = ANTVectorMath::CreateBoostVector(w_std);
  ublas::vector<double> u_boost = ANTVectorMath::CrossProduct(v_boost, w_boost);
  u_boost /= ublas::norm_2(u_boost);
  std::vector<double> u_std = ANTVectorMath::CreateStdVector(u_boost);
  SBVector3 u = SBVector3(u_std[0], u_std[1], u_std[2]);
  return u;
}

double DASDaedalus::SBInnerProduct(SBVector3 v, SBVector3 w) {
  auto res = v[0] * w[0] + v[1] * w[1] + v[2] * w[2];
  return res.getValue();
}

void DASDaedalus::LogLinkGraph() {
  std::string msg = " == Recording link graph ==";
  ANTAuxiliary::log(msg);
  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    DOTNode* source = (*lit)->source_;
    DOTNode* target = (*lit)->target_;
    std::string n1 = std::to_string(source->id_);
    if (source->pseudo_) {
      n1 += " (p)";
    }
    std::string n2 = std::to_string(target->id_);
    if (target->pseudo_) {
      n2 += " (p)";
    }
    std::string nodes = n1 + "," + n2;
    ANTAuxiliary::log(nodes);
    std::vector<double> c1 = source->coordinates_;
    std::string coords1 = std::to_string(c1[0]) + "," + std::to_string(c1[1]) + "," + std::to_string(c1[2]);
    std::vector<double> c2 = target->coordinates_;
    std::string coords2 = std::to_string(c2[0]) + "," + std::to_string(c2[1]) + "," + std::to_string(c2[2]);
    std::string coords = coords1 + "\t" + coords2;
    ANTAuxiliary::log(coords);
  }
  msg = " == End of scaffold routing ==";
  ANTAuxiliary::log(msg);
}

void DASDaedalus::Set2DPositions(ANTPart& origami) {
  /**
  * Way of setting 2D coordinates is taken from DASCadnano
  * 2D position not working.
  */
  //Edges edges = origami.GetEdges();
  //const double dist_he = 0.5; // nm
  //const double dist_edges = 1.5; // nm
  //const double inc_x = ANTConstants::BP_RISE;

  //int e_id = 0.0;
  //std::map<ANTEdge*, int> edge_ids; // assign edge ids on the fly

  //for (auto &e : edges) {
  //  auto he1 = e->halfEdge_;

  //  BaseSegment* bs = firstBasesHe_.at(he1);
  //  do {
  //    ANTNucleotide* nt = bs->nt_;
  //    SBPosition3 pos = SBPosition3();
  //    pos[0] = SBQuantity::angstrom(bs->number_ * inc_x *10);
  //    pos[2] = SBQuantity::angstrom(e_id * dist_edges *10);
  //    nt->SetPosition2D(pos);
  //    ANTNucleotide* nt_pair = nt->pair_;
  //    if (nt_pair != nullptr) {
  //      // nt_pair should never be null for daedalus
  //      // if set for debugging purposes
  //      pos[0] = SBQuantity::angstrom(bs->number_ * inc_x * 10);
  //      pos[2] += SBQuantity::angstrom(dist_he * 10);
  //      nt_pair->SetPosition2D(pos);
  //    }
  //    bs = bs->next_;
  //  } while (bs->halfEdge_ == he1);
  //  ++e_id;
  //}
}

void DASDaedalus::Set1DPositions(ANTPart & origami) {
  /**
  * Way of setting 1D and 2D coordinates is taken from DASCadnano
  */
  auto strands = origami.GetSingleStrands();

  for (auto & s : strands) {
    ANTSingleStrand* st = s.second;
    double x = 0.0;
    for (auto &n : st->nucleotides_) {
      ANTNucleotide* nt = n.second;
      SBPosition3 pos = SBPosition3();
      pos[0] = SBQuantity::nanometer(st->id_);
      pos[2] = SBQuantity::nanometer(nt->id_ / 2.0f);
      nt->SetPosition1D(pos);
    }
  }
}

int DASDaedalus::CalculateEdgeSize(SBQuantity::length nmLength) {
  SBQuantity::dimensionless zSB = nmLength / SBQuantity::nanometer(ANTConstants::BP_RISE);
  std::ldiv_t div;
  div = std::div(long(zSB.getValue()), long(10.5));
  int sz = round(div.quot * 10.5);
  if (sz < 31) sz = 31;
  return sz;
}

template <typename T>
void DASDaedalus::OutputGraph(T g, std::string filename) {
  std::ofstream dotfile(filename.c_str());
  boost::write_graphviz(dotfile, g);
}

DOTLink::DOTLink(const DOTLink & other) {
  *this = other;
}

DOTLink::~DOTLink() {
  delete source_;
  delete target_;
}

DOTLink & DOTLink::operator=(const DOTLink & other) {
  if (this != &other) {
    id_ = other.id_;
    halfEdge_ = other.halfEdge_;
    directed_ = other.directed_;
    split_ = other.split_;
    firstBase_ = other.firstBase_;
    lastBase_ = other.lastBase_;
    bp_ = other.bp_;
    source_ = new DOTNode(*other.source_);
    target_ = new DOTNode(*other.target_);
  }
  return *this;
}

DOTNode::DOTNode(const DOTNode & other) {
  *this = other;
}

DOTNode & DOTNode::operator=(const DOTNode & other) {
  if (this != &other) {
    coordinates_ = other.coordinates_;
    id_ = other.id_;
    vertex_ = other.vertex_;
    edge_ = other.edge_;
    pseudo_ = other.pseudo_;
  }
  return *this;
}
