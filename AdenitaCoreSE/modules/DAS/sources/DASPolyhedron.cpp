#include "DASPolyhedron.hpp"


DASHalfEdge::DASHalfEdge(const DASHalfEdge & other) {
  *this = other;
}

DASHalfEdge & DASHalfEdge::operator=(const DASHalfEdge & other) {
  if (this != &other) {
    id_ = other.id_;
  }
  return *this;
}

DASVertex::DASVertex(const DASVertex & w) {
  *this = w;
}

DASVertex & DASVertex::operator=(const DASVertex & other) {
  if (this != &other) {
    id_ = other.id_;
    position_ = other.position_;
  }
  return *this;
}

/* Copy constructor for ANTVertex
*/
DASVertex::DASVertex(DASVertex* w) {
  id_ = w->id_;
  halfEdge_ = w->halfEdge_;
  position_ = w->position_;
}

/* Destructor for ANTEdge
*/
DASEdge::~DASEdge() {
  // Remove all polygons conected to the edge
  DASPolygon* p1 = nullptr;
  if (halfEdge_->left_ != nullptr) p1 = halfEdge_->left_;
  DASPolygon* p2 = nullptr;
  if (halfEdge_->pair_->left_ != nullptr) p2 = halfEdge_->pair_->left_;
  if (p1 != nullptr) delete p1;
  if (p2 != nullptr) delete p2;
  if (halfEdge_->pair_ != nullptr) delete halfEdge_->pair_;
  delete halfEdge_;
}

DASEdge::DASEdge(const DASEdge & other) {
  *this = other;
}

DASEdge & DASEdge::operator=(const DASEdge & other) {
  if (this != &other) {
    // copy vertices only one vertex
    DASVertex* v1 = new DASVertex(other.halfEdge_->source_);
    // create half edges
    DASHalfEdge* he1 = new DASHalfEdge(*other.halfEdge_);
    he1->source_ = v1;
    he1->edge_ = this;
    halfEdge_ = he1;
  }
  return *this;
}

DASPolygon::DASPolygon(const DASPolygon & p) {
  *this = p;
}

/* Destructor for ANTPolygon
*/
DASPolygon::~DASPolygon() {
  DASHalfEdge* begin = halfEdge_;
  DASHalfEdge* he = begin;
  do {
    he->left_ = nullptr;
    he = he->next_;
  } while (he != begin);
}

DASPolygon & DASPolygon::operator=(const DASPolygon & other) {
  if (this != &other) {
    // copy all edges and link them
    DASHalfEdge* begin = other.halfEdge_;
    DASHalfEdge* he = begin;
    DASEdge* prev_e = nullptr;
    DASEdge* first_e = nullptr;
    DASEdge* e = nullptr;
    do {
      e = new DASEdge(*(he->edge_));
      e->halfEdge_->left_ = this;
      if (prev_e != nullptr) {
        e->halfEdge_->prev_ = prev_e->halfEdge_;
        prev_e->halfEdge_->next_ = e->halfEdge_;
      }
      else {
        first_e = e;
      }
      prev_e = e;
      he = he->next_;
    } while (he != begin);
    e->halfEdge_->next_ = first_e->halfEdge_;
    first_e->halfEdge_->prev_ = e->halfEdge_;
    halfEdge_ = first_e->halfEdge_;
  }
  return *this;
}

std::vector<double> DASVertex::GetVectorCoordinates() const {
  std::vector<double> coords = {
    position_[0].getValue() * 0.01,
    position_[1].getValue() * 0.01,
    position_[2].getValue() * 0.01,
  };
  return coords;
}

SBPosition3 DASVertex::GetSBPosition() const {
  return position_;
}

void DASVertex::SetCoordinates(SBPosition3 coordinates) {
  position_ = coordinates;
}

DASPolyhedron::DASPolyhedron(std::string filename) {
  LoadFromPLYFile(filename);
}

