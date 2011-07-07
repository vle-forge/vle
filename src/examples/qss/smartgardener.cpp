/*
 * @file examples/qss/smartgardener.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include "smartgardener.hpp"

namespace vle { namespace examples { namespace qss {

Smartgardener::Smartgardener(const devs::DynamicsInit& model,
                             const devs::InitEventList& events) :
    devs::Dynamics(model, events)
{
    // Seuil de traitement sur les plantlouses
    s = value::toDouble(events.get("s"));
    // Pourcentage de plantlouses supprimés par le traitement
    p_p = value::toDouble(events.get("p_p"));
    // Pourcentage de ladybirds supprimés par le traitement
    p_l = value::toDouble(events.get("p_l"));
    // Délai entre deux observations
    d = value::toDouble(events.get("d"));
}

Smartgardener::~Smartgardener()
{
}

void Smartgardener::output(const devs::Time& /*time*/,
                           devs::ExternalEventList& output) const
{

    // Observation la quantité de plantlouse et de ladybird
    if (state == IDLE) {
        devs::RequestEvent* e = new
            devs::RequestEvent("inst_plantlouse");
        e << devs::attribute("name", std::string("x"));
        output.addEvent(e);

        e = new devs::RequestEvent("inst_labybird");
        e << devs::attribute("name", std::string("y"));
        output.addEvent(e);
    }

    // Traitement des plantlouses (et des ladybirds)
    if (state == PEST) {
        devs::ExternalEvent* e = new devs::ExternalEvent("out_plantlouse");

        e << devs::attribute("name", std::string("x"))
            << devs::attribute("value", p_p*plantlouseamount);
        output.addEvent(e);

        e = new devs::ExternalEvent("out_ladybird");
        e << devs::attribute("name", std::string("y"))
            << devs::attribute("value", p_l*ladybirdamount);
        output.addEvent(e);
    }
}

devs::Time Smartgardener::timeAdvance() const
{
    switch (state) {
    case INIT:
        return 0.0;
    case IDLE:
        return d;
    case PEST:
        return 0.0;
    default:
        throw utils::InternalError("Smartgardener::timeAdvance");
    }
}

devs::Time Smartgardener::init(const vle::devs::Time& /* time */)
{
    state = INIT;

    return 0.0;
}

void Smartgardener::internalTransition(const devs::Time& /* event */)
{
    switch (state) {
    case INIT:
        state = IDLE;
        return;
    case IDLE:
        state = IDLE;
        return;
    case PEST:
        state = IDLE;
        return;
    default:
        throw utils::InternalError("Smartgardener::internalTransition");
    }
}

value::Value* Smartgardener::observation(const devs::ObservationEvent& event)
    const
{
    if (event.onPort("c")) {
        if (state == PEST) {
            return buildDouble(1);
        } else {
            return 0;
        }
    }

    return 0;
}

void Smartgardener::externalTransition(const devs::ExternalEventList& event,
                                          const devs::Time& /*time*/)
{
    // Récupération du nombre de plantlouses (x) et de
    // ladybirds (y)
    if (state == IDLE) {
        for (devs::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->getStringAttributeValue("name") == "x") {
                plantlouseamount = (*it)->getDoubleAttributeValue("value");

                // si le nb de plantlouses est supérieur au
                // seuil alors mode traitement
                if (plantlouseamount > s) {
                    state = PEST;
                }
            }
            else if ((*it)->getStringAttributeValue("name") == "y") {
                ladybirdamount = (*it)->getDoubleAttributeValue("value");
            }
        }
    }
}

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(smartgardener, vle::examples::qss::Smartgardener)
