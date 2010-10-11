/*
 * @file examples/fsa/Stage.hpp
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


#ifndef FSA_STAGE_HPP
#define FSA_STAGE_HPP

#include <vle/extension/fsa/Statechart.hpp>
#include <vle/extension/difference-equation/Base.hpp>

enum state_t {INIT = 1, PS, LV, IF, DF, DRG, FSLA, MP, FIN };

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

class Stage : public vf::Statechart
{
public:
    Stage(const vd::DynamicsInit& init, const vd::InitEventList& lst);

    virtual ~Stage()
    { }

private:
    void activity1(const vd::Time& /* time */) // PS
    {
        if (STiz > 0.0) {
            CumST = CumST + STiz;
        }
    }
    void activity2(const vd::Time& /* time */) // LV
    {
        CumST = CumST + STiz;
        CumST_LV = CumST_LV + STiz ;
    }
    void activity3(const vd::Time& /* time */) // IF
    {
        CumST = CumST + STiz;
        CumST_LV = CumST_LV + STiz ;
    }
    void activity4(const vd::Time& /* time */) // DF
    {
        CumST = CumST + STiz;
        CumST_LV = CumST_LV + STiz ;
        CumST_DF = CumST_DF + STiz;
    }
    void activity5(const vd::Time& /* time */) // DRG
    {
        CumST = CumST + STiz;
        CumST_DRG = CumST_DRG + STiz;
        delta = CumST_LV - stdrg;
    }
    void activity6(const vd::Time& /* time */) // FSLA
    {
        CumST = CumST + STiz;
        CumST_FSLA = CumST_FSLA + STiz;
    }
    void activity7(const vd::Time& /* time */) // MP
    {
        CumST = CumST + STiz;
    }

    void action1(const vd::Time& /* time */)
    {
        CumST_LV = 0.0;
        seuil = stif + stlv;
    }
    void action2(const vd::Time& /* time */)
    {
        CumST_DF = 0.0;
        seuil = stdrg;
    }
    void action3(const vd::Time& /* time */)
    {
        CumST_DRG = 0.0;
        stfsla = (NETpot - 1.0) * vdrg;
    }
    void action4(const vd::Time& /* time */) { CumST_FSLA = 0; }

    bool c2(const vd::Time& /* time */) { return CumST >= stlv; }
    bool c3(const vd::Time& /* time */) { return CumST >= seuil; }
    bool c5(const vd::Time& /* time */) { return CumST_DF >= seuil; }
    bool c6(const vd::Time& /* time */) { return CumST_DRG >= stfsla; }
    bool c7(const vd::Time& /* time */) { return CumST_FSLA >= seuil; }

    void in(const vd::Time& /* time */,
            const vd::ExternalEvent* event)
    {
        STi << ve::DifferenceEquation::Var("Tmoy", event);
        if (STi > 0.0) {
            STiz = STi;
        } else {
            STiz = 0.0;
        }
    }

    long date_sem;
    long date_DF;
    long date_rec;

    double stlv;
    double stif;
    double stdf;
    double stdrg;
    double stmp;
    double ifjour;
    double stfsla;
    double vdrg;
    double NETpot;
    double Nstruct;
    double P1Gpot;
    double STi;
    double STiz;
    double CumST;
    double CumST_DF;
    double CumST_FSLA;
    double CumST_LV;
    double CumST_DRG;
    double Nveg;
    double delta;
    double seuil;
};

}}} // namespace vle examples fsa

#endif
