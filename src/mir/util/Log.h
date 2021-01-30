/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_util_Log_h
#define mir_util_Log_h

#include "eckit/log/Log.h"


namespace mir {


struct Log final : protected eckit::Log {
    using eckit::Log::error;
    using eckit::Log::info;
    using eckit::Log::warning;


    using Channel = decltype(Log::debug());
    static Channel& debug();


    using eckit::Log::applicationFormat;
    using eckit::Log::syserr;


    struct Plural {
        Plural() = default;
        Plural(std::string one) : Plural(one, one + "s") {}
        Plural(std::string one, std::string notOne) : s_{one, notOne} {}
        Plural(const Plural& other) : s_{other.s_[0], other.s_[1]} {}

        const std::string& operator()(int count) const { return s_[count != 1]; }
        const std::string& operator()(size_t count) const { return s_[count != 1]; }

        operator bool() const { return !s_[0].empty(); }

        Plural& operator=(const Plural& other) {
            s_[0] = other.s_[0];
            s_[1] = other.s_[1];
            return *this;
        }

    private:
        std::string s_[2];
    };


    struct Pretty {
        Pretty(int count, const Plural& plural = Plural()) : plural_(plural), count_(count) {}
        Pretty(long count, const Plural& plural = Plural()) : plural_(plural), count_(static_cast<int>(count)) {}
        Pretty(size_t count, const Plural& plural = Plural()) : plural_(plural), count_(static_cast<int>(count)) {}

        Pretty& operator=(const Pretty&) = delete;
        Pretty(const Pretty&)            = delete;

    private:
        const Plural& plural_;
        int count_;

        void print(std::ostream&) const;

        friend std::ostream& operator<<(std::ostream& s, const Pretty& p) {
            p.print(s);
            return s;
        }
    };
};


}  // namespace mir


#endif
