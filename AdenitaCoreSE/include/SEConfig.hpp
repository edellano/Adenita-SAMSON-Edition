#pragma once

//#include <SAMSON.hpp>
//#include "ANTAuxiliary.hpp"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace rapidjson;

class SEConfig : public QObject {
  Q_OBJECT
public:
  // singleton pattern
  ~SEConfig() = default;
  // deleted functions should be public for better error message handling
  SEConfig(SEConfig const& c) = delete;
  void operator=(SEConfig const& c) = delete;
  
  static SEConfig& GetInstance();

  // visual_model settin
  float min_melting_temp = 25.0f;
  float max_melting_temp = 80.0f;
  float min_gibbs_free_energy = -10000.0f;
  bool interpolate_dimensions = true;
  float animation_step_size = 0.0f;
  double double_helix_V_color[4] = { 0.0f, 0.43f, 0.86f, 1.0f };
  bool automatic_camera = true;  // adjusting camera according to dimension 
  bool preview_editor = true;  // adjusting camera according to dimension
  // structure prediction and algorithms
  bool use_atomic_details = false;
  int magic_number = 5;  // initial twist in cadnano files
  float crossover_distance_threshold = 15;  // ansgtroms
  float crossover_angle_threshold = 25;  // angstroms
  bool use_twist = true;
  bool detect_possible_crossovers = false;
  // logging and debugging
  bool clear_log_file = false;
  int test_type = 2;  // 2 for short
  // toggle overlays and display options
  bool display_possible_crossovers = true;
  bool display_nucleotide_basis = false;
  bool display_base_pairing = false;
  bool show_overlay = false;
  // group general
  std::string usage = "u"; //just for testing purposes: the way the software is used. u = user, e = elisa,  h = haichao

public slots:
  void updateConfig();

private:
  // private constructors to implement singleton
  SEConfig();
  
  //const std::string DEFAULT_CONFIGPATH = SAMSON::getUserDataPath() + "/adenita_settings.json";
  const std::string DEFAULT_CONFIGPATH = "adenita_settings.json";
  Document setting_;
  QFileSystemWatcher configFileWatcher_;

  void loadConfig();
  void writeDoubleArray(Writer<StringBuffer> & writer, std::string key, double * arr, int length);
  void readDoubleArray(Value & val, double * arr, int length);
};

