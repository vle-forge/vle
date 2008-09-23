/**
 * @file vle/gvle/ExpMeasures.cpp
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


#include <vle/gvle/ExpMeasures.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/Measures.hpp>
#include <vle/gvle/ObserverPlugin.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

using std::cout;
using std::endl;
using std::string;
using std::list;

namespace vle
{
namespace gvle {

OutputDialogMeasures::OutputDialogMeasures(Modeling* modeling,
        const std::string& name,
        const std::string& format,
        const std::string& plugin,
        const std::string& location,
        const std::string& parameters) :
        mLabelName("Name:"),
        mLabelFormat("Format:"),
        mLabelPlugin("Plugin:"),
        mLabelLocation("Location:"),
        mButtonLocation("..."),
        mLabelParameters("Parameters:"),
        mButtonParameters("..."),
        mModeling(modeling)
{
    mEntryName.set_text(name);
    mEntryLocation.set_text(location);
    mParameters = parameters;

    //add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    //add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    mHBoxName.pack_start(mLabelName, false, true, 0);
    mHBoxName.pack_start(mEntryName, true, true, 0);

    mComboFormat.append_string("textstream");
    mComboFormat.append_string("sdmlstream");
    mComboFormat.append_string("eovstream");
    mHBoxFormat.pack_start(mLabelFormat, false, true, 0);
    mHBoxFormat.pack_start(mComboFormat, true, true, 0);
    if (format != "") mComboFormat.select_string(format);

    //const GVLE::MapObserverPlugin & list = mModeling->getObserverPluginList();
    //GVLE::MapObserverPlugin::const_iterator it = list.begin();
    //while (it != list.end()) {
    //    mComboPlugin.append_string(it->second->getObserverName());
    //    ++it;
    //}

    mHBoxPlugin.pack_start(mLabelPlugin, false, true, 0);
    mHBoxPlugin.pack_start(mComboPlugin, true, true, 0);
    if (plugin != "") mComboPlugin.select_string(plugin);

    mHBoxLocation.pack_start(mLabelLocation, false, true, 0);
    mHBoxLocation.pack_start(mEntryLocation, true, true, 0);
    mHBoxLocation.pack_start(mButtonLocation, false, true, 0);

    mHBoxParameters.pack_start(mLabelParameters, false, true, 0);
    mHBoxParameters.pack_start(mButtonParameters, false, true, 0);

    get_vbox()->add(mHBoxName);
    get_vbox()->add(mHBoxFormat);
    get_vbox()->add(mHBoxPlugin);
    get_vbox()->add(mHBoxLocation);
    get_vbox()->add(mHBoxParameters);
    show_all();

    mButtonLocation.signal_clicked().connect(
        sigc::mem_fun(*this,
                      &OutputDialogMeasures::on_button_location_clicked));

    mButtonParameters.signal_clicked().connect(
        sigc::mem_fun(*this,
                      &OutputDialogMeasures::on_button_parameters_clicked));
}

void OutputDialogMeasures::on_button_location_clicked()
{
    //Gtk::FileChooserDialog file(*this, "Output file",
    //Gtk::FILE_CHOOSER_ACTION_SAVE);
    //file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    //file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    //if (file.run() == Gtk::RESPONSE_OK) {
    //mEntryLocation.set_text(file.get_filename());
    //}
}

void OutputDialogMeasures::on_button_parameters_clicked()
{
    Glib::ustring name = mComboPlugin.get_active_string();
    //ObserverPlugin* call = mModeling->getObserverPlugin(name);

    //if (call and call->hasGraphicsInterface()) {
    //    if (mParameters != "") call->getNextXML(mParameters);
    //    if (call->run()) call->getXML(mParameters);
    //    call->finish();
    //}
}

MeasureDialog::MeasureDialog(const list < string >& lst,
                             const std::string name,
                             const std::string type,
                             const std::string timeStep,
                             const std::string output) :
        mLabelName("Name:"),
        mLabelType("Type:"),
        mLabelTimeStep("Time step:"),
        mLabelOutput("Output:")
{
    //add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    //add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    mHBoxName.pack_start(mLabelName, false, true, 0);
    mHBoxName.pack_start(mEntryName, true, true, 0);
    mEntryName.set_text(name);

    mComboType.append_string("event");
    mComboType.append_string("timed");
    mHBoxType.pack_start(mLabelType, false, true, 0);
    mHBoxType.pack_start(mComboType, true, true, 0);
    if (type != "") mComboType.select_string(type);

    mHBoxTimeStep.pack_start(mLabelTimeStep, false, true, 0);
    mHBoxTimeStep.pack_start(mEntryTimeStep, true, true, 0);
    mEntryTimeStep.set_text(timeStep);

    for (list < string >::const_iterator it = lst.begin();
            it != lst.end(); ++it) {
        mComboOutput.append_string(*it);
    }
    mHBoxOutput.pack_start(mLabelOutput, false, true, 0);
    mHBoxOutput.pack_start(mComboOutput, true, true, 0);
    if (output != "") mComboOutput.select_string(output);

    mComboType.signal_changed().connect(
        sigc::mem_fun(*this, &MeasureDialog::on_combotype_changed));

    get_vbox()->add(mHBoxName);
    get_vbox()->add(mHBoxType);
    get_vbox()->add(mHBoxTimeStep);
    get_vbox()->add(mHBoxOutput);
    show_all();
}

void MeasureDialog::on_combotype_changed()
{
    string select = mComboType.get_active_string();
    mLabelTimeStep.set_sensitive(select != "event");
    mEntryTimeStep.set_sensitive(select != "event");
}

ObservableDialog::ObservableDialog(graph::Model* top,
                                   const std::string& name,
                                   const std::string& port,
                                   const std::string& group,
                                   int index) :
        mTop(top),
        mLabelName("Name:"),
        mLabelPort("Port:"),
        mLabelGroup("Group:"),
        mLabelIndex("Index:")
{
    AssertI(top);

    //add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    //add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    mHBoxName.pack_start(mLabelName, false, true, 0);
    mHBoxName.pack_start(mComboName, true, true, 0);
    mHBoxPort.pack_start(mLabelPort, false, true, 0);
    mHBoxPort.pack_start(mComboPort, true, true, 0);

    mHBoxGroup.pack_start(mLabelGroup, false, true, 0);
    mHBoxGroup.pack_start(mEntryGroup, true, true, 0);
    mEntryGroup.set_text(group);

    mHBoxIndex.pack_start(mLabelIndex, false, true, 0);
    mHBoxIndex.pack_start(mEntryIndex, true, true, 0);
    mEntryIndex.set_text(utils::to_string(index));

    if (top->isCoupled()) {
        fillComboName((graph::CoupledModel*)(top));
    }
    if (name != "") mComboName.select_string(name);
    if (port != "") mComboPort.select_string(port);

    on_comboname_changed();

    get_vbox()->add(mHBoxName);
    get_vbox()->add(mHBoxPort);
    get_vbox()->add(mHBoxGroup);
    get_vbox()->add(mHBoxIndex);
    show_all();

    mComboName.signal_changed().connect(
        sigc::mem_fun(*this, &ObservableDialog::on_comboname_changed));
}

void ObservableDialog::fillComboName(graph::CoupledModel* /* parent */)
{
    Throw(utils::NotYetImplemented, "ObservableDialog::fillComboName");
    //if (parent) {
    //const graph::VectorModel& lst = parent->getModelList();
    //graph::VectorModel::const_iterator it = lst.begin();
    //while (it != lst.end()) {
    //if ((*it)->isCoupled()) {
    //fillComboName((graph::CoupledModel*)(*it));
    //} else if ((*it)->getStatePortNumber()) {
    //mComboName.append_string((*it)->getName());
    //}
    //++it;
    //}
    //}
}

