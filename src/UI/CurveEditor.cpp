/*
 * CurveEditor.cpp
 *
 *  Created on: 16.06.2016
 *      Author: tly
 */

#include "CurveEditor.h"

CurveEditor::CurveEditor() {
	this->set_column_homogeneous(true);
	this->set_row_homogeneous(true);
	this->set_row_spacing(10);
	this->set_column_spacing(10);

	auto curveFrame = Gtk::manage(new Gtk::Frame);
	curveFrame->add(this->curveArea);
	this->attach(*curveFrame,0,0,9,9);
	this->curveArea.set_size_request(300,300);
	auto gradient1Frame = Gtk::manage(new Gtk::Frame);
	gradient1Frame->add(this->gradientIn);
	this->attach(*gradient1Frame,0,9,9,1);
	auto gradient2Frame = Gtk::manage(new Gtk::Frame);
	gradient2Frame->add(this->gradientOut);
	this->attach(*gradient2Frame,9,0,1,9);

	this->curveArea.add_events(
		Gdk::EventMask::BUTTON_MOTION_MASK |
		Gdk::EventMask::BUTTON_PRESS_MASK |
		Gdk::EventMask::BUTTON_RELEASE_MASK |
		Gdk::EventMask::POINTER_MOTION_MASK |
		Gdk::EventMask::SCROLL_MASK
	);
	this->curveArea.signal_scroll_event().connect([this](GdkEventScroll* e){
		if(this->selectedSegment != -1){
			auto& segment = this->c.at(this->selectedSegment);
			segment.repetition += (e->direction == GDK_SCROLL_UP) - (e->direction == GDK_SCROLL_DOWN);
			this->queue_draw();
		}

		return true;
	});
	this->curveArea.signal_button_press_event().connect([this](GdkEventButton* e){
		int mx,my;
		this->curveArea.get_pointer(mx,my);
		int w = this->curveArea.get_allocated_width();
		int h = this->curveArea.get_allocated_height();

		if(e->button == 1){
			if(this->hoveringPoint != -1){
				this->selectedPoint = this->hoveringPoint;
			}else{
				float t = mx / float(w);
				auto it = std::lower_bound(this->c.begin(),this->c.end(),t,[](auto& a, auto& b){
					return a.end.x < b;
				});
				if(it == this->c.end()){
					this->selectedSegment = -1;
				}else{
					this->selectedSegment = std::distance(this->c.begin(),it);
				}
			}
			this->curveArea.get_pointer(this->lastX,this->lastY);
		}else if(e->button == 3 && this->hoveringPoint != -1){
			//not removing first or last point
			if(this->hoveringPoint != int(this->c.size()) && this->hoveringPoint != 0){
				Curve c = this->c;
				auto endPoint = c.at(this->hoveringPoint).end;
				c.erase(c.begin() + this->hoveringPoint);
				c.at(this->hoveringPoint-1).end = endPoint;
				this->setCurve(c);
				this->queue_draw();
				return true;
			}
		}else if(e->button == 3 && this->hoveringPoint == -1){
			//add point.
			float2 p{
				mx / float(w), 1 - my / float(h)
			};

			Curve c = this->c;
			auto it = std::lower_bound(c.begin(),c.end(),p.x,[](auto& a, auto& b){
				return a.end.x < b;
			});
			if(it == c.end()){
				return true;
			}else{
				float2 oldStart = it->start;
				it->start = p;
				c.insert(it,CurveSegment{oldStart,p,1,1,0,0});
				this->setCurve(c);
				this->queue_draw();
				return true;
			}
		}
		return true;
	});
	this->curveArea.signal_motion_notify_event().connect([=](GdkEventMotion* e){
		int mx,my;
		this->curveArea.get_pointer(mx,my);

		int w = this->curveArea.get_allocated_width();
		int h = this->curveArea.get_allocated_height();
		int dx = mx - this->lastX,
			dy = my - this->lastY;

		if((e->state & GDK_BUTTON1_MASK) && this->selectedPoint != -1){
			float2 newPoint = this->selectedPoint != int(this->c.size()) ?
				this->c.at(this->selectedPoint).start :
				this->c.at(this->selectedPoint-1).end;

			newPoint.x +=  dx / float(w);
			newPoint.y -= dy / float(h);
			newPoint.x = std::min(std::max(0.0f,newPoint.x),1.0f);
			newPoint.y = std::min(std::max(0.0f,newPoint.y),1.0f);

			if(this->selectedPoint != int(this->c.size())){
				auto& segment = this->c.at(this->selectedPoint);
				newPoint.x = std::min(newPoint.x,segment.end.x);
			}
			if(this->selectedPoint != 0){
				auto& segment = this->c.at(this->selectedPoint-1);
				newPoint.x = std::max(newPoint.x,segment.start.x);
			}
			if(this->selectedPoint != int(this->c.size())){
				this->c.at(this->selectedPoint).start = newPoint;
			}
			if(this->selectedPoint != 0){
				this->c.at(this->selectedPoint-1).end = newPoint;
			}
			this->queue_draw();
		}else if((e->state & GDK_BUTTON1_MASK) && this->selectedSegment != -1){
			auto& segment = this->c.at(this->selectedSegment);
			segment.paramA = std::max(segment.paramA + dx / float(w),0.0f);
			segment.paramB = std::max(segment.paramB - (segment.end.y < segment.start.y ? -1 : 1) * dy / float(h),0.0f);
			this->queue_draw();
		}else{
			int hoveringPoint = -1;

			auto p = this->c.front().start;
			int px = p.x * w, py = (1-p.y) * h;
			if((px-mx)*(px-mx)+(py-my)*(py-my) < 40){
				hoveringPoint = 0;
			}else{
				int i = 1;
				for(auto& s : this->c){
					auto p = s.end;
					float px = p.x * w, py = (1-p.y) * h;

					float d = (px-mx)*(px-mx)+(py-my)*(py-my);
					if(d < 40){
						hoveringPoint = i;
						break;
					}
					i++;
				}
			}
			if(hoveringPoint != this->hoveringPoint){
				this->curveArea.queue_draw();
			}
			this->hoveringPoint = hoveringPoint;
		}
		this->lastX = mx;
		this->lastY = my;
		return true;
	});
	this->curveArea.signal_button_release_event().connect([=](GdkEventButton* e){
		this->selectedPoint = -1;
		return true;
	});
	this->curveArea.signal_draw().connect([this](const Cairo::RefPtr< Cairo::Context>& gc){
		int w = this->curveArea.get_allocated_width();
		int h = this->curveArea.get_allocated_height();

		gc->set_line_width(1.4f);
		gc->move_to(0,h);

		const int samples = 200;
		for(int i = 0; i < samples; i++){
			float t = i/float(samples-1);
			float y = curveSample(this->c.data(),this->c.size(), t);
			gc->line_to(w * t,h * (1-y));
		}
		gc->stroke();

		gc->set_line_width(1.4f);
		auto p = this->c.front().start;
		gc->set_source_rgb(0.3,0.3,0.3);
		gc->arc(p.x * w, (1-p.y) * h,4 + 2 * (this->hoveringPoint == 0),0,2 * G_PI);
		gc->stroke();

		int i = 1;
		for(auto& s : this->c){
			auto p = s.end;
			gc->set_source_rgb(0.3,0.3,0.3);
			gc->arc(p.x * w, (1-p.y) * h,4 + 2 * (this->hoveringPoint == i),0,2 * G_PI);
			gc->stroke();

			gc->set_source_rgba(0,0,0,0.2);
			gc->move_to(p.x * w,0);
			gc->line_to(p.x * w,h);
			gc->stroke();

			i++;
		}
		this->gradientOut.queue_draw();

		return false;
	});
	this->gradientIn.signal_draw().connect([this](const Cairo::RefPtr< Cairo::Context>& gc){
		int w = this->gradientIn.get_allocated_width();
		int h = this->gradientIn.get_allocated_height();

		auto grad = Cairo::LinearGradient::create(0,0,w,h);

		int i = 0;
		for(auto& c : this->g){
			grad->add_color_stop_rgb(i / float(this->g.size()-1), c[0],c[1],c[2]);
			i++;
		}
		gc->set_source(grad);
		gc->rectangle(0,0,w,h);
		gc->fill();

		return false;
	});
	this->gradientOut.signal_draw().connect([this](const Cairo::RefPtr< Cairo::Context>& gc){
		int w = this->gradientOut.get_allocated_width();
		int h = this->gradientOut.get_allocated_height();

		for(int i = 0; i < h; i++){
			float t = 1 - i / float(h-1);
			auto col = gradientSample(this->g,curveSample(this->c.data(),this->c.size(), t));
			gc->set_source_rgb(col[0],col[1],col[2]);
			gc->move_to(0,i);
			gc->line_to(w,i);
			gc->stroke();
		}

		return false;
	});
}

void CurveEditor::setGradient(const Gradient& g) {
	this->g = g;
}

void CurveEditor::setCurve(const Curve& c) {
	this->c = c;
	this->selectedPoint = -1;
}

Curve CurveEditor::getCurve() const {
	return c;
}
