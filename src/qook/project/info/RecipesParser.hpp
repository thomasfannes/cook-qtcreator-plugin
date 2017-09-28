#ifndef HEADER_qook_project_info_RecipesParser_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_RecipesParser_hpp_ALREADY_INCLUDED

#include "qook/project/info/Parser.hpp"
#include "qook/project/info/Types.hpp"

namespace qook { namespace project { namespace info  {

class RecipesParser : public Parser<Recipes, RecipesParser>
{
private:
    friend class Parser<Recipes, RecipesParser>;
    void initialize_(ParserInternal<Recipes, RecipesParser> & p, Recipes &value);
};

}  } }

#endif
