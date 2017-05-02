/* gshell.c - Shell-related utilities
 *
 *  Copyright 2000 Red Hat, Inc.
 *  g_execvpe implementation based on GNU libc execvp:
 *   Copyright 1991, 92, 95, 96, 97, 98, 99 Free Software Foundation, Inc.
 *
 * GLib is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * GLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GLib; see the file COPYING.LIB.  If not, write
 * to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

/**
 * SECTION:shell
 * @title: Shell-related Utilities
 * @short_description: shell-like commandline handling
 *
 * GLib provides the functions g_shell_quote() and g_shell_unquote()
 * to handle shell-like quoting in strings. The function g_shell_parse_argv()
 * parses a string similar to the way a POSIX shell (/bin/sh) would.
 *
 * Note that string handling in shells has many obscure and historical
 * corner-cases which these functions do not necessarily reproduce. They
 * are good enough in practice, though.
 */

/**
 * G_SHELL_ERROR:
 *
 * Error domain for shell functions. Errors in this domain will be from
 * the #GShellError enumeration. See #GError for information on error
 * domains.
 **/

/**
 * GShellError:
 * @G_SHELL_ERROR_BAD_QUOTING: Mismatched or otherwise mangled quoting.
 * @G_SHELL_ERROR_EMPTY_STRING: String to be parsed was empty.
 * @G_SHELL_ERROR_FAILED: Some other error.
 *
 * Error codes returned by shell functions.
 **/

namespace vle {
namespace utils {
namespace details {

class ShellUtils
{
public:
    ShellUtils(){};
    ~ShellUtils(){};

    /* Single quotes preserve the literal string exactly. escape
     * sequences are not allowed; not even \' - if you want a '
     * in the quoted text, you have to do something like 'foo'\''bar'
     *
     * Double quotes allow $ ` " \ and newline to be escaped with backslash.
     * Otherwise double quotes preserve things literally.
     */
    std::pair<std::string, std::string::iterator> unquote_string_inplace(
      std::string::iterator it,
      std::string::iterator end)
    {
        std::string retval;
        char quote_char = *it;

        if (not(quote_char == '"' or quote_char == '\'')) {
            throw("Quoted text doesn't begin with a quotation mark");
        }

        ++it;
        if (quote_char == '"') {
            while (it != end) {
                switch (*it) {
                case '"':
                    /* End of the string, return now */
                    it++;
                    return std::make_pair(retval, it);
                    break;
                case '\\':
                    /* Possible escaped quote or \ */
                    it++;
                    switch (*it) {
                    case '"':
                    case '\\':
                    case '`':
                    case '$':
                    case '\n':
                        retval += *it;
                        it++;
                        break;
                    default:
                        /* not an escaped char */
                        retval += '\\';
                        break;
                    }
                    break;
                default:
                    retval += *it;
                    it++;
                    break;
                }
            }
        } else {
            while (it != end) {
                if (*it == '\'') {
                    /* End of the string, return now */
                    it++;
                    return std::make_pair(retval, it);
                } else {
                    retval += *it;
                    it++;
                }
            }
        }

        /* If we reach here this means the close quote was never
         * encountered */
        throw vle::utils::CastError(
          boost::str(vle::fmt("Unmatched quotation mark in command line "
                              "or other shell-quoted text: %1%") %
                     retval));
    }

    /**
     * g_shell_quote:
     * @unquoted_string: a literal string
     *
     * Quotes a string so that the shell (/bin/sh) will interpret the
     * quoted string to mean @unquoted_string. If you pass a filename to
     * the shell, for example, you should first quote it with this
     * function.  The return value must be freed with g_free(). The
     * quoting style used is undefined (single or double quotes may be
     * used).
     *
     * Returns: quoted string
     **/
    std::string g_shell_quote(const std::string& unquoted_string)
    {
        /* We always use single quotes, because the algorithm is cheesier.
         * We could use double if we felt like it, that might be more
         * human-readable.
         */

        std::string p = unquoted_string;
        std::string dest = "'";

        /* could speed this up a lot by appending chunks of text at a
         * time.
         */
        std::string::const_iterator itb = p.begin();
        while (itb != p.end()) {
            /* Replace literal ' with a close ', a \', and a open ' */
            if (*itb == '\'') {
                dest += "'\\''";
            } else {
                dest += *itb;
            }
            itb++;
        }
        dest += '\'';
        return dest;
    }

