#include "SETwistHelixEditorGUI.hpp"
#include "SETwistHelixEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SETwistHelixEditorGUI::SETwistHelixEditorGUI(SETwistHelixEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SETwistHelixEditorGUI::~SETwistHelixEditorGUI() {

}

SETwistHelixEditor* SETwistHelixEditorGUI::getEditor() const { return editor; }

void SETwistHelixEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SETwistHelixEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SETwistHelixEditorGUI::onMinus(bool checked)
{
  if (checked) {
    SETwistHelixEditor* editor = getEditor();
    double angle = -ADNConstants::BP_ROT;
    editor->SetTwistAngle(angle);

  }
}

void SETwistHelixEditorGUI::CheckPlusOrMinus(bool plus)
{
  if (plus) {
    ui.rdoPlus->setChecked(true);
    //onPlus(true);
  }
  else {
    ui.rdoMinus->setChecked(true);
    //onMinus(true);
  }
}

void SETwistHelixEditorGUI::onPlus(bool checked)
{
  if (checked) {
    SETwistHelixEditor* editor = getEditor();
    double angle = ADNConstants::BP_ROT;
    editor->SetTwistAngle(angle);
  }
}

void SETwistHelixEditorGUI::onTwistAngle(bool checked)
{
  if (checked) {
    SETwistHelixEditor* editor = getEditor();
    double angle = ui.spnAngle->value();
    editor->SetTwistAngle(angle);
  }
}

void SETwistHelixEditorGUI::onTwistTurns(bool checked)
{
  if (checked) {
    SETwistHelixEditor* editor = getEditor();
    double angle = ui.spnTurns->value() * ADNConstants::BP_ROT;
    editor->SetTwistAngle(angle);
  }
}

void SETwistHelixEditorGUI::onTwistAngleChanged(double angle)
{
  if (ui.rdoTwistAngle->isChecked()) {
    SETwistHelixEditor* editor = getEditor();
    double angle = ui.spnAngle->value();
    editor->SetTwistAngle(angle);
  }
}

void SETwistHelixEditorGUI::onTwistTurnsChanged(int turns)
{
  if (ui.rdoTwistTurns->isChecked()) {
    SETwistHelixEditor* editor = getEditor();
    double angle = ui.spnTurns->value() * ADNConstants::BP_ROT;
    editor->SetTwistAngle(angle);
  }
}

SBCContainerUUID SETwistHelixEditorGUI::getUUID() const { return SBCContainerUUID( "8D1CB60B-F52C-14C2-F72C-DB72B6361777" );}

QPixmap SETwistHelixEditorGUI::getLogo() const {
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/twistDS.png"));

}

QString SETwistHelixEditorGUI::getName() const {

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SETwistHelixEditor"; 

}

int SETwistHelixEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SETwistHelixEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