void ObservableDialog::on_comboname_changed()
{
    Throw(utils::NotYetImplemented, "ObservableDialog::on_combotype_changed");
    //string name_selected(getName());
    //graph::Model* finded =
    //((graph::CoupledModel*)mTop)->findModel(name_selected);
    //
    //if (finded and finded->isAtomic()) {
    //mComboPort.clear();
    //const graph::MapStringPort& lst =
    //((graph::AtomicModel*)(finded))->getStatePortList();
    //graph::MapStringPort::const_iterator it = lst.begin();
    //while (it != lst.end()) {
    //mComboPort.append_string((*it).first);
    //++it;
    //}
    //}
}

ExpMeasures::ExpMeasures(Modeling* mod) :
        mModeling(mod),
        mFrameOutputs("Outputs"),
        mButtonAddOutput("Add"),
        mButtonEditOutput("Edit"),
        mButtonDelOutput("Del"),

        mFrameEovs("Eovs"),
        mButtonAddEov("Add"),
        mButtonEditEov("Edit"),
        mButtonDelEov("Del"),

        mFrameMeasures("Measures"),
        mButtonAddMeasure("Add measure"),
        mButtonEditMeasure("Edit measure"),
        mButtonDelMeasure("Delete measure"),
        mButtonAddObservable("Add observable"),
        mButtonEditObservable("Edit observable"),
        mButtonDelObservable("Delete observable")
{
    AssertI(mod);

    mListStoreOutputs = Gtk::ListStore::create(mColumnsOutputs);
    mTreeViewOutputs.set_model(mListStoreOutputs);
    mTreeViewOutputs.append_column("Name", mColumnsOutputs.mName);
    mTreeViewOutputs.append_column("Format", mColumnsOutputs.mFormat);
    mTreeViewOutputs.append_column("Plugin", mColumnsOutputs.mPlugin);
    mTreeViewOutputs.append_column("Location", mColumnsOutputs.mLocation);
    mHBoxButtonsOutput.pack_start(mButtonAddOutput, true, true, 0);
    mHBoxButtonsOutput.pack_start(mButtonEditOutput, true, true, 0);
    mHBoxButtonsOutput.pack_start(mButtonDelOutput, true, true, 0);
    mVBoxOutputs.pack_start(mTreeViewOutputs, true, true, 0);
    mVBoxOutputs.pack_start(mHBoxButtonsOutput, false, true, 0);
    mFrameOutputs.add(mVBoxOutputs);
    mFrameOutputs.set_border_width(3);


    mListStoreEovs = Gtk::ListStore::create(mColumnsEovs);
    mTreeViewEovs.set_model(mListStoreEovs);
    //For this column, we create the CellRenderer ourselves, and connect our own signal handlers,
    //so that we can validate the data that the user enters, and control how it is displayed.
    mTreeviewColumnName.set_title("Name");
    mTreeviewColumnName.pack_start(mCellrendererName);
    mTreeViewEovs.append_column(mTreeviewColumnName);
    Gtk::ScrolledWindow *eovScrollWindow = Gtk::manage(new class Gtk::ScrolledWindow());
    eovScrollWindow->add(mTreeViewEovs);
    eovScrollWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //Tell the view column how to render the model values:
    mTreeviewColumnName.set_cell_data_func(mCellrendererName,
                                           sigc::mem_fun(*this, &ExpMeasures::eov_name_on_cell_data));
    //Make the CellRenderer editable, and handle its editing signals:
    //mCellrendererName.property_editable() = true;
    //mCellrendererName.signal_editing_started().connect(sigc::mem_fun(*this, &ExpMeasures::eov_cellrenderer_name_on_editing_started) );
    //mCellrendererName.signal_edited().connect(sigc::mem_fun(*this, &ExpMeasures::eov_cellrenderer_name_on_edited) );

    mHBoxButtonsEov.pack_start(mButtonAddEov, true, true, 0);
    mHBoxButtonsEov.pack_start(mButtonEditEov, true, true, 0);
    mHBoxButtonsEov.pack_start(mButtonDelEov, true, true, 0);
    mVBoxEovs.pack_start(*eovScrollWindow, true, true,0);

    mVBoxEovs.pack_start(mHBoxButtonsEov, false, true, 0);
    mFrameEovs.add(mVBoxEovs);
    mFrameEovs.set_border_width(3);

    mTreeStoreMeasures = Gtk::TreeStore::create(mColumnsMeasures);
    mTreeViewMeasures.set_model(mTreeStoreMeasures);
    mTreeViewMeasures.append_column("Name", mColumnsMeasures.mName);
    mTreeViewMeasures.append_column("Data", mColumnsMeasures.mText);

    mHBoxButtonsMeasure.pack_start(mButtonAddMeasure, true, true, 0);
    mHBoxButtonsMeasure.pack_start(mButtonEditMeasure, true, true, 0);
    mHBoxButtonsMeasure.pack_start(mButtonDelMeasure, true, true, 0);
    mHBoxButtonsObservable.pack_start(mButtonAddObservable, true, true, 0);
    mHBoxButtonsObservable.pack_start(mButtonEditObservable, true, true, 0);
    mHBoxButtonsObservable.pack_start(mButtonDelObservable, true, true, 0);
    mVBoxMeasures.pack_start(mTreeViewMeasures, true, true, 0);
    mVBoxMeasures.pack_start(mHBoxButtonsMeasure, false, true, 0);
    mVBoxMeasures.pack_start(mHBoxButtonsObservable, false, true, 0);
    mFrameMeasures.add(mVBoxMeasures);
    mFrameMeasures.set_border_width(3);


    pack_start(mFrameOutputs, true, true, 0);
    pack_start(mFrameEovs, true, true, 0);
    pack_start(mFrameMeasures, true, true, 0);

    mButtonAddMeasure.set_sensitive(true);
    mButtonEditMeasure.set_sensitive(false);
    mButtonDelMeasure.set_sensitive(false);

    mButtonAddObservable.set_sensitive(false);
    mButtonEditObservable.set_sensitive(false);
    mButtonDelObservable.set_sensitive(false);

    mButtonAddOutput.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_add_output));
    mButtonEditOutput.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_edit_output));
    mButtonDelOutput.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_del_output));

    mButtonAddEov.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_add_eov));
    mButtonEditEov.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_edit_eov));
    mButtonDelEov.signal_clicked().connect(sigc::mem_fun(*this, &ExpMeasures::on_button_del_eov));

    mButtonAddMeasure.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_add_measure));
    mButtonEditMeasure.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_edit_measure));
    mButtonDelMeasure.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_del_measure));
    mButtonAddObservable.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_add_observable));
    mButtonEditObservable.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_edit_observable));
    mButtonDelObservable.signal_clicked().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_button_del_observable));

    mTreeViewMeasures.signal_cursor_changed().connect(
        sigc::mem_fun(*this, &ExpMeasures::on_select_row_measure));

    set_border_width(3);

    //TO FIX
    //initOutputs();
    initMeasures();
}

