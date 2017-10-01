#ifndef HEADER_qook_project_info_BuildRecipesManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_BuildRecipesManager_hpp_ALREADY_INCLUDED

#include "qook/project/info/AsyncManager.hpp"
#include "qook/project/info/BuildRecipesParser.hpp"

namespace qook { namespace project { namespace info {

class BuildRecipesManager : public AsyncManager<BuildRecipesParser>
{
public:
    BuildRecipesManager(const BuildConfiguration *config);

    bool start_async();
};

} } }

#endif
