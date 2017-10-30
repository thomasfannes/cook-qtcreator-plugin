#include "cook/project/info/StructureManager.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include <QtConcurrent>
#include <utils/qtcassert.h>

namespace cook { namespace project { namespace info {

StructureManager::StructureManager(const BuildConfiguration * config)
    : AsyncProcessManager(config)
{
}

std::function<bool (StructureProcess &)>
StructureManager::generate_process_starter(const BuildConfiguration & config) const
{
    QString recipe_file = config.project_file().toString();
    QString build_dir = config.buildDirectory().toString();

    std::function<bool (StructureProcess &)> func = [=](StructureProcess & process)
    {
        process.start_blocking(recipe_file, build_dir);
        return true;
    };

    return func;
}

void StructureManager::on_process_finished_(bool success, const StructureProcess * process)
{
    QTC_ASSERT(process, return);

    if (success)
        recipes_ = process->associated_recipes();
    else
        recipes_.errors = process->associated_recipes().errors;
}


} } }
