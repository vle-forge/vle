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


#ifndef VLE_GVLE_SETTINGS_HPP
#define VLE_GVLE_SETTINGS_HPP

#include <vle/gvle/DllDefines.hpp>
#include <gdkmm/color.h>
#include <string>

namespace vle { namespace gvle {

/**
 * @brief vle::gvle::Settings is a singleton class to store all the variable
 * used in the GVLE graphical interface, font, font-size, colors of atomic and
 * coupled models, size of connections etc.
 *
 * @code
 * Settings::settings().setBackgroundColor(Gdk::Color("#ffff0000ffff");
 *
 * std::string font = Settings::settings().getFontEditor();
 * assert(font, "Monospace 10");
 * @endcode
 */
class GVLE_API Settings
{
public:
    /**
     * @brief Access to the only instance of the Settings class.
     *
     * @return The only instance of Settings.
     */
    static Settings& settings();

    /**
     * @brief Delete the only instance of the Settings class.
     */
    static void kill();

    /**
     * @brief Assign a default value for all the variables of this class.
     */
    void setDefault();

    /**
     * @brief Use the vle::utils::Preferences class to load the `vle.conf' file
     * and assign all variables of this class.
     */
    void load();

    /**
     * @brief use the vle::utils::Preferences class to store all variables of
     * this class into the `vle.conf' file.
     */
    void save();

    // get/set functions.

    void setAutoBuild(bool auto_build);

    bool getAutoBuild()
;
    void setHighlightSyntax(bool syntax);

    bool getHighlightSyntax();

    void setHighlightBrackets(bool brackets);

    bool getHighlightBrackets();

    void setHighlightLine(bool line);

    bool getHighlightLine();

    void setLineNumbers(bool numbers);

    bool getLineNumbers();

    void setRightMargin(bool margin);

    bool getRightMargin();

    void setAutoIndent(bool auto_indent);

    bool getAutoIndent();

    void setIndentOnTab(bool indent_tab);

    bool getIndentOnTab();

    void setIndentSize(int size);

    int getIndentSize();

    void setSmartHomeEnd(bool smart);

    bool getSmartHomeEnd();

    void setFontEditor(const std::string& font);

    const std::string& getFontEditor();

    void setForegroundColor(const Gdk::Color& color);

    const Gdk::Color& getForegroundColor() const;

    void setBackgroundColor(const Gdk::Color& color);

    const Gdk::Color& getBackgroundColor() const;

    void setSelectedColor(const Gdk::Color& color);

    const Gdk::Color& getSelectedColor() const;

    void setCoupledColor(const Gdk::Color& color);

    const Gdk::Color& getCoupledColor() const;

    void setAtomicColor(const Gdk::Color& color);

    const Gdk::Color& getAtomicColor() const;

    void setConnectionColor(const Gdk::Color& color);

    const Gdk::Color& getConnectionColor() const;

    void setFont(const std::string& font);

    const std::string& getFont() const;

    void setFontSize(const double size);

    double getFontSize() const;

    void setLineWidth(double width);

    double getLineWidth() const;

private:
    Settings();
    ~Settings();
    Settings(const Settings& other);
    Settings& operator=(const Settings& other);

    class Pimpl;
    Pimpl* mPimpl;
};

}} // namespace vle gvle

#endif
