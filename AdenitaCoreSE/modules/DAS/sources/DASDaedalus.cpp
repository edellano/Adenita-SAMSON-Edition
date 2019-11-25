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

ADNPointer<ADNPart> DASDaedalus::ApplyAlgorithm(std::string seq, std::string filename, bool center) {

  // Create DASPolyhedron
  DASPolyhedron& p = DASPolyhedron(filename);
  p.Center(SBPosition3());
  
  ADNPointer<ADNPart> daedalus_part = ApplyAlgorithm(seq, p, center);

  return daedalus_part;
}

ADNPointer<ADNPart> DASDaedalus::ApplyAlgorithm(std::string seq, DASPolyhedron & p, bool center, bool editor) {

  ADNPointer<ADNPart> daedalus_part = new ADNPart();
  auto fig = p;
  if (center) fig.Center(SBPosition3());

  // Set edge lengths in base pairs and generate the scaffold
  SetEdgeBps(min_edge_length_, daedalus_part, fig);
  
  CreateLinkGraphFromMesh(daedalus_part, fig);
  SEConfig& config = SEConfig::GetInstance();
  if (!config.custom_mesh_model) {
    if (editor) SetVerticesPositions(daedalus_part, fig, center);
    InitEdgeMap2(daedalus_part, fig);
  }
  else {
    SetVerticesPositions(daedalus_part, fig, center);
    InitEdgeMap(daedalus_part, fig);
  }

  int r_length = RoutingLength(bpLengths_);
  if (r_length > seq.size()) {
    // we should display here a message in SAMSON
    std::string msg = "Daedalus module > Selected sequence too short for targeted figure, including random nucleotides";
    ADNLogger::Log(msg);
    // fulfill with NNN bases (this should be optional)
    int d = r_length - boost::numeric_cast<int>(seq.size());
    std::string ap = "";
    std::string base = "N";
    for (int i = 0; i < d; ++i) {
      ap += base;
    }
    seq += ap;
  }

  ADNPointer<ADNSingleStrand> scaff = new ADNSingleStrand();
  scaff->SetName("Scaffold");
  scaff->IsScaffold(true);
  chains_.insert(std::make_pair(0, scaff));

  // Create undirected graph from figure
  UndirectedGraph u_graph = GenerateUndirectedGraph();
  // Compute MST
  LinkList mst = MinimumSpanningTree(u_graph);
  // Split edges not in MST
  SplitEdges(mst, daedalus_part, fig);
  // Add Pseudo-nodes
  AddPseudoNodes(fig);
  // LogLinkGraph();
  UndirectedGraph u_graph_pseudo = GenerateUndirectedGraph();
  // Create directed graph
  DirectedGraph d_graph = GenerateDirectedGraph(u_graph_pseudo, daedalus_part);
  //LogLinkGraph();
  // Route scaffold
  RouteScaffold(daedalus_part, scaff, seq, r_length);
  // Create staple objects
  CreateEdgeStaples(daedalus_part);
  CreateVertexStaples(daedalus_part, fig);

  for (auto &p : chains_) {
    daedalus_part->RegisterSingleStrand(p.second());
  }

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

DirectedGraph DASDaedalus::GenerateDirectedGraph(const UndirectedGraph &u_graph, ADNPointer<ADNPart> origami) {
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

    ADNPointer<ADNBaseSegment> bs = firstBasesHe_.at(link->halfEdge_);
    link->firstBase_ = bs;
    if (link->split_) {
      DOTNode* s = link->source_;
      if (s->pseudo_) {
        // we need to advance it the other part of the edge
        int adv = bpLengths_.at(link->halfEdge_->edge_) - link->bp_;
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

LinkList DASDaedalus::MinimumSpanningTree(const UndirectedGraph &u_graph) {
  //Path prim_path;
  LinkList prim_path;
  std::vector<Node<UndirectedGraph>> mst_path_(boost::num_vertices(u_graph));
  boost::prim_minimum_spanning_tree(u_graph, &mst_path_[0], boost::weight_map(get(&EdgeProperty::weight_, u_graph)));

  std::size_t plength = mst_path_.size();
  for (std::size_t i = 0; i < plength; ++i) {
    if (mst_path_[i] != i) {
      Node<UndirectedGraph> v = boost::vertex(i, u_graph);
      Edge<UndirectedGraph> e = boost::edge(v, mst_path_[i], u_graph).first;
      DOTLink* link = u_graph[e].link_;
      prim_path.push_back(link);
    }
  }
  return prim_path;
}

void DASDaedalus::SplitEdges(const LinkList &mst, ADNPointer<ADNPart> origami, DASPolyhedron &figure) {

  LinkList links_to_remove;
  std::map<std::pair<DOTNode*, DOTNode*>, int> links_to_add;
  int id = nodes_.rbegin()->first + 1;

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    if (std::find(mst.begin(), mst.end(), *lit) == mst.end()) {
      DOTNode* u = (*lit)->source_;
      DOTNode* v = (*lit)->target_;
      DASEdge* edge = (*lit)->halfEdge_->edge_;
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
      int e_len = bpLengths_.at(edge);
      // calculate sizes
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

void DASDaedalus::AddPseudoNodes(DASPolyhedron &figure) {

  std::map<int, DASVertex*> vertices = figure.GetVertices();
  Faces faces = figure.GetFaces();

  int new_id = 0;
  int l_id = 0;
  NodeList new_pseudos;
  NodeList new_nodes;
  LinkList new_link_list;

  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
    // keep track of original DOTNode and old
    std::map<int, DOTNode*> node_relation;
    DASHalfEdge* begin = (*fit)->halfEdge_;
    DASHalfEdge* he = begin;
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
      DASVertex* v = n->vertex_;
      if (n->pseudo_ && DASPolyhedron::IsInFace(v, *fit)) {
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
      if (DASPolyhedron::IsInFace(v->vertex_, *fit) && DASPolyhedron::IsInFace(w->vertex_, *fit)) {
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

void DASDaedalus::CreateEdgeStaples(ADNPointer<ADNPart> origami) {

  unsigned int c_id = chains_.rbegin()->first + 1;
  std::vector<DASEdge*> stapled_edges;

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    DASEdge* edge = (*lit)->halfEdge_->edge_;
    int length = bpLengths_.at(edge);
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
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, staple_length, staple_length);
          start += staple_length;
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
        }
        if (length_extra_staple > 0) {
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + crossover_staple_span_);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, crossover_staple_span_, crossover_staple_span_);
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
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + crossover_staple_span_);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, crossover_staple_span_, crossover_staple_span_);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          start += crossover_staple_span_;
        }
        for (int i = 0; i < num_staples_first; ++i) {
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, staple_length, staple_length);
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
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + span_pos);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          bs_start = MoveBackBaseSegment(FindBaseSegmentPair(origami, (*lit)->firstBase_), start + span_neg);
          ADNPointer<ADNSingleStrand> chain2 = CreateEdgeChain(origami, bs_start, c_id, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain2));
          ++c_id;
          start += span_pos + span_neg;
        }
        else {
          span_pos = 10;
          span_neg = 10;
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + span_pos);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, span_pos, span_neg);
          chains_.insert(std::make_pair(c_id, chain));
          ++c_id;
          start += span_pos;
        }
        // Remaining staples
        for (int i = 0; i < num_staples_second; ++i) {
          ADNPointer<ADNBaseSegment> bs_start = AdvanceBaseSegment((*lit)->firstBase_, start + staple_length);
          ADNPointer<ADNSingleStrand> chain = CreateEdgeChain(origami, bs_start, c_id, staple_length, staple_length);
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

void DASDaedalus::CreateVertexStaples(ADNPointer<ADNPart> origami, DASPolyhedron &figure) {

  std::map<int, DASVertex*> vertices = figure.GetVertices();
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

    std::vector<DASHalfEdge*> edges;
    DASHalfEdge* begin = vit->second->halfEdge_;
    DASHalfEdge* he = begin;
    do {
      edges.push_back(he);
      edges.push_back(he->pair_);
      he = he->pair_->next_;
    } while (he != begin);

    // move elements in ps so we start creating the vertex away from polyT
    std::rotate(edges.begin(), edges.begin() + 1, edges.end());

    unsigned int start = 0;
    for (int i = 0; i < a; ++i) {
      // 52nt span 2 edges = 4 half edges
      unsigned int sz = 4;
      std::vector<DASHalfEdge*> ps(edges.begin() + start, edges.begin() + start + sz);
      ADNPointer<ADNSingleStrand> chain = CreateVertexChain(origami, c_id, ps, bpLengths_);
      chains_.insert(std::make_pair(c_id, chain));
      ++c_id;
      start += sz;
    }
    for (int i = 0; i < b; ++i) {
      // 78nt span 3 edges = 6 half edges
      unsigned int sz = 6;
      std::vector<DASHalfEdge*> ps(edges.begin() + start, edges.begin() + start + sz);
      ADNPointer<ADNSingleStrand> chain = CreateVertexChain(origami, c_id, ps, bpLengths_);
      chains_.insert(std::make_pair(c_id, chain));
      ++c_id;
      start += sz;
    }
  }
}

