/**
 * @file vle/extension/decision/Parser.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/extension/decision/Parser.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Rules,
    (std::vector < vle::extension::decision::Parser::Rule >, rules)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Rule,
    (std::string, id)
    (std::vector < std::string >, predicates)
)


BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Temporal,
    (double, start)
    (double, finish)
    (double, minstart)
    (double, maxstart)
    (double, minfinish)
    (double, maxfinish)
    (int, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Activity,
    (std::string, id)
    (std::vector < std::string >, rules)
    (vle::extension::decision::Parser::Temporal, temporal)
    (std::string, ack)
    (std::string, output)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Activities,
    (std::vector < vle::extension::decision::Parser::Activity >, activities)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Precedence,
    (int, type)
    (std::string, first)
    (std::string, second)
    (double, mintimelag)
    (double, maxtimelag)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Precedences,
    (std::vector < vle::extension::decision::Parser::Precedence >, precedences)
)

BOOST_FUSION_ADAPT_STRUCT(
    vle::extension::decision::Parser::Plan,
    (vle::extension::decision::Parser::Rules, rules)
    (vle::extension::decision::Parser::Activities, activities)
    (vle::extension::decision::Parser::Precedences, precedences)
)

namespace vle { namespace extension { namespace decision {

struct ErrorHandler
{
    template < typename, typename, typename >
        struct result
        {
            typedef void type;
        };

    template < typename Iterator >
        void operator()(const boost::spirit::info& what,
                        Iterator err, Iterator last) const
        {
            std::cout << fmt("Syntax error: %1% in `%2%'\n") % what %
                std::string(err, last) << std::endl;
        }
};

const boost::phoenix::function < ErrorHandler > errorHandler = ErrorHandler();

template < typename Iterator >
struct WhiteSpace :
    boost::spirit::qi::grammar < Iterator >
{
    WhiteSpace()
        : WhiteSpace::base_type(start)
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi::standard_wide;

        start = space | '#' >> *(char_ - '\n')  >> '\n';
    }

    boost::spirit::qi::rule < Iterator > start;
};

template < typename Iterator >
struct PlanGrammar :
    boost::spirit::qi::grammar < Iterator,
    vle::extension::decision::Parser::Plan(), WhiteSpace < Iterator > >
{
    typedef WhiteSpace < Iterator > WhiteSpaceT;

    PlanGrammar()
        : PlanGrammar::base_type(planToken, "plan")
    {
        using boost::spirit::qi::labels::_val;
        using boost::spirit::qi::labels::_a;
        using boost::spirit::qi::labels::_r1;
        using boost::spirit::qi::_1;
        using boost::spirit::qi::lit;
        using boost::spirit::qi::lexeme;
        using boost::spirit::qi::double_;

        using boost::phoenix::push_back;
        using boost::phoenix::at_c;
        using boost::phoenix::ref;

        stringToken = lexeme['"' > +(boost::spirit::ascii::char_ - '"') > '"'];

        ruleToken =
            lit("rule")
            > lit('{')
            > lit("id")
            > lit('=')
            > stringToken[at_c<0>(_val) = _1]
            > lit(';')
            > -(lit("predicates")
            > lit('=')
            > +stringToken[push_back(at_c<1>(_val), _1)]
            > lit(';'))
            > lit('}');

        rulesToken =
            lit("rules")
            > lit('{')
            > *ruleToken[push_back(at_c<0>(_val), _1)]
            > lit('}')
            ;

        temporalToken =
            lit("temporal")
            > lit('{')
            > (
                (lit("start")
                 > lit('=')
                 > double_[at_c<0>(_val) = _1][at_c<6>(_val) = 1]
                 > lit(';')
                 > (
                     (lit("finish")
                      > lit('=')
                      > double_[at_c<1>(_val) = _1][at_c<6>(_val) = 2]
                      > lit(';'))
                     |
                     (lit("minfinish")
                      > lit('=')
                      > double_[at_c<4>(_val) = _1][at_c<6>(_val) = 3]
                      > lit(';')
                      > lit("maxfinish")
                      > lit('=')
                      > double_[at_c<5>(_val) = _1][at_c<6>(_val) = 3]
                      > lit(';'))
                     ))
                |
                (
                    (lit("minstart")
                     > lit('=')
                     > double_[at_c<2>(_val) = _1][at_c<6>(_val) = 4]
                     > lit(';')
                     > lit("maxstart")
                     > lit('=')
                     > double_[at_c<3>(_val) = _1][at_c<6>(_val) = 4]
                     > lit(';'))
                    > (
                        (lit("finish")
                         > lit('=')
                         > double_[at_c<1>(_val) = _1][at_c<6>(_val) = 5]
                         > lit(';'))
                        |
                        (lit("minfinish")
                         > lit('=')
                         > double_[at_c<4>(_val) = _1][at_c<6>(_val) = 6]
                         > lit(';')
                         > lit("maxfinish")
                         > lit('=')
                         > double_[at_c<5>(_val) = _1][at_c<6>(_val) = 6]
                         > lit(';'))))
                         )
                > lit('}');

        activityToken =
            lit("activity")
            > lit('{')
            > lit("id")
            > lit('=')
            > stringToken[at_c<0>(_val) = _1]
            > lit(';')
            > -(lit("rules")
            > lit('=')
            > +(stringToken)[push_back(at_c<1>(_val), _1)]
            > lit(';'))
            > -(temporalToken[at_c<2>(_val) = _1])
            > -(lit("ack")
            > lit('=')
            > stringToken[at_c<3>(_val) = _1]
            > lit(';'))
            > -(lit("output")
            > lit('=')
            > stringToken[at_c<4>(_val) = _1]
            > lit(';'))
            > lit('}');

        activitiesToken =
            lit("activities")
            > lit('{')
            > *activityToken[push_back(at_c<0>(_val), _1)]
            > lit('}');

        precedenceToken =
            lit("precedence")
            > lit('{')
            > lit("type")
            > lit('=')
            > (
                (lit("FS")[at_c<0>(_val) = 1])
                | (lit("SS")[at_c<0>(_val) = 2])
                | (lit("FF")[at_c<0>(_val) = 3])
               )
            > lit(';')
            > lit("first")
            > lit('=')
            > stringToken[at_c<1>(_val) = _1]
            > lit(';')
            > lit("second")
            > lit('=')
            > stringToken[at_c<2>(_val) = _1]
            > lit(';')
            > -(lit("mintimelag")
            > lit('=')
            > double_[at_c<3>(_val) = _1]
            > lit(';'))
            > -(lit("maxtimelag")
            > lit('=')
            > double_[at_c<4>(_val) = _1]
            > lit(';'))
            > lit('}');

        precedencesToken =
            lit("precedences")
            > lit('{')
            > *precedenceToken[push_back(at_c<0>(_val), _1)]
            > lit('}');

        planToken =
            rulesToken[at_c<0>(_val) = _1]
            > activitiesToken[at_c<1>(_val) = _1]
            > precedencesToken[at_c<2>(_val) = _1];

        planToken.name("plan");
        rulesToken.name("rules");
        ruleToken.name("rule");
        activitiesToken.name("activities");
        activityToken.name("activity");
        temporalToken.name("temporal");
        precedencesToken.name("precedences");
        precedenceToken.name("precedence");
        stringToken.name("string");

        using boost::spirit::qi::on_error;
        using boost::spirit::qi::rethrow;
        using boost::spirit::qi::_4;
        using boost::spirit::qi::_3;
        using boost::spirit::qi::_2;

        on_error < rethrow >(planToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(rulesToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(ruleToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(activitiesToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(activityToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(temporalToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(precedencesToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(precedenceToken, errorHandler(_4, _3, _2));
        on_error < rethrow >(stringToken, errorHandler(_4, _3, _2));
    }

    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Plan(), WhiteSpaceT > planToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Rules(), WhiteSpaceT > rulesToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Rule(), WhiteSpaceT > ruleToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Activities(), WhiteSpaceT >
            activitiesToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Activity(), WhiteSpaceT >
            activityToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Temporal(), WhiteSpaceT >
            temporalToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Precedences(), WhiteSpaceT >
            precedencesToken;
    boost::spirit::qi::rule < Iterator,
        vle::extension::decision::Parser::Precedence(), WhiteSpaceT >
            precedenceToken;
    boost::spirit::qi::rule < Iterator, std::string(), WhiteSpaceT >
        stringToken;
};

Parser::Parser(KnowledgeBase& kb, const std::string& buffer) : mKb(kb)
{
    std::string::const_iterator begin = buffer.begin();
    std::string::const_iterator end = buffer.end();
    PlanGrammar < std::string::const_iterator > pl;
    WhiteSpace < std::string::const_iterator > ws;

    Plan plan;

    bool r = boost::spirit::qi::phrase_parse(begin, end, pl, ws, plan);
    if (not (r and begin == end)) {
        throw utils::ArgError("Decision: error parsing plan");
    }

    try {
        fillKnowledegeBase(plan);
    } catch (const std::exception& e) {
        throw utils::ArgError(
            fmt(_("Decision: error fill KnowledgeBase from plan: %1%")) %
            e.what());;
    }
}

Parser::Parser(KnowledgeBase& kb, std::istream& /*stream*/) : mKb(kb)
{
    throw utils::NotYetImplemented("Parser::Parser(std::istream& stream");
    // FIXME found a solution
    //stream.unsetf(std::ios::skipws);

    //typedef std::istreambuf_iterator < char > Iterator;
    //boost::spirit::multi_pass < Iterator > begin =
        //boost::spirit::make_default_multi_pass(Iterator(stream));
    //boost::spirit::multi_pass < Iterator > end =
        //boost::spirit::make_default_multi_pass(Iterator());
    ////PlanGrammar < boost::spirit::multi_pass < Iterator > > pl;
    ////WhiteSpace < boost::spirit::multi_pass < Iterator > > ws;
    //PlanGrammar < Iterator > pl;
    //WhiteSpace < Iterator > ws;

    //Plan plan;

    //bool r = boost::spirit::qi::phrase_parse(begin, end, pl, ws, plan);
    //if (not (r and begin == end)) {
        //throw utils::ArgError("Decision: error parsing plan");
    //}

    //try {
        //fillKnowledegeBase(plan);
    //} catch (const std::exception& e) {
        //throw utils::ArgError("Decision: error fill KnowledgeBase from plan");
    //}
}

void Parser::fillKnowledegeBase(const Plan& plan)
{
    std::for_each(plan.rules.rules.begin(), plan.rules.rules.end(),
                  AddRule(mKb));

    std::for_each(plan.activities.activities.begin(),
                  plan.activities.activities.end(),
                  AddActivity(mKb));

    std::for_each(plan.precedences.precedences.begin(),
                  plan.precedences.precedences.end(),
                  AddPrecedence(mKb));
}

}}} // namespace vle model decision
