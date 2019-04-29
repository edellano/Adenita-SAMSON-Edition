#include "ADNAuxiliary.hpp"	

double ADNAuxiliary::mapRange(double input, double input_start, double input_end, double output_start, double output_end)
{
	//Y = (X - A) / (B - A) * (D - C) + C
	return (input - input_start) / (input_end - input_start) * (output_end - output_start) + output_start;	
}

//char ADNAuxiliary::getBaseSymbol(SBNode* node)
//{
//	char result;
//
//	if (false)
//	{
//		SBNodeIndexer atoms;
//		node->getNodes(atoms, SBNode::IsType(SBNode::Atom));
//		result = getBaseSymbol(atoms.size());
//	}
//	else {
//		SBSideChain* sidechain = static_cast<SBSideChain*>(node);
//		string name = sidechain->getName();
//		result = getBaseSymbol(name);
//	}
//
//	return result;
//}

char ADNAuxiliary::getBaseSymbol(std::string name)
{
	std::string symbol = name.substr(1, 2);
	//Auxiliary::log(symbol.c_str());

	return symbol.c_str()[0];
}

char ADNAuxiliary::getBaseSymbol(size_t numAtoms) {

	char symbol = '\0';
  
	return symbol;
}

void ADNAuxiliary::getHeatMapColor(double val, double min, double max, int* color)
{
  /*if (val < 0.0001f) {
    color[0] = 255;
    color[1] = 255;
    color[2] = 255;
    color[3] = 255;
    return;
  }*/

  const int NUM_COLORS = 5;
  std::vector<int*> colors;
  int color4[] = { 254, 240, 217, 255 };
  int color3[] = { 253, 204, 138, 255 };
  int color2[] = { 252, 141, 89, 255 };
  int color1[] = { 227, 74, 51, 255 };
  int color0[] = { 179, 0, 0, 255  };

  colors.push_back(color4);
  colors.push_back(color3);
  colors.push_back(color2);
  colors.push_back(color1);
  colors.push_back(color0);

  int idx1;
  int idx2;
  float fractBetween = 0;

  //Y = (X - A) / (B - A) * (D - C) + C
  double mappedVal = mapRange(val, min, max, 0, 1);

  if (mappedVal <= 0) {
    idx1 = idx2 = 0;
  }
  else if (mappedVal >= 1) {
    idx1 = idx2 = NUM_COLORS - 1;
  }
  else {

    mappedVal = mappedVal * (NUM_COLORS - 1);
    idx1 = int(mappedVal);
    idx2 = idx1 + 1;
    fractBetween = mappedVal - float(idx1);
  }

  color[0] = (colors[idx2][0] - colors[idx1][0]) * fractBetween + colors[idx1][0];
  color[1] = (colors[idx2][1] - colors[idx1][1]) * fractBetween + colors[idx1][1];
  color[2] = (colors[idx2][2] - colors[idx1][2]) * fractBetween + colors[idx1][2];
  color[3] = 255;

}

std::string ADNAuxiliary::SBPositionToString(SBPosition3 position) {
  std::string x = std::to_string(position[0].getValue());
  std::string y = std::to_string(position[1].getValue());
  std::string z = std::to_string(position[2].getValue());

  std::string st_pos = x + "," + y + "," + z;
  return st_pos;
}

std::string ADNAuxiliary::SBVector3ToString(SBVector3 vec) {
  std::string x = std::to_string(vec[0].getValue());
  std::string y = std::to_string(vec[1].getValue());
  std::string z = std::to_string(vec[2].getValue());

  std::string st_pos = x + "," + y + "," + z;
  return st_pos;
}

std::string ADNAuxiliary::UblasVectorToString(ublas::vector<double> vec) {
  std::string st_pos = "";
  if (vec.size() == 3) {
    st_pos = std::to_string(vec[0]) + "," + std::to_string(vec[1]) + "," + std::to_string(vec[2]);
  }
  else {
    bool start = true;
    for (auto &v : vec) {
      if (!start) {
        st_pos += ",";
      }
      st_pos += std::to_string(v);
      start = false;
    }
  }

  return st_pos;
}

std::string ADNAuxiliary::VectorToString(std::vector<int> vec)
{
  std::string st_pos = "";
  bool start = true;
  for (auto &v : vec) {
    if (!start) {
      st_pos += ",";
    }
    st_pos += std::to_string(v);
    start = false;
  }

  return st_pos;
}

Position3D ADNAuxiliary::StringToSBPosition(std::string position) {
  std::vector<std::string> stringArray;
  std::size_t pos = 0;
  std::size_t found;
  while ((found = position.find(',', pos)) != std::string::npos) {
    stringArray.push_back(position.substr(pos, found - pos));
    pos = found + 1;
  }
  stringArray.push_back(position.substr(pos));

  SBPosition3 sbPos = SBPosition3();
  sbPos[0] = SBQuantity::picometer(std::stod(stringArray[0]));
  sbPos[1] = SBQuantity::picometer(std::stod(stringArray[1]));
  sbPos[2] = SBQuantity::picometer(std::stod(stringArray[2]));
  return sbPos;
}

