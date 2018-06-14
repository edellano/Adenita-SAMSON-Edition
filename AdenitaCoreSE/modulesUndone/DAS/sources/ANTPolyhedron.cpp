/** \file
*  \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
*/

#include "ANTPolyhedron.hpp"

/* Constructor for ANTHalfEdge */
ANTHalfEdge::ANTHalfEdge() {
  next_ = nullptr;
  prev_ = nullptr;
  pair_ = nullptr;
  source_ = nullptr;
  left_ = nullptr;
  edge_ = nullptr;
}

ANTHalfEdge::ANTHalfEdge(const ANTHalfEdge & other) {
  *this = other;
}

ANTHalfEdge & ANTHalfEdge::operator=(const ANTHalfEdge & other) {
  if (this != &other) {
    id_ = other.id_;
  }
  return *this;
}

void ANTHalfEdge::WriteToJson(Writer<StringBuffer>& writer) {
  writer.Key(std::to_string(id_).c_str());
  writer.StartObject();

  writer.Key("id");
  writer.Int(id_);

  writer.Key("next");
  writer.Int(next_->id_);

  writer.Key("prev");
  writer.Int(prev_->id_);

  writer.Key("pair");
  writer.Int(pair_->id_);

  writer.Key("source");
  writer.Int(source_->id_);

  writer.Key("left");
  writer.Int(left_->id_);

  writer.Key("edge");
  writer.Int(edge_->id_);

  writer.EndObject();
}

ANTVertex::ANTVertex(const ANTVertex & w) {
  *this = w;
}

ANTVertex & ANTVertex::operator=(const ANTVertex & other) {
  if (this != &other) {
    id_ = other.id_;
    position_ = other.position_;
  }
  return *this;
}

/* Copy constructor for ANTVertex
*/
ANTVertex::ANTVertex(ANTVertex* w) {
  id_ = w->id_;
  halfEdge_ = w->halfEdge_;
  position_ = w->position_;
}

/* Destructor for ANTEdge
*/
ANTEdge::~ANTEdge() {
  // Remove all polygons conected to the edge
  ANTPolygon* p1 = nullptr;
  if (halfEdge_->left_ != nullptr) p1 = halfEdge_->left_;
  ANTPolygon* p2 = nullptr;
  if (halfEdge_->pair_->left_ != nullptr) p2 = halfEdge_->pair_->left_;
  if (p1 != nullptr) delete p1;
  if (p2 != nullptr) delete p2;
  if (halfEdge_->pair_ != nullptr) delete halfEdge_->pair_;
  delete halfEdge_;
}

ANTEdge::ANTEdge(const ANTEdge & other) {
  *this = other;
}

ANTEdge & ANTEdge::operator=(const ANTEdge & other) {
  if (this != &other) {
    // copy vertices only one vertex
    ANTVertex* v1 = new ANTVertex(other.halfEdge_->source_);
    // create half edges
    ANTHalfEdge* he1 = new ANTHalfEdge(*other.halfEdge_);
    he1->source_ = v1;
    he1->edge_ = this;
    halfEdge_ = he1;
  }
  return *this;
}

ANTPolygon::ANTPolygon(const ANTPolygon & p) {
  *this = p;
}

/* Destructor for ANTPolygon
*/
ANTPolygon::~ANTPolygon() {
  ANTHalfEdge* begin = halfEdge_;
  ANTHalfEdge* he = begin;
  do {
    he->left_ = nullptr;
    he = he->next_;
  } while (he != begin);
}