void DASDaedalus::InitEdgeMap(ADNPointer<ADNPart> origami, DASPolyhedron &fig) {
  unsigned int b_id = 0;
  int j_id = 0;
  Faces faces = fig.GetFaces();

  SEConfig& config = SEConfig::GetInstance();
  double dh_dist = config.dh_dist + ADNConstants::DH_DIAMETER * 10;  // angstroms

  std::map<DASHalfEdge*, SBPosition3> vertexPositions = GetVertexPositions(fig);

  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
    DASHalfEdge* begin = (*fit)->halfEdge_;
    DASHalfEdge* he = begin;
    SBVector3 norm = GetPolygonNorm(*fit);
    do {
      // iterate over face edges
      DASVertex* s = he->source_;
      DASVertex* t = he->pair_->source_;
      SBVector3 dir = (t->GetSBPosition() - s->GetSBPosition()).normalizedVersion();
      SBVector3 u = SBCrossProduct(dir, norm);
      auto coords = vertexPositions[he];

      int l = bpLengths_.at(he->edge_) + 1; // +1 since we have apolyT region at the end

      ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
      ds->SetInitialTwistAngle(7 * ADNConstants::BP_ROT);  // necessary to fix crossovers
      origami->RegisterDoubleStrand(ds);

      for (int i = 0; i < l; ++i) {
        // every step is a ADNBaseSegment
        ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment(CellType::BasePair);

        bs->SetPosition(coords);
        bs->SetE1(ADNAuxiliary::SBVectorToUblasVector(norm));
        bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(dir));
        bs->SetE2(ADNAuxiliary::SBVectorToUblasVector(u));
        bs->SetNumber(i);

        if (i == l - 1) {
          // this region is polyT
          ADNPointer<ADNLoopPair> cell = new ADNLoopPair();
          bs->SetCell(cell());
        }

        // match first segment with corresponding halfedge
        if (i == 0) {
          firstBasesHe_.insert(std::make_pair(he, bs));
        }

        origami->RegisterBaseSegmentEnd(ds, bs);

        coords += SBQuantity::angstrom(BP_RISE)*dir;
      }
      he = he->next_;
    } while (he != begin);
  }
  // Pair base segments and create double helices

  // don't register ds twice!
  std::map<DASHalfEdge*, ADNPointer<ADNDoubleStrand>> registeredDs;
  for (auto it = firstBasesHe_.begin(); it != firstBasesHe_.end(); ++it) {
    DASHalfEdge* fhe = it->first;
    DASHalfEdge* she = fhe->pair_;
    ADNPointer<ADNBaseSegment> begin = it->second;
    ADNPointer<ADNBaseSegment> bs = begin;
    DASEdge* e = fhe->edge_;
    int length = bpLengths_.at(e) + 1;  // to include polyTs
    ADNPointer<ADNBaseSegment> bs_pair = AdvanceBaseSegment(firstBasesHe_.at(she), length - 2); // since the last one is a PolyT

    for (int i = 0; i < length - 1; ++i) {
      auto bsId = origami->GetBaseSegmentIndex(bs);
      auto bsPairId = origami->GetBaseSegmentIndex(bs_pair);
      bsPairs_.insert(std::make_pair(bsId, bsPairId));
      bsPairs_.insert(std::make_pair(bsPairId, bsId));
      bs = bs->GetNext();
      bs_pair = bs_pair->GetPrev();
    }
  }
}

