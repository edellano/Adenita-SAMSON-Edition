#include "SEMergePartsEditorGUI.hpp"
#include "SEMergePartsEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEMergePartsEditorGUI::SEMergePartsEditorGUI(SEMergePartsEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;
}

SEMergePartsEditorGUI::~SEMergePartsEditorGUI() {

}

SEMergePartsEditor* SEMergePartsEditorGUI::getEditor() const { return editor; }

void SEMergePartsEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEMergePartsEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEMergePartsEditorGUI::updatePartsList() {

  SEMergePartsEditor* editor = getEditor();
  auto indexParts = editor->getPartsList();
  auto indexElements = editor->getElementsList();

  int sel1 = ui.cmbPart1->currentIndex();
  int sel2 = ui.cmbPart2->currentIndex();
  int sel3 = ui.cmbPartM->currentIndex();
  int sel4 = ui.cmbElement->currentIndex();
  ui.cmbPart1->clear();
  ui.cmbPart2->clear();
  ui.cmbPartM->clear();
  ui.cmbElement->clear();

  ui.cmbPart1->insertItem(0, QString::fromStdString("None"));
  ui.cmbPart2->insertItem(0, QString::fromStdString("None"));
  ui.cmbPartM->insertItem(0, QString::fromStdString("None"));
  ui.cmbElement->insertItem(0, QString::fromStdString("None"));

  for (auto& pair : indexParts) {
    int i = pair.first;
    ADNPointer<ADNPart> p = pair.second;
    std::string n = p->GetName();
    ui.cmbPart1->insertItem(i, QString::fromStdString(n));
    ui.cmbPart2->insertItem(i, QString::fromStdString(n));
    ui.cmbPartM->insertItem(i, QString::fromStdString(n));
  }

  for (auto& pair : indexElements) {
    int i = pair.first;
    auto p = pair.second;
    std::string n = p.GetName();
    
    ui.cmbElement->insertItem(i, QString::fromStdString(n));
  }

  if (indexParts.find(sel1) != indexParts.end()) ui.cmbPart1->setCurrentIndex(sel1);
  if (indexParts.find(sel2) != indexParts.end()) ui.cmbPart2->setCurrentIndex(sel2);
  if (indexParts.find(sel3) != indexParts.end()) ui.cmbPartM->setCurrentIndex(sel3);
  if (indexParts.find(sel4) != indexParts.end()) ui.cmbElement->setCurrentIndex(sel4);
}

void SEMergePartsEditorGUI::onMerge()
{
  int sel1 = ui.cmbPart1->currentIndex();
  int sel2 = ui.cmbPart2->currentIndex();

  SEMergePartsEditor* e = getEditor();
  e->MergeParts(sel1, sel2);
}

void SEMergePartsEditorGUI::onMove()
{
  int sel1 = ui.cmbElement->currentIndex();
  int sel2 = ui.cmbPartM->currentIndex();

  SEMergePartsEditor* e = getEditor();
  e->MoveElement(sel1, sel2);
}

SBCContainerUUID SEMergePartsEditorGUI::getUUID() const { return SBCContainerUUID( "3F52AD7B-A478-D380-AA01-2041081D06CB" );}

QPixmap SEMergePartsEditorGUI::getLogo() const {
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/merge.png"));

}

QString SEMergePartsEditorGUI::getName() const {

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "Component Editor"; 

}

int SEMergePartsEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEMergePartsEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
