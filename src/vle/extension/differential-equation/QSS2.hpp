/**
 * @file vle/extension/differential-equation/QSS2.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_QSS2_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_QSS2_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension { namespace DifferentialEquation {

struct VLE_EXTENSION_EXPORT Couple
{
    double value;
    double derivative;
};

class QSS2;

class VLE_EXTENSION_EXPORT mfi
{
public:
    mfi(QSS2*,unsigned int,unsigned int);
    virtual ~mfi();

    Couple init();
    Couple ext(const vle::devs::Time& time, unsigned int i,
               const Couple& couple);

private:
    QSS2* m_qss;
    unsigned int index;
    unsigned int m_functionNumber;
    vle::devs::Time m_lastTime;
    double fiz;
    double* c;
    double* z;
    double* mz;
    double* xt;
    double* yt;
};

typedef mfi* pmfi;

class VLE_EXTENSION_EXPORT QSS2 : public devs::Dynamics
{
    friend class mfi;

public:
    QSS2(const vle::devs::DynamicsInit& model,
         const vle::devs::InitEventList& events);

    virtual ~QSS2() { }

    // DEVS Methods
    virtual void finish();
    virtual vle::devs::Time init(const devs::Time& time);
    virtual void output(const vle::devs::Time& /* time */,
                        vle::devs::ExternalEventList& /* output */) const;
    virtual vle::devs::Time timeAdvance() const;
    virtual vle::devs::Event::EventType confluentTransitions(
        const vle::devs::Time& /* internal */,
        const vle::devs::ExternalEventList& /* extEventlist */) const;
    virtual void internalTransition(const vle::devs::Time& /* event */);
    virtual void externalTransition
        (const vle::devs::ExternalEventList& /* event */,
         const vle::devs::Time& /* time */);
    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& /* event */) const;

private:
    bool m_active;
    /** Number of functions. */
    unsigned int m_functionNumber;

    /** Initial values. */
    std::vector < std::pair < unsigned int , double > > m_initialValueList;

    std::map < unsigned int , std::string > m_variableName;
    std::map < std::string , unsigned int > m_variableIndex;

    /** State. */
    double* m_value;
    double* m_derivative;
    double* m_derivative2;
    double* m_index;
    double* m_index2;
    vle::devs::Time* m_sigma;
    vle::devs::Time* m_lastTime;
    pmfi* m_mfi;

    // Current model
    unsigned int m_currentModel;

    double m_precision;
    double m_epsilon;
    double m_threshold;

    bool delta(double a,double b,double c,double& s);
    bool intermediaire(double a,double b,double c,double cp,double& s);
    virtual double compute(unsigned int i) =0;
    inline double getDerivative(unsigned int i) const;
    inline double getDerivative2(unsigned int i) const;
    inline double getIndex(unsigned int i) const;
    inline double getIndex2(unsigned int i) const;
    inline const vle::devs::Time & getLastTime(unsigned int i) const;
    inline const vle::devs::Time & getSigma(unsigned int i) const;
    void init2();
    virtual void processPerturbation(std::string,double) { }
    inline void setIndex(unsigned int i,double p_index);
    inline void setIndex2(unsigned int i,double p_index);
    inline void setDerivative(unsigned int i,double p_derivative);
    inline void setDerivative2(unsigned int i,double p_derivative);
    inline void setLastTime(unsigned int i,const vle::devs::Time & p_time);
    inline void setSigma(unsigned int i,const vle::devs::Time & p_time);
    inline void setValue(unsigned int i,double p_value);
    void updateSigma(unsigned int i);
    void minSigma();

protected:
    double getValue(unsigned int i) const { return m_value[i]; }
};

}}} // namespace vle extension DifferentialEquation

#endif