void DASDaedalus::InitEdgeMap2(ADNPointer<ADNPart> origami, DASPolyhedron & fig)
{
  double pi = atan(1) * 4;
  // get the span distance for the beginning of half-edges w.r.t. vertices
  std::map<DASVertex*, double> vertex_span;
  Vertices vertices = fig.GetVertices();
  for (auto &pair : vertices) {
    DASVertex* vertex = pair.second;
    DASHalfEdge* start = vertex->halfEdge_;
    auto he = start;
    auto he_next = start->pair_->next_;
    double theta_max = -1;
    double theta_tot = 0.0;
    do {
      SBPosition3 v = he->source_->position_ - he->pair_->source_->position_;
      SBPosition3 w = he_next->source_->position_ - he_next->pair_->source_->position_;
      double theta = acos( ADNVectorMath::SBInnerProduct(v.normalizedVersion(), w.normalizedVersion()) );
      if (theta > theta_max) theta_max = theta;
      theta_tot += theta;
      he = he_next;
      he_next = he->pair_->next_;
    } while (he != start);

    double r = ADNConstants::DH_DIAMETER / tan(theta_max * pi / theta_tot);
    double s = (2 * pi / theta_tot) * r;
    if (theta_tot > 2 * pi) s = r;
    vertex_span[vertex] = s;
  }

  //// scale entire model
  //std::vector<DASVertex*> done_vertex;
  //Vertices originalVertices = fig.GetOriginalVertices();
  //DASVertex* orig_v = nullptr;
  //DASVertex* v = vertices.begin()->second;
  //int count = 0;
  //while (done_vertex.size() < originalVertices.size()) {
  //  orig_v = originalVertices.at(v->id_);
  //  DASHalfEdge* begin = v->halfEdge_;
  //  DASHalfEdge* he = begin;
  //  do {
  //    SBPosition3 c_v = v->GetSBPosition();
  //    SBPosition3 orig_c_v = orig_v->GetSBPosition();
  //    DASVertex* next_v = he->pair_->source_;
  //    DASVertex* orig_next_v = originalVertices.at(next_v->id_);
  //    DASEdge* edge = DASPolyhedron::GetEdgeByVertices(v, next_v);
  //    int bp_length = bpLengths_.at(edge);
  //    double length = bp_length * BP_RISE + vertex_span[v] * 10; // angstrom

  //    // modify next vertex
  //    if (std::find(done_vertex.begin(), done_vertex.end(), orig_next_v) == done_vertex.end()) {
  //      SBPosition3 orig_c_next = orig_next_v->GetSBPosition();
  //      SBVector3 dir = (orig_c_next - orig_c_v).normalizedVersion();
  //      SBQuantity::angstrom l(length);

  //      SBPosition3 new_c_next = c_v + l * dir;

  //      next_v->SetCoordinates(new_c_next);
  //      done_vertex.push_back(orig_next_v);
  //    }

  //    he = he->pair_->next_;
  //  } while (he != begin);
  //  int nId = done_vertex.at(count)->id_;
  //  v = vertices.at(nId);
  //  ++count;
  //}

  //// correct vertices
  //std::map<DASVertex*, SBPosition3> new_positions;
  //for (auto &edge : fig.GetEdges()) {
  //  auto v1 = edge->halfEdge_->source_;
  //  auto v2 = edge->halfEdge_->pair_->source_;
  //  SBVector3 vDir = (v2->position_ - v1->position_).normalizedVersion();
  //  auto edgeCenter = (v1->position_ + v2->position_) * 0.5;
  //  auto edgeLength = bpLengths_[edge];
  //  auto xtr1 = vertex_span[v1];
  //  auto xtr2 = vertex_span[v2];

  //  double length = edgeLength * ADNConstants::BP_RISE;
  //  auto v1New = v1->position_ + SBQuantity::nanometer(xtr1) * vDir;
  //  auto v2New = v2->position_ - SBQuantity::nanometer(xtr2) * vDir;
  //  new_positions[v1] = v1New;
  //  new_positions[v2] = v2New;
  //}

  // create base segments
  unsigned int b_id = 0;
  int j_id = 0;
  Faces faces = fig.GetFaces();

  SEConfig& config = SEConfig::GetInstance();
  auto dh_dist = SBQuantity::nanometer(ADNConstants::DH_DIAMETER * 0.5);

  for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
    DASHalfEdge* begin = (*fit)->halfEdge_;
    DASHalfEdge* he = begin;
    SBVector3 norm = GetPolygonNorm(*fit);
    do {
      // iterate over face half edges
      DASVertex* s = he->source_;
      DASVertex* t = he->pair_->source_;
      DASPolygon* fAdj = he->pair_->left_;
      SBVector3 adjNorm = GetPolygonNorm(fAdj);
      SBPosition3 sPos = s->GetSBPosition();
      SBPosition3 tPos = t->GetSBPosition();
      auto origSize = (tPos - sPos).norm();
      SBVector3 dir = (tPos - sPos).normalizedVersion();
      SBVector3 e1 = (adjNorm + norm).normalizedVersion();
      SBVector3 e2 = SBCrossProduct(dir, e1);

      auto xtr1 = SBQuantity::nanometer(vertex_span[s]);
      auto xtr2 = SBQuantity::nanometer(vertex_span[t]);
      auto length = bpLengths_[he->edge_] * SBQuantity::nanometer(ADNConstants::BP_RISE);
      // scale the edge to wished length + extra space
      SBPosition3 coords = sPos * (length + xtr1 + xtr2) / (tPos - sPos).norm();
      // move away from original positions
      coords += xtr1 * dir;
      // shift from pair
      coords -= dh_dist * e2;
      //auto coords = sPos - dh_dist * e2;

      int l = bpLengths_.at(he->edge_) + 1; // +1 since we have a polyT region at the end

      ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
      ds->SetInitialTwistAngle(7 * ADNConstants::BP_ROT);  // necessary to fix crossovers
      origami->RegisterDoubleStrand(ds);

      for (int i = 0; i < l; ++i) {
        // every step is a ADNBaseSegment
        ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment(CellType::BasePair);

        bs->SetPosition(coords);
        bs->SetE1(ADNAuxiliary::SBVectorToUblasVector(norm));
        bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(dir));
        bs->SetE2(ADNAuxiliary::SBVectorToUblasVector(e2));
        bs->SetNumber(i);

        if (i == l - 1) {
          // this region is polyT
          ADNPointer<ADNLoopPair> cell = new ADNLoopPair();
          bs->SetCell(cell());
        }

        // match first segment with corresponding halfedge
        if (i == 0) {
          firstBasesHe_.insert(std::make_pair(he, bs));
        }

        origami->RegisterBaseSegmentEnd(ds, bs);

        coords += SBQuantity::angstrom(BP_RISE)*dir;
      }
      he = he->next_;
    } while (he != begin);
  }
  // Pair base segments and create double helices

  // don't register ds twice!
  std::map<DASHalfEdge*, ADNPointer<ADNDoubleStrand>> registeredDs;
  for (auto it = firstBasesHe_.begin(); it != firstBasesHe_.end(); ++it) {
    DASHalfEdge* fhe = it->first;
    DASHalfEdge* she = fhe->pair_;
    ADNPointer<ADNBaseSegment> begin = it->second;
    ADNPointer<ADNBaseSegment> bs = begin;
    DASEdge* e = fhe->edge_;
    int length = bpLengths_.at(e) + 1;  // to include polyTs
    ADNPointer<ADNBaseSegment> bs_pair = AdvanceBaseSegment(firstBasesHe_.at(she), length - 2); // since the last one is a PolyT

    for (int i = 0; i < length - 1; ++i) {
      auto bsId = origami->GetBaseSegmentIndex(bs);
      auto bsPairId = origami->GetBaseSegmentIndex(bs_pair);
      bsPairs_.insert(std::make_pair(bsId, bsPairId));
      bsPairs_.insert(std::make_pair(bsPairId, bsId));
      bs = bs->GetNext();
      bs_pair = bs_pair->GetPrev();
    }
  }
}

