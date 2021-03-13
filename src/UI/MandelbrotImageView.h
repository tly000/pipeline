#pragma once

/*
 * MandelbrotImageView.h
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

#include "../Platform/CPU/CPUImage.h"
#include "GtkHeader.h"

struct MandelbrotImageView : Gtk::Overlay{
	MandelbrotImageView(struct MainWindow* window);

	void updateView(const CPUImage<unsigned>& cpuImage);

	void set_sensitive(bool s);

    void get_transformed_pointer(int& x, int& y);
    void update_transform();
protected:
    Cairo::Matrix transform;
	Glib::RefPtr<Gdk::Pixbuf> pixBuf;
	Gtk::DrawingArea drawArea;
	MainWindow* window;

	int xPos = -1, yPos = -1;
	int w = -1, h = -1;
	bool drawingRectangle = false;

	int lastXPos = -1, lastYPos = -1;

	bool mousePositionInRectangle();
	bool mousePositionOnRectangleBorder(int borderSize);
};


