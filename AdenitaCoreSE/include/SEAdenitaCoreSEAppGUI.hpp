#pragma once


#include "SBGApp.hpp" 
#include "ui_SEAdenitaCoreSEAppGUI.h"
#include <QMessageBox>


class SEAdenitaCoreSEApp;

/// This class implements the GUI of the app

// SAMSON Element generator pro tip: add GUI functionality in this class. The non-GUI functionality should go in the SEAdenitaCoreSEApp class

class SEAdenitaCoreSEAppGUI : public SBGApp {

	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SEAdenitaCoreSEAppGUI(SEAdenitaCoreSEApp* t);																									///< Constructs a GUI for the app
	virtual ~SEAdenitaCoreSEAppGUI();																										///< Destructs the GUI of the app

	//@}

	/// \name App
	//@{

	SEAdenitaCoreSEApp*												getApp() const;															///< Returns a pointer to the app

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

  void onCreate();
  void onLoadFile();
  void onSaveFile();
  void onExport();
  void onSetScaffold();
  void onCenterPart();
  void onAcceptExport();

private:
  std::string IsJsonCadnano(QString filename);

	Ui::SEAdenitaCoreSEAppGUIClass									ui;

  QComboBox* typeSelection_ = nullptr;
  QComboBox* exportType_ = nullptr;
};

