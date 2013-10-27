/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_GVLE_VIEWDRAWINGAREA_HPP
#define VLE_GVLE_VIEWDRAWINGAREA_HPP

#include <gtkmm/drawingarea.h>
#include <gdkmm/color.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/adjustment.h>
#include <map>

namespace vle { namespace vpz {

    class BaseModel;
    class CoupledModel;

}} // namespace vle graph

namespace vle { namespace gvle {

    class GVLE;
    class View;
    class Modeling;

    class Adjustment
    {
    public:
        void set(Glib::RefPtr<Gtk::Adjustment> adjust)
        {
             mValue = adjust->get_value();
             mLower = adjust->get_lower();
             mUpper = adjust->get_upper();
             mStep_increment = adjust->get_step_increment();
             mPage_increment = adjust->get_page_increment();
             mPage_size = adjust->get_page_size();
        }

        void reset(Glib::RefPtr<Gtk::Adjustment> adjust)
        {
            adjust->configure(mValue,
                              mLower,
                              mUpper,
                              mStep_increment,
                              mPage_increment,
                              mPage_size);
        }
    private:
        double mValue;
        double mLower;
        double mUpper;
        double mStep_increment;
        double mPage_increment;
        double mPage_size;
    };

    class Color
    {
    public:
        double m_r; double m_g; double m_b;
        Color(double r = 0, double g = 0, double b = 0) :
            m_r(r), m_g(g), m_b(b)
    {}
    };

    /**
     * @brief A Gtk::DrawingArea to draw the gui::GModel onto the screen. This
     * objet is attached to a gui::View that manage menu and global Gtk::Window
     * around this Gtk::DrawingArea.
     */
    class ViewDrawingArea : public Gtk::DrawingArea
    {
    public:

        static const guint MODEL_WIDTH;
        static const guint MODEL_DECAL;
        static const guint MODEL_PORT;
        static const guint MODEL_PORT_SPACING_LABEL;
        static const guint MODEL_SPACING_PORT;
        static const guint PORT_SPACING_LABEL;
        static const guint MODEL_HEIGHT;
        static const guint CURRENT_MODEL_PORT;
        static const guint CURRENT_MODEL_SPACE;
        static const double ZOOM_FACTOR_SUP;
        static const double ZOOM_FACTOR_INF;
        static const double ZOOM_MIN;
        static const double ZOOM_MAX;
        static const guint SPACING_MODEL;
        static const guint SPACING_LINE;
        static const guint SPACING_MODEL_PORT;

        ViewDrawingArea(View* view);

        virtual ~ViewDrawingArea()
        { m_cntSignalQueryTooltip.disconnect(); }

        virtual void draw()=0;

        vpz::CoupledModel* getModel()
        { return mCurrent; }

        //
        // MANAGE ZOOM / UNZOOM IN VIEW
        //

        /**
         * when zoom click, if left button then zoom, if right button then
         * unzoom, otherwise default size
         *
         * @param button 1 for left, 2 for right, otherwise other
         */
        void onZoom(int button);

        /**
         * Get current zoom value
         *
         * @return zoom value
         */
        inline double getZoom() const { return mZoom; }

        /**
         * Get current zoom value
         *
         * @return zoom value
         */
        inline void setZoom(double zoom)
        { mZoom = zoom; }

        /** Add a constant to zoom limit by 4.0 */
        void addCoefZoom();

        /** Del a constant to zoom limit by 0.1 */
        void delCoefZoom();

        /** restore zoom to default value 1.0 */
        void restoreZoom();

        /*
         * set a new coefficient to the zoom
         * @parm coef the new coefficient
         *
         */
        void setCoefZoom(double coef);


        /**
         * Calculate a new Zoom factor using 2 points and apply to the
         * DrawingArea.
         *
         * @param xmin left position
         * @param ymin top position
         * @param xmax right position
         * @param ymax bottom position
         */
        void selectZoom(int xmin, int ymin, int xmax, int ymax);

        /** Calculate the new size of DrawingArea. */
        void newSize();

        void calcRectSize();

        /**
         * @brief search the size of the widest and highest model
         *
         */
        void maxModelWidthHeight();

        /**
         * Export the view in image
         */
        void exportPng(const std::string& filename);
        void exportPdf(const std::string& filename);
        void exportSvg(const std::string& filename);

        /**
         * Order the models
         */
        virtual void onOrder()=0;

