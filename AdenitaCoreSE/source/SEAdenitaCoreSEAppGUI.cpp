#include "SEAdenitaCoreSEAppGUI.hpp"
#include "SEAdenitaCoreSEApp.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"
#include <QInputDialog>
#include "SEWireframeEditor.hpp"

SEAdenitaCoreSEAppGUI::SEAdenitaCoreSEAppGUI( SEAdenitaCoreSEApp* t ) : SBGApp( t ) {

	ui.setupUi( this );

  //change icons
  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/";

  QIcon loadIcon;
  loadIcon.addFile(string(iconPath + "load.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnLoad->setIcon(loadIcon);

  QIcon saveIcon;
  saveIcon.addFile(string(iconPath + "save.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSave->setIcon(saveIcon);

  QIcon exportIcon;
  exportIcon.addFile(string(iconPath + "export.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnExport->setIcon(exportIcon);

  QIcon searchIcon;
  searchIcon.addFile(string(iconPath + "search.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSearch->setIcon(searchIcon);

  QIcon createIcon;
  createIcon.addFile(string(iconPath + "create.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnCreate->setIcon(createIcon);

  QIcon setScaffIcon;
  setScaffIcon.addFile(string(iconPath + "setScaffold.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnSetScaff->setIcon(setScaffIcon);

  QIcon breakIcon;
  breakIcon.addFile(string(iconPath + "break.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnBreak->setIcon(breakIcon);

  QIcon connectSSIcon;
  connectSSIcon.addFile(string(iconPath + "connectSS.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnConnectSS->setIcon(connectSSIcon);

  QIcon connectDSIcon;
  connectDSIcon.addFile(string(iconPath + "connectDS.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnConnectDS->setIcon(connectDSIcon);

  QIcon mutateIcon;
  mutateIcon.addFile(string(iconPath + "mutate.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnMutate->setIcon(mutateIcon);

  QIcon insertIcon;
  insertIcon.addFile(string(iconPath + "insert.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnInsert->setIcon(insertIcon);

  QIcon deleteIcon;
  deleteIcon.addFile(string(iconPath + "delete.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnDelete->setIcon(deleteIcon);

  QIcon addLoopIcon;
  addLoopIcon.addFile(string(iconPath + "addLoop.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnAddLoop->setIcon(addLoopIcon);

  QIcon addSkipIcon;
  addSkipIcon.addFile(string(iconPath + "addSkip.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnAddSkip->setIcon(addSkipIcon);

  QIcon mapToAminoAcidIcon;
  mapToAminoAcidIcon.addFile(string(iconPath + "mapToAminoAcid.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnMapToAminoAcids->setIcon(mapToAminoAcidIcon);

  QIcon editAtomsIcon;
  editAtomsIcon.addFile(string(iconPath + "editAtoms.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnEditAtoms->setIcon(editAtomsIcon);

  QIcon paintIcon;
  paintIcon.addFile(string(iconPath + "paint.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnPaint->setIcon(paintIcon);



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

void SEAdenitaCoreSEAppGUI::onCreate()
{
  SBProxy* weClassProxy = SAMSON::getProxy("SEWireframeEditor");
  SEWireframeEditor* we = static_cast<SEWireframeEditor*>(SAMSON::getEditor(weClassProxy->getUUID(), weClassProxy->getElementUUID()));
  
  SAMSON::setActiveEditor(we);
  SAMSON::setActiveEditor(we); //has to be set active twice in order for ther popertyDialog to show up
  
}

void SEAdenitaCoreSEAppGUI::onLoadFile()
{
  SEConfig& config = SEConfig::GetInstance();

  if (config.mode == "haichao") {
    SEAdenitaCoreSEApp* t = getApp();
    t->ImportFromCadnano("C:/Development/Data_DNA_Nanomodeling/cadnano/hextube/hextube.json");

    t->ResetVisualModel(true);
    SAMSON::getActiveCamera()->center();
  }
  else {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open document: caDNAno, mesh (ply), Adenita document (adn, adnpart)"), QDir::currentPath(), tr("(Documents *.json *.ply *.adn *.adnpart)"));

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

      //add the visual model 
      t->ResetVisualModel(true);
    }

    SAMSON::getActiveCamera()->center();
  }

}

void SEAdenitaCoreSEAppGUI::onSaveFile()
{
  bool nanorobot = true;
  QStringList items;
  items << "Selected Part" << "Workspace";

  bool ok;
  QString item = QInputDialog::getItem(this, "Saving...", "Choose what you want to save:", items, 0, false, &ok);

  if (ok && !item.isEmpty()) {
    if (item == "Selected Part") {
      nanorobot = false;
    }
  }

  QString filename;
  if (nanorobot) {
    filename = QFileDialog::getSaveFileName(this, tr("Save the workspace"), QDir::currentPath(), tr("Adenita workspace (*.adn)"));
  }
  else {
    filename = QFileDialog::getSaveFileName(this, tr("Save a part"), QDir::currentPath(), tr("Adenita part (*.adnpart)"));
  }

  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->SaveFile(filename, nanorobot);
  }
}

void SEAdenitaCoreSEAppGUI::onExport()
{
  /*ADNAuxiliary::OxDNAOptions options;

  options.boxSizeX_ = ui.spnBoxSizeX->value();
  options.boxSizeY_ = ui.spnBoxSizeY->value();
  options.boxSizeZ_ = ui.spnBoxSizeZ->value();

  QString folder = QFileDialog::getExistingDirectory(this, tr("Choose an existing directory"), QDir::currentPath());
  if (!folder.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->ExportToOxDNA(folder, options);
  }*/

  QDialog* dialog = new QDialog();

  QStringList itemsSelection;
  itemsSelection << "Selected Part" << "Workspace";

  typeSelection_ = new QComboBox();
  typeSelection_->addItems(itemsSelection);

  QStringList itemsExportType;
  itemsExportType << "Sequence list" << "oxDNA";
  exportType_ = new QComboBox();
  exportType_->addItems(itemsExportType);

  QPushButton* acceptButton = new QPushButton(tr("Export"));
  acceptButton->setDefault(true);
  QPushButton* cancelButton = new QPushButton(tr("Cancel"));

  QDialogButtonBox* buttonBox_ = new QDialogButtonBox(Qt::Horizontal);
  buttonBox_->addButton(acceptButton, QDialogButtonBox::ActionRole);
  buttonBox_->addButton(cancelButton, QDialogButtonBox::ActionRole);

  QObject::connect(cancelButton, SIGNAL(released()), dialog, SLOT(close()));
  QObject::connect(acceptButton, SIGNAL(released()), this, SLOT(onAcceptExport()));

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(typeSelection_, 0, 0);
  mainLayout->addWidget(exportType_, 1, 0);
  mainLayout->addWidget(buttonBox_, 2, 0);

  dialog->setLayout(mainLayout);
  dialog->setWindowTitle(tr("Export design"));

  dialog->exec();
}

void SEAdenitaCoreSEAppGUI::onSetScaffold()
{
  std::string filename = "";
  ADNAuxiliary::ScaffoldSeq type = ADNAuxiliary::ScaffoldSeq(ui.cmbScaffolds->currentIndex());
  if (type == ADNAuxiliary::m13mp18) {
    filename = SB_ELEMENT_PATH + "/Data/m13mp18.fasta";
  }
  else if (type == ADNAuxiliary::p7249) {
    filename = SB_ELEMENT_PATH + "/Data/p7249.fasta";
  }
  else if (type == ADNAuxiliary::Custom) {
    QString fname = QFileDialog::getOpenFileName(this, tr("Select a .fasta file"), QDir::currentPath(), tr("Scaffold (*.fasta)"));
    if (!fname.isEmpty()) filename = fname.toStdString();
  }

  if (filename.size() > 0) {
    SEAdenitaCoreSEApp *t = getApp();
    std::string s = "";
    std::vector<std::string> lines;
    SBIFileReader::getFileLines(filename, lines);
    for (unsigned int i = 1; i < lines.size(); i++) {
      std::string line = lines[i];
      if (line[0] != '>') {
        s.append(line);
      }
    }
    t->SetScaffoldSequence(s);
  }

}

void SEAdenitaCoreSEAppGUI::onCenterPart()
{
  SEAdenitaCoreSEApp *t = getApp();
  t->CenterPart();
  SAMSON::getActiveCamera()->center();
}

void SEAdenitaCoreSEAppGUI::onAcceptExport()
{
  bool nanorobot = true;
  auto val = typeSelection_->currentIndex();
  if (val != 1) nanorobot = false;

  QString exportType = exportType_->currentText();

  SEAdenitaCoreSEApp* t = getApp();
  if (exportType == "Sequence List") {
    // export sequences
  }
  else if (exportType == "oxDNA") {
    ADNAuxiliary::OxDNAOptions options;
    options.boxSizeX_ = 0.0;
    options.boxSizeY_ = 0.0;
    options.boxSizeZ_ = 0.0;

    QString folder = QFileDialog::getExistingDirectory(this, tr("Choose an existing directory"), QDir::currentPath());
    t->ExportToOxDNA(folder, options, nanorobot);
  } 
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

SBCContainerUUID SEAdenitaCoreSEAppGUI::getUUID() const { return SBCContainerUUID( "386506A7-DD8B-69DD-4599-F136C1B91610" );}

QPixmap SEAdenitaCoreSEAppGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your app.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEAdenitaCoreSEAppIcon.png"));

}

QString SEAdenitaCoreSEAppGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your app inside SAMSON

	return "SEAdenitaCoreSEApp"; 

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
