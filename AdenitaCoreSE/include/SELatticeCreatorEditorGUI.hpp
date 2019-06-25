#pragma once


#include "SBGWindowWidget.hpp" 
#include "ui_SELatticeCreatorEditorGUI.h"



class SELatticeCreatorEditor;

/// This class implements the GUI of the editor

// SAMSON Element generator pro tip: add GUI functionality in this class. 
// The non-GUI functionality (and the mouse and keyboard event handling methods that are specific to the editor) should go in the SELatticeCreatorEditor class

class SELatticeCreatorEditorGUI : public SBGWindowWidget {

	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SELatticeCreatorEditorGUI(SELatticeCreatorEditor* editor);																		///< Constructs a GUI for the editor
	virtual ~SELatticeCreatorEditorGUI();																										///< Destructs the GUI of the editor

	//@}

	/// \name Editor
	//@{

	SELatticeCreatorEditor*												getEditor() const;														///< Returns a pointer to the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the widget name (used as a title for the embedding window)
	virtual QPixmap												getLogo() const;														///< Returns the widget logo
	int															getFormat() const;														///< Returns the widget format
	virtual QString												getCitation() const;													///< Returns the citation information

	//@}

	///\name Settings
	//@{

	void														loadSettings(SBGSettings* settings);										///< Load GUI settings
	void														saveSettings(SBGSettings* settings);										///< Save GUI settings

	//@}
  public slots:
  
  void                            setHoneycomb();
  void                            setSquare();
  void                            onZPatternChanged(int index);
	void														onMaxXdsChanged(int val);
	void														onMaxYdsChanged(int val);
	void														onMaxZBpsChanged(int val);

private:

	Ui::SELatticeCreatorEditorGUIClass									ui;
	SELatticeCreatorEditor*												editor;

};

