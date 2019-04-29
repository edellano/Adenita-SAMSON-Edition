#include "SEBottomUpEditor.hpp"
#include "SAMSON.hpp"


SEBottomUpEditor::SEBottomUpEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEBottomUpEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SEBottomUpEditor::~SEBottomUpEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEBottomUpEditorGUI* SEBottomUpEditor::getPropertyWidget() const { return static_cast<SEBottomUpEditorGUI*>(propertyWidget); }

void SEBottomUpEditor::SetSelected(int idx)
{
  selected_ = idx;
}

ADNPointer<ADNPart> SEBottomUpEditor::Build(double maxCutOff, double minCutOff, double maxAngle)
{
  if (selected_ == 0) return nullptr;

  auto node = indexParts_[selected_];

  SBQuantity::length maxc = SBQuantity::angstrom(maxCutOff);
  SBQuantity::length minc = SBQuantity::angstrom(maxCutOff);
  
  ADNPointer<ADNPart> part = ADNLoader::GenerateModelFromDatagraphParametrized(node(), maxc, minc, maxAngle);

  return part;
}

void SEBottomUpEditor::sendPartToAdenita(ADNPointer<ADNPart> part)
{
  if (part != nullptr) {

    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(part, false, true);
    adenita->ResetVisualModel();
  }
}

std::map<int, SBPointer<SBNode>> SEBottomUpEditor::getPartsList()
{
  indexParts_.clear();
  int lastId = 0;

  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, SBNode::IsType(SBNode::StructuralModel));

  SB_FOR(auto node, nodes) {
    ++lastId;
    indexParts_.insert(std::make_pair(lastId, node));
  }
  return indexParts_;
}

SBCContainerUUID SEBottomUpEditor::getUUID() const { return SBCContainerUUID("6C4D48E2-E097-31AF-690C-122911F0ED48"); }

QString SEBottomUpEditor::getName() const {

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEBottomUpEditor"; 

}

QString SEBottomUpEditor::getText() const {
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("Adenita bottom-up model generator"); 

}

QPixmap SEBottomUpEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEBottomUpEditorIcon.png"));

}

QKeySequence SEBottomUpEditor::getShortcut() const {
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEBottomUpEditor::getToolTip() const {
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Build an Adenita model from atomic data"); 

}

QString SEBottomUpEditor::getDescription() const
{
  return QObject::tr("Adenita | Bottom-up model generator");
}

void SEBottomUpEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

}

void SEBottomUpEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SEBottomUpEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEBottomUpEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  SEConfig& config = SEConfig::GetInstance();

  if (!preview_ || selected_ == 0) return;

  //auto sn = indexParts_[selected_];
  //if (changed_) {
  //  part_ = GenerateModelFromDatagraphParametrized(sn, maxCuttOff_, minCutOff_, maxAngle_);
  //}
  //if (part_ != nullptr) ADNDisplayHelper::displayPart(part_);
}

void SEBottomUpEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEBottomUpEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEBottomUpEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  
}

void SEBottomUpEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEBottomUpEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEBottomUpEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEBottomUpEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEBottomUpEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
