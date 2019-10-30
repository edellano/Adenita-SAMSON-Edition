#include "SEDSDNACreatorEditor.hpp"
#include "SAMSON.hpp"


SEDSDNACreatorEditor::SEDSDNACreatorEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.
  SEConfig& config = SEConfig::GetInstance();

	propertyWidget = new SEDSDNACreatorEditorGUI(this);
	propertyWidget->loadDefaultSettings();

  auto app = getAdenitaApp();
  nanorobot_ = app->GetNanorobot();

  basePairRadius_ = config.base_pair_radius;
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

void SEDSDNACreatorEditor::SetCircular(bool c)
{
  circular_ = c;
}

void SEDSDNACreatorEditor::SetManual(bool m)
{
  manual_ = m;
}

void SEDSDNACreatorEditor::SetNumberNucleotides(int n)
{
  numNts_ = n;
}

void SEDSDNACreatorEditor::SetSequence(bool s)
{
  setSequence_ = s;
}

SBPosition3 SEDSDNACreatorEditor::GetSnappedPosition()
{
  SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

  if (snappingActive_) {
    auto highlightedBaseSegments = nanorobot_->GetHighlightedBaseSegments();
    auto highlightedBaseSegmentsFromNucleotides = nanorobot_->GetHighlightedBaseSegmentsFromNucleotides();
    auto highlightedAtoms = nanorobot_->GetHighlightedAtoms();

    if (highlightedAtoms.size() == 1) {
      currentPosition = highlightedAtoms[0]->getPosition();
    }
    else if (highlightedBaseSegments.size() == 1) {
      currentPosition = highlightedBaseSegments[0]->GetPosition();
    }
    else if (highlightedBaseSegmentsFromNucleotides.size() == 1) {
      currentPosition = highlightedBaseSegmentsFromNucleotides[0]->GetPosition();
    }
  }

  return currentPosition;
}

ADNPointer<ADNPart> SEDSDNACreatorEditor::generateStrand(bool mock)
{
  auto length = (positions_.SecondPosition - positions_.FirstPosition).norm();
  auto numNucleotides = numNts_;
  if (!manual_ || numNucleotides == 0) {
    numNucleotides = round((length / SBQuantity::nanometer(ADNConstants::BP_RISE)).getValue());
  }
  // if (manual_) we already have the number of nucleotides, we just need a direction

  ADNPointer<ADNPart> part = nullptr;

  if (numNucleotides > 0) {
    part = new ADNPart();

    SBVector3 dir = (positions_.SecondPosition - positions_.FirstPosition).normalizedVersion();
    
    if (dsMode_) {
      auto ds = DASCreator::CreateDoubleStrand(part, numNucleotides, positions_.FirstPosition, dir, mock);
    }
    else {
      auto ss = DASCreator::CreateSingleStrand(part, numNucleotides, positions_.FirstPosition, dir, mock);
    }
  }

  return part;
}

ADNPointer<ADNPart> SEDSDNACreatorEditor::generateCircularStrand(bool mock)
{
  ADNPointer<ADNPart> part = nullptr;

  //auto radius = (positions_.SecondPosition - positions_.FirstPosition).norm();
  double pi = atan(1) * 4;
  auto numNucleotides = numNts_;
  auto radius = numNucleotides * SBQuantity::nanometer(ADNConstants::BP_RISE) * 0.5 / pi;
  if (!manual_ || numNucleotides == 0) {
    radius = (positions_.SecondPosition - positions_.FirstPosition).norm();
    numNucleotides = round((2 * pi * radius / SBQuantity::nanometer(ADNConstants::BP_RISE)).getValue());
  }

  if (numNucleotides > 0) {
    if (dsMode_) part = DASCreator::CreateDSRing(radius, positions_.FirstPosition, positions_.FirstVector, mock);
    else part = DASCreator::CreateSSRing(radius, positions_.FirstPosition, positions_.FirstVector, mock);
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

void SEDSDNACreatorEditor::sendPartToAdenita(ADNPointer<ADNPart> nanotube)
{
  if (nanotube != nullptr) {
    SetSequence(nanotube);

    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(nanotube);
    adenita->ResetVisualModel();
  }
}

SEAdenitaCoreSEApp * SEDSDNACreatorEditor::getAdenitaApp()
{
  SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
  return adenita;
}

void SEDSDNACreatorEditor::SetSequence(ADNPointer<ADNPart> nanotube)
{
  if (setSequence_) {
    auto singleStrands = nanotube->GetSingleStrands();
    ADNPointer<ADNSingleStrand> ss = nullptr;
    SB_FOR(ADNPointer<ADNSingleStrand> strand, singleStrands) {
      ADNPointer<ADNNucleotide> fPrime = strand->GetFivePrime();
      ADNPointer<ADNBaseSegment> bs = fPrime->GetBaseSegment();
      SBPosition3 pos = bs->GetPosition();
      if (pos == positions_.FirstPosition) {
        ss = strand;
        break;
      }
    }
    if (ss != nullptr) {
      int length = ss->getNumberOfNucleotides();
      SEDSDNACreatorEditorGUI* editor = getPropertyWidget();
      std::string seq = editor->AskUserForSequence(length);
      ss->SetSequence(seq);
    }
  }
}

void SEDSDNACreatorEditor::displayStrand()
{
  ADNDisplayHelper::displayPart(tempPart_, basePairRadius_, opaqueness_);

  //string text = "Opaqueness: ";
  //stringstream s1;
  //s1 << fixed << setprecision(2) << opaqueness_;
  //text += s1.str();

  //text += ", Radius: ";
  //stringstream s2;
  //s2 << fixed << setprecision(2) << basePairRadius_;
  //text += s2.str();

  SBPosition3 offset = SBPosition3(SBQuantity::nanometer(0),
    SBQuantity::nanometer(-1),
    SBQuantity::nanometer(0));
  std::string text = std::to_string(tempPart_->GetNumberOfBaseSegments()) + "bp";
  ADNDisplayHelper::displayText(SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport()) + offset, text);

}

SBCContainerUUID SEDSDNACreatorEditor::getUUID() const { return SBCContainerUUID("2F353D32-A630-8800-5FCA-14EBA6AC36F9"); }

QString SEDSDNACreatorEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEDSDNACreatorEditor"; 

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

	return QObject::tr("Add ssDNA and dsDNA to your model"); 

}

