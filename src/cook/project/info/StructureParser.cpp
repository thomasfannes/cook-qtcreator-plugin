#include "cook/project/info/StructureParser.hpp"
#include "cook/project/info/CookProcess.hpp"
#include "gubg/parse/polymorphic_tree/Parser.hpp"
#include "gubg/parse/polymorphic_tree/TypedParser.hpp"
#include "gubg/parse/polymorphic_tree/RootElement.hpp"

namespace cook { namespace project { namespace info {

namespace  {

#define TAG(TYPE) const static char * tag_##TYPE = #TYPE
TAG(structure);
TAG(book);
TAG(recipe);
TAG(file);
TAG(include_path);
TAG(default);
#undef TAG

#define KEY(TYPE) const static char * key_##TYPE = #TYPE
KEY(uri);
KEY(tag);
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

QString to_string(const std::string & v)
{
    return QString::fromStdString(v);
}


using FileInfoParser = gubg::parse::polymorphic_tree::TypedParser<FileInfo>;
std::shared_ptr<FileInfoParser> file_info_parser()
{
    auto p = std::make_shared<FileInfoParser>();
    p->single_attr(key_type, &FileInfo::type, convert_file_type)->set_required();
    p->single_attr(key_path, &FileInfo::file, convert_file_name)->set_required();

    return p;
}

using IncludePathParser = gubg::parse::polymorphic_tree::TypedParser<QString>;
std::shared_ptr<IncludePathParser> include_path_parser()
{
    auto p = std::make_shared<IncludePathParser>();
    auto extractor = [](QString & v) -> QString & { return v; };
    p->single_attr<QString>(key_path, extractor, to_string)->set_required();

    return p;
}


using RecipeParser = gubg::parse::polymorphic_tree::TypedParser<Recipe>;
std::shared_ptr<RecipeParser> recipe_parser()
{
    auto p = std::make_shared<RecipeParser>();

    p->single_attr(key_uri, &Recipe::uri, to_string)->set_required();
    p->single_attr(key_display_name, &Recipe::name, to_string);
    p->single_attr(key_script, &Recipe::script, convert_file_name)->set_required();
    p->single_attr(key_type, &Recipe::type, convert_target_type)->set_required();
    p->single_attr(key_build_target, &Recipe::build_target, convert_file_name)->set_required();
    p->single_attr(key_tag, &Recipe::tag, to_string)->set_required();

    {
        auto inserter = [](Recipe & recipe, auto first, auto last) { for(; first != last; ++first) recipe.files.append(*first); };
        p->multi_child<FileInfo>(tag_file, inserter, file_info_parser);
    }

    {
        auto inserter = [](Recipe & recipe, auto first, auto last) { for(; first != last; ++first) recipe.include_paths.append(*first); };
        p->multi_child<QString>(tag_include_path, inserter, include_path_parser);
    }

    p->set_allocator([]() { return Recipe(true); });

    return p;
}

using BookParser = gubg::parse::polymorphic_tree::TypedParser<Recipe>;
std::shared_ptr<BookParser> book_parser()
{
    auto p = std::make_shared<BookParser>();

    p->single_attr(key_uri, &Recipe::uri, to_string)->set_required();
    p->single_attr(key_display_name, &Recipe::name, to_string);
    p->single_attr(key_script, &Recipe::script, convert_file_name)->set_required();
    p->single_attr(key_tag, &Recipe::tag, to_string);

    auto inserter = [](Recipe & recipe, auto first, auto last) { for(; first != last; ++first) recipe.children.append(*first); };

    p->multi_child<Recipe>(tag_book, inserter, book_parser);
    p->multi_child<Recipe>(tag_recipe, inserter, recipe_parser);

    p->set_allocator([]() { return Recipe(false); });

    return p;
}

using DefaultUriParser = gubg::parse::polymorphic_tree::TypedParser<QString>;
std::shared_ptr<DefaultUriParser> default_uri_parser()
{
    auto p = std::make_shared<DefaultUriParser>();
    auto extractor = [](QString & v) -> QString & { return v; };

    p->single_attr<QString>(key_uri, extractor, to_string)->set_required();

    return p;
}

using CurRecipesParser = gubg::parse::polymorphic_tree::TypedParser<Recipes>;
std::shared_ptr<CurRecipesParser> recipes_parser()
{
    auto p = std::make_shared<CurRecipesParser>();

    p->single_child<Recipe>(tag_book, &Recipes::root, book_parser);
    p->single_child<QString>(tag_default, &Recipes::default_uri, default_uri_parser);



    return p;
}

using RootParser = gubg::parse::polymorphic_tree::RootElement<Recipes>;
std::shared_ptr<RootParser> root_parser(Recipes & recipe)
{
    auto p  = std::make_shared<RootParser>(recipe);

    auto setter = [](Recipes & lhs, const Recipes & rhs){ lhs = rhs; };
    p->single_child<Recipes>(tag_structure, setter, recipes_parser);

    return p;
}

}

class StructureParser::P
{
public:
    void reset(Recipes & recipes)
    {
        parser_.reset();
        parser_.set_root(root_parser(recipes));
        parser_.open();
    }

    void close()
    {
        parser_.close();
    }

    template <typename CharIt>
    void process(CharIt first, CharIt last)
    {
        parser_.process(first, last);
    }


    QString error_string(gubg::parse::polymorphic_tree::ReturnCode, const std::list<std::string> & current_path) const
    {
        return QString("Error while parsing at '%1': '%2'");
    }

    bool is_succesful() const
    {
        return parser_.error_code() == gubg::parse::polymorphic_tree::ReturnCode::OK;
    }

    void set_callback(const gubg::parse::polymorphic_tree::Parser::ErrorCallback & callback)
    {
        parser_.set_error_callback(callback);
    }

private:
    gubg::parse::polymorphic_tree::Parser parser_;
    QIODevice * device_;
};



StructureParser::StructureParser(QObject *parent)
    : QObject(parent),
      p_(new P()),
      cook_process_(nullptr)
{
    p_->set_callback([&](auto error_code, const auto & path){ emit error_occured(p_->error_string(error_code, path));  });
}

StructureParser::~StructureParser()
{
    delete p_;
}

void StructureParser::attach(CookProcess * process)
{
    if (cook_process_)
    {
        disconnect(cook_process_, &QIODevice::aboutToClose, this, &StructureParser::close_);
        disconnect(cook_process_, &CookProcess::standard_out_written, this, &StructureParser::process_);
    }

    cook_process_ = process;

    if (cook_process_)
    {
        connect(cook_process_, &QIODevice::aboutToClose, this, &StructureParser::close_);
        connect(cook_process_, &CookProcess::standard_out_written, this, &StructureParser::process_);
    }


    recipes_.clear();
    p_->reset(recipes_);
}

void StructureParser::close_()
{
    p_->close();

    if (cook_process_)
    {
        disconnect(cook_process_, &QIODevice::aboutToClose, this, &StructureParser::close_);
        disconnect(cook_process_, &CookProcess::standard_out_written, this, &StructureParser::process_);
        cook_process_ = nullptr;
    }

    emit finished(p_->is_succesful());
}

void StructureParser::process_(const QByteArray &array)
{
    p_->process(array.begin(), array.end());
}

bool StructureParser::is_finished() const
{
    return !cook_process_->isOpen();
}

bool StructureParser::was_succesful() const
{
    return is_finished() && p_->is_succesful();
}

const Recipes & StructureParser::data() const
{
    return recipes_;
}


} } }
