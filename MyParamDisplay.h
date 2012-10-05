/*
 *  MyParamDisplay.h
 *  Chip700
 *
 *  Created by osoumen on 12/10/05.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "vstgui.h"

class CMyParamDisplay : public CParamDisplay
{
public:
	CMyParamDisplay(const CRect& size, float valueMultipler = 1.0f, char *unitStr = 0, CBitmap* background = 0, const long style = 0);
	virtual ~CMyParamDisplay();
	
	CLASS_METHODS(CMyParamDisplay, CParamDisplay)
protected:
	static void stringConvert(float value, char *string, void *userData);
	
	static const int	UNITSTR_LEN = 64;
	char	mUnitStr[UNITSTR_LEN];
	float	mValueMultipler;
};