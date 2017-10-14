#ifndef HEADER_cook_project_info_StructureManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_StructureManager_hpp_ALREADY_INCLUDED

#include "cook/project/info/Types.hpp"
#include "cook/project/info/AsyncProcessManager.hpp"
#include "cook/project/info/StructureProcess.hpp"

namespace cook { namespace project { namespace info {

class StructureManager : public AsyncProcessManager<StructureManager, StructureProcess>
{
    Q_OBJECT

public:
    using value_type = Recipes;

    StructureManager(const BuildConfiguration *config);
    const value_type & latest() const { return recipes_; }

private:
    friend class AsyncProcessManager;

    std::function<bool (StructureProcess &)> generate_process_starter(const BuildConfiguration & config) const;
    void on_success_(const StructureProcess & process);

    Recipes recipes_;
};

} } }

#endif