DASPolyhedron::DASPolyhedron(const DASPolyhedron &p) {
  *this = p;
}

/* Destructor */
DASPolyhedron::~DASPolyhedron() {

  // Delete all Faces - std::vector<ANTPolygon*>;
  for (auto & it : faces_) {
    delete it;
  } 

  // Delete all Edges - std::vector<ANTEdge*>;
  for (auto & it : edges_) {
    delete it;
  }

  // Delete vertices
  for (auto & it : vertices_) {
    delete it.second;
  }

  // Delete all Original Vertices - std::map<int, ANTVertex*>;  
  for (auto & it : originalVertices_) {
    delete it.second;
  }
  faces_.clear();
  edges_.clear();
  vertices_.clear();
  originalVertices_.clear();
}

DASPolyhedron & DASPolyhedron::operator=(const DASPolyhedron & p) {
  if (this != &p) {
    // Delete all info first
    for (auto & it : faces_) {
      delete it;
    }
    for (auto & it : edges_) {
      delete it;
    }
    for (auto & it : vertices_) {
      delete it.second;
    }
    for (auto & it : originalVertices_) {
      delete it.second;
    }
    faces_.clear();
    edges_.clear();
    vertices_.clear();
    originalVertices_.clear();
    // copy new info
    Faces p_faces = p.GetFaces();
    Vertices p_vertices = p.GetVertices();
    std::map<int, SBPosition3> vertices;
    for (auto & p_vertice : p_vertices) {
      vertices.insert(std::make_pair(p_vertice.first, p_vertice.second->GetSBPosition()));
    }
    std::map<int, std::vector<int>> faces;
    int f_id = 0;
    for (auto & p_face : p_faces) {
      std::vector<int> vs;
      DASHalfEdge* begin = p_face->halfEdge_;
      DASHalfEdge* he = begin;
      do {
        vs.push_back(he->source_->id_);
        he = he->next_;
      } while (he != begin);
      faces.insert(std::make_pair(f_id, vs));
      ++f_id;
    }

    BuildPolyhedron(vertices, faces);
  }
  return *this;
}


Vertices DASPolyhedron::GetVertices() const {
  return vertices_;
}

Vertices DASPolyhedron::GetOriginalVertices() const {
  return originalVertices_;
}

Faces DASPolyhedron::GetFaces() const {
  return faces_;
}

size_t DASPolyhedron::GetNumVertices() const {
  return vertices_.size();
}

size_t DASPolyhedron::GetNumFaces() const
{
  return faces_.size();
}

void DASPolyhedron::SetVertices(Vertices vertices, bool isOriginal = false) {
  vertices_ = vertices;
  if (isOriginal) {
    // make a hard copy of vertices
    for (auto & vertice : vertices) {
      DASVertex* v = vertice.second;
      auto* w = new DASVertex(v);
      originalVertices_.insert(std::make_pair(vertice.first, w));
    }
  }
}

void DASPolyhedron::SetFaces(Faces faces)
{
  faces_ = faces;
}

void DASPolyhedron::SetEdges(Edges edges)
{
  edges_ = edges;
}