void DASDaedalus::CreateLinkGraphFromMesh(ADNPointer<ADNPart> origami, DASPolyhedron &figure) {

  Vertices vertices = figure.GetVertices();
  Edges edges = figure.GetEdges();
  for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
    DOTNode* node = new DOTNode();
    DASVertex* vertex = vit->second;
    node->id_ = vertex->id_;
    node->coordinates_ = vertex->GetVectorCoordinates();
    node->vertex_ = vertex;
    nodes_.insert(std::make_pair(node->id_, node));
  }
  int id = 0;
  for (auto eit = edges.begin(); eit != edges.end(); ++eit) {
    DOTLink* link = new DOTLink();
    DASHalfEdge* he = (*eit)->halfEdge_;
    link->directed_ = false;
    link->halfEdge_ = he;
    link->id_ = id;
    ++id;
    link->source_ = nodes_[he->source_->id_];
    link->target_ = nodes_[he->pair_->source_->id_];
    link->bp_ = bpLengths_.at(he->edge_);
    linkGraph_.push_back(link);
  }
}

void DASDaedalus::AddNode(DOTNode* node) {
  nodes_.insert(std::make_pair(node->id_, node));
}

void DASDaedalus::RemoveLink(DOTLink* link) {
  linkGraph_.erase(std::find(linkGraph_.begin(), linkGraph_.end(), link));
}

