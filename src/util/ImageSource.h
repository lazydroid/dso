//
// ImageSource class
//

#pragma once
#include "util/settings.h"
#include "util/globalFuncs.h"
#include "util/globalCalib.h"

using namespace dso;

class ImageSource
{
public:
	ImageSource() {};
	virtual ~ImageSource() {};

	virtual void setGlobalCalibration() = 0;
	virtual int getNumImages() = 0;
	virtual double getTimestamp(int id) = 0;
	virtual ImageAndExposure* getImage(int id, bool forceLoadDirectly=false) = 0;
	virtual float* getPhotometricGamma() = 0;
};