void DASPolyhedron::BuildPolyhedron(std::map<int, SBPosition3> vertices, std::map<int, std::vector<int>> faces) {
  
  //create indices for faces
  if (faces.size() > 0) {
    auto firstFace = faces[0];
    int verticesPerFace = boost::numeric_cast<int>(firstFace.size());
    indices_ = new unsigned int[verticesPerFace * faces.size()];

    int k = 0;
    for (auto & face : faces) {
      auto faceIndices = face.second;
      if (faceIndices.size() == 3) {
        indices_[verticesPerFace * k + 0] = faceIndices[0];
        indices_[verticesPerFace * k + 1] = faceIndices[1];
        indices_[verticesPerFace * k + 2] = faceIndices[2];
      }
      ++k;
    }
  }


  for (auto & vertice : vertices) {
    AddVertex(vertice.first, vertice.second);
  }
  // store original vertices for scaling and such
  SetVertices(vertices_, true);

  std::map<std::pair<int, int>, DASEdge*> seen_edges;
  unsigned int he_id = 0;
  unsigned int e_id = 0;
  for (auto & i : faces) {
    auto* face = new DASPolygon();
    faces_.push_back(face);
    face->id_ = i.first;

    DASHalfEdge* prev = nullptr;
    auto* first = new DASHalfEdge();
    first->id_ = he_id;
    ++he_id;
    auto v = i.second;
    for (auto j = v.begin(); j != v.end(); ++j) {
      DASVertex* vertex_i = vertices_.at(*j);
      auto* he = new DASHalfEdge();
      he->id_ = he_id;
      ++he_id;
      he->source_ = vertex_i;
      face->halfEdge_ = he;
      he->left_ = face;

      // create ANTEdge
      auto next_i = std::next(j);
      if (next_i == v.end()) {
        next_i = v.begin();
      }
      auto* edge = new DASEdge();
      auto* pair = new DASHalfEdge();
      std::pair<int, int> edge_pair = std::make_pair(*j, *next_i);
      if (*j > *next_i) {
        edge_pair = std::make_pair(*next_i, *j);
      }
      if (seen_edges.find(edge_pair) == seen_edges.end()) {
        // we need to create ANTEdge;
        edge->halfEdge_ = he;
        edge->id_ = e_id;
        seen_edges.insert(std::make_pair(edge_pair, edge));
        edges_.push_back(edge);
        vertex_i->halfEdge_ = he;
        ++e_id;
      }
      else {
        edge = seen_edges.at(edge_pair);
        pair = edge->halfEdge_;
        he->pair_ = pair;
        pair->pair_ = he;
        pair->id_ = he_id;
        ++he_id;
      }
      he->edge_ = edge;
      // add next and prev half edges
      if (prev != nullptr) {
        he->prev_ = prev;
        prev->next_ = he;
      }
      else {
        // save ref to first half-edge
        first = he;
      }
      prev = he;
    }
    // fix first half-edge
    face->halfEdge_ = first;
    first->prev_ = prev;
    prev->next_ = first;
  }
}

int DASPolyhedron::FetchNumber(std::string st, std::string tok) {
  int num = 0;
  size_t tok_size = tok.length();
  size_t pos = st.find(tok);
  if (pos != std::string::npos) {
    std::string n = st.substr(pos + tok_size);
    num = atoi(n.c_str());
  }
  return num;
}

template <typename T>
std::vector<T> PushToVector(std::vector<T> v, std::string s, std::string type, size_t i, size_t substring_length) {
  std::string sub_s = s.substr(i, substring_length);
  T elem;
  if (type == "double") {
	elem = atof(sub_s.c_str());
  }
  else if (type == "int") {
	elem = atoi(sub_s.c_str());
  }
  v.push_back(elem);
  return v;
}



template <typename T>
std::vector<T> DASPolyhedron::SplitString(std::string s, std::string type, T dummy) {
  // dummy is just a dummy variable to infer the type
  boost::trim(s);
  std::vector<T> v;
  std::string delimiter = " ";
  size_t pos = s.find(delimiter);
  size_t i = 0;
  bool more = true;
  while (more) {
	v = PushToVector(v, s, type, i, pos - i);
    i = ++pos;
    pos = s.find(delimiter, pos);

    if (pos > s.size()) {
	    v = PushToVector(v, s, type, i, s.length());
      more = false;
    }
  }
  return v;
}

void DASPolyhedron::AddVertex(int id, SBPosition3 coords) {
  auto* vertex = new DASVertex();
  vertex->id_ = id;
  vertex->SetCoordinates(coords);
  vertex->halfEdge_ = nullptr;
  vertices_.insert(std::make_pair(id, vertex));
}

