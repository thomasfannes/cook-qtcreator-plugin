#ifndef HEADER_qook_project_ParseRequestRun_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_ParseRequestRun_hpp_ALREADY_INCLUDED

#include "qook/project/InfoManagerType.hpp"

namespace qook { namespace project {

class ParseRequestRun
{
public:
    ParseRequestRun()
    {
    }

    explicit ParseRequestRun(RequestFlags request)
        : initial_(request)
    {
    }

    bool is_on_first() { return processed_ == RequestFlags(); }

    bool is_started() { return !is_finished() && processed_ == RequestFlags(); }
    bool is_finished() { return processed_ == initial_; }

    void set(InfoRequestType flag, bool success)
    {
        (success ? success_ : failure_).setFlag(flag);
        processed_.setFlag(flag);
    }

    InfoRequestType propose_flag()
    {
        if (false) {}

#define CASE(TYPE) else if (initial_.testFlag(InfoRequestType::TYPE) && !processed_.testFlag(InfoRequestType::TYPE)) { return InfoRequestType::TYPE; }
        CASE(Recipes)
        CASE(Build_Recipes)
        CASE(Ninja)
#undef CASE
        else
        {
            return InfoRequestType::Unknown;
        }
    }

    const RequestFlags & successes() const { return success_; }
    const RequestFlags & failures() const { return failure_; }
    const RequestFlags & flags() const { return initial_; }

private:
    RequestFlags initial_, processed_, success_, failure_;
};

} }

#endif

