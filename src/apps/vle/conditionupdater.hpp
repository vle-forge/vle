/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#ifndef VLE_CLI_CONDITIONUPDATER_HPP
#define VLE_CLI_CONDITIONUPDATER_HPP

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>

#include <string>
#include <vector>

#ifdef VLE_HAVE_NLS
#ifndef ENABLE_NLS
#define ENABLE_NLS
#endif
#include <libintl.h>
#include <locale.h>
#define _(x) gettext(x)
#define gettext_noop(x) x
#define N_(x) gettext_noop(x)
#else
#define _(x) x
#define N_(x) x
#endif

namespace vle {

class ConditionUpdater
{
private:
    struct accessor
    {
        accessor(std::string condition_,
                 std::string port_,
                 std::vector<std::string> values_,
                 std::string value);

        std::string condition;
        std::string port;
        std::vector<std::string> value_stack;
        std::string value;
    };

    std::vector<accessor> m_access;

    vle::value::Value* get(vle::vpz::Vpz& file, const accessor& access) const;

public:
    /**
     * @brief Add a condition change if @c cnd parameter is valid.
     *
     * @details Try to convert the string representation of the change to a
     * condition change i.e. it store the condition name, port name and
     * optionaly value accessor plus the value to assign.
     *
     * @code
     * ConditionUpdater cond;
     * cond.emplace("condition.port=10");
     * cond.emplace("condition.port.map.map2=hello");
     * cond.emplace("condition.port.1.map.1.map2=world");
     * @endcode
     *
     * @param cnd The string representation of the change.
     *
     * @return True if the insert is a success, false otherwise.
     *
     * @exception @c std::bad_alloc.
     */
    bool emplace(const std::string& cnd);

    /**
     * @brief Replace @c vle::value::Value in experimental conditions.
     *
     * @details Try to replace all @c vle::value::Value pointer by the
     * condition change.
     *
     * @code
     * ConditionUpdater cond;
     * cond.emplace("condition.port.1=hello");
     * cond.emplace("condition.port.2=world");
     *
     * auto file = vle::vpz::Vpz("empty.vpz");
     * cond.update(file);
     * @endcode
     *
     * @param file @c vle::vpz::Vpz file to be change.
     */
    void update(vle::vpz::Vpz& file) const;

    /**
     * @brief Return true condition change is empty, false is at least one
     * @change is available.
     */
    bool empty() const noexcept;
};

inline ConditionUpdater::accessor::accessor(std::string condition_,
                                            std::string port_,
                                            std::vector<std::string> values_,
                                            std::string value_)
  : condition(std::move(condition_))
  , port(std::move(port_))
  , value_stack(std::move(values_))
  , value(std::move(value_))
{}

inline bool
ConditionUpdater::emplace(const std::string& cnd)
{
    // Try to split string in two part: the accessor and the value. If their is
    // no equal character, this is a syntax error.

    auto it = cnd.find('=');
    if (it == std::string::npos)
        return false;

    auto access = cnd.substr(0, it);
    auto value = cnd.substr(it + 1, std::string::npos);

    std::vector<std::string> tokens;
    vle::utils::tokenize(access, tokens, ".", true);

    if (tokens.size() <= 1)
        return false;

    std::vector<std::string> arg(tokens.begin() + 2, tokens.end());
    m_access.emplace_back(tokens[0], tokens[1], arg, value);

    return true;
}

inline vle::value::Value*
ConditionUpdater::get(vle::vpz::Vpz& file,
                      const ConditionUpdater::accessor& access) const
{
    auto& cnd = file.project().experiment().conditions().get(access.condition);
    auto& set = cnd.getSetValues(access.port);

    if (access.value_stack.empty())
        return set[0].get();

    vle::value::Value* current = set[0].get();
    for (std::size_t i = 0, e = access.value_stack.size(); i != e; ++i) {
        if (current->isSet()) {
            errno = 0;
            long int val = strtol(access.value_stack[i].c_str(), nullptr, 10);

            if (errno or val < 0 or
                val >= static_cast<long int>(current->toSet().size()))
                throw vle::utils::ArgError(
                  _("Fails to convert '%s.%s' parameter '%lu'"
                    " as correct set index"),
                  access.condition.c_str(),
                  access.port.c_str(),
                  static_cast<unsigned long>(i));

            current = current->toSet()[static_cast<std::size_t>(val)].get();
        } else if (current->isMap()) {
            auto it = current->toMap().find(access.value_stack[i]);

            if (it == current->toMap().end())
                throw vle::utils::ArgError(
                  _("Fails to convert '%s.%s' parameter '%lu'"
                    " as correct map index"),
                  access.condition.c_str(),
                  access.port.c_str(),
                  static_cast<unsigned long>(i));

            current = it->second.get();
        } else {
            throw vle::utils::ArgError(
              _("Fails to convert '%s.%s' parameter '%lu' as correct "
                "map index"),
              access.condition.c_str(),
              access.port.c_str(),
              static_cast<unsigned long>(i));
        }

        if (not current)
            throw vle::utils::ArgError(_("Fails to convert '%s'"),
                                       access.condition.c_str());
    }

    return current;
}

inline void
ConditionUpdater::update(vle::vpz::Vpz& file) const
{
#ifndef NDEBUG
    {
        printf("condition updaters:\n");
        for (const auto& elem : m_access) {
            printf("* [%s] [%s] [", elem.condition.c_str(), elem.port.c_str());
            for (const auto& str : elem.value_stack) {
                printf("'%s'", str.c_str());
            }
            printf("] [%s]\n", elem.value.c_str());
        }
    }
#endif

    for (auto& elem : m_access) {
        auto* value = get(file, elem);

        if (value == nullptr)
            continue;

        // If @c str is empty, user want to use the default @c
        // vle::value::Value from the origin VPZ file. Otherwise, we
        // convert the value from the string.

        if (elem.value.empty())
            continue;

        switch (value->getType()) {
        case vle::value::Value::BOOLEAN:
            value->toBoolean().value() = vle::utils::to<bool>(elem.value);
            break;

        case vle::value::Value::INTEGER:
            value->toInteger().value() =
              vle::utils::to<std::int32_t>(elem.value);
            break;

        case vle::value::Value::DOUBLE:
            value->toDouble().value() = vle::utils::to<double>(elem.value);
            break;

        case vle::value::Value::STRING:
            value->toString().value() = elem.value;
            break;

        default:
            throw vle::utils::ArgError(
              _("Fails to convert `%s.%s' to complex type"),
              elem.condition.c_str(),
              elem.port.c_str());
            break;
        }
    }
}

inline bool
ConditionUpdater::empty() const noexcept
{
    return m_access.empty();
}

} // namespace vle

#endif // CONDITIONUPDATER_HPP
