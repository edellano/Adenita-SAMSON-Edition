#pragma once 

#include "SBGEditor.hpp"
#include "SELatticeCreatorEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include "ADNPart.hpp"
#include "SEAdenitaCoreSEApp.hpp"
#include "MSVDisplayHelper.hpp"

/// This class implements an editor

enum ZLatticePattern {
  ALLZ,
  TRIANGLE,
  CIRCLE
};

class SELatticeCreatorEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

	SELatticeCreatorEditor();																													///< Builds an editor					
	virtual ~SELatticeCreatorEditor();																											///< Destructs the editor

	//@}

	/// \name Identity
	//@{

  virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
  virtual QString												getName() const;														///< Returns the class name
  virtual QString	                      getDescription() const;	                      ///< Returns the menu item text
  virtual QPixmap												getLogo() const;														///< Returns the pixmap logo
  virtual int													getFormat() const;														///< Returns the format
  virtual QKeySequence										getShortcut() const;													///< Returns the shorcut
  virtual QString												getToolTip() const;														///< Returns the tool tip

  //@}

  ///\name Settings
  //@{

  virtual void												loadSettings(SBGSettings* settings);									///< Loads \p settings
  virtual void												saveSettings(SBGSettings* settings);									///< Saves \p settings

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

  void                                setLatticeType(LatticeType type);
  void                                setZPattern(ZLatticePattern pattern);
	/// \name GUI
	//@{

	SELatticeCreatorEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor

	//@}

	void															setMaxXds(int val);
	void															setMaxYds(int val);
	void															setMaxZBps(int val);
private:

  ADNPointer<ADNPart> generateLattice(bool mock = false);
  void displayLattice();
  void sendPartToAdenita(ADNPointer<ADNPart> lattice);

	LatticeType lType_ = LatticeType::Honeycomb;
  VGrid vGrid_;

  DASCreatorEditors::UIData positions_;
  bool display_ = false;
  ADNPointer<ADNPart> tempPart_ = nullptr;

	int maxXds_ = 32;
	int maxYds_ = 30;
	int maxZBps_ = 400;

	string xyText_ = "";
	string zText_ = "";

  ZLatticePattern zPattern_ = ZLatticePattern::ALLZ;

};


SB_REGISTER_TYPE(SELatticeCreatorEditor, "SELatticeCreatorEditor", "EA67625E-89B5-2EEA-156D-FC836214B0E4");
SB_DECLARE_BASE_TYPE(SELatticeCreatorEditor, SBGEditor);
