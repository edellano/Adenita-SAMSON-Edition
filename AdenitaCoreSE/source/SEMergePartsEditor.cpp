#include "SEMergePartsEditor.hpp"
#include "SAMSON.hpp"


SEMergePartsEditor::SEMergePartsEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEMergePartsEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SEMergePartsEditor::~SEMergePartsEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEMergePartsEditorGUI* SEMergePartsEditor::getPropertyWidget() const { return static_cast<SEMergePartsEditorGUI*>(propertyWidget); }

std::map<int, ADNPointer<ADNPart>> SEMergePartsEditor::getPartsList()
{
  indexParts_.clear();

  SEAdenitaCoreSEApp* t = getAdenitaApp();
  auto nr = t->GetNanorobot();
  auto parts = nr->GetParts();
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    ++lastId_;
    indexParts_.insert(std::make_pair(lastId_, p));
  }
  return indexParts_;
}

void SEMergePartsEditor::MergeParts(int idx, int jdx)
{
  if (idx == jdx) return;

  ADNPointer<ADNPart> p1 = nullptr;
  ADNPointer<ADNPart> p2 = nullptr;
  if (indexParts_.find(idx) != indexParts_.end()) p1 = indexParts_.at(idx);
  if (indexParts_.find(jdx) != indexParts_.end()) p2 = indexParts_.at(jdx);
  if (p1 != nullptr && p2 != nullptr) {
    SEAdenitaCoreSEApp* t = getAdenitaApp();
    t->MergeComponents(p1, p2);
  }
}

SEAdenitaCoreSEApp* SEMergePartsEditor::getAdenitaApp() const
{
  return static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
}

SBCContainerUUID SEMergePartsEditor::getUUID() const { return SBCContainerUUID("EB812444-8EA8-BD83-988D-AFF5987461D8"); }

QString SEMergePartsEditor::getName() const {

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEMergePartsEditor"; 

}

QString SEMergePartsEditor::getText() const {
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("Merge Components"); 

}

QPixmap SEMergePartsEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/merge.png"));

}

QKeySequence SEMergePartsEditor::getShortcut() const {
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEMergePartsEditor::getToolTip() const {
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Merge components"); 

}

QString SEMergePartsEditor::getDescription() const
{
  return QObject::tr("Adenita | Merge Adenita Components");
}

void SEMergePartsEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

}

void SEMergePartsEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SEMergePartsEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEMergePartsEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)


}

void SEMergePartsEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEMergePartsEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEMergePartsEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  
}

void SEMergePartsEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  
}

void SEMergePartsEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEMergePartsEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEMergePartsEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEMergePartsEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEMergePartsEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEMergePartsEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEMergePartsEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEMergePartsEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEMergePartsEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
