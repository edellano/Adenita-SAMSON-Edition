#pragma once


#include "SBGDataGraphNodeProperties.hpp" 
#include "ui_SEGoldnanoParticlesVisualModelProperties.h"
#include "SBPointer.hpp"
#include "SEGoldnanoParticlesVisualModel.hpp"


/// This class implements the property window of the visual model

// SAMSON Element generator pro tip: add GUI functionality in this class. The non-GUI functionality should go in the SEGoldnanoParticlesVisualModel class

// SAMSON Element generator pro tip: if you do not want to have a property window associated to you visual model, do not expose it in the descriptor. 
// Precisely, remove the line SB_ELEMENT_CLASS(SEGoldnanoParticlesVisualModelProperties); from the file SEGoldnanoParticlesDescriptor.cpp

class SEGoldnanoParticlesVisualModelProperties : public SBGDataGraphNodeProperties {

	SB_CLASS
	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SEGoldnanoParticlesVisualModelProperties();																								///< Constructs a widget to show the properties of the visual model
	virtual ~SEGoldnanoParticlesVisualModelProperties();																						///< Destructs the widget

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

	void														loadSettings(SBGSettings* settings);									///< Load GUI settings
	void														saveSettings(SBGSettings* settings);									///< Save GUI settings

	//@}

	///\name Setup
	//@{

	virtual bool												setup();																///< Initializes the properties widget
	virtual bool												setup(SBNode* node);													///< Initializes the properties widget

	//@}

	class Observer : public SBCReferenceTarget {

		Observer(SEGoldnanoParticlesVisualModelProperties* properties);
		virtual ~Observer();

		friend class SEGoldnanoParticlesVisualModelProperties;

		void														onBaseEvent(SBBaseEvent* baseEvent);								///< Base event management
		void														onVisualEvent(SBVisualEvent* visualEvent);							///< Visual event management

		SEGoldnanoParticlesVisualModelProperties* properties;

	};

private:

	friend class SEGoldnanoParticlesVisualModel;
	Ui::SEGoldnanoParticlesVisualModelPropertiesClass							ui;
	SBPointer<SEGoldnanoParticlesVisualModel>									visualModel;

	SBPointer<Observer>											observer;

};

SB_REGISTER_TYPE(SEGoldnanoParticlesVisualModelProperties, "SEGoldnanoParticlesVisualModelProperties", "7E07A97C-279E-5D33-9F37-A2D47520C149");
SB_DECLARE_BASE_TYPE(SEGoldnanoParticlesVisualModelProperties, SBGDataGraphNodeProperties);
