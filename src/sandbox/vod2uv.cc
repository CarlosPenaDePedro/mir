/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "eckit/runtime/Tool.h"
#include "eckit/runtime/Context.h"
#include "eckit/parser/Tokenizer.h"

#include "mir/api/MIRJob.h"
#include "mir/action/VOD2UVTransform.h"
#include "mir/action/Sh2ShTransform.h"

#include "mir/input/GribFileInput.h"
#include "mir/output/GribFileOutput.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Representation.h"


class VOD2UVTool : public eckit::Tool {

    virtual void run();

    void usage(const std::string& tool);

  public:
    VOD2UVTool(int argc, char **argv) :
        eckit::Tool(argc, argv) {
    }

};


void VOD2UVTool::run() {

    mir::data::MIRField field(false, 0);
    mir::api::MIRJob job;

    mir::input::GribFileInput vo("/tmp/vo.grib");
    mir::input::GribFileInput d("/tmp/d.grib");

    mir::output::GribFileOutput u("/tmp/u.grib");
    mir::output::GribFileOutput v("/tmp/v.grib");

    vo.next();
    d.next();

    mir::input::MIRInput& mvo = vo;
    mir::input::MIRInput& md = d;
    mir::output::MIROutput& mu = u;
    mir::output::MIROutput& mv = v;

    std::unique_ptr<mir::data::MIRField> vof(mvo.field());
    std::unique_ptr<mir::data::MIRField> df(md.field());

    field.values(vof->values(), 0);
    field.values(df->values(), 1);


    const mir::param::MIRParametrisation& metadata = mvo.parametrisation();
    field.representation(mir::repres::RepresentationFactory::build(metadata));

    size_t truncation = field.representation()->truncation();

    std::unique_ptr<mir::action::Action> a2(new mir::action::VOD2UVTransform(job));
    a2->execute(field);

    mir::data::MIRField uf(false, 0);
    uf.values(field.values(0));
    uf.representation(mir::repres::RepresentationFactory::build(metadata));

    mir::data::MIRField vf(false, 0);
    vf.values(field.values(1));
    vf.representation(mir::repres::RepresentationFactory::build(metadata));

    mu.save(job, mvo, uf);
    mv.save(job, mvo, vf);

}


int main( int argc, char **argv ) {
    VOD2UVTool tool(argc, argv);
    tool.start();
    return 0;
}