DOTLink* DASDaedalus::AddLink(std::pair<DOTNode*, DOTNode*> ab, int bp, DASPolyhedron &fig) {
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

void DASDaedalus::SetEdgeBps(int min_edge_bp, ADNPointer<ADNPart> part, DASPolyhedron &fig) {
  float min_length = fig.MinimumEdgeLength().second;
  float rel = min_edge_bp / min_length;

  Edges edges = fig.GetEdges();

  for (auto it = edges.begin(); it != edges.end(); ++it) {
    double l = DASPolyhedron::CalculateEdgeLength(*it);
    double bp_l = l*rel;
    int quot;
    remquo(bp_l, 10.5, &quot);
    int bp_length = int(std::floor(float(quot * 10.5)));
    bpLengths_.insert(std::make_pair(*it, bp_length));
  }
}

void DASDaedalus::RouteScaffold(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> scaff, std::string seq, int routing_length) {

  unsigned int nt_id = 0;

  std::string used_seq = seq.substr(0, routing_length);
  bool left = true;

  for (auto lit = linkGraph_.begin(); lit != linkGraph_.end(); ++lit) {
    ADNPointer<ADNBaseSegment> bs = (*lit)->firstBase_;
    for (int i = 0; i < (*lit)->bp_; ++i) {
      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      part->RegisterNucleotideThreePrime(scaff, nt);
      nt->SetType(ADNModel::ResidueNameToType(used_seq[nt_id]));
      nt->SetPosition(bs->GetPosition());
      nt->SetBackbonePosition(bs->GetPosition());
      nt->SetSidechainPosition(bs->GetPosition());
      nt->SetBaseSegment(bs);

      ADNPointer<ADNCell> cell = bs->GetCell();
      if (cell->GetType() == BasePair) {
        // scaffold only goes through dsDNA regions
        ADNPointer<ADNBasePair> bp_cell = static_cast<ADNBasePair*>(cell());
        if (left) {
          bp_cell->SetLeftNucleotide(nt);
        }
        else {
          bp_cell->SetRightNucleotide(nt);
        }
      }
      bs = bs->GetNext();
    }
    //left *= false;  // we don't need to change to right because double strands have opposite directions
  }

  // advance beginning of scaffold
  DASEdge* edge = (*linkGraph_.begin())->halfEdge_->edge_;
  float m = bpLengths_[edge] * 0.5;
  int idx1 = edge->halfEdge_->source_->id_;
  int idx2 = edge->halfEdge_->pair_->source_->id_;
  int pos = ceil(m);
  if (idx1 < idx2) pos = floor(m);
  ADNPointer<ADNNucleotide> res = AdvanceNucleotide(scaff->GetFivePrime(), pos);
  ADNBasicOperations::SetStart(res);
}

ADNPointer<ADNBaseSegment> DASDaedalus::AdvanceBaseSegment(ADNPointer<ADNBaseSegment> bs, int pos) {
  ADNPointer<ADNBaseSegment> res = bs->GetNext();
  for (int i = 0; i < pos - 1; ++i) {
    res = res->GetNext();
  }
  return res;
}

ADNPointer<ADNBaseSegment> DASDaedalus::MoveBackBaseSegment(ADNPointer<ADNBaseSegment> bs, int pos) {
  ADNPointer<ADNBaseSegment> res = bs->GetPrev();
  for (int i = 0; i < pos - 1; ++i) {
    res = res->GetPrev();
  }
  return res;
}

ADNPointer<ADNNucleotide> DASDaedalus::AdvanceNucleotide(ADNPointer<ADNNucleotide> nt, int pos)
{
  ADNPointer<ADNNucleotide> res = nt;
  for (int i = 0; i < pos - 1; ++i) {
    res = res->GetNext();
  }
  return res;
}

ADNPointer<ADNSingleStrand> DASDaedalus::CreateEdgeChain(ADNPointer<ADNPart> origami, ADNPointer<ADNBaseSegment> bs, int c_id, int pos_span, int neg_span) {

  ADNPointer<ADNSingleStrand> chain = new ADNSingleStrand();
  chain->SetName("Edge Staple " + std::to_string(c_id));

  unsigned int nt_id = 0;
  bool fst = true;
 
  for (int i = 0; i < pos_span; ++i) {
    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    nt->Init();
    origami->RegisterNucleotideThreePrime(chain, nt);

    ADNPointer<ADNCell> cell = bs->GetCell();
    ADNPointer<ADNNucleotide> ntOld = nullptr;
    if (cell->GetType() == BasePair) {
      // edge staples only goes through dsDNA regions
      ADNPointer<ADNBasePair> bp_cell = static_cast<ADNBasePair*>(cell());
      bp_cell->SetRemainingNucleotide(nt);
      ntOld = nt->GetPair();
    }

    if (ntOld != nullptr) {
      nt->SetType(ADNModel::GetComplementaryBase(ntOld->GetType()));
    }
    nt->SetPosition(bs->GetPosition());
    nt->SetBackbonePosition(bs->GetPosition());
    nt->SetSidechainPosition(bs->GetPosition());
    nt->SetBaseSegment(bs);
    bs = bs->GetPrev();
  }
  // move base segment to other edge
  bs = FindBaseSegmentPair(origami, bs);

  for (int i = 0; i < neg_span; ++i) {
    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    nt->Init();
    origami->RegisterNucleotideThreePrime(chain, nt);

    ADNPointer<ADNCell> cell = bs->GetCell();
    ADNPointer<ADNNucleotide> ntOld = nullptr;
    if (cell->GetType() == BasePair) {
      // edge staples only goes through dsDNA regions
      ADNPointer<ADNBasePair> bp_cell = static_cast<ADNBasePair*>(cell());
      bp_cell->SetRemainingNucleotide(nt);
      ntOld = nt->GetPair();
    }
    if (ntOld != nullptr) {
      nt->SetType(ADNModel::GetComplementaryBase(ntOld->GetType()));
    }
    nt->SetPosition(bs->GetPosition());
    nt->SetBackbonePosition(bs->GetPosition());
    nt->SetSidechainPosition(bs->GetPosition());
    nt->SetBaseSegment(bs);

    bs = bs->GetPrev();
  }

  return chain;
}

ADNPointer<ADNSingleStrand> DASDaedalus::CreateVertexChain(ADNPointer<ADNPart> part, int c_id, std::vector<DASHalfEdge*>ps, EdgeBps &lengths) {
  ADNPointer<ADNSingleStrand> chain = new ADNSingleStrand();
  chain->SetName("Vertex Staple " + std::to_string(c_id));

  bool beg = false;
  int len = 0;
  ADNPointer<ADNNucleotide> prev_nt = nullptr;
  int count = 1;

  for (auto pit = ps.begin(); pit != ps.end(); ++pit) {
    ADNPointer<ADNBaseSegment> bs = firstBasesHe_.at(*pit);
    // vertex of lower index gets end, vertex of higher index gets beginning
    if (beg) {
      len = vertex_staple_span_end_;
      beg = false;
    }
    else {
      len = vertex_staple_span_start_;
      bs = AdvanceBaseSegment(bs, lengths.at((*pit)->edge_) - len);
      beg = true;
    }
    for (int i = 0; i < len; ++i) {
      // this are the ADNBasePair nts
      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      part->RegisterNucleotideFivePrime(chain, nt);

      ADNPointer<ADNCell> cell = bs->GetCell();
      nt->SetPosition(bs->GetPosition());
      nt->SetBackbonePosition(bs->GetPosition());
      nt->SetSidechainPosition(bs->GetPosition());

      ADNPointer<ADNNucleotide> ntOld = nullptr;
      if (cell->GetType() == BasePair) {
        // edge staples only goes through dsDNA regions
        ADNPointer<ADNBasePair> bp_cell = static_cast<ADNBasePair*>(cell());
        bp_cell->SetRemainingNucleotide(nt);
        ntOld = nt->GetPair();
      }
      if (ntOld != nullptr) {
        nt->SetType(ADNModel::GetComplementaryBase(ntOld->GetType()));
      }
      
      prev_nt = nt;
      nt->SetBaseSegment(bs);

      bs = bs->GetNext();
    }
    if (count % 2 != 0) {
      ADNPointer<ADNCell> cell = bs->GetCell();
      if (cell->GetType() != LoopPair) {
        std::string msg = "PolyT region not reached";
        ADNLogger::LogDebug(msg);
      }
      else {
        ADNPointer<ADNLoopPair> loop_cell = static_cast<ADNLoopPair*>(cell());
        std::string seq = "";
        for (int i = 0; i < num_poly_t_; ++i) {
          seq += "T";
        }
        ADNPointer<ADNLoop> loop = DASCreator::CreateLoop(chain, prev_nt, seq, part);
        loop->SetBaseSegment(bs);
        if (prev_nt->IsLeft()) {
          loop_cell->SetLeftLoop(loop);
        }
        else if (prev_nt->IsRight()) {
          loop_cell->SetRightLoop(loop);
        }
        prev_nt = loop->GetStart();  // we move backwards
        bs = bs->GetNext();
      }
    }

    ++count;
  }

  ADNPointer<ADNNucleotide> start = AdvanceNucleotide(chain->GetFivePrime(), 7);
  ADNBasicOperations::SetStart(start);  // we set this as start to match original Daedalus algorithm
  return chain;
}

void DASDaedalus::SetVerticesPositions(ADNPointer<ADNPart> origami, DASPolyhedron &fig, bool center) {
  std::vector<DASVertex*> done_vertex;
  Vertices vertices = fig.GetVertices();
  Vertices originalVertices = fig.GetOriginalVertices();
  SBPosition3 cm = SBPosition3();
  DASVertex* orig_v = nullptr;
  DASVertex* v = vertices.begin()->second;
  int count = 0;
  //DASVertex* orig_v = originalVertices.at(v->id_);
  //done_vertex.push_back(orig_v);
  //cm += v->GetSBPosition();
  while (done_vertex.size() < originalVertices.size()) {
    orig_v = originalVertices.at(v->id_);
    DASHalfEdge* begin = v->halfEdge_;
    DASHalfEdge* he = begin;
    do {
      SBPosition3 c_v = v->GetSBPosition();
      SBPosition3 orig_c_v = orig_v->GetSBPosition();
      DASVertex* next_v = he->pair_->source_;
      DASVertex* orig_next_v = originalVertices.at(next_v->id_);
      DASEdge* edge = DASPolyhedron::GetEdgeByVertices(v, next_v);
      int bp_length = bpLengths_.at(edge);
      double length = bp_length * BP_RISE; // angstrom

      // modify next vertex
      if (std::find(done_vertex.begin(), done_vertex.end(), orig_next_v) == done_vertex.end()) {
        SBPosition3 orig_c_next = orig_next_v->GetSBPosition();
        SBVector3 dir = (orig_c_next - orig_c_v).normalizedVersion();
        SBQuantity::angstrom l(length);

        SBPosition3 new_c_next = c_v + l * dir;

        next_v->SetCoordinates(new_c_next);
        cm += new_c_next;
        done_vertex.push_back(orig_next_v);
      }

      he = he->pair_->next_;
      //v = he->source_;
      //orig_v = originalVertices.at(v->id_);
    } while (he != begin);
    int nId = done_vertex.at(count)->id_;
    v = vertices.at(nId);
    ++count;
  }
  cm /= boost::numeric_cast<int>(vertices.size());
  // center
  if (center) {
    for (auto vit = vertices.begin(); vit != vertices.end(); ++vit) {
      SBPosition3 c = vit->second->GetSBPosition();
      c = c - cm;
      vit->second->SetCoordinates(c);
    }
  }
}

void DASDaedalus::LogEdgeMap(ADNPointer<ADNPart> origami) {
  std::vector<ADNPointer<ADNBaseSegment>> bases;
  auto edgeMap = origami->GetBaseSegments();
  // log the edge map as n x 3 matrix
  std::string msg = "Daedalus module > Recording edge map";
  ADNLogger::LogDebug(msg);
  SB_FOR(SBStructuralNode* n, edgeMap) {
    ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(n);
    if (bsPairs_.find(bs->getNodeIndex()) == bsPairs_.end()) {
      bases.push_back(bs);
    }
  }
  msg = "Daedalus module > Unpaired bases";
  ADNLogger::LogDebug(msg);
  msg = "Daedalus module > ";
  for (auto vit = bases.begin(); vit != bases.end(); ++vit) {
    msg += std::to_string((*vit)->getNodeIndex()) + ",";
  }
  ADNLogger::LogDebug(msg);
  msg = "Daedalus module > End of unpaired bases";
  ADNLogger::LogDebug(msg);
  msg = "Daedalus module > End of edge map record";
  ADNLogger::LogDebug(msg);
}

SBVector3 DASDaedalus::SBCrossProduct(SBVector3 v, SBVector3 w) {
  return ADNVectorMath::SBCrossProduct(v, w);
}

double DASDaedalus::SBInnerProduct(SBVector3 v, SBVector3 w) {
  return ADNVectorMath::SBInnerProduct(v, w);
}

void DASDaedalus::LogLinkGraph() {
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
    std::vector<double> c1 = source->coordinates_;
    std::string coords1 = std::to_string(c1[0]) + "," + std::to_string(c1[1]) + "," + std::to_string(c1[2]);
    std::vector<double> c2 = target->coordinates_;
    std::string coords2 = std::to_string(c2[0]) + "," + std::to_string(c2[1]) + "," + std::to_string(c2[2]);
    std::string coords = coords1 + "\t" + coords2;
  }
}