std::string ExpMeasures::buildMeasureText(const std::string& type,
        const std::string& timeStep,
        const std::string& output)
{
    string text;

    if (type == "timed") {
        text = (boost::format("timed, time-step = [%1%] on output = [%2%]") %
                timeStep % output).str();
    } else {
        text = (boost::format("event, on output = [%1%]") % output).str();
    }
    return text;
}

std::string ExpMeasures::buildObservableText(const std::string& model,
        const std::string& port,
        const std::string& group,
        int index)
{
    std::string text;

    if (group.empty()) {
        text = (boost::format("port %1% of %2%") % port % model).str();
    } else {
        if (index == 0) {
            text = (boost::format("port %1% of %2% with group %3%") %
                    port % model % group).str();
        } else {
            text = (boost::format(
                        "port %1% of %2% with group %3% and index %4%") %
                    port % model % group % index).str();
        }
    }
    return text;
}

void ExpMeasures::initOutputs()
{
    Throw(utils::NotYetImplemented, "ExpMeasures::initOutputs");
    //const vpz::Outputs& outs = mModeling->measures().outputs();
    //std::map < std::string, vpz::Output >::const_iterator it;
    //
    //for (it = outs.outputs().begin(); it != outs.outputs().end(); ++it) {
    //Gtk::TreeModel::Row row = *(mListStoreOutputs->append());
    //row[mColumnsOutputs.mName] = (*it).first;
    //row[mColumnsOutputs.mFormat] = (*it).second.streamformat();
    //row[mColumnsOutputs.mPlugin] = (*it).second.plugin();
    //row[mColumnsOutputs.mLocation] = (*it).second.location();
    //}
}


