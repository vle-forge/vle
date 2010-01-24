/**
 * @file vle/gvle/Settings.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_SETTINGS_HPP
#define VLE_GVLE_SETTINGS_HPP

#include <vle/gvle/DllDefines.hpp>
#include <vle/utils/Preferences.hpp>
#include <gdkmm/color.h>

namespace vle { namespace gvle {

class VLE_GVLE_EXPORT EditorSettings
{
public:
    EditorSettings(const std::string& font)
        : mFontEditor(font)
    {}

    void setDefault();
    void load(vle::utils::Preferences& prefs);
    virtual void operator=(const EditorSettings& src);
    void save(vle::utils::Preferences& prefs);

    void setHighlightSyntax(bool syntax)
    { mHighlightSyntax = syntax; }

    bool getHighlightSyntax()
    { return mHighlightSyntax; }

    void setHighlightBrackets(bool brackets)
    { mHighlightBrackets = brackets; }

    bool getHighlightBrackets()
    { return mHighlightBrackets; }

    void setHighlightLine(bool line)
    { mHighlightLine = line; }

    bool getHighlightLine()
    { return mHighlightLine; }

    void setLineNumbers(bool numbers)
    { mLineNumbers = numbers; }

     bool getLineNumbers()
    { return mLineNumbers; }

    void setRightMargin(bool margin)
    { mRightMargin = margin; }

    bool getRightMargin()
    { return mRightMargin; }

    void setAutoIndent(bool auto_indent)
    { mAutoIndent = auto_indent; }

    bool getAutoIndent()
    { return mAutoIndent; }

    void setIndentOnTab(bool indent_tab)
    { mIndentOnTab = indent_tab; }

    bool getIndentOnTab()
    { return mIndentOnTab; }

    void setIndentSize(int size)
    { mIndentSize = size; }

    int getIndentSize()
    { return mIndentSize; }

    void setSmartHomeEnd(bool smart)
    { mSmartHomeEnd = smart; }

    bool getSmartHomeEnd()
    { return mSmartHomeEnd; }

    void setFontEditor(const std::string& font)
    { mFontEditor = font; }

    const std::string& getFontEditor()
    { return mFontEditor; }

private:
    bool mHighlightSyntax;
    bool mHighlightBrackets;
    bool mHighlightLine;
    bool mLineNumbers;
    bool mRightMargin;
    bool mAutoIndent;
    bool mIndentOnTab;
    int  mIndentSize;
    bool mSmartHomeEnd;
    std::string mFontEditor;
};

class VLE_GVLE_EXPORT GraphicsSettings
{
public:
    GraphicsSettings(const std::string& font)
        : mFont(font)
    {}

    void setDefault();
    void load(vle::utils::Preferences& prefs);
    virtual void operator=(const GraphicsSettings& src);
    void save(vle::utils::Preferences& prefs);

    void setForegroundColor(const Gdk::Color& color)
    { mForegroundColor = color; }

    const Gdk::Color& getForegroundColor() const
    { return mForegroundColor; }

    void setBackgroundColor(const Gdk::Color& color)
    { mBackgroundColor = color; }

    const Gdk::Color& getBackgroundColor() const
    { return mBackgroundColor; }

    void setSelectedColor(const Gdk::Color& color)
    { mSelectedColor = color; }

    const Gdk::Color& getSelectedColor() const
    { return mSelectedColor; }

    void setCoupledColor(const Gdk::Color& color)
    { mCoupledColor = color; }

    const Gdk::Color& getCoupledColor() const
    { return mCoupledColor; }

    void setAtomicColor(const Gdk::Color& color)
    { mAtomicColor = color; }

    const Gdk::Color& getAtomicColor() const
    { return mAtomicColor; }

    void setConnectionColor(const Gdk::Color& color)
    { mConnectionColor = color; }

    const Gdk::Color& getConnectionColor() const
    { return mConnectionColor; }

    void setFont(const std::string& font)
    { mFont = font; }

    const std::string& getFont() const
    { return mFont; }

    void setFontSize(const double size)
    { mFontSize = size; }

    double getFontSize() const
    { return mFontSize; }

    void setLineWidth(double width)
    { mLineWidth = width; }

    double getLineWidth() const
    { return mLineWidth; }

private:
    Gdk::Color makeColorFromString(const std::string& value);
    std::string makeStringFromColor(const Gdk::Color& color);

    Gdk::Color mBackgroundColor;
    Gdk::Color mForegroundColor;
    Gdk::Color mAtomicColor;
    Gdk::Color mCoupledColor;
    Gdk::Color mSelectedColor;
    Gdk::Color mConnectionColor;
    std::string mFont;
    double mFontSize;
    double mLineWidth;
};

class VLE_GVLE_EXPORT Settings : public EditorSettings, public GraphicsSettings
{
public:
    Settings()
        : EditorSettings(""), GraphicsSettings("")
    {}

    Settings(const std::string& editorFont,
             const std::string& graphicsFont)
        : EditorSettings(editorFont), GraphicsSettings(graphicsFont)
    {}

    void setDefault();

    void load();

    virtual void operator=(const Settings& src);

    void save();

    static Settings& settings()
    { if (mSettings == 0) mSettings = new Settings; return *mSettings; }

    Settings& settings(const std::string& editorFont,
                       const std::string& graphicsFont)
    {
        if (mSettings == 0) {
            mSettings = new Settings(editorFont, graphicsFont);
        }
        return *mSettings;
    }

    void kill()
    { if (mSettings) delete mSettings; }

private:
    static Settings* mSettings;
};

}} // namespace vle gvle

#endif
