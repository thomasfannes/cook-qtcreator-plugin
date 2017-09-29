#include "qook/project/info/BuildRecipesParser.hpp"
#include "qook/project/info/Parser.hxx"
#include "gubg/parse/polymorphic_tree/FixedStructure.hpp"
#include "gubg/parse/polymorphic_tree/RootElement.hpp"

namespace qook { namespace project { namespace info {

namespace {

#define TAG(TYPE) const static char * tag_##TYPE = #TYPE
TAG(details);
TAG(recipe);
TAG(file);
TAG(include_path);
TAG(active);
#undef TAG

#define KEY(TYPE) const static char * key_##TYPE = #TYPE
KEY(uri);
KEY(script);
KEY(display_name);
KEY(type);
KEY(build_target);
KEY(path);
#undef KEY

TargetType convert_target_type(const std::string & v)
{
    if (false) {}
    else if(v == "Executable")
        return TargetType::Executable;
    else if (v == "StaticLibrary")
        return TargetType::StaticLibrary;
    else
        return TargetType::Unknown;
}

FileType convert_file_type(const std::string & v)
{
    if (false) {}
    else if(v == "source")
        return FileType::Source;
    else if(v == "header")
        return FileType::Header;
    else
        return FileType::Unknown;
}

Utils::FileName convert_file_name(const std::string & v)
{
    return Utils::FileName::fromString(QString::fromStdString(v));
}

}

using namespace gubg::parse::polymorphic_tree;
template <typename T> using FSE = FixedStructureElement<T>;

template bool Parser<BuildRecipes, BuildRecipesParser>::parse(BuildRecipes & info, const QByteArray & input);

void BuildRecipesParser::initialize_(ParserInternal<BuildRecipes, BuildRecipesParser> &p, BuildRecipes &value)
{
    RootElement<BuildRecipes> * root = new RootElement<BuildRecipes>(value, tag_details);
    {
        FSE<BuildRecipe> & c = root->composite_node<BuildRecipe>(tag_recipe, [](auto & l, const auto & r) { l.recipes.insert(r.uri, r); });
        {
            c.attribute(key_uri,            [](auto & l, const auto & r) { l.uri = QString::fromStdString(r); });
            c.attribute(key_display_name,   [](auto & l, const auto & r) { l.name = QString::fromStdString(r); });
            c.attribute(key_script,         [](auto & l, const auto & r) { l.script = convert_file_name(r); });
            c.attribute(key_type,           [](auto & l, const auto & r) { l.type = convert_target_type(r); });
            c.attribute(key_build_target,   [](auto & l, const auto & r) { l.build_target = convert_file_name(r); });

            {
                FSE<FileInfo> & f = c.composite_node<FileInfo>(tag_file, [](auto & l, const auto & r) { l.files.append(r); });
                {
                    f.attribute(key_type, [](auto & l, const auto & r) { l.type = convert_file_type(r); });
                    f.attribute(key_path, [](auto & l, const auto & r) { l.file = convert_file_name(r); });
                }
            }

            {
                FSE<QString> & f = c.composite_node<QString>(tag_include_path, [](auto & l, const auto & r) { l.include_paths.append(r); });
                {
                    f.attribute(key_path, [](auto & l, const auto & r) { l = QString::fromStdString(r); });
                }
            }
        }
    }
    {
        FSE<QString> & c = root->node<QString>(tag_active, [](auto & l, const auto & r) { l.default_uri = r; });
        {
            c.attribute(key_uri, [](auto & l, const auto & r) { l = QString::fromStdString(r); });
        }
    }

    p.set_root(root);
}

} } }