void ExpMeasures::on_button_add_output()
{
    Throw(utils::NotYetImplemented, "ExpMeasures::on_button_add_output");
    //OutputDialogMeasures dialog(mModeling,"", "", "", "", "");
    //if (dialog.run() == Gtk::RESPONSE_OK and
    //not mModeling->measures().outputs().exist(dialog.getName())) {
    //if (dialog.getFormat() == "eovstream") {
    //mModeling->measures().addEovStreamOutput(dialog.getName(),
    //dialog.getPlugin(), dialog.getLocation());
    //mModeling->measures().outputs().find(
    //dialog.getName()).setXML(dialog.getParameters());
    //} else if (dialog.getFormat() == "textstream") {
    //mModeling->measures().addTextStreamOutput(dialog.getName(),
    //dialog.getLocation());
    //} else if (dialog.getFormat() == "sdmlstream") {
    //mModeling->measures().addSdmlStreamOutput(dialog.getName(),
    //dialog.getLocation());
    //} else {
    //ThrowInternal();
    //}
    //Gtk::TreeModel::Row row = *(mListStoreOutputs->append());
    //row[mColumnsOutputs.mName] = dialog.getName();
    //row[mColumnsOutputs.mFormat] = dialog.getFormat();
    //row[mColumnsOutputs.mPlugin] = dialog.getPlugin();
    //row[mColumnsOutputs.mLocation] = dialog.getLocation();
    //}
}