SBVector3 DASDaedalus::GetPolygonNorm(DASPolygon * face)
{
  DASHalfEdge* he = face->halfEdge_;
  DASVertex* v1 = he->prev_->source_;
  DASVertex* v2 = he->source_;
  DASVertex* v3 = he->next_->source_;

  ublas::vector<double> p1 = ADNVectorMath::CreateBoostVector(v1->GetVectorCoordinates());
  ublas::vector<double> p2 = ADNVectorMath::CreateBoostVector(v2->GetVectorCoordinates());
  ublas::vector<double> p3 = ADNVectorMath::CreateBoostVector(v3->GetVectorCoordinates());

  ublas::vector<double> crs = ADNVectorMath::CrossProduct(p2 - p1, p3 - p2);
  crs /= ublas::norm_2(crs);
  std::vector<double> crss = ADNVectorMath::CreateStdVector(crs);

  SBVector3 w = SBVector3();
  w[0] = crss[0];
  w[1] = crss[1];
  w[2] = crss[2];
  return w;
}

ADNPointer<ADNBaseSegment> DASDaedalus::FindBaseSegmentPair(ADNPointer<ADNPart> origami, ADNPointer<ADNBaseSegment> bs)
{
  int idx = origami->GetBaseSegmentIndex(bs);
  int pairIdx = bsPairs_.at(idx);
  auto bases = origami->GetBaseSegments();

  return bases[pairIdx];
}

