#include "SEWireframeEditor.hpp"
#include "SAMSON.hpp"


SEWireframeEditor::SEWireframeEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEWireframeEditorGUI(this);
	propertyWidget->loadDefaultSettings();

}

SEWireframeEditor::~SEWireframeEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEWireframeEditorGUI* SEWireframeEditor::getPropertyWidget() const { return static_cast<SEWireframeEditorGUI*>(propertyWidget); }

void SEWireframeEditor::setWireframeType(DASCreator::EditorType type)
{
  wireframeType_ = type;
}

ADNPointer<ADNPart> SEWireframeEditor::generateCuboid(bool mock /*= false*/)
{
  SEConfig& config = SEConfig::GetInstance();

  ADNPointer<ADNPart> part = nullptr;

  SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

  int zSize = 31;

  SBQuantity::length faceRadius = (currentPosition - positions_.FirstPosition).norm();

  if (positions_.positionsCounter > 1) {
    faceRadius = (positions_.SecondPosition - positions_.FirstPosition).norm();
    SBQuantity::length zNM = (currentPosition - positions_.SecondPosition).norm();
    zSize = DASDaedalus::CalculateEdgeSize(zNM);
  }

  int bpSize = DASDaedalus::CalculateEdgeSize(faceRadius * 2);

  int xSize = bpSize;
  int ySize = bpSize;

  if (mock) {
    
    part = new ADNPart();

    SBVector3 xDir(1.0, 0.0, 0.0);
    SBVector3 yDir(0.0, 1.0, 0.0);
    SBVector3 zDir(0.0, 0.0, 1.0);

    SBQuantity::nanometer xLength = SBQuantity::nanometer(ADNConstants::BP_RISE * xSize);
    SBQuantity::nanometer yLength = SBQuantity::nanometer(ADNConstants::BP_RISE * ySize);
    SBQuantity::nanometer zLength = SBQuantity::nanometer(ADNConstants::BP_RISE * zSize);

    auto topLeftFront = positions_.FirstPosition;

    // create 12 double helices
    DASCreator::AddDoubleStrandToADNPart(part, xSize, topLeftFront, xDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, ySize, topLeftFront, -yDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, zSize, topLeftFront, -zDir, true);

    SBPosition3 bottomRightFront = topLeftFront + (xLength*xDir - yLength*yDir);

    DASCreator::AddDoubleStrandToADNPart(part, xSize, bottomRightFront, -xDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, ySize, bottomRightFront, yDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, zSize, bottomRightFront, -zDir, true);

    SBPosition3 bottomLeftBack = topLeftFront - (yLength*yDir + zLength*zDir);
    
    DASCreator::AddDoubleStrandToADNPart(part, xSize, bottomLeftBack, xDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, ySize, bottomLeftBack, yDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, zSize, bottomLeftBack, zDir, true);

    SBPosition3 topRightBack = topLeftFront + (xLength*xDir - zLength*zDir);

    DASCreator::AddDoubleStrandToADNPart(part, xSize, topRightBack, -xDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, ySize, topRightBack, -yDir, true);
    DASCreator::AddDoubleStrandToADNPart(part, zSize, topRightBack, zDir, true);

    return part;
    
  } else {
    part = new ADNPart();
    DASPolyhedron p = DASPolyhedron();
    std::map<int, SBPosition3> vertices;
    std::map<int, std::vector<int>> faces;

    SBVector3 xDir(1.0, 0.0, 0.0);
    SBVector3 yDir(0.0, 1.0, 0.0);
    SBVector3 zDir(0.0, 0.0, 1.0);

    SBQuantity::nanometer xLength = SBQuantity::nanometer(ADNConstants::BP_RISE * xSize);
    SBQuantity::nanometer yLength = SBQuantity::nanometer(ADNConstants::BP_RISE * ySize);
    SBQuantity::nanometer zLength = SBQuantity::nanometer(ADNConstants::BP_RISE * zSize);

    // current position is top-left
    SBPosition3 vertex1 = positions_.FirstPosition;
    SBPosition3 vertex2 = vertex1 + xLength*xDir;
    SBPosition3 vertex3 = vertex2 - yLength*yDir;
    SBPosition3 vertex4 = vertex3 - xLength*xDir;
    SBPosition3 vertex5 = vertex1 - zLength*zDir;
    SBPosition3 vertex6 = vertex5 + xLength*xDir;
    SBPosition3 vertex7 = vertex6 - yLength*yDir;
    SBPosition3 vertex8 = vertex7 - xLength*xDir;

    vertices.insert(std::make_pair(0, vertex2));
    vertices.insert(std::make_pair(1, vertex1));
    vertices.insert(std::make_pair(2, vertex4));
    vertices.insert(std::make_pair(3, vertex3));
    vertices.insert(std::make_pair(4, vertex7));
    vertices.insert(std::make_pair(5, vertex6));
    vertices.insert(std::make_pair(6, vertex5));
    vertices.insert(std::make_pair(7, vertex8));

    // faces
    std::vector<int> face1 = { 3, 0, 1, 2 };
    std::vector<int> face2 = { 3, 4, 5, 0 };
    std::vector<int> face3 = { 0, 5, 6, 1 };
    std::vector<int> face4 = { 1, 6, 7, 2 };
    std::vector<int> face5 = { 2, 7, 4, 3 };
    std::vector<int> face6 = { 5, 4, 7, 6 };

    faces.insert(std::make_pair(0, face1));
    faces.insert(std::make_pair(1, face2));
    faces.insert(std::make_pair(2, face3));
    faces.insert(std::make_pair(3, face4));
    faces.insert(std::make_pair(4, face5));
    faces.insert(std::make_pair(5, face6));

    p.BuildPolyhedron(vertices, faces);

    DASDaedalus *alg = new DASDaedalus();
    int minSize = std::min(bpSize, zSize);
    std::string seq = "";
    alg->SetMinEdgeLength(minSize);
    part = alg->ApplyAlgorithm(seq, p, false);

    if (part != nullptr) sendPartToAdenita(part);
  }


  return part;
}

