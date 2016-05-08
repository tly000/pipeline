#include "MandelbrotImageView.h"
#include <iostream>
#include "PipelineUI.h"

/*
 * MandelbrotImageView.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

int sgn(int x){
	return (x < 0) ? -1 : 1;
}

MandelbrotImageView::MandelbrotImageView(struct MainWindow* window)
:image(Glib::ustring("test_double.bmp")),
 window(window){
	this->add(image);
	this->add_overlay(drawArea);

	drawArea.add_events(
		Gdk::EventMask::BUTTON_MOTION_MASK |
		Gdk::EventMask::BUTTON_PRESS_MASK |
		Gdk::EventMask::BUTTON_RELEASE_MASK |
		Gdk::EventMask::POINTER_MOTION_MASK
	);

	drawArea.signal_button_press_event().connect([this](GdkEventButton* e){
		if(this->mousePositionInRectangle()){
			if(e->type == GdkEventType::GDK_2BUTTON_PRESS && (e->button == 1 || e->button == 3)){
				float scale = this->w / float(this->pixBuf->get_width());
				int xPosCorrected = this->xPos - (this->image.get_allocated_width() - this->pixBuf->get_width()) * 0.5;
				int yPosCorrected = this->yPos - (this->image.get_allocated_height() - this->pixBuf->get_height()) * 0.5;

				float x = 2.0 * (xPosCorrected + this->w/2.0f)/ float(this->pixBuf->get_width()) - 1;
				float y = 2.0 * (yPosCorrected + this->h/2.0f)/ float(this->pixBuf->get_height()) - 1;
				y *= float(this->pixBuf->get_height())/float(this->pixBuf->get_width());
				if(e->button == 1){
					this->window->getSelectedPlatform()->translate(x,y);
					this->window->getSelectedPlatform()->scale(scale);
				}else{
					this->window->getSelectedPlatform()->scale(1/scale);
					this->window->getSelectedPlatform()->translate(-x, -y);
				}

				this->w = -1;

				this->window->calculateNow();
				return true;
			}
			return false;
		}
		if(e->button == 1){
			this->drawingRectangle = true;
			this->image.get_pointer(this->xPos,this->yPos);
			this->w = -1;
			drawArea.queue_draw();
		}
		return true;
	});
	auto setRectangleFromMouse = [this]{
		int xPos,yPos;
		this->image.get_pointer(xPos,yPos);
		this->w = xPos - this->xPos;
		this->h = yPos - this->yPos;

		float ratio = float(this->pixBuf->get_width()) / float(this->pixBuf->get_height());
		if(abs(this->w) < ratio * abs(this->h)){
			this->w = ratio * sgn(this->w) * abs(this->h);
		}else{
			this->h = 1/ratio * sgn(this->h) * abs(this->w);
		}
	};
	drawArea.signal_motion_notify_event().connect([=](GdkEventMotion* e){
		if(this->drawingRectangle){
			setRectangleFromMouse();
			drawArea.queue_draw();
		}else{
			auto window = this->window->property_window().get_value();
			static auto cursor1 = Gdk::Cursor::create(window->get_display(),Gdk::CursorType::FLEUR);
			static auto cursor2 = Gdk::Cursor::create(window->get_display(),Gdk::CursorType::ARROW);

			int mx,my;
			this->image.get_pointer(mx,my);
			if(this->mousePositionInRectangle()){
				if(window->get_cursor() != cursor1){
					window->set_cursor(cursor1);
				}
				if(e->state & GDK_BUTTON1_MASK){
					if(this->lastXPos != -1){
						int dx = mx - this->lastXPos;
						int dy = my - this->lastYPos;

						this->xPos += dx;
						this->yPos += dy;

						this->queue_draw();
					}
				}
			}else{
				if(window->get_cursor() != cursor2){
					window->set_cursor(cursor2);
				}
			}

			this->lastXPos = mx;
			this->lastYPos = my;
		}
		return true;
	});
	drawArea.signal_button_release_event().connect([=](GdkEventButton* e){
		if(drawingRectangle){
			setRectangleFromMouse();
			if(this->w < 0){
				this->xPos += w;
				w = -w;
			}
			if(this->h < 0){
				this->yPos += h;
				h = -h;
			}
			if(std::max(abs(this->w),abs(this->h)) < 10){
				this->w = -1;
			}
			drawArea.queue_draw();
			drawingRectangle = false;
		}
		return true;
	});

	drawArea.signal_draw().connect([this](const Cairo::RefPtr< Cairo::Context>& gc){
		static const std::vector<double> dashPattern = {
			3
		};

		if(this->xPos != -1 && this->w != -1){
			gc->set_line_width(1);
			gc->set_source_rgb(0,0,0);
			gc->set_dash(dashPattern,0);
			gc->rectangle(this->xPos + 0.5,this->yPos + 0.5,this->w,this->h);
			gc->stroke();
			gc->set_source_rgb(1,1,1);
			gc->set_dash(dashPattern,3);
			gc->rectangle(this->xPos+ 0.5 ,this->yPos + 0.5,this->w,this->h);
			gc->stroke();
		}
		return false;
	});
}

void MandelbrotImageView::updateView(const CPUImage<unsigned>& cpuImage) {
	auto pix = Gdk::Pixbuf::create_from_data(
		reinterpret_cast<const guint8*>(cpuImage.getDataBuffer().data()),
		Gdk::Colorspace::COLORSPACE_RGB,
		true,8,cpuImage.getWidth(),
		cpuImage.getHeight(),
		4 * cpuImage.getWidth()
	);

	int newWidth = cpuImage.getWidth();
	int newHeight = cpuImage.getWidth();
	bool needsScaling = false;

	if(needsScaling){
		pixBuf = pix->scale_simple(newWidth,newHeight,Gdk::INTERP_HYPER);
	}else{
		pixBuf = pix;
	}
	this->image.set(pixBuf);
}

bool MandelbrotImageView::mousePositionInRectangle() {
	int mx,my;
	this->image.get_pointer(mx,my);
	return this->w != -1 &&
		(mx > this->xPos) && (mx <= this->xPos + this->w) &&
		(my > this->yPos) && (my <= this->yPos + this->h);
}

bool MandelbrotImageView::mousePositionOnRectangleBorder(int borderSize) {
	int mx,my;
	this->image.get_pointer(mx,my);

	if(this->w != -1){
		int cx = this->xPos + this->w / 2;
		int cy = this->yPos + this->h / 2;

		mx = abs(mx - cx);
		my = abs(my - cy);

		return mx <= this->w/2 && my <= this->h/2 &&
			   (abs(mx - this->w / 2) <= borderSize || abs(my - this->h / 2) <= borderSize);
	}
	return false;
}
