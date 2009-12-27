/**
 * @file vle/gvle/Settings.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/Settings.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

Settings* Settings::mSettings = 0;

void EditorSettings::setDefault()
{
    mHighlightSyntax = true;
    mHighlightBrackets = true;
    mHighlightLine = true;
    mLineNumbers = true;
    mRightMargin = true;
    mAutoIndent = true;
    mIndentOnTab = true;
    mIndentSize = 4;
    mSmartHomeEnd = true;
}

void EditorSettings::load(vle::utils::Preferences& prefs)
{
    std::string value;

    setDefault();

    value = prefs.getAttributes("gvle.editor", "highlightSyntax");
    if (not value.empty())
        mHighlightSyntax = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "highlightBrackets");
    if (not value.empty())
        mHighlightBrackets = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "highlightLine");
    if (not value.empty())
        mHighlightLine = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "lineNumbers");
    if (not value.empty())
        mLineNumbers = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "rightMargin");
    if (not value.empty())
        mRightMargin = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "autoIndent");
    if (not value.empty())
        mAutoIndent = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "indentOnTab");
    if (not value.empty())
        mIndentOnTab = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "indentSize");
    if (not value.empty())
        mIndentSize = boost::lexical_cast<int>(value);

    value = prefs.getAttributes("gvle.editor", "smartHomeEnd");
    if (not value.empty())
        mSmartHomeEnd = boost::lexical_cast<bool>(value);

    value = prefs.getAttributes("gvle.editor", "fontEditor");
    if (not value.empty()) mFontEditor = value;
}

void EditorSettings::operator=(const EditorSettings& src)
{
    mHighlightSyntax   = src.mHighlightSyntax;
    mHighlightBrackets = src.mHighlightBrackets;
    mHighlightLine     = src.mHighlightLine;
    mLineNumbers       = src.mLineNumbers;
    mRightMargin       = src.mRightMargin;
    mAutoIndent        = src.mAutoIndent;
    mIndentOnTab       = src.mIndentOnTab;
    mIndentSize        = src.mIndentSize;
    mSmartHomeEnd      = src.mSmartHomeEnd;
    mFontEditor        = src.mFontEditor;
}

void EditorSettings::save(vle::utils::Preferences& prefs)
{
    prefs.setAttributes("gvle.editor",
			"highlightSyntax",
			mHighlightSyntax ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"highlightBrackets",
			mHighlightBrackets ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"highlightLine",
			mHighlightLine ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"lineNumbers",
			mLineNumbers ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"rightMargin",
			mRightMargin ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"autoIndent",
			mAutoIndent ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"indentOnTab",
			mIndentOnTab ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"indentSize",
			boost::lexical_cast<std::string>(mIndentSize));
    prefs.setAttributes("gvle.editor",
			"smartHomeEnd",
			mSmartHomeEnd ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"fontEditor",
			mFontEditor);
}

void GraphicsSettings::setDefault()
{
    mBackgroundColor = makeColorFromString("1 1 1");
    mForegroundColor = makeColorFromString("0 0 0");
    mAtomicColor = makeColorFromString("0.666667 0.835294 0.698039");
    mCoupledColor = makeColorFromString("0.666667 0.698024 0.835279");
    mSelectedColor = makeColorFromString("0.835279 0.666667 0.698024");
    mConnectionColor = makeColorFromString("0 0 0");
    mFontSize = 10.0;
    mLineWidth = 2.0;
}

void GraphicsSettings::load(vle::utils::Preferences& prefs)
{
    std::string value;

    setDefault();

    value = prefs.getAttributes("gvle.graphics", "background");
    if (not value.empty())
        mBackgroundColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "foreground");
    if (not value.empty())
        mForegroundColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "atomic");
    if (not value.empty())
        mAtomicColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "coupled");
    if (not value.empty())
        mCoupledColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "selected");
    if (not value.empty())
        mSelectedColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "connection");
    if (not value.empty())
        mConnectionColor = makeColorFromString(value);

    value = prefs.getAttributes("gvle.graphics", "font");
    if (not value.empty()) {
        mFont = value;
    }

    value = prefs.getAttributes("gvle.graphics", "fontSize");
    if (not value.empty())
        mFontSize = boost::lexical_cast<double>(value);

    value = prefs.getAttributes("gvle.graphics", "lineWidth");
    if (not value.empty())
        mLineWidth = boost::lexical_cast<double>(value);
}

Gdk::Color GraphicsSettings::makeColorFromString(const std::string& value)
{
    std::vector< std::string > splitVec;
    boost::split(splitVec, value, boost::is_any_of(" "));
    double r = boost::lexical_cast<double>(splitVec[0]);
    double g = boost::lexical_cast<double>(splitVec[1]);
    double b = boost::lexical_cast<double>(splitVec[2]);
    Gdk::Color color;
    color.set_rgb_p(r, g, b);
    return color;
}

std::string GraphicsSettings::makeStringFromColor(const Gdk::Color& color)
{
    std::ostringstream oss;
    oss << color.get_red_p() << " "
	<< color.get_green_p() << " "
	<< color.get_blue_p();
    return oss.str();
}

void GraphicsSettings::operator=(const GraphicsSettings& src)
{
    mForegroundColor = src.mForegroundColor;
    mBackgroundColor = src.mBackgroundColor;
    mSelectedColor   = src.mSelectedColor;
    mCoupledColor    = src.mCoupledColor;
    mAtomicColor     = src.mAtomicColor;
    mConnectionColor = src.mConnectionColor;
    mFont            = src.mFont;
    mFontSize        = src.mFontSize;
    mLineWidth       = src.mLineWidth;
}

void GraphicsSettings::save(vle::utils::Preferences& prefs)
{
    prefs.setAttributes("gvle.graphics",
			"foreground",
			makeStringFromColor(mForegroundColor));
    prefs.setAttributes("gvle.graphics",
			"background",
			makeStringFromColor(mBackgroundColor));
    prefs.setAttributes("gvle.graphics",
			"atomic",
			makeStringFromColor(mAtomicColor));
    prefs.setAttributes("gvle.graphics",
			"coupled",
			makeStringFromColor(mCoupledColor));
    prefs.setAttributes("gvle.graphics",
			"selected",
			makeStringFromColor(mSelectedColor));
    prefs.setAttributes("gvle.graphics",
			"connection",
			makeStringFromColor(mConnectionColor));
    prefs.setAttributes("gvle.graphics",
			"font",
			mFont);
    std::ostringstream oss;
    oss << mFontSize;
    prefs.setAttributes("gvle.graphics", "fontSize", oss.str());

    oss.str("");
    oss << mLineWidth;
    prefs.setAttributes("gvle.graphics", "lineWidth", oss.str());
}

void Settings::load()
{
    vle::utils::Preferences prefs;
    prefs.load();

    EditorSettings::load(prefs);
    GraphicsSettings::load(prefs);
}

void Settings::operator=(const Settings& src)
{
    EditorSettings::operator=(src);
    GraphicsSettings::operator=(src);
}

void Settings::save()
{
    vle::utils::Preferences prefs;
    prefs.load();

    EditorSettings::save(prefs);
    GraphicsSettings::save(prefs);
    prefs.save();
}

void Settings::setDefault()
{
    EditorSettings::setDefault();
    GraphicsSettings::setDefault();
}

}} // namespace vle gvle
