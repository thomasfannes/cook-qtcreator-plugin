#ifndef HEADER_qook_project_info_DetailedRecipesManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_DetailedRecipesManager_hpp_ALREADY_INCLUDED

#include "qook/project/info/AsyncManager.hpp"
#include "qook/project/info/DetailedRecipesParser.hpp"

namespace qook { namespace project { namespace info {

class DetailedRecipesManager : public AsyncManager<DetailedRecipesParser>
{
public:
    DetailedRecipesManager(const BuildConfiguration *config);

    bool start_async(const QString &uri);
};

} } }

#endif