        /**
         * @brief Add to the Gdk stack a call to expose event function with the
         * need to redraw the buffer with the draw() function.
         */
        void queueRedraw();

        bool onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                            const Glib::RefPtr<Gtk::Tooltip>& tooltip);


        void set(Glib::RefPtr<Gtk::Adjustment> vadjust, Glib::RefPtr<Gtk::Adjustment> hadjust)
        {
            mVAdjustment.set(vadjust);
            mHAdjustment.set(hadjust);
        }

        void reset(Glib::RefPtr<Gtk::Adjustment> vadjust, Glib::RefPtr<Gtk::Adjustment> hadjust)
        {
            mVAdjustment.reset(vadjust);
            mHAdjustment.reset(hadjust);
        }

    protected:

        typedef std::pair < int, int > Point;
        typedef std::vector < Point > StraightLine;

        class Connection
        {
        public:
            int xs, ys, xd, yd, x1, x2, y1, y3, y4;
            bool top;

            Connection(int xs, int ys, int xd, int yd,
                       int x1, int x2, int y1, int y3, int y4) :
                xs(xs), ys(ys), xd(xd), yd(yd), x1(x1), x2(x2),
                y1(y1), y3(y3), y4(y4), top(false)
            {}
        };

        void drawCurrentCoupledModel();
        void drawCurrentModelPorts();

        virtual void preComputeConnection(vpz::BaseModel* src,
                                  const std::string& srcport,
                                  vpz::BaseModel* dst,
                                  const std::string& dstport)=0;
        void preComputeConnection();

        virtual void preComputeConnectInfo()=0;

        virtual std::string getConnectionInfo(int mHighlightLine)=0;

        virtual StraightLine computeConnection(int xs, int ys, int xd, int yd,
                                       int index)=0;
        virtual void computeConnection(vpz::BaseModel* src, const std::string& srcport,
                               vpz::BaseModel* dst, const std::string& dstport,
                               int index)=0;
        void computeConnection();

        void highlightLine(int mx, int my);

        void drawConnection();
        void drawHighlightConnection();
        void drawChildrenModels();
        virtual void drawChildrenModel(vpz::BaseModel* model,
                                       const Gdk::Color& color)=0;
        void drawLines();
        void drawLink();
        void drawZoomFrame();

        void setUndefinedModels();

        bool on_configure_event(GdkEventConfigure* event);
//        bool on_expose_event(GdkEventExpose* event);   No in GTK3
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& context);
	
        bool on_motion_notify_event(GdkEventMotion* event);
        void on_realize();

        void on_gvlepointer_button_1(vpz::BaseModel* mdl, bool state);
        void delUnderMouse(int x, int y);

        /**
         * add a link between two model prec Model and model under mouse x, y
         *
         * @param x mouse position
         * @param y mouse position
         */
        void addLinkOnButtonPress(int x, int y);
        void addLinkOnMotion(int x, int y);
        void addLinkOnButtonRelease(int x, int y);

        /**
         * Change the color of the drawing
         */
        void setColor(const Gdk::Color& color);

        /**
         * Return nearest connection between all connection and mouse position
         */
        void delConnection();

        View*                           mView;
        vpz::CoupledModel*              mCurrent;
        Modeling*                       mModeling;
        GVLE*                           mGVLE;

        Gdk::Point                      mMouse;
        Gdk::Point                      mPrecMouse;

        int                             mHeight;
        int                             mWidth;
        int                             mRectHeight;
        int                             mRectWidth;
        double                          mZoom;
        double                          mOffset;
        Cairo::RefPtr<Cairo::Context>   mContext;
        Glib::RefPtr < Gdk::Window >    mWin;

        std::map < int, Point > mInPts;
        std::map < int, Point > mOutPts;
        std::vector < Connection > mConnections;
        std::map < Point, bool > mDirect;
        std::vector < StraightLine > mLines;
        std::vector < vpz::BaseModel* > mModelInfo;
        int mHighlightLine;

        // Grid
        std::list < std::string > mGrid;
        int                       mCasesWidth;
        int                       mCasesHeight;

        int                       mMaxWidth;
        int                       mMaxHeight;

        sigc::connection m_cntSignalQueryTooltip;

        // Adjusment
        Adjustment mVAdjustment;
        Adjustment mHAdjustment;
    };

}} // namespace vle gvle

#endif
