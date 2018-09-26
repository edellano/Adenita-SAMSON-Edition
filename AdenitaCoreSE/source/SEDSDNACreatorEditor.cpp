#include "SEDSDNACreatorEditor.hpp"
#include "SAMSON.hpp"


SEDSDNACreatorEditor::SEDSDNACreatorEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEDSDNACreatorEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SEDSDNACreatorEditor::~SEDSDNACreatorEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEDSDNACreatorEditorGUI* SEDSDNACreatorEditor::getPropertyWidget() const { return static_cast<SEDSDNACreatorEditorGUI*>(propertyWidget); }

void SEDSDNACreatorEditor::SetMode(bool m)
{
  dsMode_ = m;
}

void SEDSDNACreatorEditor::SetShowBox(bool s)
{
  showBox_ = s;
}

void SEDSDNACreatorEditor::SetBoxSize(double height, double width, double depth)
{
  boxHeight_ = SBQuantity::nanometer(height);
  boxWidth_ = SBQuantity::nanometer(width);
  boxDepth_ = SBQuantity::nanometer(depth);
}

ADNPointer<ADNPart> SEDSDNACreatorEditor::generateStrand(bool mock)
{
  ADNPointer<ADNPart> part = new ADNPart();

  auto roundHeight = (positions_.Second - positions_.First).norm();
  auto numNucleotides = roundHeight / SBQuantity::nanometer(ADNConstants::BP_RISE);
  SBVector3 dir = (positions_.Second - positions_.First).normalizedVersion();

  if (dsMode_) {
    DASCreator::CreateDoubleStrand(part, numNucleotides.getValue(), positions_.First, dir, mock);
  }
  else {
    DASCreator::CreateSingleStrand(part, numNucleotides.getValue(), positions_.First, dir, mock);
  }

  return part;
}

void SEDSDNACreatorEditor::displayBox()
{
  if (showBox_) {
    // draw a box centered at origin
    SBVector3 x = SBVector3(1.0, 0.0, 0.0);
    SBVector3 y = SBVector3(0.0, 1.0, 0.0);
    SBVector3 z = SBVector3(0.0, 0.0, 1.0);

    auto xMax = boxWidth_ * 0.5;
    auto xMin = -xMax;
    auto yMax = boxHeight_ * 0.5;
    auto yMin = -yMax;
    auto zMax = boxDepth_ * 0.5;
    auto zMin = -zMax;

    SBPosition3 v1 = xMin * x + yMax * y + zMax * z;
    SBPosition3 v2 = xMax * x + yMax * y + zMax * z;
    SBPosition3 v3 = xMax * x + yMin * y + zMax * z;
    SBPosition3 v4 = xMin * x + yMin * y + zMax * z;
    SBPosition3 v5 = xMin * x + yMax * y + zMin * z;
    SBPosition3 v6 = xMax * x + yMax * y + zMin * z;
    SBPosition3 v7 = xMax * x + yMin * y + zMin * z;
    SBPosition3 v8 = xMin * x + yMin * y + zMin * z;


    ADNDisplayHelper::displayLine(v1, v2);
    ADNDisplayHelper::displayLine(v2, v3);
    ADNDisplayHelper::displayLine(v3, v4);
    ADNDisplayHelper::displayLine(v4, v1);

    ADNDisplayHelper::displayLine(v5, v6);
    ADNDisplayHelper::displayLine(v6, v7);
    ADNDisplayHelper::displayLine(v7, v8);
    ADNDisplayHelper::displayLine(v8, v5);

    ADNDisplayHelper::displayLine(v1, v5);
    ADNDisplayHelper::displayLine(v2, v6);
    ADNDisplayHelper::displayLine(v3, v7);
    ADNDisplayHelper::displayLine(v4, v8);
  }
}

void SEDSDNACreatorEditor::resetPositions()
{
  positions_.First = SBPosition3();
  positions_.Second = SBPosition3();
  positions_.cnt = 0;
}

void SEDSDNACreatorEditor::sendPartToAdenita(ADNPointer<ADNPart> nanotube)
{
  SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  adenita->AddPartToActiveLayer(nanotube);
  adenita->ResetVisualModel();
}

void SEDSDNACreatorEditor::displayStrand()
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
      colorsV(index, 3) = 0.4f;

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

SBCContainerUUID SEDSDNACreatorEditor::getUUID() const { return SBCContainerUUID("2F353D32-A630-8800-5FCA-14EBA6AC36F9"); }

QString SEDSDNACreatorEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEDSDNACreatorEditor"; 

}

QString SEDSDNACreatorEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("SEDSDNACreatorEditor"); 

}

QPixmap SEDSDNACreatorEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEDSDNACreatorEditorIcon.png"));

}

QKeySequence SEDSDNACreatorEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEDSDNACreatorEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("SAMSON Element generator pro tip: modify me"); 

}

void SEDSDNACreatorEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

}

void SEDSDNACreatorEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SEDSDNACreatorEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEDSDNACreatorEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  SEConfig& config = SEConfig::GetInstance();
  displayBox();

  if (display_) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

    if (positions_.cnt == 1) {
      ADNDisplayHelper::displayLine(positions_.First, currentPosition);
      positions_.Second = currentPosition;
    }

    if (config.preview_editor) tempPart_ = generateStrand(true);

    if (tempPart_ != nullptr) {
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      displayStrand();

      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
    }
  }
}

void SEDSDNACreatorEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEDSDNACreatorEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEDSDNACreatorEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.cnt == 0) {
    positions_.First = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.cnt++;
  }
}

void SEDSDNACreatorEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.cnt == 1) {
    positions_.Second = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.cnt++;

    ADNPointer<ADNPart> part = generateStrand();
    sendPartToAdenita(part);
    resetPositions();
    display_ = false;
    tempPart_ == nullptr;
  }
}

void SEDSDNACreatorEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    SAMSON::requestViewportUpdate();
  }
  SAMSON::requestViewportUpdate();
}

void SEDSDNACreatorEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEDSDNACreatorEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEDSDNACreatorEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEDSDNACreatorEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEDSDNACreatorEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEDSDNACreatorEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEDSDNACreatorEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEDSDNACreatorEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