void DASPolyhedron::AddFace(int id, std::vector<int> vertices) {
  auto* face = new DASPolygon();
  face->id_ = id;
  auto* prev = new DASHalfEdge();
  auto* first = new DASHalfEdge();
  for (auto j = vertices.begin(); j != vertices.end(); ++j) {
    DASVertex* vertex_i = GetVertexById(*j);
    // Next vertex
    auto k = std::next(j);
    if (k == vertices.end()) {
      k = vertices.begin();
    }

    auto* he = new DASHalfEdge();
    he->source_ = vertex_i;
    he->left_ = face;
    face->halfEdge_ = he;
    // add next and prev half edges
    if (prev != nullptr) {
      he->prev_ = prev;
      prev->next_ = he;
    }
    else {
      // save ref to first half-edge
      first = he;
    }
    prev = he;

    DASVertex* vertex_k = GetVertexById(*k);
    auto* edge = new DASEdge();
    bool new_edge = true;
    for (auto & eit : edges_) {
      if (eit->halfEdge_->source_ == vertex_i && eit->halfEdge_->next_->source_ == vertex_k) {
        edge = eit;
        he->pair_ = edge->halfEdge_;
        new_edge = false;
        break;
      }
      if (eit->halfEdge_->source_ == vertex_k && eit->halfEdge_->next_->source_ == vertex_i) {
        edge = eit;
        he->pair_ = edge->halfEdge_;
        new_edge = false;
        break;
      }
    }
    if (new_edge) {
      edge->halfEdge_ = he;
      edges_.push_back(edge);
    }
    he->edge_ = edge;
  }
  // fix first half-edge
  face->halfEdge_ = first;
  first->prev_ = prev;
  prev->next_ = first;
  faces_.push_back(face);
}

DASVertex* DASPolyhedron::GetVertexById(int id) {
  return vertices_.at(id);
}

std::pair<DASEdge*, double> DASPolyhedron::MinimumEdgeLength() {
  double min_length = 10000000000000;
  DASEdge* min_edge;
  for (auto & edge : edges_) {
    double length = CalculateEdgeLength(edge);
    if (length < min_length) {
      min_length = length;
      min_edge = edge;
    }
  }
  return std::make_pair(min_edge, min_length);
}

std::pair<DASEdge*, double> DASPolyhedron::MaximumEdgeLength() {
  double max_length = 0;
  DASEdge* max_edge;
  for (auto & edge : edges_) {
    double length = CalculateEdgeLength(edge);
    if (length > max_length) {
      max_length = length;
      max_edge = edge;
    }
  }
  return std::make_pair(max_edge, max_length);
}

double DASPolyhedron::CalculateEdgeLength(DASEdge* edge) {
  std::vector<double> s_c = edge->halfEdge_->source_->GetVectorCoordinates();
  std::vector<double> t_c = edge->halfEdge_->pair_->source_->GetVectorCoordinates();
  auto s = ADNVectorMath::CreateBoostVector(s_c);
  auto t = ADNVectorMath::CreateBoostVector(t_c);
  return ublas::norm_2(t - s);
}

DASEdge* DASPolyhedron::GetEdgeByVertices(DASVertex* source, DASVertex* target) {
  DASEdge* edge = new DASEdge();
  DASHalfEdge* begin = source->halfEdge_;
  DASHalfEdge* he = begin;
  do {
    if (he->pair_->source_ == target) {
      edge = he->edge_;
      break;
    }
    he = he->pair_->next_;
  } while (he != begin);
  return edge;
}

unsigned int * DASPolyhedron::GetIndices()
{
  return indices_;
}

int DASPolyhedron::GetVertexDegree(DASVertex* v) {
  int degree = 0;
  DASHalfEdge* begin = v->halfEdge_;
  DASHalfEdge* he = begin;
  do {
    ++degree;
    he = he->pair_->next_;
  } while (he != begin);
  return degree;
}

