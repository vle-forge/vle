/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_SIMPLEVIEWDRAWINGAREA_HPP
#define VLE_GVLE_SIMPLEVIEWDRAWINGAREA_HPP

#include <vle/gvle/ViewDrawingArea.hpp>

namespace vle { namespace vpz {

    class BaseModel;
    class CoupledModel;

}} // namespace vle graph

namespace vle { namespace gvle {

    /**
     * @brief A Gtk::DrawingArea to draw the gui::GModel onto the screen. This
     * objet is attached to a gui::View that manage menu and global Gtk::Window
     * around this Gtk::DrawingArea.
     */
    class SimpleViewDrawingArea : public ViewDrawingArea
    {
    public:
        typedef struct connexion
        {
            vpz::BaseModel* source;
            vpz::BaseModel* destination;
        } Connexion;
	static const gint MODEL_RADIUS;
        SimpleViewDrawingArea(View* view);

        virtual ~SimpleViewDrawingArea() {}

	/**
	 * @brief draw the current view
	 */
	void draw();

	/**
	 * @brief get the position of the input connexion in the current
	 * model
	 * @param p the port
	 * @param x the x coordinate
	 * @param y the y coordinate
	 */
	void getCurrentModelInPosition(const std::string& p, int& x, int& y);

	/**
	 * @brief get the position of the output connexion in the current
	 * model
	 * @param p the port
	 * @param x the x coordinate
	 * @param y the y coordinate
	 */
        void getCurrentModelOutPosition(const std::string& p, int& x, int& y);

	/**
	 * @brief get the position of the input connexion in a model
	 * @param xs the x coordinate of source model
	 * @param ys the y coordinate of source model
	 * @param dst the destination model
	 * @param center the coordinates of the circle center
	 * @param x the x coordinate
	 * @param y the y coordinate
	 */
        void getModelInPosition(int xs, int ys,
				vpz::BaseModel* dst,
				vpz::BaseModel* center,
				int&x, int& y);

	/**
	 * @brief get the position of the output connexion in a model
	 * @param src the source model
	 * @param xd the x coordinate of destination model
	 * @param yd the y coordinate of destination model
	 * @param center the coordinates of the circle center
	 * @param x the x coordinate
	 * @param y the y coordinate
	 */
	void getModelOutPosition(vpz::BaseModel* src,
				 int xd, int yd,
				 vpz::BaseModel* center,
				 int&x, int& y);

	/**
	 * Order the models
	 */
	virtual void onOrder();

	void preComputeConnectInfo();

	std::string getConnectionInfo(int mHighlightLine);

    private:

	void preComputeConnection(int xs, int ys, int xd, int yd,
	  int ytms, int ybms);
        virtual void preComputeConnection(vpz::BaseModel* src,
                                  const std::string& srcport,
                                  vpz::BaseModel* dst,
                                  const std::string& dstport);
	virtual void computeConnection(vpz::BaseModel* src,
				       const std::string& portsrc,
				       vpz::BaseModel* dst,
				       const std::string& portdst, int index);
	virtual StraightLine computeConnection(int xs, int ys,
					       int xd, int yd,
					       int index);

	/**
	 * @brief return the positive solution  in an intersection
	 * @param xs the x coordinate of the source model
	 * @param ys the y coordinate of the source model
	 * @param xd the x coordinate of the destination model
	 * @param yd the y coordinate of the destination model
	 * @param xc the x coordinate of the center
	 * @param yc the y coordinate of the center
	 */
	float getPositiveDelta(int xs, int ys,
			       int xd, int yd,
			       int xc, int yc);

	/**
	 * @brief return the negative solution  in an intersection
	 * @param xs the x coordinate of the source model
	 * @param ys the y coordinate of the source model
	 * @param xd the x coordinate of the destination model
	 * @param yd the y coordinate of the destination model
	 * @param xc the x coordinate of the center
	 * @param yc the y coordinate of the center
	 */
	float getNegativeDelta(int xs, int ys,
			       int xd, int yd,
			       int xc, int yc);

	/**
	 * @brief get the director coefficient of a line
	 * @param x1 the x coordinate of the first point
	 * @param y1 the y coordinate of the first point
	 * @param x2 the x coordinate of the second point
	 * @param x2 the x coordinate of the second point
	 */
	inline float directorCoefficient(float x1, float y1,
					 float x2, float y2)
	{ return (y2- y1) / (x2 - x1); }


	/**
	 * @brief draw the children model in the current model
	 * @param model the model to draw
	 * @param color the color to draw
	 */
        void drawChildrenModel(vpz::BaseModel* model,const Gdk::Color& color);

	bool on_button_press_event(GdkEventButton* event);
	bool on_button_release_event(GdkEventButton* event);

	connexion record;
	std::vector < connexion > mConnectionInfo;
    };


}} // namespace vle gvle

#endif
