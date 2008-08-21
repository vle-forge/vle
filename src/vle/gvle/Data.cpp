/**
 * @file vle/gvle/Data.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/gvle/Data.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <gtkmm/stock.h>
#include <iostream>

using std::string;
using namespace vle;
using namespace utils;

namespace vle
{
namespace gvle {

TypeDialog::TypeDialog(const std::string & type, const std::string & value) :
        Gtk::Dialog("Define a data type", true, true),
        mFrameType("Type"),
        mHBoxType(false, 5),
        mLabelValueType("Value:")
{
    add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
    Gtk::Button* ok = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    mComboType.append_string("String");
    mComboType.append_string("Double");
    mComboType.append_string("Integer");
    mComboType.append_string("Boolean");
    mHBoxType.add(mComboType);
    mHBoxType.add(mLabelValueType);
    mHBoxType.add(mEntryValueType);
    mFrameType.add(mHBoxType);
    get_vbox()->pack_start(mFrameType, false, true, 0);

    ok->set_flags(Gtk::CAN_DEFAULT);
    ok->grab_default();

    mComboType.select_string(type);
    mEntryValueType.set_text(value);

    show_all();
}

string TypeDialog::getType() const
{
    return mComboType.get_active_string();
}

string TypeDialog::getValue() const
{
    string type(getType());
    string value(mEntryValueType.get_text());

    Data::validDataTypeValue(type, value);

    return value;
}

Data::Data(const std::string& xml) :
        Gtk::Dialog("Define a data type", true, true),
        mFrameType("Type"),
        mHBoxType(false, 5),
        mLabelValueType("Value:"),
        mFrameList("List"),
        mButtonAddList("Add"),
        mButtonEditList("Edit"),
        mButtonDelList("Delete"),
        mFrameRandom("Random"),
        mLabelInstance("Instance:"),
        mLabelSeed("Seed:"),
        mLabelAverage("Average:"),
        mLabelStandardDeviation("Deviation"),
        mLabelMin("Min:"),
        mLabelMax("Max:")
{
    string type = "DOUBLE";
    string value = "0.0";
    xmlpp::Element* root = 0;
    xmlpp::DomParser dom;

    if (xml != "") {
        dom.parse_memory(xml);
        root = dom.get_document()->get_root_node();
        type = root->get_name();
        if (xml::exist_attribute(root,"VALUE"))
            value = xml::get_attribute(root,"VALUE");
    }

    mButtonCancel = add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
    mButtonValid = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    makeFrameType(type,value);
    makeFrameList(root,(type == "LIST"));
    makeFrameRandom(root,(type == "RANDOM"));

    get_vbox()->add(mVBox);
    mVBox.pack_start(mFrameType, false, true, 0);
    show_all();
    get_size(mWindowWidth, mWindowHeight);

    mButtonValid->set_flags(Gtk::CAN_DEFAULT);
    mButtonValid->grab_default();

    mVBox.add(mFrameList);
    mVBox.add(mFrameRandom);
    on_combotype_changed();
}

string Data::getDataXML() const
{
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("TEMP");
    string type(mComboType.get_active_string());
    string value(mEntryValueType.get_text());

    if (type == "Double") {
        root->set_name("DOUBLE");
        root->set_attribute("VALUE", utils::to_string(utils::to_double(value)));
    } else if (type == "Integer") {
        root->set_name("INTEGER");
        root->set_attribute("VALUE", utils::to_string(utils::to_int(value)));
    } else if (type == "Boolean") {
        root->set_name("BOOLEAN");
        root->set_attribute("VALUE", utils::to_string(utils::to_boolean(value)));
    } else if (type == "String") {
        root->set_name("STRING");
        root->set_attribute("VALUE", value);
    } else if (type == "Integer") {
        root->set_name("DOUBLE");
        root->set_attribute("VALUE", utils::to_string(utils::to_double(value)));
    } else if (type == "Random") {
        root->set_name("RANDOM");
        getRandomDataXML(root);
    } else if (type == "List") {
        root->set_name("LIST");
        getListDataXML(root);
    }
    return doc.write_to_string_formatted();
}

void Data::getRandomDataXML(xmlpp::Element* root) const
{
    string randomtype(mComboTypeRandom.get_active_string());

    root->set_attribute("INSTANCE",
                        utils::to_string(utils::to_double(mInstance.get_text())));

    if (randomtype == "Normal") {
        root->set_attribute("TYPE", "normal");
        root->set_attribute("SEED",
                            utils::to_string(utils::to_double(mSeed.get_text())));
        root->set_attribute("AVERAGE",
                            utils::to_string(utils::to_double(mAverage.get_text())));
        root->set_attribute("STANDARD_DEVIATION",
                            utils::to_string(utils::to_double(
                                                 mStandardDeviation.get_text())));
    } else if (randomtype == "LogNormal") {
        root->set_attribute("TYPE", "lognormal");
        root->set_attribute("SEED",
                            utils::to_string(utils::to_double(mSeed.get_text())));
        root->set_attribute("AVERAGE",
                            utils::to_string(utils::to_double(mAverage.get_text())));
        root->set_attribute("STANDARD_DEVIATION",
                            utils::to_string(utils::to_double(
                                                 mStandardDeviation.get_text())));
    } else if (randomtype == "Uniform") {
        root->set_attribute("TYPE", "uniform");
        root->set_attribute("SEED",
                            utils::to_string(utils::to_double(mSeed.get_text())));
        root->set_attribute("MIN",
                            utils::to_string(utils::to_double(mMin.get_text())));
        root->set_attribute("MAX",
                            utils::to_string(utils::to_double(mMax.get_text())));
    }
}

void Data::getListDataXML(xmlpp::Element* root) const
{
    Gtk::TreeModel::Children children = mListStoreList->children();
    Gtk::TreeModel::Children::const_iterator it = children.begin();
    while (it != children.end()) {
        Gtk::TreeModel::Row row = (*it);
        if (row) {
            Glib::ustring type(row[mTreeColumnList.mType]);
            Glib::ustring value(row[mTreeColumnList.mValue]);

            xmlpp::Element* rowadd = root->add_child("temp");
            if (type == "Double") {
                rowadd->set_name("DOUBLE");
                rowadd->set_attribute("VALUE",
                                      utils::to_string(utils::to_double(value)));
            } else if (type == "Integer") {
                rowadd->set_name("INTEGER");
                rowadd->set_attribute("VALUE",
                                      utils::to_string(utils::to_int(value)));
            } else if (type == "Boolean") {
                rowadd->set_name("BOOLEAN");
                rowadd->set_attribute("VALUE",
                                      utils::to_string(utils::to_boolean(value)));
            } else if (type == "String") {
                rowadd->set_name("STRING");
                rowadd->set_attribute("VALUE", value);
            } else if (type == "Integer") {
                rowadd->set_name("DOUBLE");
                rowadd->set_attribute("VALUE",
                                      utils::to_string(utils::to_double(value)));
            }
        }
        ++it;
    }
}

void Data::validDataTypeValue(const std::string& type, std::string& value)
{
    if (type == "Double")
        value = utils::to_string(utils::to_double(value));
    else if (type == "Integer")
        value = utils::to_string(utils::to_int(value));
    else if (type == "Boolean")
        value = utils::to_string(utils::to_boolean(value));
}

void Data::makeFrameType(const std::string& type, const std::string& value)
{
    mComboType.append_string("String");
    mComboType.append_string("Random");
    mComboType.append_string("List");
    mComboType.append_string("Integer");
    mComboType.append_string("Double");
    mComboType.append_string("Boolean");
    mHBoxType.add(mComboType);
    mHBoxType.add(mLabelValueType);
    mHBoxType.add(mEntryValueType);
    mFrameType.add(mHBoxType);

    if (type == "STRING") mComboType.select_string("String");
    if (type == "RANDOM") mComboType.select_string("Random");
    if (type == "LIST") mComboType.select_string("List");
    if (type == "INTEGER") mComboType.select_string("Integer");
    if (type == "DOUBLE") mComboType.select_string("Double");
    if (type == "BOOLEAN") mComboType.select_string("Boolean");
    mEntryValueType.set_text(value);

    mComboType.signal_changed().connect(sigc::mem_fun(*this,
                                        &Data::on_combotype_changed));
}

void Data::makeFrameList(xmlpp::Element* root, bool b)
{
    mListStoreList = Gtk::ListStore::create(mTreeColumnList);
    mTreeViewList.set_model(mListStoreList);
    mTreeViewList.append_column("Type", mTreeColumnList.mType);
    mTreeViewList.append_column("Value", mTreeColumnList.mValue);
    mTreeViewList.set_size_request(-1, 100);
    mHBoxButtonList.pack_end(mButtonAddList, true, true, 0);
    mHBoxButtonList.pack_end(mButtonEditList, true, true, 0);
    mHBoxButtonList.pack_end(mButtonDelList, true, true, 0);
    mScrolledWindow.add(mTreeViewList);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    mVBoxList.pack_start(mScrolledWindow, true, true, 0);
    mVBoxList.pack_end(mHBoxButtonList, false, true, 0);
    mFrameList.add(mVBoxList);

    if (b) {
        xmlpp::Node::NodeList v_valueList = root->get_children();
        xmlpp::Node::NodeList::iterator it = v_valueList.begin();

        while (it != v_valueList.end()) {
            xmlpp::Element* v_valueNode = (xmlpp::Element*)(*it);
            string v_name = v_valueNode->get_name();

            if (v_name == "DOUBLE" || v_name == "INTEGER" ||
                    v_name == "BOOLEAN" || v_name == "STRING") {
                string v_value = xml::get_attribute(v_valueNode,"VALUE");

                Gtk::TreeModel::Row row = *mListStoreList->append();
                if (v_name == "STRING") row[mTreeColumnList.mType] = "String";
                if (v_name == "INTEGER") row[mTreeColumnList.mType] = "Integer";
                if (v_name == "DOUBLE") row[mTreeColumnList.mType] = "Double";
                if (v_name == "BOOLEAN") row[mTreeColumnList.mType] = "Boolean";
                row[mTreeColumnList.mValue] = v_value;
            }
            ++it;
        }
    }

    mButtonAddList.signal_clicked().connect(sigc::mem_fun(*this,
                                            &Data::on_buttonadd_clicked));
    mButtonEditList.signal_clicked().connect(sigc::mem_fun(*this,
            &Data::on_buttonedit_clicked));
    mButtonDelList.signal_clicked().connect(sigc::mem_fun(*this,
                                            &Data::on_buttondel_clicked));
}

void Data::makeFrameRandom(xmlpp::Element* root, bool b)
{
    mComboTypeRandom.append_string("Uniform");
    mComboTypeRandom.append_string("Normal");
    mComboTypeRandom.append_string("LogNormal");

    mInstance.set_activates_default();
    mSeed.set_activates_default();;
    mAverage.set_activates_default();;
    mStandardDeviation.set_activates_default();;
    mMin.set_activates_default();;
    mMax.set_activates_default();;

    mHBoxInstance.pack_start(mLabelInstance, false, false, 0);
    mHBoxInstance.pack_start(mInstance, true, true, 0);
    mHBoxSeed.pack_start(mLabelSeed, false, false, 0);
    mHBoxSeed.pack_start(mSeed, true, true, 0);
    mHBoxAverage.pack_start(mLabelAverage, false, false, 0);
    mHBoxAverage.pack_start(mAverage, true, true, 0);
    mHBoxStandardDeviation.pack_start(mLabelStandardDeviation, false, false, 0);
    mHBoxStandardDeviation.pack_start(mStandardDeviation, true, true, 0);
    mHBoxMinMax.pack_start(mLabelMin, false, false, 0);
    mHBoxMinMax.pack_start(mMin, true, true, 0);
    mHBoxMinMax.pack_start(mLabelMax, false, false, 0);
    mHBoxMinMax.pack_start(mMax, true, true, 0);
    mVBoxRandom.add(mComboTypeRandom);
    mVBoxRandom.add(mHBoxInstance);
    mVBoxRandom.add(mHBoxSeed);
    mVBoxRandom.add(mHBoxAverage);
    mVBoxRandom.add(mHBoxStandardDeviation);
    mVBoxRandom.add(mHBoxMinMax);
    mFrameRandom.add(mVBoxRandom);

    if (b) {
        string v_type = xml::get_attribute(root,"TYPE");
        string v_number = xml::get_attribute(root,"INSTANCE");

        mInstance.set_text(v_number);
        if (v_type == "normal") {
            string v_seed = xml::get_attribute(root,"SEED");
            string v_average = xml::get_attribute(root,"AVERAGE");
            string v_standardDeviation = xml::get_attribute(root,
                                         "STANDARD_DEVIATION");

            mSeed.set_text(v_seed);
            mAverage.set_text(v_average);
            mStandardDeviation.set_text(v_standardDeviation);
            mComboTypeRandom.select_string("Normal");
        }
        if (v_type == "lognormal") {
            string v_seed = xml::get_attribute(root,"SEED");
            string v_average = xml::get_attribute(root,"AVERAGE");
            string v_standardDeviation = xml::get_attribute(root,
                                         "STANDARD_DEVIATION");

            mSeed.set_text(v_seed);
            mAverage.set_text(v_average);
            mStandardDeviation.set_text(v_standardDeviation);
            mComboTypeRandom.select_string("LogNormal");
        }
        if (v_type == "uniform") {
            string v_seed = xml::get_attribute(root,"SEED");
            string v_min = xml::get_attribute(root,"MIN");
            string v_max = xml::get_attribute(root,"MAX");

            mSeed.set_text(v_seed);
            mMin.set_text(v_min);
            mMax.set_text(v_max);
            mComboTypeRandom.select_string("Uniform");
        }
    }

    mComboTypeRandom.signal_changed().connect(sigc::mem_fun(*this,
            &Data::on_combotyperandom_changed));
    on_combotyperandom_changed();
}

void Data::hideComplex()
{
    mFrameRandom.hide_all();
    mFrameList.hide_all();
    resize(mWindowWidth, mWindowHeight);
    mEntryValueType.set_sensitive(true);
}

void Data::on_combotype_changed()
{
    string txt = mComboType.get_active_string();

    if (txt == "Random") {
        mFrameRandom.show_all();
        mFrameList.hide_all();
        mEntryValueType.set_sensitive(false);
    } else if (txt == "List") {
        mFrameRandom.hide_all();
        mFrameList.show_all();
        mEntryValueType.set_sensitive(false);
    } else {
        hideComplex();
//        mEntryValueType.set_text("");
    }
}

void Data::on_buttonadd_clicked()
{
    TypeDialog typ("","");

    if (typ.run() == Gtk::RESPONSE_OK) {
        Gtk::TreeModel::Row row = *mListStoreList->append();
        string type(typ.getType());
        string value(typ.getValue());
        Data::validDataTypeValue(type, value);
        row[mTreeColumnList.mType] = type;
        row[mTreeColumnList.mValue] = value;
    }
}

void Data::on_buttonedit_clicked()
{
    Glib::RefPtr <Gtk::TreeSelection> selection = mTreeViewList.get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring type = row[mTreeColumnList.mType];
        Glib::ustring value = row[mTreeColumnList.mValue];

        TypeDialog typ(type,value);

        if (typ.run() == Gtk::RESPONSE_OK) {
            string type2 = typ.getType();
            string value2 = typ.getValue();
            Data::validDataTypeValue(type2, value2);
            row[mTreeColumnList.mType] = type2;
            row[mTreeColumnList.mValue] = value2;
        }
    }
}

void Data::on_buttondel_clicked()
{
    Glib::RefPtr <Gtk::TreeSelection> selection = mTreeViewList.get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        mListStoreList->erase(row);
    }
}
void Data::on_combotyperandom_changed()
{
    string txt = mComboTypeRandom.get_active_string();

    if (txt == "Normal" or txt == "LogNormal") {
        mSeed.set_sensitive(true);
        mAverage.set_sensitive(true);
        mStandardDeviation.set_sensitive(true);
        mMin.set_sensitive(false);
        mMax.set_sensitive(false);
        mLabelSeed.set_sensitive(true);
        mLabelAverage.set_sensitive(true);
        mLabelStandardDeviation.set_sensitive(true);
        mLabelMin.set_sensitive(false);
        mLabelMax.set_sensitive(false);
    } else if (txt == "Uniform") {
        mSeed.set_sensitive(true);
        mAverage.set_sensitive(false);
        mStandardDeviation.set_sensitive(false);
        mMin.set_sensitive(true);
        mMax.set_sensitive(true);
        mLabelSeed.set_sensitive(true);
        mLabelAverage.set_sensitive(false);
        mLabelStandardDeviation.set_sensitive(false);
        mLabelMin.set_sensitive(true);
        mLabelMax.set_sensitive(true);
    }
}

}
} // namespace vle gvle