Edges DASPolyhedron::GetEdges() const {
  return edges_;
}

bool DASPolyhedron::IsInFace(DASVertex* v, DASPolygon* f) {
  bool is = false;
  DASHalfEdge* begin = f->halfEdge_;
  DASHalfEdge* he = begin;
  do {
    DASVertex* w = he->source_;
    if (w->id_ == v->id_) {
      is = true;
    }
    he = he->next_;
  } while (he != begin);
  return is;
}

DASHalfEdge* DASPolyhedron::GetHalfEdge(unsigned int id) {
  DASHalfEdge* res = nullptr;
  for (auto & vit : vertices_) {
    DASHalfEdge* begin = vit.second->halfEdge_;
    DASHalfEdge* he = begin;
    do {
      if (he->id_ == id) {
        res = he;
        break;
      }
      else if (he->pair_->id_ == id) {
        res = he->pair_;
        break;
      }
      he = he->pair_->next_;
    } while (he != begin);
  }

  return res;
}

DASHalfEdge* DASPolyhedron::GetHalfEdge(DASVertex* v, DASVertex* w) {
  DASHalfEdge* begin = v->halfEdge_;
  DASHalfEdge* he = begin;
  do {
    if (he->pair_->source_ == w) {
      return he;
    }
    he = he->pair_->next_;
  } while (he != begin);

  // v and w do not share an edge
  std::exit(EXIT_FAILURE);
}

void DASPolyhedron::Scale(double scalingFactor) {
  std::vector<std::vector<double>> sc;
  std::vector<double> v1 = { double(scalingFactor), 0.0, 0.0 };
  std::vector<double> v2 = { 0.0, double(scalingFactor), 0.0 };
  std::vector<double> v3 = { 0.0, 0.0, double(scalingFactor) };
  sc.push_back(v1);
  sc.push_back(v2);
  sc.push_back(v3);
  
  ublas::matrix<double> scaleMatrix = ADNVectorMath::CreateBoostMatrix(sc);
  for (auto & originalVertice : originalVertices_) {
    std::vector<double> c = originalVertice.second->GetVectorCoordinates();
    ublas::vector<double> cB = ADNVectorMath::CreateBoostVector(c);
    ublas::vector<double> res = ublas::prod(scaleMatrix, cB);
    std::vector<double> r = ADNVectorMath::CreateStdVector(res);
    DASVertex* w = vertices_.at(originalVertice.first);
    SBPosition3 coords = SBPosition3();
    coords[0] = SBQuantity::angstrom(r[0]);
    coords[1] = SBQuantity::angstrom(r[1]);
    coords[2] = SBQuantity::angstrom(r[2]);
    w->SetCoordinates(coords);
  }
}

void DASPolyhedron::Center(SBPosition3 center) {
  std::vector<std::vector<double>> sc;
  for (auto & vertice : vertices_) {
    std::vector<double> coords = vertice.second->GetVectorCoordinates();
    sc.push_back(coords);
  }
  ublas::matrix<double> positions = ADNVectorMath::CreateBoostMatrix(sc);
  ublas::vector<double> cm = ADNVectorMath::CalculateCM(positions);
  std::vector<double> cm_std = ADNVectorMath::CreateStdVector(cm);
  SBPosition3 cm_sb = SBPosition3();
  cm_sb[0] = SBQuantity::angstrom(cm_std[0]);
  cm_sb[1] = SBQuantity::angstrom(cm_std[1]);
  cm_sb[2] = SBQuantity::angstrom(cm_std[2]);
  SBPosition3 R = center - cm_sb;
  for (auto & vertice : vertices_) {
    SBPosition3 pos = vertice.second->GetSBPosition();
    vertice.second->SetCoordinates(pos + R);
  }
  for (auto & originalVertice : originalVertices_) {
    SBPosition3 pos = originalVertice.second->GetSBPosition();
    originalVertice.second->SetCoordinates(pos + R);
  }
}