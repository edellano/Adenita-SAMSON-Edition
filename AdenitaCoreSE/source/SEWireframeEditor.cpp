#include "SEWireframeEditor.hpp"
#include "SAMSON.hpp"


SEWireframeEditor::SEWireframeEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEWireframeEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SEWireframeEditor::~SEWireframeEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEWireframeEditorGUI* SEWireframeEditor::getPropertyWidget() const { return static_cast<SEWireframeEditorGUI*>(propertyWidget); }

ADNPointer<ADNPart> SEWireframeEditor::generateWireframe(bool mock /*= false*/)
{
  auto radius = (positions_.SecondPosition - positions_.FirstPosition).norm();
  unsigned int numNucleotides;

  ADNPointer<ADNPart> part = nullptr;
  string filename;

  if (editorType_ == DASCreator::WireframeCube) {
    part = new ADNPart();

    double a = sqrt(pow(radius.getValue(), 2) * 2);
    numNucleotides = a / (ADNConstants::BP_RISE * 1000);
    filename = SB_ELEMENT_PATH + "/Data/02_cube.ply";
  }

  int min_edge_size = 31;
  if (numNucleotides > 31) {
    for (int i = 1; i < numNucleotides + 10.5; i++) {
      int p = (i - 1) * 10.5;
      int n = i * 10.5;

      if (numNucleotides > p && numNucleotides < n) {
        min_edge_size = p;
      }
    }
  }

  if (mock) {
    DASPolyhedron polyhedron = DASPolyhedron(filename);

    float min_length = polyhedron.MinimumEdgeLength().second;
    float rel = min_edge_size / min_length;

    auto faces = polyhedron.GetFaces();

    for (auto fit = faces.begin(); fit != faces.end(); ++fit) {
      std::map<int, DOTNode*> node_relation;
      auto begin = (*fit)->halfEdge_;
      auto he = begin;
      do {
        auto sourcePos = he->source_->GetSBPosition();
        auto targetPos = he->next_->source_->GetSBPosition();
        
        sourcePos *= (min_edge_size * 2);
        targetPos *= (min_edge_size * 2);
        
        SBVector3 dir = (targetPos - sourcePos).normalizedVersion();

        DASCreator::AddDoubleStrandToADNPart(part, min_edge_size, sourcePos, dir, true);

        he = he->next_;
      } while (he != begin);
    }
  }
  else {
    
    DASDaedalus *alg = new DASDaedalus();
    alg->SetMinEdgeLength(min_edge_size);
    std::string seq = "";
    part = alg->ApplyAlgorithm(seq, filename);

    auto & doubleStrands = part->GetDoubleStrands();

  }


  return part;
}

void SEWireframeEditor::sendPartToAdenita(ADNPointer<ADNPart> part)
{
  if (part != nullptr) {

    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(part);
    adenita->ResetVisualModel();
  }
}

SBCContainerUUID SEWireframeEditor::getUUID() const { return SBCContainerUUID("ED358EAC-14D1-A0EA-9A3A-F8035E019249"); }

QString SEWireframeEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEWireframeEditor"; 

}

QString SEWireframeEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("Daedalus DNA Nanostructures"); 

}

QPixmap SEWireframeEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEWireframeEditorIcon.png"));

}

QKeySequence SEWireframeEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEWireframeEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Create wireframe DNA nanostructures using Daedalus"); 

}

QString SEWireframeEditor::getDescription() const
{
  return QObject::tr("Adenita | Daedalus DNA Nanostructures");
}

void SEWireframeEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.
  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/wireframeCreator.png";
  SAMSON::setViewportCursor(QCursor(QPixmap(iconPath.c_str())));
}

void SEWireframeEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.
  SAMSON::unsetViewportCursor();

}

void SEWireframeEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEWireframeEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  SEConfig& config = SEConfig::GetInstance();


  if (display_) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

    if (positions_.positionsCounter == 1) {
      ADNDisplayHelper::displayLine(positions_.FirstPosition, currentPosition);
      positions_.SecondPosition = currentPosition;
    }

    if (config.preview_editor) {
      tempPart_ = generateWireframe(true);
      ADNDisplayHelper::displayPart(tempPart_);
    }

    if (tempPart_ != nullptr) {
      
    }
  }


}

void SEWireframeEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEWireframeEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEWireframeEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (positions_.positionsCounter == 0) {
    positions_.FirstPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;

    positions_.FirstVector = SAMSON::getActiveCamera()->getBasisZ().normalizedVersion();
    positions_.vectorsCounter++;
  }
}

void SEWireframeEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.


  if (positions_.positionsCounter == 1) {
    positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;


    ADNPointer<ADNPart> part = generateWireframe();

    sendPartToAdenita(part);
    DASCreatorEditors::resetPositions(positions_);
    display_ = false;
    tempPart_ == nullptr;
  }
}

void SEWireframeEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    //SAMSON::requestViewportUpdate();
  }

  SAMSON::requestViewportUpdate();
}

void SEWireframeEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEWireframeEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEWireframeEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEWireframeEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEWireframeEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEWireframeEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEWireframeEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEWireframeEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
