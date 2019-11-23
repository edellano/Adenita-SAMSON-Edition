#include "SEAdenitaCoreSEAppGUI.hpp"
#include "SEAdenitaCoreSEApp.hpp"
#include "SEAdenitaCoreSettingsGUI.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"
#include <QInputDialog>
#include <QFormLayout>
#include "SEWireframeEditor.hpp"
#include "SEBreakEditor.hpp"
#include "SEConnectSSDNAEditor.hpp"
#include "SEDeleteEditor.hpp"
#include "SETwistHelixEditor.hpp"
#include "SEDNATwisterEditor.hpp"
#include "SETaggingEditor.hpp"
#include "SELatticeCreatorEditor.hpp"
#include "SENanotubeCreatorEditor.hpp"
#include "SEDSDNACreatorEditor.hpp"
#include "SEMergePartsEditor.hpp"
#include <QPixmap>
#include <QTimer>

SEAdenitaCoreSEAppGUI::SEAdenitaCoreSEAppGUI( SEAdenitaCoreSEApp* t ) : SBGApp( t ) {

	ui.setupUi( this );

  //change icons
  string iconsPath = SB_ELEMENT_PATH + "/Resource/icons/";

  QIcon loadIcon;
  loadIcon.addFile(string(iconsPath + "load.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnLoad->setIcon(loadIcon);

  QIcon saveAllIcon;
  saveAllIcon.addFile(string(iconsPath + "saveAll.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSaveAll->setIcon(saveAllIcon);

  QIcon saveSelectionIcon;
  saveSelectionIcon.addFile(string(iconsPath + "saveSelection.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSaveSelection->setIcon(saveSelectionIcon);

  QIcon exportIcon; 
  exportIcon.addFile(string(iconsPath + "export.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnExport->setIcon(exportIcon);

  QIcon setScaffIcon;
  setScaffIcon.addFile(string(iconsPath + "setScaffold.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSetScaff->setIcon(setScaffIcon);
  
  QIcon calcProperties;
  calcProperties.addFile(string(iconsPath + "calculate.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnCalculateBindingProperties->setIcon(calcProperties);

  QIcon setStart;
  setStart.addFile(string(iconsPath + "set5Prime.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSetStart->setIcon(setStart);

  QIcon createBp;
  createBp.addFile(string(iconsPath + "createBasePair.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnCreateBasePair->setIcon(createBp);

  QIcon generateSequences;
  generateSequences.addFile(string(iconsPath + "generateSeqs.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnGenerateSequence->setIcon(generateSequences);

  // Editor icons
  QIcon breakEditor;
  breakEditor.addFile(string(iconsPath + "break.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnBreakEditor->setIcon(breakEditor);

  QIcon deleteEditor;
  deleteEditor.addFile(string(iconsPath + "delete.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnDeleteEditor->setIcon(deleteEditor);

  QIcon connectEditor;
  connectEditor.addFile(string(iconsPath + "connectDS.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnConnectEditor->setIcon(connectEditor);

  QIcon twistDsDNA;
  twistDsDNA.addFile(string(iconsPath + "twistDS.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnDNATwisterEditor->setIcon(twistDsDNA);

  QIcon mergePartsEditor;
  mergePartsEditor.addFile(string(iconsPath + "merge.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnMergePartsEditor->setIcon(mergePartsEditor);

  QIcon strandCreator;
  strandCreator.addFile(string(iconsPath + "SEDSDNACreatorEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnDsDNACreatorEditor->setIcon(strandCreator);

  QIcon nanotubeEditor;
  nanotubeEditor.addFile(string(iconsPath + "SENanotubeCreatorEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnNanotubeCreator->setIcon(nanotubeEditor);

  QIcon latticeEditor;
  latticeEditor.addFile(string(iconsPath + "SELatticeCreatorEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnLatticeCreatorEditor->setIcon(latticeEditor);

  QIcon wireframeEditor;
  wireframeEditor.addFile(string(iconsPath + "SEWireframeEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnWireframeEditor->setIcon(wireframeEditor);

  QIcon tagEditor;
  tagEditor.addFile(string(iconsPath + "SETaggingEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnTaggingEditor->setIcon(tagEditor);

  QIcon dnaTwister;
  dnaTwister.addFile(string(iconsPath + "SEDNATwisterEditorIcon.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnTwisterEditor->setIcon(dnaTwister);

  QIcon settings;
  settings.addFile(string(iconsPath + "settings.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSettings->setIcon(settings);

  // disable debug menu if compiling in release mode
  #if NDEBUG
  ui.tabWidget->removeTab(2);
  #endif

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(CheckForLoadedParts()));
  timer->start();
}

SEAdenitaCoreSEAppGUI::~SEAdenitaCoreSEAppGUI() {

}

SEAdenitaCoreSEApp* SEAdenitaCoreSEAppGUI::getApp() const { return static_cast<SEAdenitaCoreSEApp*>(SBGApp::getApp()); }

void SEAdenitaCoreSEAppGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your app can save its GUI state from one session to the next

}

void SEAdenitaCoreSEAppGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your app can save its GUI state from one session to the next

}

std::string SEAdenitaCoreSEAppGUI::GetScaffoldFilename()
{
  SEConfig& c = SEConfig::GetInstance();

  std::string filename = "";
  ADNAuxiliary::ScaffoldSeq type = ADNAuxiliary::ScaffoldSeq(c.scaffType);
  if (type == ADNAuxiliary::m13mp18) {
    filename = SB_ELEMENT_PATH + "/Data/m13mp18.fasta";
  }
  else if (type == ADNAuxiliary::p7249) {
    filename = SB_ELEMENT_PATH + "/Data/p7249.fasta";
  }
  else if (type == ADNAuxiliary::Custom) {
    filename = c.scaffCustomFilename;
  }

  return filename;
}

void SEAdenitaCoreSEAppGUI::onLoadFile()
{
  SEConfig& config = SEConfig::GetInstance();
    
  if (config.mode == SEConfigMode::HAICHAO) {
    
    SEAdenitaCoreSEApp* t = getApp();
    t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/hextube/hextube.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/inner_ring_and_sheet/inner ring.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/samples/skip.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/samples/simple.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/samples/loop.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/samples/loop_skip.json");
    //t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/robot_bent/robot_v1.9_bent.json");

    t->ResetVisualModel();
    SAMSON::getActiveCamera()->center();

  }
  else if (config.mode == SEConfigMode::ELISA) {

    SEAdenitaCoreSEApp* t = getApp();
    t->LoadPartWithDaedalus("C:/Users/DeLlanoE/Documents/code/Data_DNA_Nanomodeling/daedalus_samples/46_reinforced_cube.ply", 42);

    t->ResetVisualModel();
    SAMSON::getActiveCamera()->center();

  }
  else {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open document: caDNAno, mesh (ply), Adenita document (adn, adnpart)"), QDir::currentPath(), tr("(Documents *.json *.ply *.adn *.adnpart)"));
    bool loadVM = true;

    if (!filename.isEmpty()) {
      SEAdenitaCoreSEApp* t = getApp();

      if (filename.endsWith(".json")) {
        // either cadnano or old Adenita format
        std::string format = IsJsonCadnano(filename);
        if (format == "cadnano") {
          t->ImportFromCadnano(filename);
        }
        else if (format == "adenita") {
          t->LoadPart(filename);
        }
        else {
          QMessageBox msgBox;
          loadVM = false;
          msgBox.setText("Unknown json format. Current supported formats include Cadnano and legacy Adenita parts");
          msgBox.exec();
        }
      }
      else if (filename.endsWith(".ply")) {
        bool ok;
        int i = QInputDialog::getInt(this, tr("Wireframe structure (Daedalus)"),
          tr("Minimum edge size (bp): "), 42, 31, 1050, 1, &ok);
        if (ok) {
          div_t d = div(i, 10.5);
          int minSize = floor(d.quot * 10.5);
          t->LoadPartWithDaedalus(filename, minSize);
        }
      }
      else if (filename.endsWith(".adnpart")) {
        t->LoadPart(filename);
      }
      else if (filename.endsWith(".adn")) {
        t->LoadParts(filename);
      }
      else {
        loadVM = false;
      }

      //add the visual model
      if (loadVM) { 
        t->ResetVisualModel();
      }
    }
    SAMSON::getActiveCamera()->center();
  }
  
  SAMSON::hideProgressBar();

}

void SEAdenitaCoreSEAppGUI::onSaveFile()
{
  SEAdenitaCoreSEApp* t = getApp();

  QString filename = QFileDialog::getSaveFileName(this, tr("Save the workspace"), QDir::currentPath(), tr("Adenita workspace (*.adn)"));
  if (!filename.isEmpty()) {
    t->SaveFile(filename);
  }
}

void SEAdenitaCoreSEAppGUI::onExport()
{
  SEAdenitaCoreSEApp* t = getApp();

  QDialog* dialog = new QDialog();

  QComboBox* typeSelection = new QComboBox();
  
  auto nr = t->GetNanorobot();
  auto parts = nr->GetParts();
  int i = 0;
  std::map<int, ADNPointer<ADNPart>> indexParts;
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    std::string n = p->GetName();
    typeSelection->insertItem(i, QString::fromStdString(n));
    indexParts.insert(std::make_pair(i, p));
    ++i;
  }
  typeSelection->insertItem(i, QString::fromStdString("SelectedPart"));
  int sel_idx = i;
  typeSelection->insertItem(i+1, QString::fromStdString("Workspace"));
  int all_idx = i + 1;

  QStringList itemsExportType;
  itemsExportType << "Sequence list" << "oxDNA";
  QComboBox* exportType = new QComboBox();
  exportType->addItems(itemsExportType);

  QPushButton* acceptButton = new QPushButton(tr("Export"));
  acceptButton->setDefault(true);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"));

  QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(acceptButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

  QObject::connect(cancelButton, SIGNAL(released()), dialog, SLOT(close()));
  QObject::connect(acceptButton, SIGNAL(released()), dialog, SLOT(accept()));

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(typeSelection, 0, 0);
  mainLayout->addWidget(exportType, 1, 0);
  mainLayout->addWidget(buttonBox, 2, 0);

  dialog->setLayout(mainLayout);
  dialog->setWindowTitle(tr("Export design"));

  int dialogCode = dialog->exec();

  if (dialogCode == QDialog::Accepted) {
    
    auto val = typeSelection->currentIndex();
    ADNPointer<ADNPart> part = nullptr;
    if (val == sel_idx) {
      part = nr->GetSelectedParts()[0];
    }
    else if (val != all_idx) {
      part = indexParts.at(val);
    }

    QString eType = exportType->currentText();

    if (eType == "Sequence list") {
      // export sequences
      auto filename = QFileDialog::getSaveFileName(this, tr("Sequence List"), QDir::currentPath(), tr("Sequence List (*.csv)"));
      t->ExportToSequenceList(filename, part);
    }
    else if (eType == "oxDNA") {
      std::pair<SBPosition3, SBPosition3> boundingBox = part->GetBoundingBox();
      auto bbSize = boundingBox.second - boundingBox.first;

      ADNAuxiliary::OxDNAOptions options;
      double sysX = bbSize[0].getValue() * 0.001;  // nm
      double sysY = bbSize[1].getValue() * 0.001;  // nm
      double sysZ = bbSize[2].getValue() * 0.001;  // nm

      double refVal = max(sysX, sysY);
      refVal = max(refVal, sysZ);

      // oxDNA dialog
      QDialog* dialogOxDNA = new QDialog();
      QFormLayout *oxDNALayout = new QFormLayout;
      QLabel* info = new QLabel;
      info->setText("System size (nm): " + QString::number(sysX, 'g',2) + " x " + QString::number(sysY, 'g', 2) + " x " + QString::number(sysZ, 'g', 2));
      QDoubleSpinBox* boxX = new QDoubleSpinBox();
      boxX->setRange(0.0, 99999.9);
      boxX->setValue(refVal * 3);
      boxX->setDecimals(2);
      QDoubleSpinBox* boxY = new QDoubleSpinBox();
      boxY->setRange(0.0, 99999.9);
      boxY->setValue(refVal * 3);
      boxY->setDecimals(2);
      QDoubleSpinBox* boxZ = new QDoubleSpinBox();
      boxZ->setRange(0.0, 99999.9);
      boxZ->setValue(refVal * 3);
      boxZ->setDecimals(2);

      QPushButton* accButton = new QPushButton(tr("Continue"));
      accButton->setDefault(true);
      QPushButton* cButton = new QPushButton(tr("Cancel"));

      QDialogButtonBox* bttBox = new QDialogButtonBox(Qt::Horizontal);
      bttBox->addButton(accButton, QDialogButtonBox::ActionRole);
      bttBox->addButton(cButton, QDialogButtonBox::ActionRole);

      QObject::connect(cButton, SIGNAL(released()), dialogOxDNA, SLOT(close()));
      QObject::connect(accButton, SIGNAL(released()), dialogOxDNA, SLOT(accept()));

      oxDNALayout->addRow(info);
      oxDNALayout->addRow(QString("Box size X (nm)"), boxX);
      oxDNALayout->addRow(QString("Box size Y (nm)"), boxY);
      oxDNALayout->addRow(QString("Box size Z (nm)"), boxZ);
      oxDNALayout->addRow(bttBox);

      dialogOxDNA->setLayout(oxDNALayout);
      dialogOxDNA->setWindowTitle(tr("Set bounding box size"));

      int dCode = dialogOxDNA->exec();

      if (dCode == QDialog::Accepted) {
        options.boxSizeX_ = boxX->value();
        options.boxSizeY_ = boxY->value();
        options.boxSizeZ_ = boxZ->value();

        QString folder = QFileDialog::getExistingDirectory(this, tr("Choose an existing directory"), QDir::currentPath(), QFileDialog::DontUseNativeDialog);
        t->ExportToOxDNA(folder, options, part);
      }
    }

  }
}

void SEAdenitaCoreSEAppGUI::onSaveSelection()
{
  SEAdenitaCoreSEApp* t = getApp();

  QDialog* dialog = new QDialog();

  QComboBox* typeSelection = new QComboBox();

  auto nr = t->GetNanorobot();
  auto parts = nr->GetParts();
  int i = 0;
  std::map<int, ADNPointer<ADNPart>> indexParts;
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    std::string n = p->GetName();
    typeSelection->insertItem(i, QString::fromStdString(n));
    indexParts.insert(std::make_pair(i, p));
    ++i;
  }
  typeSelection->insertItem(i, QString::fromStdString("Selected Component"));
  int sel_idx = i;

  QPushButton* acceptButton = new QPushButton(tr("Save"));
  acceptButton->setDefault(true);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"));

  QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(acceptButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

  QObject::connect(cancelButton, SIGNAL(released()), dialog, SLOT(close()));
  QObject::connect(acceptButton, SIGNAL(released()), dialog, SLOT(accept()));

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(typeSelection, 0, 0);
  mainLayout->addWidget(buttonBox, 1, 0);

  dialog->setLayout(mainLayout);
  dialog->setWindowTitle(tr("Export design"));

  int dialogCode = dialog->exec();

  if (dialogCode == QDialog::Accepted) {
    auto val = typeSelection->currentIndex();
    ADNPointer<ADNPart> part = nullptr;
    if (val == sel_idx) {
      part = nr->GetSelectedParts()[0];
    }
    else {
      part = indexParts.at(val);
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save a part"), QDir::currentPath(), tr("Adenita part (*.adnpart)"));

    if (!filename.isEmpty()) {
      t->SaveFile(filename, part);
    }
  }
}

void SEAdenitaCoreSEAppGUI::onSetScaffold()
{
  std::string filename = GetScaffoldFilename();
  SEAdenitaCoreSEApp *t = getApp();
  t->SetScaffoldSequence(filename);
}

void SEAdenitaCoreSEAppGUI::onAddNtThreeP()
{
  SEAdenitaCoreSEApp *t = getApp();
  int numNt = 1;
  t->AddNtThreeP(numNt);
}

void SEAdenitaCoreSEAppGUI::onCenterPart()
{
  SEAdenitaCoreSEApp *t = getApp();
  t->CenterPart();
  SAMSON::getActiveCamera()->center();
}

void SEAdenitaCoreSEAppGUI::onCatenanes()
{
  QDialog* dialog = new QDialog();

  QLabel* numberLabel = new QLabel();
  numberLabel->setText("Number of catenanes");
  QSpinBox* number = new QSpinBox();
  number->setRange(1, 9999);
  number->setValue(2);

  QLabel* radiusLabel = new QLabel();
  radiusLabel->setText("Radius (nm)");
  QDoubleSpinBox* radius = new QDoubleSpinBox();
  radius->setRange(0.0, 99999.9);
  radius->setValue(20.0);
  radius->setDecimals(2);

  QPushButton* acceptButton = new QPushButton(tr("Create catenanes"));
  acceptButton->setDefault(true);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"));

  QDialogButtonBox* buttonBox_ = new QDialogButtonBox(Qt::Horizontal);
  buttonBox_->addButton(acceptButton, QDialogButtonBox::ActionRole);
  buttonBox_->addButton(cancelButton, QDialogButtonBox::ActionRole);

  QObject::connect(cancelButton, SIGNAL(released()), dialog, SLOT(reject()));
  QObject::connect(acceptButton, SIGNAL(released()), dialog, SLOT(accept()));

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(numberLabel, 0, 0);
  mainLayout->addWidget(number, 0, 1);
  mainLayout->addWidget(radiusLabel, 1, 0);
  mainLayout->addWidget(radius, 1, 1);
  mainLayout->addWidget(buttonBox_, 2, 0);

  dialog->setLayout(mainLayout);
  dialog->setWindowTitle(tr("Create Catenanes"));

  int dialogCode = dialog->exec();

  if (dialogCode == QDialog::Accepted ) {
    int num = number->value();
    SBQuantity::length R = SBQuantity::nanometer(radius->value());
    SBVector3 normal = SBVector3();
    normal[0] = 0.0;
    normal[1] = 0.0;
    normal[2] = 1.0;
    SBPosition3 center = SBPosition3();
    SEAdenitaCoreSEApp* t = getApp();
    t->LinearCatenanes(R, center, normal, num);
  }
}

void SEAdenitaCoreSEAppGUI::onKinetoplast()
{
  QDialog* dialog = new QDialog();

  QLabel* rowsLabel = new QLabel();
  rowsLabel->setText("Number of rows");
  QSpinBox* rows = new QSpinBox();
  rows->setRange(1, 9999);
  rows->setValue(2);

  QLabel* colsLabel = new QLabel();
  colsLabel->setText("Number of columns");
  QSpinBox* cols = new QSpinBox();
  cols->setRange(1, 9999);
  cols->setValue(3);

  QLabel* radiusLabel = new QLabel();
  radiusLabel->setText("Radius (nm)");
  QDoubleSpinBox* radius = new QDoubleSpinBox();
  radius->setRange(0.0, 99999.9);
  radius->setValue(20.0);
  radius->setDecimals(2);

  QPushButton* acceptButton = new QPushButton(tr("Create catenanes"));
  acceptButton->setDefault(true);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"));

  QDialogButtonBox* buttonBox_ = new QDialogButtonBox(Qt::Horizontal);
  buttonBox_->addButton(acceptButton, QDialogButtonBox::ActionRole);
  buttonBox_->addButton(cancelButton, QDialogButtonBox::ActionRole);

  QObject::connect(cancelButton, SIGNAL(released()), dialog, SLOT(reject()));
  QObject::connect(acceptButton, SIGNAL(released()), dialog, SLOT(accept()));

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(rowsLabel, 0, 0);
  mainLayout->addWidget(rows, 0, 1);
  mainLayout->addWidget(colsLabel, 1, 0);
  mainLayout->addWidget(cols, 1, 1);
  mainLayout->addWidget(radiusLabel, 2, 0);
  mainLayout->addWidget(radius, 2, 1);
  mainLayout->addWidget(buttonBox_, 3, 0);

  dialog->setLayout(mainLayout);
  dialog->setWindowTitle(tr("Create Kinetoplast"));

  int dialogCode = dialog->exec();

  if (dialogCode == QDialog::Accepted) {
    int r = rows->value();
    int c = cols->value();
    SBQuantity::length R = SBQuantity::nanometer(radius->value());
    SBVector3 normal = SBVector3();
    normal[0] = 0.0;
    normal[1] = 0.0;
    normal[2] = 1.0;
    SBPosition3 center = SBPosition3();
    SEAdenitaCoreSEApp* t = getApp();
    t->Kinetoplast(R, center, normal, r, c);
  }
}

void SEAdenitaCoreSEAppGUI::onCalculateBindingProperties()
{

  QDialog dialog(this);
  QFormLayout form(&dialog);

  form.addRow(new QLabel("Parameters for Calculating the Binding Region Properties"));

  //oligo conc
  QLineEdit *oligoConcText = new QLineEdit(&dialog);
  oligoConcText->setText("100");
  form.addRow(new QLabel("Oligo Conc"), oligoConcText);

  QLineEdit *monovalentConcText = new QLineEdit(&dialog);
  monovalentConcText->setText("5");
  form.addRow(new QLabel("Concentration of monovalent cations"), monovalentConcText);

  QLineEdit *divalentConcText = new QLineEdit(&dialog);
  divalentConcText->setText("16");
  form.addRow(new QLabel("Concentration of divalent cations"), divalentConcText);

  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
  form.addRow(&buttonBox);
  QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

  if (dialog.exec() == QDialog::Accepted) {

    bool oligoConcOk = false;
    int oligoConc = oligoConcText->text().toInt(&oligoConcOk);

    bool monovalentConcOk = false;
    int monovalentConc = monovalentConcText->text().toInt(&monovalentConcOk);

    bool divalentConcOk = false;
    int divalentConc = divalentConcText->text().toInt(&divalentConcOk);

    if (oligoConcOk && monovalentConcOk && divalentConcOk) {
      SEAdenitaCoreSEApp* t = getApp();

      bool res = t->CalculateBindingRegions(oligoConc, monovalentConc, divalentConc);

      if (res) {
        SEAdenitaVisualModel* adenitaVm = static_cast<SEAdenitaVisualModel*>(t->GetVisualModel());
        if (adenitaVm != nullptr) {
          adenitaVm->changePropertyColors(1, 0);
          SAMSON::requestViewportUpdate();
        }
      }
    }
    else {
      QMessageBox messageBox;
      messageBox.critical(0, "Error", "Check whether the entered values are numeric! Check if the Structural Model is selected");
      messageBox.setFixedSize(500, 200);
    }
  }
}

void SEAdenitaCoreSEAppGUI::onSetStart()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->SetStart();
}

void SEAdenitaCoreSEAppGUI::onTestNeighbors()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->TestNeighbors();
}

void SEAdenitaCoreSEAppGUI::onOxDNAImport()
{
  QString topoFile = QFileDialog::getOpenFileName(this, tr("Select a topology file"), QDir::currentPath(), tr("(OxDNA topology *.*)"));
  QString configFile = QFileDialog::getOpenFileName(this, tr("Select a configuration file"), QDir::currentPath(), tr("(OxDNA configuration *.*)"));
  SEAdenitaCoreSEApp* t = getApp();
  t->ImportFromOxDNA(topoFile.toStdString(), configFile.toStdString());
}

void SEAdenitaCoreSEAppGUI::onFromDatagraph()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->FromDatagraph();
}

void SEAdenitaCoreSEAppGUI::onHighlightXOs()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->HighlightXOs();
}

void SEAdenitaCoreSEAppGUI::onHighlightPosXOs()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->HighlightPosXOs();
}

void SEAdenitaCoreSEAppGUI::onExportToCanDo()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Choose a filename"), QDir::currentPath(), tr("(CanDo .cndo)"));
  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->ExportToCanDo(filename);
  }
}

void SEAdenitaCoreSEAppGUI::onFixDesigns()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->FixDesigns();
}

void SEAdenitaCoreSEAppGUI::onCreateBasePair()
{
  SEAdenitaCoreSEApp* t = getApp();
  t->CreateBasePair();
}

void SEAdenitaCoreSEAppGUI::onGenerateSequence()
{
  QDialog dialog(this);
  QFormLayout form(&dialog);

  form.addRow(new QLabel("Generate Sequence"));

  // gc content
  QDoubleSpinBox *gcCont = new QDoubleSpinBox(&dialog);
  gcCont->setMaximum(100.0);
  gcCont->setMinimum(0.0);
  gcCont->setValue(40.0);
  form.addRow(new QLabel("GC Content"), gcCont);

  QSpinBox *contigousGs = new QSpinBox(&dialog);
  contigousGs->setMinimum(0);
  contigousGs->setMaximum(100);
  form.addRow(new QLabel("Maximum amount of contiguous Gs"), contigousGs);

  QCheckBox* chk = new QCheckBox(&dialog);
  chk->setChecked(false);
  form.addRow(new QLabel("Overwrite existing base pairs"), chk);

  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
  form.addRow(&buttonBox);
  QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

  if (dialog.exec() == QDialog::Accepted) {
    double gc100 = gcCont->value() / 100;
    int maxContGs = contigousGs->value();
    bool overwrite = chk->isChecked();
    SEAdenitaCoreSEApp* t = getApp();
    t->GenerateSequence(gc100, maxContGs, overwrite);
  }
}

void SEAdenitaCoreSEAppGUI::onSettings()
{
  SEAdenitaCoreSettingsGUI diag(this);
  diag.exec();
}

void SEAdenitaCoreSEAppGUI::onBreakEditor()
{
  SEBreakEditor* be = static_cast<SEBreakEditor*>(SAMSON::getEditor(SBCContainerUUID("CFACD1E5-FCD1-916F-2CF7-4B60979F1A77"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(be);
  HighlightEditor(ui.btnBreakEditor);
}

void SEAdenitaCoreSEAppGUI::onConnectEditor()
{
  SEConnectSSDNAEditor* c = static_cast<SEConnectSSDNAEditor*>(SAMSON::getEditor(SBCContainerUUID("48FDCE78-A55E-FDA2-237E-319202E56080"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnConnectEditor);
}

void SEAdenitaCoreSEAppGUI::onDeleteEditor()
{
  SEDeleteEditor* c = static_cast<SEDeleteEditor*>(SAMSON::getEditor(SBCContainerUUID("592B8158-15E9-B621-0BCB-D7DA210FF149"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnDeleteEditor);
}

void SEAdenitaCoreSEAppGUI::onDNATwistEditor()
{
  SETwistHelixEditor* c = static_cast<SETwistHelixEditor*>(SAMSON::getEditor(SBCContainerUUID("4B60FECA-2A79-680F-F289-B4908A924409"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnDNATwisterEditor);
}

void SEAdenitaCoreSEAppGUI::onMergePartsEditor()
{
  SEMergePartsEditor* c = static_cast<SEMergePartsEditor*>(SAMSON::getEditor(SBCContainerUUID("EB812444-8EA8-BD83-988D-AFF5987461D8"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnMergePartsEditor);
}

void SEAdenitaCoreSEAppGUI::onCreateStrandEditor()
{
  SEDSDNACreatorEditor* c = static_cast<SEDSDNACreatorEditor*>(SAMSON::getEditor(SBCContainerUUID("86204A08-DFD6-97A8-2BE2-4CFC8B4169A3"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnDsDNACreatorEditor);
}

void SEAdenitaCoreSEAppGUI::onNanotubeCreatorEditor()
{
  SENanotubeCreatorEditor* c = static_cast<SENanotubeCreatorEditor*>(SAMSON::getEditor(SBCContainerUUID("4B6A0B18-48B5-233A-28A4-BA3EF3D56AB8"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnNanotubeCreator);
}

void SEAdenitaCoreSEAppGUI::onLatticeCreatorEditor()
{
  SELatticeCreatorEditor* c = static_cast<SELatticeCreatorEditor*>(SAMSON::getEditor(SBCContainerUUID("EA67625E-89B5-2EEA-156D-FC836214B0E4"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnLatticeCreatorEditor);
}

void SEAdenitaCoreSEAppGUI::onWireframeEditor()
{
  SEWireframeEditor* c = static_cast<SEWireframeEditor*>(SAMSON::getEditor(SBCContainerUUID("F1F29042-3D87-DA61-BC5C-D3348EB2E1FA"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnWireframeEditor);
}

void SEAdenitaCoreSEAppGUI::onTaggingEditor()
{
  SETaggingEditor* c = static_cast<SETaggingEditor*>(SAMSON::getEditor(SBCContainerUUID("473D2F88-5D06-25F5-EB58-053661504C43"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnTaggingEditor);
}

void SEAdenitaCoreSEAppGUI::onTwisterEditor()
{
  SEDNATwisterEditor* c = static_cast<SEDNATwisterEditor*>(SAMSON::getEditor(SBCContainerUUID("677B1667-7856-12E6-5901-E8EAC729501A"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  SAMSON::setActiveEditor(c);
  HighlightEditor(ui.btnTwisterEditor);
}

std::string SEAdenitaCoreSEAppGUI::IsJsonCadnano(QString filename)
{
  std::string format = "unknown";

  FILE* fp;
  fopen_s(&fp, filename.toStdString().c_str(), "rb");
  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  if (d.HasMember("vstrands")) {
    format = "cadnano";
  }
  else if (d.HasMember("doubleStrands")) {
    format = "adenita";
  }

  return format;
}

void SEAdenitaCoreSEAppGUI::HighlightEditor(QToolButton* b)
{
  // remove current
  if (highlightedEditor_ != nullptr) highlightedEditor_->setStyleSheet(QString("border: none"));
  b->setStyleSheet(QString("border: 2px solid #FFFFFF"));
  highlightedEditor_ = b;
}

void SEAdenitaCoreSEAppGUI::CheckForLoadedParts()
{
  SEAdenitaCoreSEApp* adenita = getApp();
  SBNodeIndexer nodeIndexer;
  SAMSON::getActiveDocument()->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNPointer<ADNPart> part = static_cast<ADNPart*>(n);
    adenita->AddLoadedPartToNanorobot(part);
  }
}

void SEAdenitaCoreSEAppGUI::keyPressEvent(QKeyEvent* event)
{
  if (event->modifiers() == Qt::ControlModifier) {
    {
      SBProxy* ep = nullptr;

      if (event->key() == Qt::Key_D) {
        ep = SAMSON::getProxy("SEDeleteEditor");
      }

      if (event->key() == Qt::Key_B) {
        ep = SAMSON::getProxy("SEBreakEditor");
      }

      if (event->key() == Qt::Key_G) {
        ep = SAMSON::getProxy("SEConnectSSDNAEditor");
      }

      if (ep) {
        SBEditor * e = SAMSON::getEditor(ep->getUUID(), ep->getElementUUID());
        SAMSON::setActiveEditor(e);
      }
    }
  }
}


SBCContainerUUID SEAdenitaCoreSEAppGUI::getUUID() const { return SBCContainerUUID( "386506A7-DD8B-69DD-4599-F136C1B91610" );}

QPixmap SEAdenitaCoreSEAppGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your app.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEAdenitaCoreSEAppIcon.png"));

}

QString SEAdenitaCoreSEAppGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your app inside SAMSON

	return "Adenita"; 

}

int SEAdenitaCoreSEAppGUI::getFormat() const { 
	
	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)
	
	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEAdenitaCoreSEAppGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this app in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";
	
}