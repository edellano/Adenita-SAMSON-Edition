#include "SEWireframeEditorGUI.hpp"
#include "SEWireframeEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEWireframeEditorGUI::SEWireframeEditorGUI(SEWireframeEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/";

  QIcon tetrahedronIcon;
  tetrahedronIcon.addFile(QString(string(iconPath + "tetrahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbTetrahedron->setIcon(tetrahedronIcon);

  QIcon cubeIcon;
  cubeIcon.addFile(QString(string(iconPath + "cube.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbCube->setIcon(cubeIcon);
  
  QIcon cuboctahedronIcon;
  cuboctahedronIcon.addFile(QString(string(iconPath + "cuboctahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbCuboctahedron->setIcon(cuboctahedronIcon);

  QIcon dodecahedronIcon;
  dodecahedronIcon.addFile(QString(string(iconPath + "dodecahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbDodecahedron->setIcon(dodecahedronIcon);

  QIcon icosahedronIcon;
  icosahedronIcon.addFile(QString(string(iconPath + "icosahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbIcosahedron->setIcon(icosahedronIcon);

  QIcon icosidodecahedronIcon;
  icosidodecahedronIcon.addFile(QString(string(iconPath + "icosidodecahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbIcosidodecahedron->setIcon(icosidodecahedronIcon);

  QIcon octahedronIcon;
  octahedronIcon.addFile(QString(string(iconPath + "octahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbOctahedron->setIcon(octahedronIcon);

  QIcon rhombicuboctahedronIcon;
  rhombicuboctahedronIcon.addFile(QString(string(iconPath + "rhombicuboctahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbRhombicuboctahedron->setIcon(rhombicuboctahedronIcon);

  QIcon snubCubeIcon;
  snubCubeIcon.addFile(QString(string(iconPath + "snub_Cube.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbSnub_cube->setIcon(snubCubeIcon);

  QIcon truncated_cubeIcon;
  truncated_cubeIcon.addFile(QString(string(iconPath + "truncated_cube.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbTruncated_cube->setIcon(truncated_cubeIcon);

  QIcon truncated_cuboctahedronIcon;
  truncated_cuboctahedronIcon.addFile(QString(string(iconPath + "Truncated_cuboctahedron.png").c_str()), QSize(), QIcon::Normal, QIcon::Off);
  ui.rdbTruncated_cuboctahedron->setIcon(truncated_cuboctahedronIcon);

  ui.rdbCuboid->setIcon(cubeIcon);

}

SEWireframeEditorGUI::~SEWireframeEditorGUI() {

}

SEWireframeEditor* SEWireframeEditorGUI::getEditor() const { return editor; }

void SEWireframeEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEWireframeEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEWireframeEditorGUI::onTetrahedronClicked()
{
  editor->setWireframeType(DASCreator::Tetrahedron);
}

void SEWireframeEditorGUI::onCubeClicked()
{
  editor->setWireframeType(DASCreator::Cube);
}

void SEWireframeEditorGUI::onCuboidClicked()
{
  editor->setWireframeType(DASCreator::Cuboid);
}

void SEWireframeEditorGUI::onOctahedronClicked()
{
  editor->setWireframeType(DASCreator::Octahedron);
}

void SEWireframeEditorGUI::onDodecahedronClicked()
{
  editor->setWireframeType(DASCreator::Dodecahedron);
}

void SEWireframeEditorGUI::onCubocahedronClicked()
{
  editor->setWireframeType(DASCreator::Cubocahedron);
}

void SEWireframeEditorGUI::onIcosahedronClicked()
{
  editor->setWireframeType(DASCreator::Icosahedron);
}

void SEWireframeEditorGUI::onIcosidodecahedronClicked()
{
  editor->setWireframeType(DASCreator::Icosidodecahedron);
}

void SEWireframeEditorGUI::onRhombicuboctahedronClicked()
{
  editor->setWireframeType(DASCreator::Rhombicuboctahedron);
}

void SEWireframeEditorGUI::onSnubCubeClicked()
{
  editor->setWireframeType(DASCreator::Snub_cube);
}

void SEWireframeEditorGUI::onTruncatedCubeClicked()
{
  editor->setWireframeType(DASCreator::Truncated_cube);
}

void SEWireframeEditorGUI::onTruncatedCuboctahedronClicked()
{
  editor->setWireframeType(DASCreator::Truncated_cuboctahedron);
}

void SEWireframeEditorGUI::onHelixClicked()
{
  editor->setWireframeType(DASCreator::Helix);
}

void SEWireframeEditorGUI::onStickmanClicked()
{
  editor->setWireframeType(DASCreator::Stickman);
}

void SEWireframeEditorGUI::onBunnyClicked()
{
  editor->setWireframeType(DASCreator::Bunny);
}

SBCContainerUUID SEWireframeEditorGUI::getUUID() const { return SBCContainerUUID( "3A914FAF-691B-B3A1-3B91-58E77E7CCC6D" );}

QPixmap SEWireframeEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEWireframeEditorIcon.png"));

}

QString SEWireframeEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SEWireframeEditor"; 

}

int SEWireframeEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEWireframeEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