ADNPointer<ADNPart> SEWireframeEditor::generateWireframe(bool mock)
{
  auto radius = (positions_.SecondPosition - positions_.FirstPosition).norm();
  unsigned int numNucleotides;

  ADNPointer<ADNPart> part = nullptr;
  string filename;
  if (wireframeType_ == DASCreator::Tetrahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2);
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) * 1.3;
    filename = SB_ELEMENT_PATH + "/Data/01_tetrahedron.ply";

  } else if (wireframeType_ == DASCreator::Cube) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2);
    numNucleotides = a / (ADNConstants::BP_RISE * 1000);
    filename = SB_ELEMENT_PATH + "/Data/02_cube.ply";
  } else if (wireframeType_ == DASCreator::Octahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2);
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/03_octahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Dodecahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 2;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/04_dodecahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Icosahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/05_icosahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Icosahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/05_icosahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Cubocahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/06_cuboctahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Icosidodecahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/07_icosidodecahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Rhombicuboctahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/08_rhombicuboctahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Snub_cube) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/09_snub_cube.ply";
  }
  else if (wireframeType_ == DASCreator::Truncated_cube) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/10_truncated_cube.ply";
  }
  else if (wireframeType_ == DASCreator::Truncated_cuboctahedron) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/11_truncated_cuboctahedron.ply";
  }
  else if (wireframeType_ == DASCreator::Helix) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/49_helix.ply";
  }
  else if (wireframeType_ == DASCreator::Stickman) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/51_stickman.ply";
  }
  else if (wireframeType_ == DASCreator::Bottle) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/52_bottle.ply";
  }
  else if (wireframeType_ == DASCreator::Bunny) {
    part = new ADNPart();
    double a = sqrt(pow(radius.getValue(), 2) * 2) / 4;
    numNucleotides = a / (ADNConstants::BP_RISE * 1000) / 1.5;
    filename = SB_ELEMENT_PATH + "/Data/53_bunny.ply";
  }
  int min_edge_size = 31;
  if (numNucleotides > 31) {
    int quot;
    remquo(numNucleotides, 10.5, &quot);
    min_edge_size = int(std::floor(float(quot * 10.5)));
  }

  DASPolyhedron polyhedron = DASPolyhedron(filename);
  polyhedron.Center(positions_.FirstPosition);

  if (mock) {
    part = CreateMockDaedalusWireframe(polyhedron, min_edge_size);
  }
  else {
    DASDaedalus *alg = new DASDaedalus();
    alg->SetMinEdgeLength(min_edge_size);
    std::string seq = "";
    part = alg->ApplyAlgorithm(seq, polyhedron, false, true);
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

ADNPointer<ADNPart> SEWireframeEditor::CreateMockDaedalusWireframe(DASPolyhedron & polyhedron, int min_edge_length)
{
  ADNPointer<ADNPart> mock = new ADNPart();

  DASDaedalus *alg = new DASDaedalus();
  alg->SetMinEdgeLength(min_edge_length);
  alg->SetEdgeBps(min_edge_length, mock, polyhedron);
  alg->SetVerticesPositions(mock, polyhedron, false);
  alg->InitEdgeMap(mock, polyhedron);
  return mock;
}

SBCContainerUUID SEWireframeEditor::getUUID() const { return SBCContainerUUID("ED358EAC-14D1-A0EA-9A3A-F8035E019249"); }

QString SEWireframeEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEWireframeEditor"; 

}

QPixmap SEWireframeEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEWireframeEditorIcon.png"));

}

int SEWireframeEditor::getFormat() const
{

  // SAMSON Element generator pro tip: modify these default settings to configure the window
  //
  // SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
  // SBGWindow::Lockable : let users lock the window on top
  // SBGWindow::Resizable : let users resize the window
  // SBGWindow::Citable : let users obtain citation information (implement getCitation)

  return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QKeySequence SEWireframeEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEWireframeEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Create wireframe DNA nanostructures using Daedalus"); 

}

