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


#include <vle/gvle/Settings.hpp>
#include <vle/utils/Preferences.hpp>
#include <boost/cast.hpp>

namespace vle { namespace gvle {

class Settings::Pimpl
{
public:
    Pimpl()
    {
        setDefault();
        load();
    }

    ~Pimpl()
    {
    }

    void setDefault()
    {
        mFontEditor = "Monospace 8";
        mFont = "Sans 10";

        mAutoBuild = true;
        mHighlightSyntax = true;
        mHighlightBrackets = true;
        mHighlightLine = true;
        mLineNumbers = true;
        mRightMargin = true;
        mAutoIndent = true;
        mIndentOnTab = true;
        mIndentSize = 4;
        mSmartHomeEnd = true;

        mBackgroundColor.set("#ffffffffffff");
        mForegroundColor.set("#000000000000");
        mAtomicColor.set("#0000ffff0000");
        mCoupledColor.set("#00000000ffff");
        mSelectedColor.set("#ffff00000000");
        mConnectionColor.set("#000000000000");

        mFontSize = 10.0;
        mLineWidth = 2.0;
    }

    void load()
    {
        utils::Preferences prefs;
        std::string s;
        double d = 0.0;
        uint32_t i = 0;

        prefs.get("gvle.packages.auto-build", &mAutoBuild);
        prefs.get("gvle.editor.highlight-syntax", &mHighlightSyntax);
        prefs.get("gvle.editor.highlight-brackets", &mHighlightBrackets);
        prefs.get("gvle.editor.highlight-line", &mHighlightLine);
        prefs.get("gvle.editor.show-line-numbers", &mLineNumbers);
        prefs.get("gvle.editor.show-right-margin", &mRightMargin);
        prefs.get("gvle.editor.auto-indent", &mAutoIndent);
        prefs.get("gvle.editor.indent-on-tab", &mIndentOnTab);
        prefs.get("gvle.editor.smart-home-end", &mSmartHomeEnd);

        if (prefs.get("gvle.editor.indent-size", &i)) {
            mIndentSize = boost::numeric_cast < int >(i);
            i = 0;
        }

        if (prefs.get("gvle.editor.font", &s) and not s.empty()) {
            mFontEditor = s;
            s.clear();
        }

        if (prefs.get("gvle.graphics.background-color", &s) and not s.empty()) {
            mBackgroundColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.foreground-color", &s) and not s.empty()) {
            mForegroundColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.atomic-color", &s) and not s.empty()) {
            mAtomicColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.coupled-color", &s) and not s.empty()) {
            mCoupledColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.selected-color", &s) and not s.empty()) {
            mSelectedColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.connection-color", &s) and not s.empty()) {
            mConnectionColor = convert(s);
            s.clear();
        }

        if (prefs.get("gvle.graphics.font", &s) and not s.empty()) {
            mFont = s;
            s.clear();
        }

        if (prefs.get("gvle.graphics.font-size", &d) and d > 0.0) {
            mFontSize = d;
            d = 0.0;
        }

        if (prefs.get("gvle.graphics.line-width", &d) and d > 0.0) {
            mLineWidth = d;
            d = 0.0;
        }
    }

    void save()
    {
        utils::Preferences prefs;

        prefs.set("gvle.packages.auto-build", mAutoBuild);
        prefs.set("gvle.editor.highlight-syntax", mHighlightSyntax);
        prefs.set("gvle.editor.highlight-brackets", mHighlightBrackets);
        prefs.set("gvle.editor.highlight-line", mHighlightLine);
        prefs.set("gvle.editor.show-line-numbers", mLineNumbers);
        prefs.set("gvle.editor.show-right-margin", mRightMargin);
        prefs.set("gvle.editor.auto-indent", mAutoIndent);
        prefs.set("gvle.editor.indent-on-tab", mIndentOnTab);
        prefs.set("gvle.editor.indent-size",
                  boost::numeric_cast < uint32_t >(mIndentSize));
        prefs.set("gvle.editor.smart-home-end", mSmartHomeEnd);
        prefs.set("gvle.editor.font", mFontEditor);

        prefs.set("gvle.graphics.background-color", convert(mBackgroundColor));
        prefs.set("gvle.graphics.foreground-color", convert(mForegroundColor));
        prefs.set("gvle.graphics.atomic-color", convert(mAtomicColor));
        prefs.set("gvle.graphics.coupled-color", convert(mCoupledColor));
        prefs.set("gvle.graphics.selected-color", convert(mSelectedColor));
        prefs.set("gvle.graphics.connection-color", convert(mConnectionColor));

        prefs.set("gvle.graphics.font", mFont);
        prefs.set("gvle.graphics.font-size", mFontSize);
        prefs.set("gvle.graphics.line-width", mLineWidth);
    }

    bool mAutoBuild;
    bool mHighlightSyntax;
    bool mHighlightBrackets;
    bool mHighlightLine;
    bool mLineNumbers;
    bool mRightMargin;
    bool mAutoIndent;
    bool mIndentOnTab;
    bool mSmartHomeEnd;
    int  mIndentSize;
    std::string mFontEditor;

    Gdk::Color mBackgroundColor;
    Gdk::Color mForegroundColor;
    Gdk::Color mAtomicColor;
    Gdk::Color mCoupledColor;
    Gdk::Color mSelectedColor;
    Gdk::Color mConnectionColor;
    std::string mFont;
    double mFontSize;
    double mLineWidth;

    static std::string convert(const Gdk::Color& color)
    {
        std::string str = color.to_string();
        if (not str.empty()) {
            str.assign(str, 1, std::string::npos);
        }

        return str;
    }

