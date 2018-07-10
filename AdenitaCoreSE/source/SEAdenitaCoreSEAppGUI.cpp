#include "SEAdenitaCoreSEAppGUI.hpp"
#include "SEAdenitaCoreSEApp.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEAdenitaCoreSEAppGUI::SEAdenitaCoreSEAppGUI( SEAdenitaCoreSEApp* t ) : SBGApp( t ) {

	ui.setupUi( this );

  //change icons
  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/";

  QIcon loadIcon;
  loadIcon.addFile(string(iconPath + "load.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
  ui.btnLoad->setIcon(loadIcon);
  ui.btnLoad->setIconSize(QSize(40, 40));

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

void SEAdenitaCoreSEAppGUI::onLoadPart()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Select an ANTPart .json file"), QDir::currentPath(), tr("Json (*.json)"));
  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->LoadPart(filename);
  }

  SAMSON::getActiveCamera()->center();
}

void SEAdenitaCoreSEAppGUI::onSavePart()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save an ANTPart .json file"), QDir::currentPath(), tr("Json (*.json)"));
  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->SavePart(filename);
  }
}

void SEAdenitaCoreSEAppGUI::onLoadPLYFile()
{
  int minEdgeSize = ui.spnDaedalusMinEdgeSize->value();
  QString filename = QFileDialog::getOpenFileName(this, tr("Select a .ply file"), QDir::currentPath(), tr("Mesh (*.ply)"));
  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->LoadPartWithDaedalus(filename, minEdgeSize);
  }

  SAMSON::getActiveCamera()->center();
}

void SEAdenitaCoreSEAppGUI::onImportFromCadnano()
{
  ADNConstants::CadnanoLatticeType typ = ADNConstants::CadnanoLatticeType::Honeycomb;
  if (ui.rdoCadnanoSquareLattice->isChecked()) {
    typ = ADNConstants::CadnanoLatticeType::Square;
  }
  QString filename = QFileDialog::getOpenFileName(this, tr("Select a .json file"), QDir::currentPath(), tr("Cadnano (*.json)"));
  if (!filename.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->ImportFromCadnano(filename, typ);
  }

  SAMSON::getActiveCamera()->center();
}

void SEAdenitaCoreSEAppGUI::onExportToOxDNA()
{
  ADNAuxiliary::OxDNAOptions options;

  options.boxSizeX_ = ui.spnBoxSizeX->value();
  options.boxSizeY_ = ui.spnBoxSizeY->value();
  options.boxSizeZ_ = ui.spnBoxSizeZ->value();

  QString folder = QFileDialog::getExistingDirectory(this, tr("Choose an existing directory"), QDir::currentPath());
  if (!folder.isEmpty()) {
    SEAdenitaCoreSEApp* t = getApp();
    t->ExportToOxDNA(folder, options);
  }
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
