#pragma once

#include <string>
#include <boost/numeric/conversion/cast.hpp>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <SAMSON.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <SEConfig.hpp>


namespace ublas = boost::numeric::ublas;

using Position3D = SBPosition3;

namespace ADNAuxiliary {
  using namespace ublas;

	enum SelectionFlag {
		Unselected = 0,
		Selected = 1
	};

	enum VisibilityFlag {
		Invisible = 0,
		Visible = 1
	};

  enum SortingType {
    StrandID = 0,
    Length = 1,
    GCCont = 2,
    MeltTemp = 3,
    Gibbs = 4
  };

  enum OverlayType {
    NoOverlay = 0,
    Crossovers = 1,
    PossibleCrossovers = 2
  };

  enum ScaffoldSeq {
    m13mp18 = 0,
    p7249 = 1,
    Custom = 2,
  };


  struct SphericalCoord {
    double phi;
    double theta;
    double r;
  };

  enum OxDNAInteractionType {
    DNA = 0,
    DNA2 = 1,
    Patchy = 2,
    LJ = 3,
  };

  enum OxDNASimulationType {
    MD = 0,
    MC = 1,
    VMMC = 2,
  };

  enum OxDNABackend {
    CPU = 0,
    CUDA = 1,
  };

  struct OxDNAOptions {
    OxDNAInteractionType interactionType_;
    OxDNASimulationType simType_;
    OxDNABackend backend_;
    bool debug_;
    unsigned int steps_;
    std::string temp_;
    double saltConcentration_ = 0.0;
    double mdTimeStep_ = 0.0;
    double mcDeltaTranslation_ = 0.0;
    double mcDeltaRotation_ = 0.0;
    double boxSizeX_ = 0.0;  // in nm
    double boxSizeY_ = 0.0;
    double boxSizeZ_ = 0.0;
  };

	double mapRange(double input, double input_start, double input_end, double output_start, double output_end);
	//char getBaseSymbol(SBNode* node);
	char getBaseSymbol(std::string name);
	char getBaseSymbol(size_t numAtoms); 
  void getHeatMapColor(double val, double min, double max, int* color);

  std::string UblasVectorToString(ublas::vector<double> vec);
  std::string VectorToString(std::vector<int> vec);
  std::string SBPositionToString(Position3D pos);
  std::string SBVector3ToString(SBVector3 vec);

  ublas::vector<double> StringToUblasVector(std::string vec);
  Position3D StringToSBPosition(std::string vec);
  SBVector3 StringToSBVector(std::string vec);
  std::vector<int> StringToVector(std::string vec);

  Position3D UblasVectorToSBPosition(ublas::vector<double> pos);
  
  bool ValidateSequence(std::string seq);
};
