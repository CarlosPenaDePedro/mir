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


#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Triplet.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/FactoryOption.h"
#include "eckit/option/SimpleOption.h"

#include "mir/repres/Representation.h"
#include "mir/repres/proxy/HEALPix.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Mutex.h"


namespace mir::tools {


using Renumber = std::vector<size_t>;

static util::once_flag ONCE;
static util::recursive_mutex* MUTEX = nullptr;

class ReorderFactory;
static std::map<std::string, ReorderFactory*>* M = nullptr;


static void init() {
    MUTEX = new util::recursive_mutex();
    M     = new std::map<std::string, ReorderFactory*>();
}


struct Reorder {
    Reorder() = default;

    virtual ~Reorder() = default;

    virtual Renumber reorder() = 0;

    Reorder(const Reorder&)            = delete;
    Reorder(Reorder&&)                 = delete;
    Reorder& operator=(const Reorder&) = delete;
    Reorder& operator=(Reorder&&)      = delete;
};


class ReorderFactory {
private:
    std::string name_;
    virtual Reorder* make(size_t N) = 0;

protected:
    explicit ReorderFactory(const std::string& name) : name_(name) {
        util::call_once(ONCE, init);
        util::lock_guard<util::recursive_mutex> lock(*MUTEX);

        if (M->find(name) == M->end()) {
            (*M)[name] = this;
            return;
        }

        throw exception::SeriousBug("ReorderFactory: duplicated Reorder '" + name + "'");
    }

    virtual ~ReorderFactory() {
        util::lock_guard<util::recursive_mutex> lock(*MUTEX);
        M->erase(name_);
    }

public:
    ReorderFactory(const ReorderFactory&)            = delete;
    ReorderFactory(ReorderFactory&&)                 = delete;
    ReorderFactory& operator=(const ReorderFactory&) = delete;
    ReorderFactory& operator=(ReorderFactory&&)      = delete;

    static Reorder* build(const std::string& name, size_t N) {
        util::call_once(ONCE, init);
        util::lock_guard<util::recursive_mutex> lock(*MUTEX);

        if (auto j = M->find(name); j != M->end()) {
            return j->second->make(N);
        }

        list(Log::error() << "ReorderFactory: unknown '" << name << "', choices are:\n") << std::endl;
        throw exception::SeriousBug("ReorderFactory: unknown '" + name + "'");
    }

    static std::ostream& list(std::ostream& out) {
        util::call_once(ONCE, init);
        util::lock_guard<util::recursive_mutex> lock(*MUTEX);

        const auto* sep = "";
        for (const auto& j : *M) {
            out << sep << j.first;
            sep = ", ";
        }

        return out;
    }
};


template <class T>
class ReorderBuilder : public ReorderFactory {
    Reorder* make(size_t N) override { return new T(N); }

public:
    explicit ReorderBuilder(const std::string& name) : ReorderFactory(name) {}
};


struct Identity final : Reorder {
    explicit Identity(size_t N) : N_(N) {}

private:
    Renumber reorder() override {
        Renumber renumber(N_);
        std::iota(renumber.begin(), renumber.end(), 0);
        return renumber;
    }

    const size_t N_;
};


struct HEALPix : Reorder {
    explicit HEALPix(size_t N) :
        N_(N),
        Nside_([N] {
            auto Nside = static_cast<size_t>(std::sqrt(static_cast<double>(N) / 12.));
            ASSERT_MSG(12 * Nside * Nside == N, "Expected N = 12 * Nside ** 2, got N=" + std::to_string(N));
            return Nside;
        }()),
        healpix_(static_cast<int>(Nside_)) {}

    size_t N() const { return N_; }
    size_t Nside() const { return Nside_; }
    const repres::proxy::HEALPix::Reorder& healpix() const { return healpix_; }

private:
    const size_t N_;
    const size_t Nside_;
    const repres::proxy::HEALPix::Reorder healpix_;
};


struct HEALPixRingToNested final : HEALPix {
    explicit HEALPixRingToNested(size_t N) : HEALPix(N) {}
    Renumber reorder() override {
        Renumber map(N());
        for (size_t i = 0; i < N(); ++i) {
            map[i] = static_cast<size_t>(healpix().ring_to_nest(static_cast<int>(i)));
        }
        return map;
    }
};


struct HEALPixNestedToRing final : HEALPix {
    explicit HEALPixNestedToRing(size_t N) : HEALPix(N) {}
    Renumber reorder() override {
        Renumber map(N());
        for (size_t i = 0; i < N(); ++i) {
            map[i] = static_cast<size_t>(healpix().nest_to_ring(static_cast<int>(i)));
        }
        return map;
    }
};


static const std::string IDENTITY = "identity";

static const ReorderBuilder<Identity> __reorder1(IDENTITY);
static const ReorderBuilder<HEALPixRingToNested> __reorder2("healpix-ring-to-nested");
static const ReorderBuilder<HEALPixNestedToRing> __reorder3("healpix-nested-to-ring");


struct MIRMatrixReorder : MIRTool {
    MIRMatrixReorder(int argc, char** argv) : MIRTool(argc, argv) {
        using eckit::option::FactoryOption;
        using eckit::option::SimpleOption;
        options_.push_back(new FactoryOption<ReorderFactory>("reorder-rows", "Reordering rows method", IDENTITY));
        options_.push_back(new FactoryOption<ReorderFactory>("reorder-cols", "Reordering columns method", IDENTITY));
        options_.push_back(new SimpleOption<bool>("transpose", "Transpose matrix", false));
    }

    int numberOfPositionalArguments() const override { return 2; }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool
                    << " [--reorder-rows=...]"
                       " [--reorder-cols=...]"
                       " [--transpose=[0|1]]"
                       " input-matrix output-matrix"
                    << std::endl;
    }

    void execute(const eckit::option::CmdArgs&) override;
};


void MIRMatrixReorder::execute(const eckit::option::CmdArgs& args) {
    // load input matrix
    eckit::linalg::SparseMatrix M;
    M.load(args(0));


    // renumbering maps
    auto rows = std::unique_ptr<Reorder>(ReorderFactory::build(args.getString("reorder-rows"), M.rows()))->reorder();
    ASSERT(rows.size() == M.rows());

    auto cols = std::unique_ptr<Reorder>(ReorderFactory::build(args.getString("reorder-cols"), M.cols()))->reorder();
    ASSERT(cols.size() == M.cols());


    // expand triplets, renumbering directly
    std::vector<eckit::linalg::Triplet> trips;
    trips.reserve(M.nonZeros());

    auto transpose = args.getBool("transpose");
    for (auto i = M.begin(), end = M.end(); i != end; ++i) {
        if (transpose) {
            trips.emplace_back(cols.at(i.col()), rows.at(i.row()), *i);
        }
        else {
            trips.emplace_back(rows.at(i.row()), cols.at(i.col()), *i);
        }
    }


    // compress triplets, create output matrix
    std::sort(trips.begin(), trips.end());
    eckit::linalg::SparseMatrix W(M.rows(), M.cols(), trips);


    // create output matrix
    W.save(args(1));
}


}  // namespace mir::tools


int main(int argc, char** argv) {
    mir::tools::MIRMatrixReorder tool(argc, argv);
    return tool.start();
}
