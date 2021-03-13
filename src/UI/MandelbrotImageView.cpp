#include "MandelbrotImageView.h"
#include "PipelineUI.h"

/*
 * MandelbrotImageView.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

int sgn(int x) {
    return (x < 0) ? -1 : 1;
}

MandelbrotImageView::MandelbrotImageView(struct MainWindow *window)
    : window(window), transform(Cairo::identity_matrix()) {
    this->add(drawArea);
    this->set_sensitive(true);
    this->set_size_request(512, 512);

    this->signal_size_allocate().connect([this](const Gtk::Allocation &alloc) {
        update_transform();
    });

    drawArea.signal_button_press_event().connect([this](GdkEventButton *e) {
        if (!pixBuf)
            return false;

        if (mousePositionInRectangle()) {
            if (e->type == GdkEventType::GDK_2BUTTON_PRESS && (e->button == 1 || e->button == 3)) {
                float scale = this->w / float(this->pixBuf->get_width());
                float x = 2.0f * (this->xPos + this->w / 2.0f) / float(this->pixBuf->get_width()) - 1;
                float y = 2.0f * (this->yPos + this->h / 2.0f) / float(this->pixBuf->get_height()) - 1;
                y *= float(this->pixBuf->get_height()) / float(this->pixBuf->get_width());
                if (e->button == 1) {
                    this->window->getSelectedPlatform()->translate(x, y);
                    this->window->getSelectedPlatform()->scale(scale);
                } else {
                    this->window->getSelectedPlatform()->scale(1 / scale);
                    this->window->getSelectedPlatform()->translate(-x, -y);
                }

                this->w = -1;

                this->window->calculateNow();
                return true;
            }
            return false;
        }
        if (e->button == 1) {
            this->drawingRectangle = true;
            this->get_transformed_pointer(this->xPos, this->yPos);
            this->w = -1;
            drawArea.queue_draw();
        }
        return true;
    });
    auto setRectangleFromMouse = [this] {
        int xPos, yPos;
        this->get_transformed_pointer(xPos, yPos);
        this->w = xPos - this->xPos;
        this->h = yPos - this->yPos;

        float ratio = float(this->pixBuf->get_width()) / float(this->pixBuf->get_height());
        if (abs(this->w) < ratio * abs(this->h)) {
            this->w = ratio * sgn(this->w) * abs(this->h);
        } else {
            this->h = 1 / ratio * sgn(this->h) * abs(this->w);
        }
    };
    drawArea.signal_motion_notify_event().connect([=](GdkEventMotion *e) {
        if (this->drawingRectangle) {
            setRectangleFromMouse();
            drawArea.queue_draw();
        } else {
            auto window = this->window->property_window().get_value();
            static auto cursor1 = Gdk::Cursor::create(window->get_display(), Gdk::CursorType::FLEUR);
            static auto cursor2 = Gdk::Cursor::create(window->get_display(), Gdk::CursorType::ARROW);

            int mx, my;
            this->get_transformed_pointer(mx, my);
            if (this->mousePositionInRectangle() || draggingRectangle) {
                if (window->get_cursor() != cursor1) {
                    window->set_cursor(cursor1);
                }
                if (e->state & GDK_BUTTON1_MASK) {
                    draggingRectangle = true;
                    if (this->lastXPos != -1) {
                        int dx = mx - this->lastXPos;
                        int dy = my - this->lastYPos;

                        this->xPos += dx;
                        this->yPos += dy;

                        this->queue_draw();
                    }
                } else {
                    draggingRectangle = false;
                }
            } else {
                draggingRectangle = false;
                if (window->get_cursor() != cursor2) {
                    window->set_cursor(cursor2);
                }
            }

            this->lastXPos = mx;
            this->lastYPos = my;
        }
        return true;
    });
    drawArea.signal_button_release_event().connect([=](GdkEventButton *e) {
        draggingRectangle = false;
        if (drawingRectangle) {
            setRectangleFromMouse();
            if (this->w < 0) {
                this->xPos += w;
                w = -w;
            }
            if (this->h < 0) {
                this->yPos += h;
                h = -h;
            }
            if (std::max(abs(this->w), abs(this->h)) < 10) {
                this->w = -1;
            }
            drawArea.queue_draw();
            drawingRectangle = false;
        }
        return true;
    });

    drawArea.signal_draw().connect([this](const Cairo::RefPtr<Cairo::Context> &gc) {
        static const std::vector<double> dashPattern = {
            3};

        gc->save();
        gc->transform(transform);

        if (!pixBuf)
            return false;
        Gdk::Cairo::set_source_pixbuf(gc, pixBuf);
        gc->paint();
        gc->restore();

        if (this->xPos != -1 && this->w != -1) {
            double x = this->xPos, y = this->yPos;
            double w = this->w, h = this->h;
            transform.transform_point(x, y);
            transform.transform_distance(w, h);

            gc->set_line_width(1.0);
            gc->set_source_rgb(0, 0, 0);
            gc->set_dash(dashPattern, 0);
            gc->rectangle(x + 0.5, y + 0.5, w, h);
            gc->stroke();
            gc->set_source_rgb(1, 1, 1);
            gc->set_dash(dashPattern, 3);
            gc->rectangle(x + 0.5, y + 0.5, w, h);
            gc->stroke();
        }
        return false;
    });
}

void MandelbrotImageView::updateView(const CPUImage<unsigned> &cpuImage) {
    pixBuf = Gdk::Pixbuf::create_from_data(
        reinterpret_cast<const guint8 *>(cpuImage.getDataBuffer().data()),
        Gdk::Colorspace::COLORSPACE_RGB,
        true, 8, cpuImage.getWidth(),
        cpuImage.getHeight(),
        4 * cpuImage.getWidth());
    update_transform();
    queue_draw();
    while (Gtk::Main::events_pending()) {
        Gtk::Main::iteration(false);
    }
}

bool MandelbrotImageView::mousePositionInRectangle() {
    int mx, my;
    this->get_transformed_pointer(mx, my);
    return this->w != -1 &&
           (mx > this->xPos) && (mx <= this->xPos + this->w) &&
           (my > this->yPos) && (my <= this->yPos + this->h);
}

bool MandelbrotImageView::mousePositionOnRectangleBorder(int borderSize) {
    int mx, my;
    this->get_transformed_pointer(mx, my);

    if (this->w != -1) {
        int cx = this->xPos + this->w / 2;
        int cy = this->yPos + this->h / 2;

        mx = abs(mx - cx);
        my = abs(my - cy);

        return mx <= this->w / 2 && my <= this->h / 2 &&
               (abs(mx - this->w / 2) <= borderSize || abs(my - this->h / 2) <= borderSize);
    }
    return false;
}

void MandelbrotImageView::set_sensitive(bool s) {
    if (s) {
        drawArea.add_events(
            Gdk::EventMask::BUTTON_MOTION_MASK |
            Gdk::EventMask::BUTTON_PRESS_MASK |
            Gdk::EventMask::BUTTON_RELEASE_MASK |
            Gdk::EventMask::POINTER_MOTION_MASK);
    } else {
        drawArea.set_events(
            drawArea.get_events() & ~(
                                        Gdk::EventMask::BUTTON_MOTION_MASK |
                                        Gdk::EventMask::BUTTON_PRESS_MASK |
                                        Gdk::EventMask::BUTTON_RELEASE_MASK |
                                        Gdk::EventMask::POINTER_MOTION_MASK));
    }
}

void MandelbrotImageView::get_transformed_pointer(int &x, int &y) {
    get_pointer(x, y);
    auto t_inv = transform;
    t_inv.invert();
    double x_transformed = x, y_transformed = y;
    t_inv.transform_point(x_transformed, y_transformed);
    x = x_transformed;
    y = y_transformed;
}

void MandelbrotImageView::update_transform() {
    if (!pixBuf)
        return;

    const double x_ratio = std::min(double(get_allocated_width()) / double(pixBuf->get_width()), 1.0);
    const double y_ratio = std::min(double(get_allocated_height()) / double(pixBuf->get_height()), 1.0);
    const double scale = std::min(x_ratio, y_ratio);

    const double translate_x = 0.5 * (double(get_allocated_width()) - scale * double(pixBuf->get_width()));
    const double translate_y = 0.5 * (double(get_allocated_height()) - scale * double(pixBuf->get_height()));

    transform = Cairo::identity_matrix();
    transform.translate(int(translate_x), int(translate_y));
    transform.scale(scale, scale);
}
