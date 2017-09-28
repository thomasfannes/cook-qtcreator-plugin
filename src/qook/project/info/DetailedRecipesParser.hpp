#ifndef HEADER_qook_project_info_DetailedRecipesParser_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_DetailedRecipesParser_hpp_ALREADY_INCLUDED

#include "qook/project/info/Parser.hpp"
#include "qook/project/info/Types.hpp"

namespace qook { namespace project { namespace info {

class DetailedRecipesParser : public Parser<DetailedRecipes, DetailedRecipesParser>
{
private:
    friend class Parser<DetailedRecipes, DetailedRecipesParser>;
    void initialize_(ParserInternal<DetailedRecipes, DetailedRecipesParser> & p, DetailedRecipes & value);
};

} } }

#endif
