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
        return process.start_blocking(recipe_file, build_dir);
    };

    return func;
}

void StructureManager::on_success_(const StructureProcess & process)
{
    recipes_ = process.associated_recipes();
}


} } }