    /**
     * g_shell_unquote:
     * @quoted_string: shell-quoted string
     * @error: error return location or NULL
     *
     * Unquotes a string as the shell (/bin/sh) would. Only handles
     * quotes; if a string contains file globs, arithmetic operators,
     * variables, backticks, redirections, or other special-to-the-shell
     * features, the result will be different from the result a real shell
     * would produce (the variables, backticks, etc. will be passed
     * through literally instead of being expanded). This function is
     * guaranteed to succeed if applied to the result of
     * g_shell_quote(). If it fails, it returns %NULL and sets the
     * error. The @quoted_string need not actually contain quoted or
     * escaped text; g_shell_unquote() simply goes through the string and
     * unquotes/unescapes anything that the shell would. Both single and
     * double quotes are handled, as are escapes including escaped
     * newlines. The return value must be freed with g_free(). Possible
     * errors are in the #G_SHELL_ERROR domain.
     *
     * Shell quoting rules are a bit strange. Single quotes preserve the
     * literal string exactly. escape sequences are not allowed; not even
     * \' - if you want a ' in the quoted text, you have to do something
     * like 'foo'\''bar'.  Double quotes allow $, `, ", \, and newline to
     * be escaped with backslash. Otherwise double quotes preserve things
     * literally.
     *
     * Returns: an unquoted string
     **/
    std::string g_shell_unquote(std::string& quoted_string)
    {
        std::string unquoted = quoted_string;
        std::string retval;

        std::string::iterator start = quoted_string.begin();

        /* The loop allows cases such as
         * "foo"blah blah'bar'woo foo"baz"la la la\'\''foo'
         */
        while (start != quoted_string.end()) {
            /* Append all non-quoted chars, honoring backslash escape
             */

            while ((start != quoted_string.end()) &&
                   not(*start == '"' or *start == '\'')) {
                if (*start == '\\') {
                    /* all characters can get escaped by backslash,
                     * except newline, which is removed if it follows
                     * a backslash outside of quotes
                     */
                    start++;
                    if (start != quoted_string.end()) {
                        if (*start != '\n') {
                            retval += *start;
                        }
                        start++;
                    }
                } else {
                    retval += *start;
                    start++;
                }
            }
            if (start != quoted_string.end()) {
                std::pair<std::string, std::string::iterator> res =
                  unquote_string_inplace(start, quoted_string.end());

                retval += res.first;
                start = res.second;
            }
        }
        return retval;
    }

