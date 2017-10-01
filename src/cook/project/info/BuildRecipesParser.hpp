#ifndef HEADER_cook_project_info_BuildRecipesParser_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_BuildRecipesParser_hpp_ALREADY_INCLUDED

#include "cook/project/info/Parser.hpp"
#include "cook/project/info/Types.hpp"

namespace cook { namespace project { namespace info {

class BuildRecipesParser : public Parser<BuildRecipes, BuildRecipesParser>
{
private:
    friend class Parser<BuildRecipes, BuildRecipesParser>;
    void initialize_(ParserInternal<BuildRecipes, BuildRecipesParser> & p, BuildRecipes & value);
};

} } }

#endif