ANTPolygon & ANTPolygon::operator=(const ANTPolygon & other) {
  if (this != &other) {
    // copy all edges and link them
    ANTHalfEdge* begin = other.halfEdge_;
    ANTHalfEdge* he = begin;
    ANTEdge* prev_e = nullptr;
    ANTEdge* first_e = nullptr;
    ANTEdge* e = nullptr;
    do {
      e = new ANTEdge(*(he->edge_));
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

std::vector<double> ANTVertex::GetVectorCoordinates() const {
  std::vector<double> coords = {
    position_[0].getValue() * 0.01,
    position_[1].getValue() * 0.01,
    position_[2].getValue() * 0.01,
  };
  return coords;
}

SBPosition3 ANTVertex::GetSBPosition() const {
  return position_;
}

void ANTVertex::SetCoordinates(SBPosition3 coordinates) {
  position_ = coordinates;
}

void ANTVertex::WriteToJson(Writer<StringBuffer>& writer) {
  std::string key = std::to_string(id_);
  writer.Key(key.c_str());
  writer.StartObject();

  writer.Key("id");
  writer.Int(id_);

  std::string position = ANTAuxiliary::SBPosition3ToString(position_);
  writer.Key("position");
  writer.String(position.c_str());

  writer.Key("halfedge");
  if (halfEdge_ != nullptr) writer.Int(halfEdge_->id_);
  else writer.Int(-1);

  writer.EndObject();
}

ANTVertex * ANTVertex::LoadFromJson(Value::ConstMemberIterator itr, std::map<int, ANTHalfEdge*>& halfEdges) {
  ANTVertex* v = new ANTVertex();
  v->id_ = itr->value["id"].GetInt();
  v->position_ = ANTAuxiliary::StringToSBPosition3(itr->value["position"].GetString());
  int he_id = itr->value["halfedge"].GetInt();
  if (halfEdges.find(he_id) != halfEdges.end()) v->halfEdge_ = halfEdges.find(he_id)->second;

  return v;
}

ANTPolyhedron::ANTPolyhedron(std::string filename) {
  LoadFromPLYFile(filename);
}

ANTPolyhedron::ANTPolyhedron(const ANTPolyhedron &p) {
  *this = p;
}

/* Destructor */
ANTPolyhedron::~ANTPolyhedron() {

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

ANTPolyhedron & ANTPolyhedron::operator=(const ANTPolyhedron & p) {
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
      ANTHalfEdge* begin = p_face->halfEdge_;
      ANTHalfEdge* he = begin;
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


Vertices ANTPolyhedron::GetVertices() const {
  return vertices_;
}

Vertices ANTPolyhedron::GetOriginalVertices() const {
  return originalVertices_;
}

Faces ANTPolyhedron::GetFaces() const {
  return faces_;
}

size_t ANTPolyhedron::GetNumVertices() const {
  return vertices_.size();
}

size_t ANTPolyhedron::GetNumFaces() const
{
  return faces_.size();
}

void ANTPolyhedron::SetVertices(Vertices vertices, bool isOriginal = false) {
  vertices_ = vertices;
  if (isOriginal) {
    // make a hard copy of vertices
    for (auto & vertice : vertices) {
      ANTVertex* v = vertice.second;
      auto* w = new ANTVertex(v);
      originalVertices_.insert(std::make_pair(vertice.first, w));
    }
  }
}

void ANTPolyhedron::SetFaces(Faces faces)
{
  faces_ = faces;
}

void ANTPolyhedron::SetEdges(Edges edges)
{
  edges_ = edges;
}

void ANTPolyhedron::BuildPolyhedron(std::map<int, SBPosition3> vertices, std::map<int, std::vector<int>> faces) {
  
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

  std::map<std::pair<int, int>, ANTEdge*> seen_edges;
  unsigned int he_id = 0;
  unsigned int e_id = 0;
  for (auto & i : faces) {
    auto* face = new ANTPolygon();
    faces_.push_back(face);
    face->id_ = i.first;

    ANTHalfEdge* prev = nullptr;
    auto* first = new ANTHalfEdge();
    first->id_ = he_id;
    ++he_id;
    auto v = i.second;
    for (auto j = v.begin(); j != v.end(); ++j) {
      ANTVertex* vertex_i = vertices_.at(*j);
      auto* he = new ANTHalfEdge();
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
      auto* edge = new ANTEdge();
      auto* pair = new ANTHalfEdge();
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

void ANTPolyhedron::LoadFromPLYFile(std::string filename) {

  //get file lines
  std::vector<std::string> lines;
  SBIFileReader::getFileLines(filename, lines);

  unsigned int start_vertices = 0;
  unsigned int start_faces = 0;

  int counter_vertices = 0;
  int counter_faces = 0;
  int total_vertices = 0;
  int total_faces = 0;

  // Check first line is ply
  if (lines[0] != "ply") throw 20;

  //initialize values
  for (unsigned int i = 1; i < lines.size(); i++) {
    std::string line = lines[i];

    // Fetch number of vertices and faces
    if (total_vertices == 0) {
      std::string num_vertices_tok = "element vertex ";
      total_vertices = FetchNumber(line, num_vertices_tok);
    }
    if (total_faces == 0) {
      std::string num_faces_tok = "element face ";
      total_faces = FetchNumber(line, num_faces_tok);
    }
    if (line == "end_header") {
      start_vertices = i + 1;
      start_faces = start_vertices + total_vertices;
      break;
    }
  }

  int currentSerialNumber = 0;

  // Fetch vertices
  std::map<int, SBPosition3> vertices;
  for (unsigned int i = start_vertices; i < start_faces; i++) {
    std::string line = lines[i];
    std::vector<double> coords = SplitString(line, std::string("double"), 0.1);
    SBPosition3 coordsSB = SBPosition3();
    coordsSB[0] = SBQuantity::angstrom(coords[0]);
    coordsSB[1] = SBQuantity::angstrom(coords[1]);
    coordsSB[2] = SBQuantity::angstrom(coords[2]);
    vertices.insert(std::make_pair(currentSerialNumber, coordsSB));
    ++currentSerialNumber;
  }

  // Fetch faces
  currentSerialNumber = 0;

  std::map<int, std::vector<int>> faces;
  for (unsigned int i = start_faces; i < start_faces + total_faces; i++) {
    std::string line = lines[i];

    std::vector<int> f = SplitString(line, std::string("int"), 1);
    std::vector<int> v;
    
    for (int j = 1; j <= f[0]; j++) {
      v.push_back(f[j]);
    }
    faces.insert(std::make_pair(currentSerialNumber, v));
    ++currentSerialNumber;
  }

  BuildPolyhedron(vertices, faces);
}

int ANTPolyhedron::FetchNumber(std::string st, std::string tok) {
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
std::vector<T> ANTPolyhedron::SplitString(std::string s, std::string type, T dummy) {
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

void ANTPolyhedron::AddVertex(int id, SBPosition3 coords) {
  auto* vertex = new ANTVertex();
  vertex->id_ = id;
  vertex->SetCoordinates(coords);
  vertex->halfEdge_ = nullptr;
  vertices_.insert(std::make_pair(id, vertex));
}

void ANTPolyhedron::AddFace(int id, std::vector<int> vertices) {
  auto* face = new ANTPolygon();
  face->id_ = id;
  auto* prev = new ANTHalfEdge();
  auto* first = new ANTHalfEdge();
  for (auto j = vertices.begin(); j != vertices.end(); ++j) {
    ANTVertex* vertex_i = GetVertexById(*j);
    // Next vertex
    auto k = std::next(j);
    if (k == vertices.end()) {
      k = vertices.begin();
    }

    auto* he = new ANTHalfEdge();
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

    ANTVertex* vertex_k = GetVertexById(*k);
    auto* edge = new ANTEdge();
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

ANTVertex* ANTPolyhedron::GetVertexById(int id) {
  return vertices_.at(id);
}

std::pair<ANTEdge*, double> ANTPolyhedron::MinimumEdgeLength() {
  double min_length = 10000000000000;
  ANTEdge* min_edge;
  for (auto & edge : edges_) {
    double length = CalculateEdgeLength(edge);
    if (length < min_length) {
      min_length = length;
      min_edge = edge;
    }
  }
  return std::make_pair(min_edge, min_length);
}

std::pair<ANTEdge*, double> ANTPolyhedron::MaximumEdgeLength() {
  double max_length = 0;
  ANTEdge* max_edge;
  for (auto & edge : edges_) {
    double length = CalculateEdgeLength(edge);
    if (length > max_length) {
      max_length = length;
      max_edge = edge;
    }
  }
  return std::make_pair(max_edge, max_length);
}

double ANTPolyhedron::CalculateEdgeLength(ANTEdge* edge) {
  std::vector<double> s_c = edge->halfEdge_->source_->GetVectorCoordinates();
  std::vector<double> t_c = edge->halfEdge_->pair_->source_->GetVectorCoordinates();
  auto s = ANTVectorMath::CreateBoostVector(s_c);
  auto t = ANTVectorMath::CreateBoostVector(t_c);
  return ublas::norm_2(t - s);
}

ANTEdge* ANTPolyhedron::GetEdgeByVertices(ANTVertex* source, ANTVertex* target) {
  ANTEdge* edge = new ANTEdge();
  ANTHalfEdge* begin = source->halfEdge_;
  ANTHalfEdge* he = begin;
  do {
    if (he->pair_->source_ == target) {
      edge = he->edge_;
      break;
    }
    he = he->pair_->next_;
  } while (he != begin);
  return edge;
}

unsigned int * ANTPolyhedron::GetIndices()
{
  return indices_;
}

int ANTPolyhedron::GetVertexDegree(ANTVertex* v) {
  int degree = 0;
  ANTHalfEdge* begin = v->halfEdge_;
  ANTHalfEdge* he = begin;
  do {
    ++degree;
    he = he->pair_->next_;
  } while (he != begin);
  return degree;
}

Edges ANTPolyhedron::GetEdges() const {
  return edges_;
}

bool ANTPolyhedron::IsInFace(ANTVertex* v, ANTPolygon* f) {
  bool is = false;
  ANTHalfEdge* begin = f->halfEdge_;
  ANTHalfEdge* he = begin;
  do {
    ANTVertex* w = he->source_;
    if (w->id_ == v->id_) {
      is = true;
    }
    he = he->next_;
  } while (he != begin);
  return is;
}

ANTHalfEdge* ANTPolyhedron::GetHalfEdge(unsigned int id) {
  ANTHalfEdge* res = nullptr;
  for (auto & vit : vertices_) {
    ANTHalfEdge* begin = vit.second->halfEdge_;
    ANTHalfEdge* he = begin;
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

ANTHalfEdge* ANTPolyhedron::GetHalfEdge(ANTVertex* v, ANTVertex* w) {
  ANTHalfEdge* begin = v->halfEdge_;
  ANTHalfEdge* he = begin;
  do {
    if (he->pair_->source_ == w) {
      return he;
    }
    he = he->pair_->next_;
  } while (he != begin);

  // v and w do not share an edge
  std::exit(EXIT_FAILURE);
}

void ANTPolyhedron::Scale(double scalingFactor) {
  std::vector<std::vector<double>> sc;
  std::vector<double> v1 = { double(scalingFactor), 0.0, 0.0 };
  std::vector<double> v2 = { 0.0, double(scalingFactor), 0.0 };
  std::vector<double> v3 = { 0.0, 0.0, double(scalingFactor) };
  sc.push_back(v1);
  sc.push_back(v2);
  sc.push_back(v3);
  
  ublas::matrix<double> scaleMatrix = ANTVectorMath::CreateBoostMatrix(sc);
  for (auto & originalVertice : originalVertices_) {
    std::vector<double> c = originalVertice.second->GetVectorCoordinates();
    ublas::vector<double> cB = ANTVectorMath::CreateBoostVector(c);
    ublas::vector<double> res = ublas::prod(scaleMatrix, cB);
    std::vector<double> r = ANTVectorMath::CreateStdVector(res);
    ANTVertex* w = vertices_.at(originalVertice.first);
    SBPosition3 coords = SBPosition3();
    coords[0] = SBQuantity::angstrom(r[0]);
    coords[1] = SBQuantity::angstrom(r[1]);
    coords[2] = SBQuantity::angstrom(r[2]);
    w->SetCoordinates(coords);
  }
}

void ANTPolyhedron::Center(SBPosition3 center) {
  std::vector<std::vector<double>> sc;
  for (auto & vertice : vertices_) {
    std::vector<double> coords = vertice.second->GetVectorCoordinates();
    sc.push_back(coords);
  }
  ublas::matrix<double> positions = ANTVectorMath::CreateBoostMatrix(sc);
  ublas::vector<double> cm = ANTVectorMath::CalculateCM(positions);
  std::vector<double> cm_std = ANTVectorMath::CreateStdVector(cm);
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

void ANTPolyhedron::WriteToJson(Writer<StringBuffer>& writer) {
  writer.StartObject();

  writer.Key("vertices");
  writer.StartObject();
  for (auto &p : vertices_) {
    ANTVertex* v = p.second;
    v->WriteToJson(writer);
  }
  writer.EndObject();

  writer.Key("original_vertices");
  writer.StartObject();
  for (auto &p : originalVertices_) {
    ANTVertex* v = p.second;
    v->WriteToJson(writer);
  }
  writer.EndObject();

  writer.Key("edges");
  writer.StartObject();
  for (auto &p : edges_) {
    writer.Key(std::to_string(p->id_).c_str());
    writer.StartObject();
    writer.Key("id");
    writer.Int(p->id_);
    writer.Key("halfedge");
    writer.Int(p->halfEdge_->id_);
    writer.EndObject();
  }
  writer.EndObject();

  writer.Key("faces");
  writer.StartObject();
  for (auto &p : faces_) {
    writer.Key(std::to_string(p->id_).c_str());
    writer.StartObject();
    writer.Key("id");
    writer.Int(p->id_);
    writer.Key("halfedge");
    writer.Int(p->halfEdge_->id_);
    writer.EndObject();
  }
  writer.EndObject();

  // iterate over halfedges
  writer.Key("halfedges");
  writer.StartObject();
  for (auto &p : edges_) {
    ANTHalfEdge* he = p->halfEdge_;
    he->WriteToJson(writer);
    ANTHalfEdge* he_pair = he->pair_;
    if (he_pair != nullptr) he_pair->WriteToJson(writer);
  }
  writer.EndObject();

  writer.EndObject();
}

ANTPolyhedron ANTPolyhedron::LoadFromJson(Value & value, std::map<int, ANTHalfEdge*> halfEdges, std::map<int, ANTPolygon*> fs) {
  ANTPolyhedron poly = ANTPolyhedron();

  Value& val_halfedges = value["halfedges"];
  std::multimap<int, ANTHalfEdge*> edge_halfEdge_Map;
  std::multimap<int, ANTHalfEdge*> vertex_halfEdge_Map;
  std::map<int, ANTHalfEdge*> face_halfEdge_Map;
  for (Value::ConstMemberIterator itr = val_halfedges.MemberBegin(); itr != val_halfedges.MemberEnd(); ++itr) {
    ANTHalfEdge* he = new ANTHalfEdge();
    int he_id = itr->value["id"].GetInt();
    int e_id = itr->value["edge"].GetInt();
    int s_id = itr->value["source"].GetInt();
    int l_id = itr->value["left"].GetInt();

    he->id_ = he_id;
    halfEdges.insert(std::make_pair(he_id, he));
    edge_halfEdge_Map.insert(std::make_pair(e_id, he));
    vertex_halfEdge_Map.insert(std::make_pair(s_id, he));
    face_halfEdge_Map.insert(std::make_pair(l_id, he));
  }
  // second loop for fetching next, prev and pairs
  for (Value::ConstMemberIterator itr = val_halfedges.MemberBegin(); itr != val_halfedges.MemberEnd(); ++itr) {
    int he_id = itr->value["id"].GetInt();
    ANTHalfEdge* he = halfEdges.at(he_id);
    int prev = itr->value["prev"].GetInt();
    int next = itr->value["next"].GetInt();
    int pair = itr->value["pair"].GetInt();

    if (halfEdges.find(prev) != halfEdges.end()) he->prev_ = halfEdges.find(prev)->second;
    if (halfEdges.find(pair) != halfEdges.end()) he->pair_ = halfEdges.find(pair)->second;
    if (halfEdges.find(next) != halfEdges.end()) he->next_ = halfEdges.find(next)->second;
  }

  Value& val_edges = value["edges"];
  std::vector<ANTEdge*> edges;
  for (Value::ConstMemberIterator itr = val_edges.MemberBegin(); itr != val_edges.MemberEnd(); ++itr) {
    ANTEdge* e = new ANTEdge();
    int e_id = itr->value["id"].GetInt();
    int he_id = itr->value["halfedge"].GetInt();
    e->id_ = e_id;
    if (halfEdges.find(he_id) != halfEdges.end()) e->halfEdge_ = halfEdges.find(he_id)->second;
    if (edge_halfEdge_Map.find(e_id) != edge_halfEdge_Map.end()) edge_halfEdge_Map.find(e_id)->second->edge_ = e;
    edges.push_back(e);
    // fetch edges for halfedges
    if (edge_halfEdge_Map.find(e->id_) != edge_halfEdge_Map.end()) {
      for (auto it = edge_halfEdge_Map.find(e->id_); it != edge_halfEdge_Map.end(); ++it) {
        ANTHalfEdge* he = it->second;
        he->edge_ = e;
      }
    }
  }

  Value& val_vertices = value["vertices"];
  std::map<int, ANTVertex*> vertices;
  for (Value::ConstMemberIterator itr = val_vertices.MemberBegin(); itr != val_vertices.MemberEnd(); ++itr) {
    ANTVertex* v = ANTVertex::LoadFromJson(itr, halfEdges);
    if (vertex_halfEdge_Map.find(v->id_) != vertex_halfEdge_Map.end()) vertex_halfEdge_Map.find(v->id_)->second->source_ = v;
    vertices.insert(std::make_pair(v->id_, v));
    // fetch sources for halfedges
    for (auto it = vertex_halfEdge_Map.find(v->id_); it != vertex_halfEdge_Map.end(); ++it) {
      ANTHalfEdge* he = it->second;
      he->source_ = v;
    }
  }

  Value& val_origVertices = value["original_vertices"];
  std::map<int, ANTVertex*> originalVertices;
  for (Value::ConstMemberIterator itr = val_origVertices.MemberBegin(); itr != val_origVertices.MemberEnd(); ++itr) {
    ANTVertex* v = ANTVertex::LoadFromJson(itr, halfEdges);
    originalVertices.insert(std::make_pair(v->id_, v));
  }

  Value& val_faces = value["faces"];
  std::vector<ANTPolygon*> faces;
  for (Value::ConstMemberIterator itr = val_faces.MemberBegin(); itr != val_faces.MemberEnd(); ++itr) {
    ANTPolygon* f = new ANTPolygon();
    f->id_ = itr->value["id"].GetInt();
    int he_id = itr->value["halfedge"].GetInt();
    if (halfEdges.find(he_id) != halfEdges.end()) f->halfEdge_ = halfEdges.find(he_id)->second;
    if (face_halfEdge_Map.find(f->id_) != face_halfEdge_Map.end()) face_halfEdge_Map.find(f->id_)->second->left_ = f;
    faces.push_back(f);
    fs.insert(std::make_pair(f->id_, f));
  }

  poly.SetVertices(vertices, false);
  poly.SetVertices(originalVertices, true);
  poly.SetFaces(faces);
  poly.SetEdges(edges);

  return poly;
}
