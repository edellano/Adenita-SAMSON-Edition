#include "SENanotubeCreatorEditor.hpp"
#include "SAMSON.hpp"


SENanotubeCreatorEditor::SENanotubeCreatorEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SENanotubeCreatorEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SENanotubeCreatorEditor::~SENanotubeCreatorEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SENanotubeCreatorEditorGUI* SENanotubeCreatorEditor::getPropertyWidget() const { return static_cast<SENanotubeCreatorEditorGUI*>(propertyWidget); }

ADNPointer<ADNPart> SENanotubeCreatorEditor::generateNanotube()
{
  ADNPointer<ADNPart> part = nullptr;

  auto radius = (positions_.Third - positions_.Second).norm();
  auto roundHeight = (positions_.Second - positions_.First).norm();
  auto numNucleotides = roundHeight / SBQuantity::nanometer(ADNConstants::BP_RISE);
  SBVector3 dir = (positions_.Second - positions_.First).normalizedVersion();

  //if (radius > SBQuantity::length(0.0) && roundHeight > SBQuantity::length(0.0)) {
  if (radius > SBQuantity::length(0.0)) {
    part = DASCreator::CreateNanotube(radius, positions_.First, dir, numNucleotides.getValue());
  }

  return part;
}

void SENanotubeCreatorEditor::displayNanotube()
{
  SEConfig& config = SEConfig::GetInstance();

  auto doubleStrands = tempPart_->GetDoubleStrands();
  unsigned int nPositions = tempPart_->GetNumberOfBaseSegments();

  ADNArray<float> positions = ADNArray<float>(3, nPositions);
  ADNArray<float> radiiV = ADNArray<float>(nPositions);
  ADNArray<unsigned int> flags = ADNArray<unsigned int>(nPositions);
  ADNArray<float> colorsV = ADNArray<float>(4, nPositions);
  ADNArray<unsigned int> nodeIndices = ADNArray<unsigned int>(nPositions);

  unsigned int index = 0;

  SB_FOR(auto doubleStrand, doubleStrands) {
    auto baseSegments = doubleStrand->GetBaseSegments();

    SB_FOR(auto baseSegment, baseSegments) {
      auto cell = baseSegment->GetCell();

      if (cell->GetType() == BasePair) {
        SBPosition3 pos = baseSegment->GetPosition();
        positions(index, 0) = (float)pos.v[0].getValue();
        positions(index, 1) = (float)pos.v[1].getValue();
        positions(index, 2) = (float)pos.v[2].getValue();
      }

      colorsV(index, 0) = config.double_strand_color[0];
      colorsV(index, 1) = config.double_strand_color[1];
      colorsV(index, 2) = config.double_strand_color[2];
      colorsV(index, 3) = config.double_strand_color[3];

      radiiV(index) = config.base_pair_radius;

      flags(index) = baseSegment->getInheritedFlags();

      ++index;

    }
  }
  
  SAMSON::displaySpheres(
    nPositions,
    positions.GetArray(),
    radiiV.GetArray(),
    colorsV.GetArray(),
    flags.GetArray());

}

void SENanotubeCreatorEditor::resetPositions()
{
  positions_.First = SBPosition3();
  positions_.Second = SBPosition3();
  positions_.Third = SBPosition3();
  positions_.Fourth = SBPosition3();
  positions_.Fifth = SBPosition3();
  positions_.Sixth = SBPosition3();
  positions_.cnt = 0;
}

void SENanotubeCreatorEditor::sendPartToAdenita(ADNPointer<ADNPart> nanotube)
{
  SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  adenita->AddPartToActiveLayer(nanotube);
}

SBCContainerUUID SENanotubeCreatorEditor::getUUID() const { return SBCContainerUUID("F9068FA3-69DE-B6FA-2B42-C80DA5302A0D"); }

QString SENanotubeCreatorEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SENanotubeCreatorEditor"; 

}

QString SENanotubeCreatorEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("SENanotubeCreatorEditor"); 

}

QPixmap SENanotubeCreatorEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SENanotubeCreatorEditorIcon.png"));

}

QKeySequence SENanotubeCreatorEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SENanotubeCreatorEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Nanotube Creator"); 

}

void SENanotubeCreatorEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

}

void SENanotubeCreatorEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SENanotubeCreatorEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SENanotubeCreatorEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  SEConfig& config = SEConfig::GetInstance();

  if (display_) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

    if (positions_.cnt == 1) {
      ADNDisplayHelper::displayLine(positions_.First, currentPosition);
    }
    else if (positions_.cnt == 2) {
      ADNDisplayHelper::displayLine(positions_.First, positions_.Second);
      ADNDisplayHelper::displayLine(positions_.Second, currentPosition);

      positions_.Third = currentPosition;

      if (config.preview_editor) tempPart_ = generateNanotube();
    }

    if (tempPart_ != nullptr) {

      glEnable(GL_DEPTH_TEST);

      displayNanotube();

      glDisable(GL_DEPTH_TEST);

    }
  }

}

void SENanotubeCreatorEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SENanotubeCreatorEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SENanotubeCreatorEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.cnt == 0) {
    positions_.First = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.cnt++;
  }
  else if (positions_.cnt == 2) {
    positions_.Third = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.cnt++;

    auto radius = (positions_.Third - positions_.Second).norm();

    ADNPointer<ADNPart> part = generateNanotube();

    resetPositions();
    display_ = false;
  }
  
  if (tempPart_ != nullptr) {
    sendPartToAdenita(tempPart_);
    tempPart_ == nullptr;
  }
}

void SENanotubeCreatorEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (tempPart_ != nullptr) {
    tempPart_ = nullptr;
  }

  if (positions_.cnt == 1) {
    positions_.Second = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.cnt++;
  }

}

void SENanotubeCreatorEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    SAMSON::requestViewportUpdate();
  }
  SAMSON::requestViewportUpdate();

}

void SENanotubeCreatorEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SENanotubeCreatorEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SENanotubeCreatorEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SENanotubeCreatorEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SENanotubeCreatorEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SENanotubeCreatorEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SENanotubeCreatorEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SENanotubeCreatorEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
