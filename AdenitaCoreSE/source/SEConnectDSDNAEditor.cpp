#include "SEConnectDSDNAEditor.hpp"
#include "SAMSON.hpp"


SEConnectDSDNAEditor::SEConnectDSDNAEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEConnectDSDNAEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SEConnectDSDNAEditor::~SEConnectDSDNAEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEConnectDSDNAEditorGUI* SEConnectDSDNAEditor::getPropertyWidget() const { return static_cast<SEConnectDSDNAEditorGUI*>(propertyWidget); }

void SEConnectDSDNAEditor::SetMode(Mode m)
{
  mode_ = m;
}

SEAdenitaCoreSEApp* SEConnectDSDNAEditor::getAdenitaApp() const
{
  return static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
}

SBCContainerUUID SEConnectDSDNAEditor::getUUID() const { return SBCContainerUUID("C39F7E83-85A0-63D8-E213-96D73780DF78"); }

QString SEConnectDSDNAEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

  return "SEConnectDSDNAEditor";

}

QString SEConnectDSDNAEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("Connect dsDNA strands"); 

}

QPixmap SEConnectDSDNAEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/connectDS.png"));

}

QKeySequence SEConnectDSDNAEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEConnectDSDNAEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Connect dsDNA strands"); 

}

QString SEConnectDSDNAEditor::getDescription() const
{
  return QObject::tr("Adenita | dsDNA Crossovers");
}

void SEConnectDSDNAEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.
  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/cursor_connectDS.png";
  SAMSON::setViewportCursor(QCursor(QPixmap(iconPath.c_str())));

}

void SEConnectDSDNAEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

  SAMSON::unsetViewportCursor();
}

void SEConnectDSDNAEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEConnectDSDNAEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  if (display_) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

    //check if a nucleotide got selected
    auto app = getAdenitaApp();
    auto nanorobot = app->GetNanorobot();

    auto highlightedNucleotides = nanorobot->GetHighlightedNucleotides();

    if (highlightedNucleotides.size() == 1) {
      currentPosition = highlightedNucleotides[0]->GetBackbone()->GetPosition();
    }

    ADNDisplayHelper::displayCylinder(start_->GetBackbone()->GetPosition(), currentPosition);
  }

}

void SEConnectDSDNAEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEConnectDSDNAEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEConnectDSDNAEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (event->buttons() == Qt::LeftButton && !display_) {
    //check if a nucleotide got selected
    auto app = getAdenitaApp();
    auto nanorobot = app->GetNanorobot();

    auto selectedNucleotides = nanorobot->GetSelectedNucleotides();
    auto highlightedNucleotides = nanorobot->GetHighlightedNucleotides();

    SB_FOR(auto node, selectedNucleotides) {
      node->setSelectionFlag(false);
    }

    if (highlightedNucleotides.size() == 1) {
      auto nt = highlightedNucleotides[0];

      if (mode_ == Single || nt->IsEnd()) {
        nt->setSelectionFlag(true);
        start_ = nt;
        display_ = true;
      }
    }
  }
}

void SEConnectDSDNAEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  
  if (display_) {
    display_ = false;

    auto app = getAdenitaApp();
    auto nanorobot = app->GetNanorobot();

    auto highlightedNucleotides = nanorobot->GetHighlightedNucleotides();

    if (highlightedNucleotides.size() == 1) {
      auto start = start_;
      ADNPointer<ADNNucleotide> end = highlightedNucleotides[0];
      if (!end->IsEnd()) end = end->GetNext();
      ADNPointer<ADNPart> part = nanorobot->GetPart(end->GetStrand());
      if (start->GetEnd() == ThreePrime) {
        auto store = start;
        start = end;
        end = store;
      }
      if (mode_ == Single) {
        auto ssLeftOvers = DASOperations::CreateCrossover(part, start, end);
        if (ssLeftOvers.first != nullptr) nanorobot->RemoveSingleStrand(ssLeftOvers.first);
        if (ssLeftOvers.second != nullptr) nanorobot->RemoveSingleStrand(ssLeftOvers.second);
        if (ssLeftOvers.third != nullptr) nanorobot->RemoveSingleStrand(ssLeftOvers.third);
        if (ssLeftOvers.fourth != nullptr) nanorobot->RemoveSingleStrand(ssLeftOvers.fourth);
        app->ResetVisualModel();
      }
      else {
        std::string seq = "NNNNNNNNNNNN";
        if (start->GetEnd() == FivePrime && end->GetEnd() == ThreePrime) {
          ADNPointer<ADNSingleStrand> ss1 = end->GetStrand();
          ADNPointer<ADNSingleStrand> ss2 = start->GetStrand();
          DASOperations::LinkSingleStrands(part, ss1, ss2, seq);
          nanorobot->RemoveSingleStrand(ss1);
          nanorobot->RemoveSingleStrand(ss2);
          app->ResetVisualModel();
        }
        else if (start->GetEnd() == FivePrime && end->GetEnd() == ThreePrime) {
          ADNPointer<ADNSingleStrand> ss1 = start->GetStrand();
          ADNPointer<ADNSingleStrand> ss2 = end->GetStrand();
          DASOperations::LinkSingleStrands(part, ss1, ss2, seq);
          nanorobot->RemoveSingleStrand(ss1);
          nanorobot->RemoveSingleStrand(ss2);
          app->ResetVisualModel();
        }
      }
    }
  }

}

void SEConnectDSDNAEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (display_) {
    SAMSON::requestViewportUpdate();
  }
}

void SEConnectDSDNAEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEConnectDSDNAEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEConnectDSDNAEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEConnectDSDNAEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEConnectDSDNAEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEConnectDSDNAEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEConnectDSDNAEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEConnectDSDNAEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
