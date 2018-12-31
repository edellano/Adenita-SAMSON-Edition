#include "SEConfig.hpp"

SEConfig & SEConfig::GetInstance()
{
  static SEConfig instance;
  return instance;
}

void SEConfig::updateDebugConfig()
{
  FILE* fp = fopen(DEBUG_CONFIGPATH.c_str(), "rb");
  if (fp != NULL) {
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    setting_.ParseStream(is);

    if (setting_.FindMember("min_cutoff") != setting_.MemberEnd()) {
      debugOptions.minCutOff = setting_["min_cutoff"].GetDouble();
    }
    
    if (setting_.FindMember("max_cutoff") != setting_.MemberEnd()) {
      debugOptions.maxCutOff = setting_["max_cutoff"].GetDouble();
    }

    if (setting_.FindMember("display_nucleotide_basis") != setting_.MemberEnd()) {
      debugOptions.display_nucleotide_basis = setting_["display_nucleotide_basis"].GetBool();
    }

    if (setting_.FindMember("display_base_pairing") != setting_.MemberEnd()) {
      debugOptions.display_base_pairing = setting_["display_base_pairing"].GetBool();
    }

    if (setting_.FindMember("custom_bool") != setting_.MemberEnd()) {
      debugOptions.customBool = setting_["custom_bool"].GetBool();
    }

    if (setting_.FindMember("custom_double") != setting_.MemberEnd()) {
      debugOptions.customDouble = setting_["custom_double"].GetDouble();
    }

    if (setting_.FindMember("custom_int") != setting_.MemberEnd()) {
      debugOptions.customInt = setting_["custom_int"].GetInt();
    }
  }
}

SEConfig::SEConfig() {
  loadConfig();
  loadDebugConfig();
}

void SEConfig::loadConfig() {
  QFileInfo check_file(DEFAULT_CONFIGPATH.c_str());

  if (!(check_file.exists() && check_file.isFile())) {
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("min_melting_temp");
    writer.Double(min_melting_temp);

    writer.Key("max_melting_temp");
    writer.Double(max_melting_temp);

    writer.Key("min_gibbs_free_energy");
    writer.Double(min_gibbs_free_energy);

    writer.Key("max_gibbs_free_energy");
    writer.Double(max_gibbs_free_energy);

    writer.Key("interpolate_dimensions");
    writer.Bool(interpolate_dimensions);

    writer.Key("animation_step_size");
    writer.Double(animation_step_size);

    writeDoubleArray(writer, "double_helix_V_color", double_helix_V_color, 4);
    writeDoubleArray(writer, "nucleotide_E_Color", nucleotide_E_Color, 4);
    writeDoubleArray(writer, "double_strand_color", double_strand_color, 4);
    writeDoubleArray(writer, "adenine_color", adenine_color, 4);
    writeDoubleArray(writer, "thymine_color", thymine_color, 4);
    writeDoubleArray(writer, "guanine_color", guanine_color, 4);
    writeDoubleArray(writer, "cytosine_color", cytosine_color, 4);
    writeDoubleArray(writer, "staple_colors", staple_colors, 48);
    
    writer.Key("nucleotide_V_radius");
    writer.Double(nucleotide_V_radius);

    writer.Key("nucleotide_E_radius");
    writer.Double(nucleotide_E_radius);

    writer.Key("base_pair_radius");
    writer.Double(base_pair_radius);

    writer.Key("num_staple_colors");
    writer.Double(num_staple_colors);

    writer.Key("automatic_camera");
    writer.Bool(automatic_camera);

    writer.Key("preview_editor");
    writer.Bool(preview_editor);

    writer.Key("use_atomic_details");
    writer.Bool(use_atomic_details);

    writer.Key("crossover_distance_threshold");
    writer.Double(crossover_distance_threshold);

    writer.Key("crossover_angle_threshold");
    writer.Double(crossover_angle_threshold);

    writer.Key("use_twist");
    writer.Bool(use_twist);

    writer.Key("detect_possible_crossovers");
    writer.Bool(detect_possible_crossovers);

    writer.Key("dh_dist");
    writer.Double(dh_dist);

    writer.Key("clear_log_file");
    writer.Bool(clear_log_file);

    writer.Key("test_type");
    writer.Int(test_type);

    writer.Key("display_possible_crossovers");
    writer.Bool(display_possible_crossovers);

    writer.Key("show_overlay");
    writer.Bool(show_overlay);

    writer.Key("mode");
    writer.Int(mode);

    writer.Key("auto_set_scaffold_sequence");
    writer.Bool(auto_set_scaffold_sequence);

    writer.Key("auto_calculate_binding_regions");
    writer.Bool(auto_calculate_binding_regions);
    
    writer.EndObject();

    std::ofstream out(DEFAULT_CONFIGPATH);
    out << s.GetString();
    out.close();
  }

  updateConfig();

  configFileWatcher_.addPath(DEFAULT_CONFIGPATH.c_str());

  QObject::connect(&configFileWatcher_, SIGNAL(fileChanged(const QString &)), this, SLOT(updateConfig()));
}

