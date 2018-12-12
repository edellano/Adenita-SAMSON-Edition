#pragma once


#include "SBGDataGraphNodeProperties.hpp" 
#include "ui_SEAdenitaVisualModelProperties.h"
#include "SBPointer.hpp"
#include "SEAdenitaVisualModel.hpp"


/// This class implements the property window of the visual model

// SAMSON Element generator pro tip: add GUI functionality in this class. The non-GUI functionality should go in the SEAdenitaVisualModel class

// SAMSON Element generator pro tip: if you do not want to have a property window associated to you visual model, do not expose it in the descriptor. 
// Precisely, remove the line SB_ELEMENT_CLASS(SEAdenitaVisualModelProperties); from the file SEAdenitaDescriptor.cpp

class SEAdenitaVisualModelProperties : public SBGDataGraphNodeProperties {

	SB_CLASS
	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SEAdenitaVisualModelProperties();																								///< Constructs a widget to show the properties of the visual model
	virtual ~SEAdenitaVisualModelProperties();																						///< Destructs the widget

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

		Observer(SEAdenitaVisualModelProperties* properties);
		virtual ~Observer();

		friend class SEAdenitaVisualModelProperties;

		void														onBaseEvent(SBBaseEvent* baseEvent);								///< Base event management
		void														onVisualEvent(SBVisualEvent* visualEvent);							///< Visual event management

		SEAdenitaVisualModelProperties* properties;

	};

public slots:
void                              onDiscreteSliderScaleChanged(int val);
void                              onSliderScaleChanged(int val);
void                              onSliderVisibilityChanged(int val);
void                              onSpinboxVisibilityChanged(double val);
void                              onSliderDimensionChanged(int val);
void                              onPropertyColorsChanged(int propertyIdx);

private:

	friend class SEAdenitaVisualModel;
	Ui::SEAdenitaVisualModelPropertiesClass							ui;
	SBPointer<SEAdenitaVisualModel>									visualModel;

	SBPointer<Observer>											observer;

};

SB_REGISTER_TYPE(SEAdenitaVisualModelProperties, "SEAdenitaVisualModelProperties", "02A68809-10D4-43FA-13A0-032DDB897BEE");
SB_DECLARE_BASE_TYPE(SEAdenitaVisualModelProperties, SBGDataGraphNodeProperties);
