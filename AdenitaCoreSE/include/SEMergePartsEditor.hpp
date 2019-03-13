#pragma once 

#include "SBGEditor.hpp"
#include "SEMergePartsEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include "SEAdenitaCoreSEApp.hpp"


/// This class implements an editor

class SEMergePartsEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

  SEMergePartsEditor();																													///< Builds an editor					
	virtual ~SEMergePartsEditor();																											///< Destructs the editor

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

  SEMergePartsEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor

  struct Element {
    int type = -1;
    ADNPointer<ADNSingleStrand> ss;
    ADNPointer<ADNDoubleStrand> ds;

    std::string GetName() {
      std::string n = "";
      if (type == 0) {
        n = ds->getName();
      }
      else if (type == 1) {
        n = ss->getName();
      }
      return n;
    }
  };

  std::map<int, ADNPointer<ADNPart>> getPartsList();
  std::map<int, Element> getElementsList();
  void MergeParts(int idx, int jdx);
  void MoveElement(int edx, int pdx);

private:
  SEAdenitaCoreSEApp*					          getAdenitaApp() const;															///< Returns a pointer to the app

  std::map<int, ADNPointer<ADNPart>> indexParts_;
  std::map<int, Element> indexElements_;
};


SB_REGISTER_TYPE(SEMergePartsEditor, "SEMergePartsEditor", "EB812444-8EA8-BD83-988D-AFF5987461D8");
SB_DECLARE_BASE_TYPE(SEMergePartsEditor, SBGEditor);