SBVector3 ADNAuxiliary::StringToSBVector(std::string vec) {
  std::vector<std::string> stringArray;
  std::size_t pos = 0;
  std::size_t found;
  while ((found = vec.find(',', pos)) != std::string::npos) {
    stringArray.push_back(vec.substr(pos, found - pos));
    pos = found + 1;
  }
  stringArray.push_back(vec.substr(pos));

  SBVector3 sbVec = SBVector3();
  sbVec[0] = SBQuantity::dimensionless(std::stod(stringArray[0]));
  sbVec[1] = SBQuantity::dimensionless(std::stod(stringArray[1]));
  sbVec[2] = SBQuantity::dimensionless(std::stod(stringArray[2]));
  return sbVec;
}

ublas::vector<double> ADNAuxiliary::StringToUblasVector(std::string vec) 
{
  std::vector<std::string> stringArray;
  std::size_t pos = 0;
  std::size_t found;
  while ((found = vec.find(',', pos)) != std::string::npos) {
    stringArray.push_back(vec.substr(pos, found - pos));
    pos = found + 1;
  }
  stringArray.push_back(vec.substr(pos));

  ublas::vector<double> uVec = ublas::vector<double>(stringArray.size());
  for (size_t i = 0; i < stringArray.size(); ++i) {
    uVec[i] = std::stod(stringArray[i]);
  }
  
  return uVec;
}

std::vector<int> ADNAuxiliary::StringToVector(std::string vec)
{
  std::vector<std::string> stringArray;
  std::size_t pos = 0;
  std::size_t found;
  while ((found = vec.find(',', pos)) != std::string::npos) {
    stringArray.push_back(vec.substr(pos, found - pos));
    pos = found + 1;
  }
  stringArray.push_back(vec.substr(pos));

  std::vector<int> uVec = std::vector<int>(stringArray.size());
  for (size_t i = 0; i < stringArray.size(); ++i) {
    uVec[i] = std::stoi(stringArray[i]);
  }

  return uVec;
}

Position3D ADNAuxiliary::UblasVectorToSBPosition(ublas::vector<double> pos)
{
  Position3D sbPos;
  sbPos[0] = SBQuantity::picometer(pos[0]);
  sbPos[1] = SBQuantity::picometer(pos[1]);
  sbPos[2] = SBQuantity::picometer(pos[2]);

  return sbPos;
}

SBVector3 ADNAuxiliary::UblasVectorToSBVector(ublas::vector<double> pos)
{
  SBVector3 sbPos;
  sbPos[0] = SBQuantity::dimensionless(pos[0]);
  sbPos[1] = SBQuantity::dimensionless(pos[1]);
  sbPos[2] = SBQuantity::dimensionless(pos[2]);

  return sbPos;
}

ublas::vector<double> ADNAuxiliary::SBVectorToUblasVector(SBVector3 v)
{
  ublas::vector<double> w(3, 0.0);
  w[0] = v[0].getValue();
  w[1] = v[1].getValue();
  w[2] = v[2].getValue();

  return w;
}

ublas::vector<double> ADNAuxiliary::SBPositionToUblas(Position3D pos)
{
  ublas::vector<double> v = ublas::vector<double>(3, 0.0);
  v[0] = pos[0].getValue();
  v[1] = pos[1].getValue();
  v[2] = pos[2].getValue();
  return v;
}

std::vector<double> ADNAuxiliary::SBPositionToVector(Position3D pos)
{
  std::vector<double> v(3);
  v[0] = pos[0].getValue();
  v[1] = pos[1].getValue();
  v[2] = pos[2].getValue();
  return v;
}

SBPosition3 ADNAuxiliary::VectorToSBPosition(std::vector<double> v)
{
  SBPosition3 pos = SBPosition3();
  if (v.size() == 3) {
    pos = SBPosition3(SBQuantity::nanometer(v[0]), SBQuantity::nanometer(v[1]), SBQuantity::nanometer(v[2]));
  }
  return pos;
}

SBVector3 ADNAuxiliary::VectorToSBVector(std::vector<double> v)
{
  SBVector3 w = SBVector3();
  if (v.size() == 3) {
    w = SBVector3(SBQuantity::dimensionless(v[0]), SBQuantity::dimensionless(v[1]), SBQuantity::dimensionless(v[2]));
  }
  return w;
}

bool ADNAuxiliary::ValidateSequence(std::string seq)
{
  bool val = true;
  for (auto& c: seq) {
    if (c != 'N' && c != 'A' && c != 'C' && c != 'T' && c != 'G') {
      val = false;
      break;
    }
  }
  return val;
}

QString ADNAuxiliary::AdenitaCitation()
{
  return
    "If you use this Adenita in your work, please cite: <br/>"
    "<br/>"
    "[1] <br />"
    "You can also cite the visual model as: <br />"
    "[2] <br />"
    "[3] ";
}