    static Gdk::Color convert(const std::string& str)
    {
        std::string tmp = '#' + str;
        Gdk::Color color(tmp);

        return color;
    }

    static Settings* mSettings;
};

Settings *Settings::Pimpl::mSettings = 0;

void Settings::load()
{
    mPimpl->load();
}

void Settings::save()
{
    mPimpl->save();
}

void Settings::setDefault()
{
    mPimpl->setDefault();
}

Settings& Settings::settings()
{
    if (Settings::Pimpl::mSettings == 0) {
        Settings::Pimpl::mSettings = new Settings();
    }

    return *Settings::Pimpl::mSettings;
}

void Settings::kill()
{
    if (Settings::Pimpl::mSettings == 0) {
        Settings::Pimpl::mSettings->save();
        delete Settings::Pimpl::mSettings;
        Settings::Pimpl::mSettings = 0;
    }
}

Settings::Settings()
    : mPimpl(new Settings::Pimpl())
{
}

Settings::~Settings()
{
    delete mPimpl;
}

void Settings::setHighlightSyntax(bool syntax)
{
    settings().mPimpl->mHighlightSyntax = syntax;
}

bool Settings:: getHighlightSyntax()
{
    return settings().mPimpl->mHighlightSyntax;
}

void Settings::setHighlightBrackets(bool brackets)
{
    settings().mPimpl->mHighlightBrackets = brackets;
}

bool Settings:: getHighlightBrackets()
{
    return settings().mPimpl->mHighlightBrackets;
}

void Settings::setHighlightLine(bool line)
{
    settings().mPimpl->mHighlightLine = line;
}

bool Settings:: getHighlightLine()
{
    return settings().mPimpl->mHighlightLine;
}

void Settings::setLineNumbers(bool numbers)
{
    settings().mPimpl->mLineNumbers = numbers;
}

bool Settings:: getLineNumbers()
{
    return settings().mPimpl->mLineNumbers;
}

void Settings::setRightMargin(bool margin)
{
    settings().mPimpl->mRightMargin = margin;
}

bool Settings:: getRightMargin()
{
    return settings().mPimpl->mRightMargin;
}

void Settings::setAutoBuild(bool auto_build)
{
    settings().mPimpl->mAutoBuild = auto_build;
}

bool Settings:: getAutoBuild()
{
    return settings().mPimpl->mAutoBuild;
}

void Settings::setAutoIndent(bool auto_indent)
{
    settings().mPimpl->mAutoIndent = auto_indent;
}

bool Settings:: getAutoIndent()
{
    return settings().mPimpl->mAutoIndent;
}

void Settings::setIndentOnTab(bool indent_tab)
{
    settings().mPimpl->mIndentOnTab = indent_tab;
}

bool Settings:: getIndentOnTab()
{
    return settings().mPimpl->mIndentOnTab;
}

void Settings::setIndentSize(int size)
{
    settings().mPimpl->mIndentSize = size;
}

int Settings::getIndentSize()
{
    return settings().mPimpl->mIndentSize;
}

void Settings::setSmartHomeEnd(bool smart)
{
    settings().mPimpl->mSmartHomeEnd = smart;
}

bool Settings:: getSmartHomeEnd()
{
    return settings().mPimpl->mSmartHomeEnd;
}

void Settings::setFontEditor(const std::string& font)
{
    settings().mPimpl->mFontEditor = font;
}

const std::string& Settings::getFontEditor()
{
    return settings().mPimpl->mFontEditor;
}

void Settings::setForegroundColor(const Gdk::Color& color)
{
    settings().mPimpl->mForegroundColor = color;
}

const Gdk::Color& Settings::getForegroundColor() const
{
    return settings().mPimpl->mForegroundColor;
}

void Settings::setBackgroundColor(const Gdk::Color& color)
{
    settings().mPimpl->mBackgroundColor = color;
}

const Gdk::Color& Settings::getBackgroundColor() const
{
    return settings().mPimpl->mBackgroundColor;
}

void Settings::setSelectedColor(const Gdk::Color& color)
{
    settings().mPimpl->mSelectedColor = color;
}

const Gdk::Color& Settings::getSelectedColor() const
{
    return settings().mPimpl->mSelectedColor;
}

void Settings::setCoupledColor(const Gdk::Color& color)
{
    settings().mPimpl->mCoupledColor = color;
}

const Gdk::Color& Settings::getCoupledColor() const
{
    return settings().mPimpl->mCoupledColor;
}

void Settings::setAtomicColor(const Gdk::Color& color)
{
    settings().mPimpl->mAtomicColor = color;
}

const Gdk::Color& Settings::getAtomicColor() const
{
    return settings().mPimpl->mAtomicColor;
}

void Settings::setConnectionColor(const Gdk::Color& color)
{
    settings().mPimpl->mConnectionColor = color;
}

const Gdk::Color& Settings::getConnectionColor() const
{
    return settings().mPimpl->mConnectionColor;
}

void Settings::setFont(const std::string& font)
{
    settings().mPimpl->mFont = font;
}

const std::string& Settings::getFont() const
{
    return settings().mPimpl->mFont;
}

void Settings::setFontSize(const double size)
{
    settings().mPimpl->mFontSize = size;
}

double Settings::getFontSize() const
{
    return settings().mPimpl->mFontSize;
}

void Settings::setLineWidth(double width)
{
    settings().mPimpl->mLineWidth = width;
}

double Settings::getLineWidth() const
{
    return settings().mPimpl->mLineWidth;
}

}} // namespace vle gvle