std::map<DASHalfEdge*, SBPosition3> DASDaedalus::GetVertexPositions(DASPolyhedron & fig)
{
  std::map<DASHalfEdge*, SBPosition3> vertexPositions;

  Faces faces = fig.GetFaces();

  SEConfig& config = SEConfig::GetInstance();
  double dh_dist = config.dh_dist + ADNConstants::DH_DIAMETER * 10;  // angstroms  

  std::vector<int> facesList(faces.size());
  std::iota(facesList.begin(), facesList.end(), 1);

  SBPosition3 cm = fig.GetCenter();
  SBQuantity::length start = SBQuantity::angstrom(dh_dist * 0.2);
  SBQuantity::length inc = SBQuantity::angstrom(dh_dist * 0.1);
  SBQuantity::length dist = SBQuantity::angstrom(dh_dist);
  SBQuantity::length tol = dist * 0.08;
  while (!facesList.empty()) {
    for (auto it = facesList.begin(); it != facesList.end();) {
      auto face = faces[*it-1];
      SBPosition3 faceCM = face->GetCenter();
      SBVector3 n = (faceCM - cm).normalizedVersion();
      int total_count = 0;
      int count = 0;
      DASHalfEdge* begin = face->halfEdge_;
      DASHalfEdge* he = begin;
      do {
        auto pos = he->source_->GetSBPosition();

        if (vertexPositions.find(he) == vertexPositions.end()) {
          vertexPositions[he] = pos;
        }
        else {
          // check distance before updating
          if (vertexPositions.find(he->pair_->next_) != vertexPositions.end()) {
            auto v = vertexPositions[he];
            auto w = vertexPositions[he->pair_->next_];
            SBQuantity::length d = (w - v).norm();
            if (ADNVectorMath::IsNearlyZero(abs((d - dist).getValue()), tol.getValue())) {
              ++count;
            }
            else {
              if (d > dist) {
                vertexPositions[he] -= inc * n;
                count++;
              }
              else {
                vertexPositions[he] += inc * n;
              }
            }
          }
        }
        
        he = he->next_;
        ++total_count;
      } while (he != begin);
      // delete if face is set
      if (count > total_count*0.5) {
        it = facesList.erase(it);
      }
      else {
        ++it;
      }
    }
  }
  
  return vertexPositions;
}

int DASDaedalus::CalculateEdgeSize(SBQuantity::length nmLength) {
  SBQuantity::dimensionless zSB = nmLength / SBQuantity::nanometer(ADNConstants::BP_RISE);
  std::ldiv_t div;
  div = std::div(long(zSB.getValue()), long(10.5));
  int sz = std::floor(div.quot * 10.5);
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
