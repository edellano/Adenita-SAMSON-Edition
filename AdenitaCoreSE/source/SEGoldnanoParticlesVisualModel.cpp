#include "SEGoldnanoParticlesVisualModel.hpp"
#include "SAMSON.hpp"


SEGoldnanoParticlesVisualModel::SEGoldnanoParticlesVisualModel() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

}

SEGoldnanoParticlesVisualModel::SEGoldnanoParticlesVisualModel(const SBNodeIndexer& nodeIndexer) {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

    SAMSON::getActiveDocument()->getNodes(goldAtoms_,
    SBNode::IsType(SBNode::Atom) && SBNode::IsSelected());
  
}

SEGoldnanoParticlesVisualModel::~SEGoldnanoParticlesVisualModel() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

}

 bool SEGoldnanoParticlesVisualModel::isSerializable() const {

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Modify the line below to "return true;" if you want this visual model be serializable (hence copyable, savable, etc.)

	return false;
	
}

void SEGoldnanoParticlesVisualModel::serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) const {

	SBMVisualModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to serialize your visual model.

}

void SEGoldnanoParticlesVisualModel::unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) {

	SBMVisualModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
	
	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to unserialize your visual model.

}

void SEGoldnanoParticlesVisualModel::eraseImplementation() {

	// SAMSON Element generator pro tip: modify this function when you need to perform special tasks when your visual model is erased (e.g. disconnect from nodes you are connected to).
	// Important: this function must be undoable (i.e. only call undoable functions or add an undo command to the undo stack)

}

void SEGoldnanoParticlesVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  displayGoldSphere();
}

void SEGoldnanoParticlesVisualModel::displayGoldSphere()
{
  unsigned int numberOfAtoms = goldAtoms_.size();

  float* positionData = new float[3 * numberOfAtoms];
  float* radiusData = new float[numberOfAtoms];
  float* colorData = new float[4 * numberOfAtoms];
  unsigned int* flagData = new unsigned int[numberOfAtoms];

  SBNodeMaterial* material = getMaterial();

  for (unsigned int i = 0; i < numberOfAtoms; i++) {

    SBAtom* currentAtom = static_cast<SBAtom*>(goldAtoms_.getNode(i));

    SBPosition3 position = currentAtom->getPosition();

    positionData[3 * i + 0] = (float)position.v[0].getValue();
    positionData[3 * i + 1] = (float)position.v[1].getValue();
    positionData[3 * i + 2] = (float)position.v[2].getValue();

    radiusData[i] = (float)currentAtom->getVanDerWaalsRadius().getValue();

    if (material) {
      material->getColorScheme()->getColor(colorData + 4 * i, currentAtom);
    }
    else {
      colorData[4 * i + 0] = 1.0f;
      colorData[4 * i + 1] = 1.0f;
      colorData[4 * i + 2] = 1.0f;
      colorData[4 * i + 3] = 1.0f;
    }

    colorData[4 * i + 0] = 0.0f;
    colorData[4 * i + 1] = 0.4f;
    colorData[4 * i + 2] = 0.6f;
    colorData[4 * i + 3] = 1.0f;

    flagData[i] = currentAtom->getInheritedFlags() | getInheritedFlags();

  }

  SAMSON::displaySpheres(
    numberOfAtoms,
    positionData,
    radiusData,
    colorData,
    flagData);

  delete[] positionData;
  delete[] radiusData;
  delete[] colorData;
  delete[] flagData;
}

void SEGoldnanoParticlesVisualModel::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function so that your visual model can cast shadows to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEGoldnanoParticlesVisualModel::displayForSelection() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to perform object picking.
	// Instead of rendering colors, your visual model is expected to write the index of a data graph node (obtained with getIndex()).
	// Implement this function so that your visual model can be selected (if you render its own index) or can be used to select other objects (if you render 
	// the other objects' indices), for example thanks to the utility functions provided by SAMSON (e.g. displaySpheresSelection, displayTrianglesSelection, etc.)

}

void SEGoldnanoParticlesVisualModel::expandBounds(SBIAPosition3& bounds) const {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine the model's spatial bounds. 
	// When this function returns, the bounds interval vector should contain the visual model. 

}

void SEGoldnanoParticlesVisualModel::collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData) {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine your model's influence on ambient occlusion.
	// Implement this function if you want your visual model to occlude other objects in ambient occlusion calculations.
	//
	// The ambientOcclusionData represents a nCellsX x nCellsY x nCellsZ grid of occlusion densities over the spatial region (boxOrigin, boxSize).
	// If your model represents geometry at position (x, y, z), then the occlusion density in corresponding grid nodes should be increased.
	//
	// Assuming x, y and z are given in length units (SBQuantity::length, SBQuantity::angstrom, etc.), the grid coordinates are:
	// SBQuantity::dimensionless xGrid = nCellsX * (x - boxOrigin.v[0]) / boxSize.v[0];
	// SBQuantity::dimensionless yGrid = nCellsY * (x - boxOrigin.v[1]) / boxSize.v[1];
	// SBQuantity::dimensionless zGrid = nCellsZ * (x - boxOrigin.v[2]) / boxSize.v[2];
	//
	// The corresponding density can be found at ambientOcclusionData[((int)zGrid.getValue() + 0)*nCellsY*nCellsX + ((int)yGrid.getValue() + 0)*nCellsX + ((int)xGrid.getValue() + 0)] (beware of grid bounds).
	// For higher-quality results, the influence of a point can be spread over neighboring grid nodes.

}

void SEGoldnanoParticlesVisualModel::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events (e.g. when a node for which you provide a visual representation emits a base signal, such as when it is erased)

}

void SEGoldnanoParticlesVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEGoldnanoParticlesVisualModel::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)

}

