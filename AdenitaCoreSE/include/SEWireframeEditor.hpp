#pragma once 

#include "SBGEditor.hpp"
#include "SEWireframeEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include <QOpenGLFunctions_4_3_Core>

#include "SEAdenitaCoreSEApp.hpp"
#include "ADNPart.hpp"
#include "ADNDisplayHelper.hpp"
#include "DASCreator.hpp"
/// This class implements an editor

class SEWireframeEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

	SEWireframeEditor();																													///< Builds an editor					
	virtual ~SEWireframeEditor();																											///< Destructs the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the class name
	virtual QString												getText() const;														///< Returns the menu item text
	virtual QPixmap												getLogo() const;														///< Returns the pixmap logo
	virtual QKeySequence										getShortcut() const;													///< Returns the shorcut
	virtual QString												getToolTip() const;														///< Returns the tool tip
  virtual QString	getDescription() const;	///< Returns the menu item text

	//@}

	/// \name Editing
	//@{

	virtual void												beginEditing();															///< Called when editing is about to begin
	virtual void												endEditing();															///< Called when editing is about to end

	//@}

	/// \name Actions
	//@{

	virtual void												getActions(SBVector<SBAction*>& actionVector);							///< Gets the editor's actions

	//@}

	/// \name Rendering
	//@{

	virtual void												display();																///< Displays the editor
	virtual void												displayForShadow();														///< Displays the editor for shadow purposes
	virtual void												displayInterface();														///< Displays the 2D interface in the viewport

	//@}

	/// \name GUI Events
	//@{

	virtual void												mousePressEvent(QMouseEvent* event);									///< Handles mouse press event
	virtual void												mouseReleaseEvent(QMouseEvent* event);									///< Handles mouse release event
	virtual void												mouseMoveEvent(QMouseEvent* event);										///< Handles mouse move event
	virtual void												mouseDoubleClickEvent(QMouseEvent* event);								///< Handles mouse double click event

	virtual void												wheelEvent(QWheelEvent* event);											///< Handles wheel event

	virtual void												keyPressEvent(QKeyEvent* event);										///< Handles key press event
	virtual void												keyReleaseEvent(QKeyEvent* event);										///< Handles key release event

	//@}

	/// \name SAMSON Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDynamicalEvent(SBDynamicalEvent* dynamicalEvent);						///< Handles dynamical events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

	//@}

	/// \name GUI
	//@{

	SEWireframeEditorGUI*								getPropertyWidget() const;												///< Returns the property widget of the editor

	//@}

  void                                setWireframeType(DASCreator::EditorType type);

private:
  ADNPointer<ADNPart> generateCuboid(bool mock = false);
  ADNPointer<ADNPart> generateWireframe(bool mock = false);
  void sendPartToAdenita(ADNPointer<ADNPart> part);

  DASCreatorEditors::UIData positions_;
  bool display_ = false;
  ADNPointer<ADNPart> tempPart_ = nullptr;
  DASCreator::EditorType wireframeType_ = DASCreator::Tetrahedron;

};


SB_REGISTER_TYPE(SEWireframeEditor, "SEWireframeEditor", "F1F29042-3D87-DA61-BC5C-D3348EB2E1FA");
SB_DECLARE_BASE_TYPE(SEWireframeEditor, SBGEditor);