void ExpMeasures::on_button_edit_output()
{
    Throw(utils::NotYetImplemented, "ExpMeasures::on_button_edit_output");
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewOutputs.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsOutputs.mName];
    //vpz::Output& output(mModeling->measures().outputs().find(name));
    //
    //OutputDialogMeasures dialog(mModeling, name, output.streamformat(),
    //output.plugin(), output.location(),
    //output.xml());
    //if (dialog.run() == Gtk::RESPONSE_OK and (name == dialog.getName() or
    //not mModeling->measures().outputs().exist(dialog.getName()))) {
    //mModeling->measures().delOutput(name);
    //if (dialog.getFormat() == "eovstream") {
    //mModeling->measures().addEovStreamOutput(dialog.getName(),
    //dialog.getPlugin(), dialog.getLocation());
    //mModeling->measures().outputs().find(
    //dialog.getName()).setXML(dialog.getParameters());
    //} else if (dialog.getFormat() == "textstream") {
    //mModeling->measures().addTextStreamOutput(dialog.getName(),
    //dialog.getLocation());
    //} else if (dialog.getFormat() == "sdmlstream") {
    //mModeling->measures().addSdmlStreamOutput(dialog.getName(),
    //dialog.getLocation());
    //} else {
    //ThrowInternal();
    //}
    //row[mColumnsOutputs.mName] = dialog.getName();
    //row[mColumnsOutputs.mFormat] = dialog.getFormat();
    //row[mColumnsOutputs.mPlugin] = dialog.getPlugin();
    //row[mColumnsOutputs.mLocation] = dialog.getLocation();
    //}
    //}
}

void ExpMeasures::on_button_del_output()
{
    Throw(utils::NotYetImplemented, "ExpMeasures::on_button_del_output");
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewOutputs.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsOutputs.mName];
    //mModeling->measures().outputs().delOutput(name);
    //mListStoreOutputs->erase(row);
    //}
}

void ExpMeasures::initMeasures()
{
    Throw(utils::NotYetImplemented, "ExpMeasures::initMeasures");
    //const vpz::Measures& msrs = mModeling->measures();
    //std::map < std::string, vpz::Measure >::const_iterator it;
    //
    //for (it = msrs.measures().begin(); it != msrs.measures().end(); ++it) {
    //Gtk::TreeModel::Row row = *(mTreeStoreMeasures->append());
    //row[mColumnsMeasures.mName] = (*it).first;
    //row[mColumnsMeasures.mMeasure] = (*it).first;
    //
    //Glib::ustring txt;
    //
    //if ((*it).second.type() == vpz::Measure::TIMED) {
    //txt = (boost::format("timed, time-step = [%1%] on output = [%2%]")
    //% (*it).second.timestep() % (*it).second.output()).str();
    //} else {
    //txt = (boost::format("event, on output = [%1%]") %
    //(*it).second.output()).str();
    //}
    //row[mColumnsMeasures.mText] = txt;
    //
    //std::list < vpz::Observable >::const_iterator jt;
    //for (jt = (*it).second.observables().begin();
    //jt != (*it).second.observables().end(); ++jt) {
    //
    //Gtk::TreeModel::Row rowo =
    //*(mTreeStoreMeasures->append(row.children()));
    //
    //rowo[mColumnsMeasures.mName] = "";
    //rowo[mColumnsMeasures.mText] = buildObservableText(
    //(*jt).modelname(), (*jt).portname(), (*jt).group(),
    //(*jt).index());
    //rowo[mColumnsMeasures.mModel] = (*jt).modelname();
    //rowo[mColumnsMeasures.mPort] = (*jt).portname();
    //rowo[mColumnsMeasures.mGroup] = (*jt).group();
    //rowo[mColumnsMeasures.mIndex] = (*jt).index();
    //rowo[mColumnsMeasures.mMeasure] = (*it).first;
    //
    //
    //}
    //}
}
std::vector < std::pair < std::string, std::string > > ExpMeasures::translate_output_to_eov()
{
    std::vector < std::pair < string, string > > liste;
    //const Measures::Outputs_t& out = mModeling->getOutputs();
    //Measures::Outputs_t::const_iterator it = out.begin();
    //xmlpp::DomParser dom;
    //xmlpp::Element *root;
    //Glib::ustring name, output, format;

    //while (it != out.end()) {
    //dom.parse_memory((*it).second);
    //root = utils::XML::get_root_node(dom, "OUTPUT");
    //name = utils::XML::get_attribute(root, "PLUGIN");
    //output = utils::XML::get_attribute(root, "NAME");
    //format = utils::XML::get_attribute(root, "FORMAT");

    //if (format == "net")
    //liste.push_back(std::pair<string, string>(name, output));
    //++it;
    //}

    return liste;
}