void SEWireframeEditor::loadSettings(SBGSettings * settings)
{
  if (settings == NULL) return;

  // SAMSON Element generator pro tip: complete this function so your importer can save its GUI state from one session to the next

}

void SEWireframeEditor::saveSettings(SBGSettings* settings) {

  if (settings == NULL) return;

  // SAMSON Element generator pro tip: complete this function so your importer can save its GUI state from one session to the next

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

  if (!display_) return;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

  if (wireframeType_ == DASCreator::Cuboid) {
    if (positions_.positionsCounter < 3) {
      SBVector3 xDir(1.0, 0.0, 0.0);
      SBVector3 yDir(0.0, 1.0, 0.0);
      SBVector3 zDir(0.0, 0.0, 1.0);

      if (positions_.positionsCounter == 1) {
        SBQuantity::length radius = (currentPosition - positions_.FirstPosition).norm();
        SBQuantity::length side = 2 * radius;
        auto xySide = DASDaedalus::CalculateEdgeSize(side);
        SBPosition3 xPos = positions_.FirstPosition + SBQuantity::nanometer(xySide*ADNConstants::BP_RISE)*xDir;
        SBPosition3 yPos = positions_.FirstPosition - SBQuantity::nanometer(xySide*ADNConstants::BP_RISE)*yDir;

        std::string xyText = std::to_string(xySide) + " bp";
        ADNDisplayHelper::displayLine(positions_.FirstPosition, xPos, xyText);
        ADNDisplayHelper::displayLine(positions_.FirstPosition, yPos, xyText);
        if (config.preview_editor) tempPart_ = generateCuboid(true);
        ADNDisplayHelper::displayPart(tempPart_);
      }
      else if (positions_.positionsCounter == 2) {
        SBQuantity::length radius = (positions_.SecondPosition - positions_.FirstPosition).norm();
        SBQuantity::length side = 2 * radius;
        auto xySide = DASDaedalus::CalculateEdgeSize(side);
        auto zLength = DASDaedalus::CalculateEdgeSize((currentPosition - positions_.SecondPosition).norm());
        SBPosition3 xPos = positions_.FirstPosition + SBQuantity::nanometer(xySide*ADNConstants::BP_RISE)*xDir;
        SBPosition3 yPos = positions_.FirstPosition - SBQuantity::nanometer(xySide*ADNConstants::BP_RISE)*yDir;
        SBPosition3 zPos = positions_.FirstPosition - SBQuantity::nanometer(zLength*ADNConstants::BP_RISE) *zDir;

        std::string xyText = std::to_string(xySide) + " bp";
        std::string zText = std::to_string(zLength) + " bp";
        ADNDisplayHelper::displayLine(positions_.FirstPosition, xPos, xyText);
        ADNDisplayHelper::displayLine(positions_.FirstPosition, yPos, xyText);
        ADNDisplayHelper::displayLine(positions_.FirstPosition, zPos, zText);

        positions_.ThirdPosition = currentPosition;

        if (config.preview_editor) tempPart_ = generateCuboid(true);
        ADNDisplayHelper::displayPart(tempPart_);

      }
    }
  }
  else {
    if (positions_.positionsCounter == 1) {
      ADNDisplayHelper::displayLine(positions_.FirstPosition, currentPosition);
      positions_.SecondPosition = currentPosition;
    }

    if (config.preview_editor) {
      tempPart_ = generateWireframe(true);
      ADNDisplayHelper::displayPart(tempPart_, 1000.0f, 1.0f);
    }

    if (tempPart_ != nullptr) {

    }
  }

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

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
  
  if (wireframeType_ == DASCreator::Cuboid) {
    if (positions_.positionsCounter == 0) {
      positions_.FirstPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
      positions_.positionsCounter++;
    }
    else if (positions_.positionsCounter == 2) {
      positions_.ThirdPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
      positions_.positionsCounter++;

      ADNPointer<ADNPart> part = generateCuboid();

      sendPartToAdenita(part);
      DASCreatorEditors::resetPositions(positions_);
      display_ = false;
      tempPart_ == nullptr;

    }
  }
  else {

    if (positions_.positionsCounter == 0) {
      positions_.FirstPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
      positions_.positionsCounter++;

      positions_.FirstVector = SAMSON::getActiveCamera()->getBasisZ().normalizedVersion();
      positions_.vectorsCounter++;
    }
  }
}

void SEWireframeEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (wireframeType_ == DASCreator::Cuboid) {
    positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
  }
  else {
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
}

void SEWireframeEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    //SAMSON::requestViewportUpdate();

    if (wireframeType_ == DASCreator::Cuboid) {
      positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    }
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

  if (event->key() == Qt::Key_Escape) {
    display_ = false;
    DASCreatorEditors::resetPositions(positions_);
    SAMSON::requestViewportUpdate();
  }
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