    std::vector<std::string> tokenize_command_line(
      std::vector<std::string>& retval,
      const std::string& cmd)
    {
        char current_quote = '\0';
        std::string::const_iterator p = cmd.begin();
        std::string current_token;
        bool current_token_exist = false;
        bool quoted = false;

        while (p != cmd.end()) {
            if (current_quote == '\\') {
                if (*p == '\n') {
                    /* we append nothing; backslash-newline become nothing */
                } else {
                    /* we append the backslash and the current char,
                     * to be interpreted later after tokenization
                     */
                    current_token_exist = true;
                    current_token += '\\';
                    current_token += *p;
                }
                current_quote = '\0';
            } else if (current_quote == '#') {
                /* Discard up to and including next newline */
                while (*p and (*p != '\n')) {
                    ++p;
                }
                current_quote = '\0';

                if (*p == '\0') {
                    break;
                }
            } else if (current_quote) {
                if ((*p == current_quote) and
                    /* check that it isn't an escaped double quote */
                    not(current_quote == '"' and quoted)) {
                    /* close the quote */
                    current_token_exist = true;
                    current_quote = '\0';
                }
                /* Everything inside quotes, and the close quote,
                 * gets appended literally.
                 */
                current_token += *p;
            } else {
                switch (*p) {
                case '\n':
                    retval.push_back(current_token);
                    current_token = "";
                    current_token_exist = false;
                    break;
                case ' ':
                case '\t':
                    /* If the current token contains the previous char, delimit
                     * the current token. A nonzero length
                     * token should always contain the previous char.
                     */
                    if ((current_token != "") and current_token.size() > 0) {
                        retval.push_back(current_token);
                        current_token = "";
                        current_token_exist = false;
                    }
                    /* discard all unquoted blanks (don't add them to a token)
                     */
                    break;
                /* single/double quotes are appended to the token,
                 * escapes are maybe appended next time through the loop,
                 * comment chars are never appended.
                 */

                case '\'':
                case '"':
                    current_token_exist = true;
                    current_token += *p;
                    current_quote = *p;
                    break;
                case '\\':
                    current_quote = *p;
                    break;
                case '#':
                    if (p == cmd.begin()) {
                        /* '#' was the first char */
                        current_quote = *p;
                        break;
                    }
                    switch (*(p - 1)) {
                    case ' ':
                    case '\n':
                    case '\0':
                        current_quote = *p;
                        break;
                    default:
                        current_token_exist = true;
                        current_token += *p;
                        break;
                    }
                    break;

                default:
                    /* Combines rules 4) and 6) - if we have a token, append to
                     * it,
                     * otherwise create a new token.
                     */
                    current_token_exist = true;
                    current_token += *p;
                    break;
                }
            }
            /* We need to count consecutive backslashes mod 2,
             * to detect escaped doublequotes.
             */
            if (*p != '\\') {
                quoted = false;
            } else {
                quoted = !quoted;
            }
            ++p;
        }
        if (current_token_exist) {
            retval.push_back(current_token);
            current_token = "";
            //
            // current_token_exist is never read after.
            //
            // current_token_exist = false;
        }

        if (current_quote) {
            if (current_quote == '\\') {
                throw vle::utils::CastError(
                  _("Text ended just after a '\\' character."));
            } else {
                throw vle::utils::CastError(
                  _("Text ended before matching quote was found"));
            }
            throw vle::utils::CastError("error");
        }

        if (retval.empty()) {
            throw vle::utils::CastError(
              _("Text was empty (or contained only whitespace)"));
        }

        /* we appended backward */
        // retval = retval.reverse; //TODO

        return retval;
    }

    /**
     * g_shell_parse_argv:
     * @command_line: command line to parse
     * @argcp: (out) (optional): return location for number of args, or %NULL
     * @argvp: (out) (optional) (array length=argcp zero-terminated=1): return
     *   location for array of args, or %NULL
     * @error: (optional): return location for error, or %NULL
     *
     * Parses a command line into an argument vector, in much the same way
     * the shell would, but without many of the expansions the shell would
     * perform (variable expansion, globs, operators, filename expansion,
     * etc. are not supported). The results are defined to be the same as
     * those you would get from a UNIX98 /bin/sh, as long as the input
     * contains none of the unsupported shell expansions. If the input
     * does contain such expansions, they are passed through
     * literally. Possible errors are those from the #G_SHELL_ERROR
     * domain. Free the returned vector with g_strfreev().
     *
     * Returns: %TRUE on success, %FALSE if error set
     **/
    bool g_shell_parse_argv(const std::string& cmd,
                            int& argcp,
                            std::vector<std::string>& argvp)
    {
        /* Code based on poptParseArgvString() from libpopt */
        std::vector<std::string> tokens;

        tokenize_command_line(tokens, cmd);
        if (tokens.empty()) {
            return false;
        }

        /* Because we can't have introduced any new blank space into the
         * tokens (we didn't do any new expansions), we don't need to
         * perform field splitting. If we were going to honor IFS or do any
         * expansions, we would have to do field splitting on each word
         * here. Also, if we were going to do any expansion we would need to
         * remove any zero-length words that didn't contain quotes
         * originally; but since there's no expansion we know all words have
         * nonzero length, unless they contain quotes.
         *
         * So, we simply remove quotes, and don't do any field splitting or
         * empty word removal, since we know there was no way to introduce
         * such things.
         */

        argcp = tokens.size();
        argvp.resize(argcp);
        for (unsigned int i = 0; i < tokens.size(); i++) {
            argvp[i] = g_shell_unquote(tokens[i]);
        }
        return true;
    }
};
}
}
} // namespaces