void ExpMeasures::on_button_add_eov()
{
    //std::vector < std::pair < string, string > > liste_output = translate_output_to_eov();
    //list < Glib::ustring > liste;
    //int response;

    //const Measures::Eovs_t& eovs = mModeling->getEovs();
    //Measures::Eovs_t::const_iterator it;

    //for (it = eovs.begin(); it != eovs.end(); ++it)
    //liste.push_back((*it).first);

    //EovDialogMeasures dialog("", liste, liste_output);
    //response = dialog.run();

    //if (response == Gtk::RESPONSE_OK) {
    //mModeling->addMeasuresEovs(dialog.get_name(), dialog.get_xml());
    //eov_add_row(dialog.get_name());
    //}
}

void ExpMeasures::on_button_edit_eov()
{
    //std::vector < std::pair < string, string > > liste_output = translate_output_to_eov();
    //list < Glib::ustring > liste;
    //int response;

    //Glib::ustring name = eov_get_selected_row();
    //if (name != "")
    //{
    //const string eov_xml = mModeling->getMeasuresEovs(name);
    //EovDialogMeasures dialog(name, liste, liste_output, eov_xml);

    //response = dialog.run();
    //if (response == Gtk::RESPONSE_OK) {
    //if (dialog.has_name_changed()) {
    //mModeling->delEovs(dialog.get_old_name());
    //mModeling->addMeasuresEovs(dialog.get_name(), dialog.get_xml());
    //eov_del_row(dialog.get_old_name());
    //eov_add_row(dialog.get_name());
    //} else {
    //mModeling->addMeasuresEovs(dialog.get_name(), dialog.get_xml());
    //}
    //}
    //}
}

void ExpMeasures::on_button_del_eov()
{
    //string name = eov_get_selected_row();
    //if (name != "") {
    //eov_del_row(name);
    //mModeling->delEovs(name);
    //}
}

void ExpMeasures::updateEovs(std::string /* lib */, std::string /* output */)
{
    //list < Glib::ustring > liste;
    //const Measures::Eovs_t& eovs = mModeling->getEovs();
    //Measures::Eovs_t::const_iterator it;
    //string new_xml;
    //
    //for (it = eovs.begin(); it != eovs.end(); ++it) {
    //new_xml = Eov::checkDeletePlugin((*it).second, lib, output);
    //mModeling->addMeasuresEovs((*it).first, new_xml);
    //}
}

void ExpMeasures::eov_name_on_cell_data(
    Gtk::CellRenderer* /* renderer */,
    const Gtk::TreeModel::iterator& /* iter */)
{
    //renderer = renderer;
    //if(iter)
    //{
    //Gtk::TreeModel::Row row = *iter;
    //Glib::ustring view_text = row[mColumnsEovs.mName];
    //mCellrendererName.property_text() = view_text;
    //}
}

void ExpMeasures::eov_cellrenderer_name_on_editing_started(
    Gtk::CellEditable* /* cell_editable */,
    const Glib::ustring& /* path */)
{
    //Glib::ustring _path = path;
    //if(mNameRetry)
    //{
    //Gtk::CellEditable* celleditable_validated = cell_editable;
    //
    //Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
    //if(pEntry)
    //{
    //pEntry->set_text(mInvalidTextForRetry);
    //mNameRetry = false;
    //mInvalidTextForRetry.clear();
    //}
    //}
}

void ExpMeasures::eov_cellrenderer_name_on_edited(
    const Glib::ustring& /*path_string */,
    const Glib::ustring& /* new_text */)
{
    //Gtk::TreePath path(path_string);
    //std::vector < string > liste;
    //const Measures::Eovs_t& eovs = mModeling->getEovs();
    //Measures::Eovs_t::const_iterator it;

    //for (it = eovs.begin(); it != eovs.end(); ++it)
    //liste.push_back((*it).first);

    //if (std::find(liste.begin(), liste.end(), new_text) != liste.end())
    //{
    //widgets::Error("Name already exist");
    //mInvalidTextForRetry = new_text;
    //mNameRetry = true;

    //mTreeViewEovs.set_cursor(path, mTreeviewColumnName, mCellrendererName, true);
    //} else {
    //Gtk::TreeModel::iterator iter = mListStoreEovs->get_iter(path);
    //if(iter) {
    //Gtk::TreeModel::Row row = *iter;
    //Glib::ustring old_name = row[mColumnsEovs.mName];
    //Glib::ustring xml = mModeling->getMeasuresEovs(old_name);
    //mModeling->delEovs(old_name);
    //mModeling->addMeasuresEovs(new_text, xml);
    //row[mColumnsEovs.mName] = new_text;

    //}
    //}
}

