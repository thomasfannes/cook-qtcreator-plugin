#ifndef HEADER_qook_project_info_BuildRecipesParser_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_BuildRecipesParser_hpp_ALREADY_INCLUDED

#include "qook/project/info/Parser.hpp"
#include "qook/project/info/Types.hpp"

namespace qook { namespace project { namespace info {

class BuildRecipesParser : public Parser<BuildRecipes, BuildRecipesParser>
{
private:
    friend class Parser<BuildRecipes, BuildRecipesParser>;
    void initialize_(ParserInternal<BuildRecipes, BuildRecipesParser> & p, BuildRecipes & value);
};

} } }

#endif