void SEDSDNACreatorEditor::loadSettings(SBGSettings* settings) {

  if (settings == NULL) return;

  // SAMSON Element generator pro tip: complete this function so your importer can save its GUI state from one session to the next

}

void SEDSDNACreatorEditor::saveSettings(SBGSettings* settings) {

  if (settings == NULL) return;

  // SAMSON Element generator pro tip: complete this function so your importer can save its GUI state from one session to the next

}

QString SEDSDNACreatorEditor::getDescription() const
{
  return QObject::tr("Adenita | DNA strands Editor");
}

int SEDSDNACreatorEditor::getFormat() const {

  // SAMSON Element generator pro tip: modify these default settings to configure the window
  //
  // SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
  // SBGWindow::Lockable : let users lock the window on top
  // SBGWindow::Resizable : let users resize the window
  // SBGWindow::Citable : let users obtain citation information (implement getCitation)

  return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

void SEDSDNACreatorEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.
  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/dsCreator.png";
  SAMSON::setViewportCursor(QCursor(QPixmap(iconPath.c_str())));

}

void SEDSDNACreatorEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.
  SAMSON::unsetViewportCursor();
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
    
    if (positions_.positionsCounter == 1) {

      positions_.SecondPosition = GetSnappedPosition();

      ADNDisplayHelper::displayLine(positions_.FirstPosition, positions_.SecondPosition);
    }

    if (config.preview_editor) {
      if (!circular_) tempPart_ = generateStrand(true);
      else tempPart_ = generateCircularStrand(true);
    }

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

  if (positions_.positionsCounter == 0) {
    positions_.FirstPosition = GetSnappedPosition();
    positions_.positionsCounter++;

    positions_.FirstVector = SAMSON::getActiveCamera()->getBasisZ().normalizedVersion();
    positions_.vectorsCounter++;
  }
}

void SEDSDNACreatorEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 1) {
    //positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;

    ADNPointer<ADNPart> part = nullptr;
    if (!circular_) part = generateStrand();
    else part = generateCircularStrand();
    if (part != nullptr) {
      int test3 = part->GetNumberOfSingleStrands();
      sendPartToAdenita(part);
      DASCreatorEditors::resetPositions(positions_);
      display_ = false;
      tempPart_ == nullptr;
    }
  }
}

void SEDSDNACreatorEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    //SAMSON::requestViewportUpdate();
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
  if (display_) {
    SEConfig& config = SEConfig::GetInstance();

    if (event->key() == Qt::Key_Up) {
      opaqueness_ += 0.1f;
      if (opaqueness_ > 1.0f) opaqueness_ = 1.0f;
    }
    else if (event->key() == Qt::Key_Down) {
      opaqueness_ -= 0.1f;
      if (opaqueness_ < 0.0f) opaqueness_ = 0.0f;
    } 
    else if (event->key() == Qt::Key_Left) {
      basePairRadius_ -= 100.0f;
      if (basePairRadius_ < 200.0f) basePairRadius_ = 200.0f;
    }
    else if (event->key() == Qt::Key_Right) {
      basePairRadius_ += 100.0f;
      if (basePairRadius_ > config.base_pair_radius) basePairRadius_ = config.base_pair_radius;
    }

    SAMSON::requestViewportUpdate();

  }

  if (event->key() == Qt::Key_Control) {
    snappingActive_ = false;
  }
}

void SEDSDNACreatorEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->key() == Qt::Key_Control) {
    snappingActive_ = true;
  }


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