std::string ExpMeasures::eov_get_selected_row()
{
    string result;
    //Glib::RefPtr < Gtk::TreeSelection > selection = mTreeViewEovs.get_selection();
    //Gtk::TreeModel::iterator it = selection->get_selected();
    //if (it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name(row[mColumnsEovs.mName]);
    //result = name;
    //}
    return result;
}

void ExpMeasures::eov_add_row(const std::string& /* name */)
{
    //Gtk::TreeModel::Row row = *(mListStoreEovs->append());
    //if (row) {
    //row[mColumnsEovs.mName] = name;
    //}
}

void ExpMeasures::eov_del_row(const std::string& /* name */)
{
    //Gtk::TreeModel::Children children = mListStoreEovs->children();
    //Gtk::TreeModel::iterator it = children.begin();
    //while (it != children.end()) {
    //Gtk::TreeModel::Row row = *it;
    //if (row[mColumnsEovs.mName] == name) {
    //mListStoreEovs->erase(it);
    //break;
    //}
    //++it;
    //}
}

void ExpMeasures::on_button_add_measure()
{
    //const vpz::Outputs& outputs = mModeling->outputs();
    //std::list < std::string > lst = outputs.outputsname();
    //
    //MeasureDialog dialog(lst, "", "", "", "");
    //if (dialog.run() == Gtk::RESPONSE_OK) {
    //if (dialog.getType() == "timed") {
    //mModeling->measures().addTimedMeasure(
    //dialog.getName(), utils::to_double(dialog.getTimeStep(), 1.0),
    //dialog.getOutput());
    //} else if (dialog.getType() == "event") {
    //mModeling->measures().addEventMeasure(
    //dialog.getName(), dialog.getOutput());
    //} else {
    //ThrowInternal();
    //}
    //
    //Gtk::TreeModel::Row row = *(mTreeStoreMeasures->append());
    //row[mColumnsMeasures.mName] = dialog.getName();
    //row[mColumnsMeasures.mMeasure] = dialog.getName();
    //
    //if (dialog.getType() == "timed") {
    //row[mColumnsMeasures.mText] = (boost::format(
    //"Timed event on %1% with %2% timestep") %
    //dialog.getOutput() % dialog.getTimeStep()).str();
    //} else {
    //row[mColumnsMeasures.mText] = (boost::format(
    //"Event on %1%") % dialog.getOutput()).str();
    //}
    //}
}

void ExpMeasures::on_button_edit_measure()
{
    //const vpz::Outputs& outputs = mModeling->outputs();
    //std::list < std::string > lst = outputs.outputsname();
    //
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mName];
    //vpz::Measure& msr(mModeling->measures().find(name));
    //
    //if (not name.empty()) {
    //MeasureDialog dialog(lst, name, msr.streamtype(),
    //utils::to_string(msr.timestep()), msr.output());
    //
    //if (dialog.run() == Gtk::RESPONSE_OK and (name == dialog.getName() or
    //not mModeling->measures().exist(dialog.getName()))) {
    //mModeling->measures().delMeasure(name);
    //if (dialog.getType() == "timed") {
    //vpz::Measure& m = mModeling->measures().addTimedMeasure(
    //dialog.getName(),
    //utils::to_double(dialog.getTimeStep(), 1.0),
    //dialog.getOutput());
    //row[mColumnsMeasures.mText] = (boost::format(
    //"Timed event on %1% with %2% timestep.") %
    //m.output() % m.timestep()).str();
    //} else if (dialog.getType() == "event") {
    //vpz::Measure& m = mModeling->measures().addEventMeasure(
    //dialog.getName(), dialog.getOutput());
    //row[mColumnsMeasures.mText] = (boost::format(
    //"Event on %1%.") % m.output()).str();
    //} else {
    //ThrowInternal();
    //}
    //row[mColumnsMeasures.mName] = name;
    //row[mColumnsMeasures.mMeasure] = name;
    //}
    //}
    //}
}

void ExpMeasures::on_button_del_measure()
{
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mName];
    //if (not name.empty()) {
    //mModeling->measures().delMeasure(name);
    //mTreeStoreMeasures->erase(row);
    //}
    //}
}

