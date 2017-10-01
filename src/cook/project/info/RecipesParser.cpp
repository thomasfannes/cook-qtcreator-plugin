#include "cook/project/info/RecipesParser.hpp"
#include "cook/project/info/Parser.hxx"
#include "gubg/parse/polymorphic_tree/FixedStructure.hpp"
#include "gubg/parse/polymorphic_tree/RootElement.hpp"

namespace cook { namespace project { namespace info {

namespace {

#define TAG(TYPE) const static char * tag_##TYPE = #TYPE
TAG(recipes);
TAG(recipe);
TAG(default);
#undef TAG

#define KEY(TYPE) const static char * key_##TYPE = #TYPE
KEY(uri);
KEY(display_name);
#undef KEY

}

using namespace gubg::parse::polymorphic_tree;
template <typename T> using FSE = FixedStructureElement<T>;

template bool Parser<Recipes, RecipesParser>::parse(Recipes & info, const QByteArray & input);

void RecipesParser::initialize_(ParserInternal<Recipes, RecipesParser> & p, Recipes & value)
{
    RootElement<Recipes> * root = new RootElement<Recipes>(value, tag_recipes);
    {
        FSE<Recipe> & c = root->node<Recipe>(tag_recipe, [](auto & l, const auto & r) {l.recipes.insert(r.uri, r); });
        {
            c.attribute(key_uri, [](auto & l, const auto & r) { l.uri = QString::fromStdString(r); });
            c.composite_attribute(key_display_name, [](auto & l, const auto & r) { l.name = QString::fromStdString(r); });
        }
    }

    {
        FSE<QString> & c = root->node<QString>(tag_default, [](auto & l, const auto & r) { l.default_uri = r; });
        {
            c.attribute(key_uri, [](auto & l, const auto & r) { l = QString::fromStdString(r); });
        }
    }
    p.set_root(root);
}

} } }
