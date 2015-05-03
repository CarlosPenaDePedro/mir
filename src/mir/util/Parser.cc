/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Pedro Maciel
/// @date Apr 2015


#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdFile.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"
#include "eckit/parser/StringTools.h"

#include "mir/util/Parser.h"

// TODO: Cache bitmaps


namespace mir {
namespace util {


Parser::Parser(const eckit::PathName& path):
    path_(path),
    in_(std::string(path)),
    parser_(in_) {

    // in_.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

}


Parser::~Parser() {
    in_.close();
}

// TODO: Some lex/yacc, I am not pround of that
void Parser::fill(ParserConsumer& consumer) {
    char c;
    std::string name;
    std::string tmp;
    double dvalue = 0;
    long lvalue = 0;
    bool key = true;
    bool value = false;
    bool word = false;

    for (;;) {
        c = peek();

        // std::cout << "->" << c << std::endl;
        switch (c) {
        case '#':
            consumeComment();
            break;

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (key || word) {
                tmp += next();

            } else {
                if (value) {
                    if (readNumber(lvalue, dvalue)) {
                        consumer.set(name, dvalue);
                    } else {
                        consumer.set(name, lvalue);
                    }
                } else {
                    throw eckit::SeriousBug("Parser: no current name");
                }
                key = true;
                value = false;
                name = "";
                tmp = "";
                word = false;
            }
            break;

        case ':':
            consumer.scope(eckit::StringTools::trim(tmp));
            tmp = ""; word = false;
            next();
            break;

        case '=':
            name = tmp;
            tmp = ""; word = false;
            next();
            key = false;
            value = true;
            break;

        case ' ':
        case '\t':
            if (key) {
                next();
            } else {
                tmp += next();
            }
            break;

        case 0:
        case '\n':
        case '\r':

            if (value) {
                tmp = eckit::StringTools::trim(tmp);
                if (tmp == "false" || tmp == "true") {
                    consumer.set(name, tmp[0] == 't');
                } else {
                    consumer.set(name, eckit::StringTools::trim(tmp));
                }
            }
            word = false;
            key = true;
            value = false;
            name = "";
            tmp = ""; word = false;
            if (c == 0) {
                return;
            }
            next();
            break;

        default:
            if (isalpha(c) || (c == '_') || (c == '.') || (c == '-'))  {
                c = next();
                tmp += c;
                word = true;
                // std::cout << tmp << std::endl;
            } else {
                eckit::StrStream os;
                os << path_ << ": unexpected character " << (isprint(c) ? c : int(c)) << eckit::StrStream::ends;
                throw eckit::SeriousBug(os);
            }
            break;
        }
    }
}

void Parser::consumeComment() {
    char c;
    // std::cout << "Parser::consumeComment" << std::endl;
    while ( (c = next()) != 0) {
        if (c == '\n' || c == '\r') {
            break;
        }
    }
}

bool Parser::readNumber(long& lvalue, double& dvalue) {
    // std::cout << "Parser::readNumber" << std::endl;
    bool real = false;
    std::string s;
    char c = next();
    if (c == '-') {
        s += c;
        c = next();
    }

    switch (c) {
    case '0': s += c; break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        s += c;
        while (isdigit(peek())) {
            s += next();
        }
        break;
    default:
        throw eckit::SeriousBug(std::string("Parser::readNumber invalid char '") + c + "'");
        break;
    }

    if (peek() == '.') {
        real = true;
        s += next();
        c = next();
        if (!isdigit(c))
            throw eckit::SeriousBug(std::string("Parser::readNumber invalid char '") + c + "'");
        s += c;
        while (isdigit(peek())) {
            s += next();
        }
    }


    c = peek();
    if (c == 'e' || c == 'E') {
        real = true;
        s += next();

        c = next();
        if (c == '-' || c == '+') {
            s += c;
            c = next();
        }

        if (!isdigit(c))
            throw eckit::SeriousBug(std::string("Parser::readNumber invalid char '") + c + "'");
        s += c;
        while (isdigit(peek())) {
            s += next();
        }

    }

    if (real) {
        dvalue = eckit::Translator<std::string, double>()(s);
        // std::cout << "real " << dvalue << std::endl;
        return true;
    } else {
        lvalue = eckit::Translator<std::string, long>()(s);
        // std::cout << "int " << lvalue << std::endl;
        return false;
    }
}

char Parser::peek() {
    return parser_.peek(true);
}


char Parser::next() {
    return parser_.next(true);
}

void Parser::print(std::ostream &out) const {
    out << "Parser[path=" << path_ << "]";
}


}  // namespace util
}  // namespace mir