void SEConfig::loadDebugConfig()
{
  QFileInfo check_file(DEBUG_CONFIGPATH.c_str());

  if (!(check_file.exists() && check_file.isFile())) {
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("min_cutoff");
    writer.Double(debugOptions.minCutOff);

    writer.Key("max_cutoff");
    writer.Double(debugOptions.maxCutOff);

    writer.Key("display_nucleotide_basis");
    writer.Bool(debugOptions.display_nucleotide_basis);

    writer.Key("display_base_pairing");
    writer.Bool(debugOptions.display_base_pairing);

    writer.Key("custom_bool");
    writer.Bool(debugOptions.customBool);

    writer.Key("custom_double");
    writer.Double(debugOptions.customDouble);

    writer.Key("custom_int");
    writer.Int(debugOptions.customInt);

    writer.EndObject();

    std::ofstream out(DEBUG_CONFIGPATH);
    out << s.GetString();
    out.close();
  }

  updateDebugConfig();

  debugConfigFileWatcher_.addPath(DEBUG_CONFIGPATH.c_str());

  QObject::connect(&debugConfigFileWatcher_, SIGNAL(fileChanged(const QString &)), this, SLOT(updateDebugConfig()));
}

void SEConfig::updateConfig() {
  FILE* fp = fopen(DEFAULT_CONFIGPATH.c_str(), "rb");
  if (fp != NULL) {
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    setting_.ParseStream(is);

    if (setting_.FindMember("show_overlay") != setting_.MemberEnd()) {
      show_overlay = setting_["show_overlay"].GetBool();
    }

    if (setting_.FindMember("min_melting_temp") != setting_.MemberEnd()) {
      min_melting_temp = setting_["min_melting_temp"].GetDouble();
    }

    if (setting_.FindMember("max_melting_temp") != setting_.MemberEnd()) {
      max_melting_temp = setting_["max_melting_temp"].GetDouble();
    }

    if (setting_.FindMember("min_gibbs_free_energy") != setting_.MemberEnd()) {
      min_gibbs_free_energy = setting_["min_gibbs_free_energy"].GetDouble();
    }

    if (setting_.FindMember("max_gibbs_free_energy") != setting_.MemberEnd()) {
      max_gibbs_free_energy = setting_["max_gibbs_free_energy"].GetDouble();
    }

    // not used currently
    if (setting_.FindMember("interpolate_dimensions") != setting_.MemberEnd()) {
      interpolate_dimensions = setting_["interpolate_dimensions"].GetBool();
    }

    // not used currently
    if (setting_.FindMember("animation_step_size") != setting_.MemberEnd()) {
      animation_step_size = setting_["animation_step_size"].GetDouble();
    }

    if (setting_.FindMember("mode") != setting_.MemberEnd()) {
      mode = static_cast<SEConfigMode>(setting_["mode"].GetInt());
    }

    if (setting_.FindMember("use_atomic_details") != setting_.MemberEnd()) {
      use_atomic_details = setting_["use_atomic_details"].GetBool();
    }

    if (setting_.FindMember("clear_log_file") != setting_.MemberEnd()) {
      clear_log_file = setting_["clear_log_file"].GetBool();
    }

    // not used currently
    if (setting_.FindMember("test_type") != setting_.MemberEnd()) {
      test_type = setting_["test_type"].GetInt();
    }

    // not used currently
    if (setting_.FindMember("automatic_camera") != setting_.MemberEnd()) {
      automatic_camera = setting_["automatic_camera"].GetBool();
    }

    if (setting_.FindMember("preview_editor") != setting_.MemberEnd()) {
      preview_editor = setting_["preview_editor"].GetBool();
    }

    // not used currently
    if (setting_.FindMember("crossover_distance_threshold") != setting_.MemberEnd()) {
      crossover_distance_threshold = setting_["crossover_distance_threshold"].GetDouble();
    }

    // not used currently
    if (setting_.FindMember("crossover_angle_threshold") != setting_.MemberEnd()) {
      crossover_angle_threshold = setting_["crossover_angle_threshold"].GetDouble();
    }

    // not used currently
    if (setting_.FindMember("display_possible_crossovers") != setting_.MemberEnd()) {
      display_possible_crossovers = setting_["display_possible_crossovers"].GetBool();
    }

    // not used currently
    if (setting_.FindMember("use_twist") != setting_.MemberEnd()) {
      use_twist = setting_["use_twist"].GetBool();
    }

    // not used currently
    if (setting_.FindMember("detect_possible_crossovers") != setting_.MemberEnd()) {
      detect_possible_crossovers = setting_["detect_possible_crossovers"].GetBool();
    }

    if (setting_.FindMember("dh_dist") != setting_.MemberEnd()) {
      dh_dist = setting_["dh_dist"].GetDouble();
    }

    if (setting_.FindMember("double_helix_V_color") != setting_.MemberEnd()) {
      Value& double_helix_V_colorVal = setting_["double_helix_V_color"];
      readDoubleArray(double_helix_V_colorVal, double_helix_V_color, 4);
    }

    if (setting_.FindMember("nucleotide_E_Color") != setting_.MemberEnd()) {
      Value& nucleotide_E_ColorVal = setting_["nucleotide_E_Color"];
      readDoubleArray(nucleotide_E_ColorVal, nucleotide_E_Color, 4);
    }

    if (setting_.FindMember("double_strand_color") != setting_.MemberEnd()) {
      Value& double_strand_colorVal = setting_["double_strand_color"];
      readDoubleArray(double_strand_colorVal, double_strand_color, 4);
    }

    if (setting_.FindMember("adenine_color") != setting_.MemberEnd()) {
      Value& adenine_colorVal = setting_["adenine_color"];
      readDoubleArray(adenine_colorVal, adenine_color, 4);
    }

    if (setting_.FindMember("thymine_color") != setting_.MemberEnd()) {
      Value& thymine_colorVal = setting_["thymine_color"];
      readDoubleArray(thymine_colorVal, thymine_color, 4);
    }

    if (setting_.FindMember("guanine_color") != setting_.MemberEnd()) {
      Value& guanine_colorVal = setting_["guanine_color"];
      readDoubleArray(guanine_colorVal, guanine_color, 4);
    }

    if (setting_.FindMember("cytosine_color") != setting_.MemberEnd()) {
      Value& cytosine_colorVal = setting_["cytosine_color"];
      readDoubleArray(cytosine_colorVal, cytosine_color, 4);
    }

    if (setting_.FindMember("staple_colors") != setting_.MemberEnd()) {
      Value& staple_colorsVal = setting_["staple_colors"];
      readDoubleArray(staple_colorsVal, staple_colors, 48);
    }

    if (setting_.FindMember("nucleotide_V_radius") != setting_.MemberEnd()) {
      nucleotide_V_radius = setting_["nucleotide_V_radius"].GetDouble();
    }

    if (setting_.FindMember("nucleotide_E_radius") != setting_.MemberEnd()) {
      nucleotide_E_radius = setting_["nucleotide_E_radius"].GetDouble();
    }

    if (setting_.FindMember("base_pair_radius") != setting_.MemberEnd()) {
      base_pair_radius = setting_["base_pair_radius"].GetDouble();
    }

    if (setting_.FindMember("num_staple_colors") != setting_.MemberEnd()) {
      num_staple_colors = setting_["num_staple_colors"].GetDouble();
    }

    if (setting_.FindMember("auto_set_scaffold_sequence") != setting_.MemberEnd()) {
      auto_set_scaffold_sequence = setting_["auto_set_scaffold_sequence"].GetBool();
    }

    if (setting_.FindMember("auto_calculate_binding_regions") != setting_.MemberEnd()) {
      auto_calculate_binding_regions = setting_["auto_calculate_binding_regions"].GetBool();
    }
  }
}

void SEConfig::writeDoubleArray(Writer<StringBuffer> & writer, std::string key, double * arr, int length) {
  writer.Key(key.c_str());

  writer.StartArray();
  for (int i = 0; i < length; ++i) {
    writer.Double(arr[i]);
  }
  
  writer.EndArray();
}

void SEConfig::readDoubleArray(Value & val, double * arr, int length) {
  for (int i = 0; i < length; ++i) {
    arr[i] = val[i].GetDouble();
  }
}

