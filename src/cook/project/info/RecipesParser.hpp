#ifndef HEADER_cook_project_info_RecipesParser_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_RecipesParser_hpp_ALREADY_INCLUDED

#include "cook/project/info/Parser.hpp"
#include "cook/project/info/Types.hpp"

namespace cook { namespace project { namespace info  {

class RecipesParser : public Parser<Recipes, RecipesParser>
{
private:
    friend class Parser<Recipes, RecipesParser>;
    void initialize_(ParserInternal<Recipes, RecipesParser> & p, Recipes &value);
};

}  } }

#endif
