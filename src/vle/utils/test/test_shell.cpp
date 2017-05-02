/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vle/utils/details/ShellUtils.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vle.hpp>

void
g_shell()
{
    // tests taken from glib-2.47.3 (shell.c)

    vle::utils::details::ShellUtils su;
    std::string res, input;

    //    { "", "''" },
    //    { "a", "'a'" },
    //    { "(", "'('" },
    //    { "'", "''\\'''" },
    //    { "'a", "''\\''a'" },
    //    { "a'", "'a'\\'''" },
    //    { "a'a", "'a'\\''a'" }

    input = "";
    res = su.g_shell_quote(input);
    Ensures((res == "''"));

    input = "a";
    res = su.g_shell_quote(input);
    Ensures((res == "'a'"));

    input = "(";
    res = su.g_shell_quote(input);
    Ensures((res == "'('"));

    input = "'";
    res = su.g_shell_quote(input);
    Ensures((res == "''\\'''"));

    input = "'a";
    res = su.g_shell_quote(input);
    Ensures((res == "''\\''a'"));

    input = "a'";
    res = su.g_shell_quote(input);
    Ensures((res == "'a'\\'''"));

    input = "a'a";
    res = su.g_shell_quote(input);
    Ensures((res == "'a'\\''a'"));

    //    { "", "", -1 },
    //    { "a", "a", -1 },
    //    { "'a'", "a", -1 },
    //    { "'('", "(", -1 },
    //    { "''\\'''", "'", -1 },
    //    { "''\\''a'", "'a", -1 },
    //    { "'a'\\'''", "a'", -1 },
    //    { "'a'\\''a'", "a'a", -1 },
    //    { "\\\\", "\\", -1 },
    //    { "\\\n", "", -1 },
    //    { "'\\''", NULL, G_SHELL_ERROR_BAD_QUOTING },
    //    { "\"\\\"\"", "\"", -1 },
    //    { "\"", NULL, G_SHELL_ERROR_BAD_QUOTING },
    //    { "'", NULL, G_SHELL_ERROR_BAD_QUOTING },
    //    { "\x22\\\\\"", "\\", -1 },
    //    { "\x22\\`\"", "`", -1 },
    //    { "\x22\\$\"", "$", -1 },
    //    { "\x22\\\n\"", "\n", -1 },
    //    { "\"\\'\"", "\\'", -1 },
    //    { "\x22\\\r\"", "\\\r", -1 },
    //    { "\x22\\n\"", "\\n", -1 }

    input = "";
    res = su.g_shell_unquote(input);
    Ensures((res == ""));

    input = "a";
    res = su.g_shell_unquote(input);
    Ensures((res == "a"));

    input = "'a'";
    res = su.g_shell_unquote(input);
    Ensures((res == "a"));

    input = "'('";
    res = su.g_shell_unquote(input);
    Ensures((res == "("));

    input = "''\\'''";
    res = su.g_shell_unquote(input);
    Ensures((res == "'"));

    input = "''\\''a'";
    res = su.g_shell_unquote(input);
    Ensures((res == "'a"));

    input = "'a'\\'''";
    res = su.g_shell_unquote(input);
    Ensures((res == "a'"));

    input = "'a'\\''a'";
    res = su.g_shell_unquote(input);
    Ensures((res == "a'a"));

    input = "\\\\";
    res = su.g_shell_unquote(input);
    Ensures((res == "\\"));

    input = "\\\n";
    res = su.g_shell_unquote(input);
    Ensures((res == ""));

    input = "'\\''";
    EnsuresThrow(su.g_shell_unquote(input), vle::utils::CastError);

    input = "\"\\\"\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\""));

    input = "\"";
    EnsuresThrow(su.g_shell_unquote(input), vle::utils::CastError);

    input = "'";
    EnsuresThrow(su.g_shell_unquote(input), vle::utils::CastError);

    input = "\x22\\\\\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\\"));

    input = "\x22\\`\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "`"));

    input = "\x22\\$\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "$"));

    input = "\x22\\\n\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\n"));

    input = "\"\\'\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\\'"));

    input = "\x22\\\r\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\\\r"));

    input = "\x22\\n\"";
    res = su.g_shell_unquote(input);
    Ensures((res == "\\n"));

    //    { "foo bar", 2, { "foo", "bar", NULL }, -1 },
    //      { "foo 'bar'", 2, { "foo", "bar", NULL }, -1 },
    //      { "foo \"bar\"", 2, { "foo", "bar", NULL }, -1 },
    //      { "foo '' 'bar'", 3, { "foo", "", "bar", NULL }, -1 },
    //      { "foo \"bar\"'baz'blah'foo'\\''blah'\"boo\"", 2, { "foo",
    //      "barbazblahfoo'blahboo", NULL }, -1 },
    //      { "foo \t \tblah\tfoo\t\tbar  baz", 5, { "foo", "blah", "foo",
    //      "bar", "baz", NULL }, -1 },
    //      { "foo '    spaces more spaces lots of     spaces in this   '  \t",
    //      2, { "foo", "    spaces more spaces lots of     spaces in this   ",
    //      NULL }, -1 },
    //      { "foo \\\nbar", 2, { "foo", "bar", NULL }, -1 },
    //      { "foo '' ''", 3, { "foo", "", "", NULL }, -1 },
    //      { "foo \\\" la la la", 5, { "foo", "\"", "la", "la", "la", NULL },
    //      -1 },
    //      { "foo \\ foo woo woo\\ ", 4, { "foo", " foo", "woo", "woo ", NULL
    //      }, -1 },
    //      { "foo \"yada yada \\$\\\"\"", 2, { "foo", "yada yada $\"", NULL },
    //      -1 },
    //      { "foo \"c:\\\\\"", 2, { "foo", "c:\\", NULL }, -1 },
    //      { "foo # bla bla bla\n bar", 2, { "foo", "bar", NULL }, -1 },
    //      { "foo a#b", 2, { "foo", "a#b", NULL }, -1 },
    //      { "#foo", 0, { NULL }, G_SHELL_ERROR_EMPTY_STRING },
    //      { "foo bar \\", 0, { NULL }, G_SHELL_ERROR_BAD_QUOTING },
    //      { "foo 'bar baz", 0, { NULL }, G_SHELL_ERROR_BAD_QUOTING },
    //      { "foo '\"bar\" baz", 0, { NULL }, G_SHELL_ERROR_BAD_QUOTING },
    //      { "", 0, { NULL }, G_SHELL_ERROR_EMPTY_STRING },
    //      { "  ", 0, { NULL }, G_SHELL_ERROR_EMPTY_STRING },
    //      { "# foo bar", 0, { NULL }, G_SHELL_ERROR_EMPTY_STRING },
    //      {"foo '/bar/summer'\\''09 tours.pdf'", 2, {"foo", "/bar/summer'09
    //      tours.pdf", NULL}, -1}

    int argc = 0;
    std::vector<std::string> argvp;

    input = "foo bar";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "bar"));

    input = "foo 'bar'";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "bar"));

    input = "foo \"bar\"";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "bar"));

    input = "foo '' 'bar'";
    su.g_shell_parse_argv(input, argc, argvp); // dbg
    EnsuresEqual(argc, 3);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == ""));
    res = argvp[2];
    Ensures((res == "bar"));

    input = "foo \"bar\"'baz'blah'foo'\\''blah'\"boo\"";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "barbazblahfoo'blahboo"));

    input = "foo \t \tblah\tfoo\t\tbar  baz";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 5);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "blah"));
    res = argvp[2];
    Ensures((res == "foo"));
    res = argvp[3];
    Ensures((res == "bar"));
    res = argvp[4];
    Ensures((res == "baz"));

    input = "foo '    spaces more spaces lots of     spaces in this   '  \t";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "    spaces more spaces lots of     spaces in this   "));

    input = "foo \\\nbar";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "bar"));

    input = "foo '' ''";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 3);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == ""));
    res = argvp[2];
    Ensures((res == ""));

    input = "foo \\\" la la la";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 5);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "\""));
    res = argvp[2];
    Ensures((res == "la"));
    res = argvp[3];
    Ensures((res == "la"));
    res = argvp[4];
    Ensures((res == "la"));

    input = "foo \\ foo woo woo\\ ";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 4);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == " foo"));
    res = argvp[2];
    Ensures((res == "woo"));
    res = argvp[3];
    Ensures((res == "woo "));

    input = "foo \"yada yada \\$\\\"\"";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "yada yada $\""));

    input = "foo \"c:\\\\\"";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "c:\\"));

    input = "foo # bla bla bla\n bar";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "bar"));

    input = "foo a#b";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "a#b"));

    input = "#foo";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "foo bar \\";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "foo 'bar baz";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "foo '\"bar\" baz";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "  ";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "# foo bar";
    EnsuresThrow(su.g_shell_parse_argv(input, argc, argvp),
                 vle::utils::CastError);

    input = "foo '/bar/summer'\\''09 tours.pdf'";
    su.g_shell_parse_argv(input, argc, argvp);
    EnsuresEqual(argc, 2);
    res = argvp[0];
    Ensures((res == "foo"));
    res = argvp[1];
    Ensures((res == "/bar/summer'09 tours.pdf"));
}

int
main()
{
    vle::Init app;

    g_shell();

    return unit_test::report_errors();
}