void ExpMeasures::on_button_add_observable()
{
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mName];
    //vpz::Measure& msr = mModeling->measures().find(name);
    //
    //ObservableDialog dialog(mModeling->getTopModel(), "", "", "", 0);
    //if (dialog.run() == Gtk::RESPONSE_OK) {
    //try {
    //msr.addObservable(dialog.getName(),
    //dialog.getPort(), dialog.getGroup(),
    //dialog.getIndex());
    //Gtk::TreeModel::Row child =
    //*(mTreeStoreMeasures->append(row.children()));
    //
    //child[mColumnsMeasures.mText] =
    //buildObservableText(dialog.getName(),dialog.getPort(),
    //dialog.getGroup(),dialog.getIndex());
    //child[mColumnsMeasures.mModel] = dialog.getName();
    //child[mColumnsMeasures.mPort] = dialog.getPort();
    //child[mColumnsMeasures.mGroup] = dialog.getGroup();
    //child[mColumnsMeasures.mIndex] = dialog.getIndex();
    //} catch(const std::exception& e) {
    //widgets::Error((boost::format(
    //"Failed to add observable '%1%' on port '%2%'.") %
    //dialog.getName() %
    //dialog.getPort()).str());
    //}
    //}
    //}
}

void ExpMeasures::on_button_edit_observable()
{
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mMeasure];
    //
    //Gtk::TreeModel::iterator jt = row.parent();
    //if (name.empty() and jt and *jt) {
    //Gtk::TreeModel::Row rowparent = *jt;
    //name = rowparent[mColumnsMeasures.mName];
    //vpz::Measure& msr = mModeling->measures().find(name);
    //const Glib::ustring model = row[mColumnsMeasures.mModel];
    //const Glib::ustring port = row[mColumnsMeasures.mPort];
    //const Glib::ustring group = row[mColumnsMeasures.mGroup];
    //const int index = row[mColumnsMeasures.mIndex];
    //
    //ObservableDialog dialog(mModeling->getTopModel(), model,
    //port, group, index);
    //
    //if (dialog.run() == Gtk::RESPONSE_OK) {
    //msr.delObservable(model, port, group, index);
    //
    //row[mColumnsMeasures.mText] =
    //buildObservableText(dialog.getName(),dialog.getPort(),
    //dialog.getGroup(),dialog.getIndex());
    //row[mColumnsMeasures.mModel] = dialog.getName();
    //row[mColumnsMeasures.mPort] = dialog.getPort();
    //row[mColumnsMeasures.mGroup] = dialog.getGroup();
    //row[mColumnsMeasures.mIndex] = dialog.getIndex();
    //msr.addObservable(dialog.getName(),dialog.getPort(),
    //dialog.getGroup(),dialog.getIndex());
    //}
    //}
    //}
}

void ExpMeasures::on_button_del_observable()
{
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mName];
    //
    //if (name.empty()) {
    //Gtk::TreeModel::iterator jt = row.parent();
    //if (jt and *jt) {
    //Gtk::TreeModel::Row rowparent = *jt;
    //
    //name = rowparent[mColumnsMeasures.mName];
    //vpz::Measure& msr = mModeling->measures().find(name);
    //const Glib::ustring model = row[mColumnsMeasures.mModel];
    //const Glib::ustring port = row[mColumnsMeasures.mPort];
    //const Glib::ustring group = row[mColumnsMeasures.mGroup];
    //int index = row[mColumnsMeasures.mIndex];
    //
    //msr.delObservable(model, port, group, index);
    //}
    //mTreeStoreMeasures->erase(row);
    //}
    //}
}

void ExpMeasures::on_select_row_measure()
{
    //Glib::RefPtr < Gtk::TreeSelection > select;
    //select = mTreeViewMeasures.get_selection();
    //Gtk::TreeModel::iterator it = select->get_selected();
    //if (it and *it) {
    //Gtk::TreeModel::Row row = *it;
    //Glib::ustring name = row[mColumnsMeasures.mName];
    //if (name.empty()) {
    //mButtonAddMeasure.set_sensitive(false);
    //mButtonEditMeasure.set_sensitive(false);
    //mButtonDelMeasure.set_sensitive(false);
    //mButtonAddObservable.set_sensitive(true);
    //mButtonEditObservable.set_sensitive(true);
    //mButtonDelObservable.set_sensitive(true);
    //} else {
    //mButtonAddMeasure.set_sensitive(true);
    //mButtonEditMeasure.set_sensitive(true);
    //mButtonDelMeasure.set_sensitive(true);
    //mButtonAddObservable.set_sensitive(true);
    //mButtonEditObservable.set_sensitive(false);
    //mButtonDelObservable.set_sensitive(false);
    //}
    //}
}

}} // namespace vle gvle
