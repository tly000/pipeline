#pragma once
#include "GtkHeader.h"
#include "../Type/Curve.h"
#include "../Type/Gradient.h"
/*
 * CurveEditor.h
 *
 *  Created on: 16.06.2016
 *      Author: tly
 */

struct CurveEditor : Gtk::Grid{
	Gtk::DrawingArea curveArea;
	Gtk::DrawingArea gradientIn;
	Gtk::DrawingArea gradientOut;

	CurveEditor();

	void setGradient(const Gradient& g);
	void setCurve(const Curve& c);

	Curve getCurve() const;
protected:
	Gradient g;
	Curve c;
	std::array<float,256> histogram{{}};

	int selectedPoint = -1;
	int selectedSegment = -1;
	int hoveringPoint = -1;
	int lastX, lastY;
};



