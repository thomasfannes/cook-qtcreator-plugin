#include "qook/project/Parser.hpp"
#include "qook/project/Structure.hpp"
#include "gubg/parse/polymorphic_tree/Parser.hpp"
#include "gubg/parse/polymorphic_tree/FixedStructure.hpp"
#include <functional>

namespace qook { namespace project {

namespace  {

const static char * tag_root = "cook";
const static char * tag_book = "book";
const static char * tag_recipe = "recipe";
const static char * tag_file = "file";
const static char * tag_include_path = "include_path";

const static char * key_uri = "uri";
const static char * key_script = "script";
const static char * key_display_name = "display_name";
const static char * key_tag = "tag";
const static char * key_type = "type";
const static char * key_build_target = "build_target";
const static char * key_path = "path";

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



struct ParserInt : public gubg::parse::polymorphic_tree::Parser
{
    ParserInt(project::Parser * parser)
        : parser_(parser)
    {
        initialize();
    }

    void initialize()
    {
        FixedStructureElement<Cook> & r = element.node<Cook>(tag_root, [](Cook & r, const Cook & d) {r = d; });
        {
            FixedStructureElement<Book> & b = r.composite_node<Book>(tag_book, [](Cook & r, const Book & b) { r.books.append(b); });
            b.attribute(key_uri,            [](Book & b, const std::string & v) { b.uri = QString::fromStdString(v); });
            b.attribute(key_script,         [](Book & b, const std::string & v) { b.script = convert_file_name(v); });
            b.attribute(key_display_name,   [](Book & b, const std::string & v) { b.display_name = QString::fromStdString(v); });
            {
                FixedStructureElement<QString> & sb = b.composite_node<QString>(tag_book, [](Book & b, const QString & d) { b.subboooks.append(d); });
                sb.attribute(key_uri,       [](QString & s, const std::string & v) { s= QString::fromStdString(v); });
            }

            {
                FixedStructureElement<QString> & sb = b.composite_node<QString>(tag_recipe, [](Book & b, const QString & d) { b.recipes.append(d); });
                sb.attribute(key_uri,       [](QString & s, const std::string & v) { s= QString::fromStdString(v); });
            }
        }

        {
            FixedStructureElement<Recipe> & c = r.composite_node<Recipe>(tag_recipe, [](Cook & r, const Recipe & d) { r.recipes.append(d); });
            c.attribute(key_uri,            [](Recipe & r, const std::string v) { r.uri = QString::fromStdString(v); });
            c.attribute(key_tag,            [](Recipe & r, const std::string v) { r.tag = QString::fromStdString(v); });
            c.attribute(key_script,         [](Recipe & r, const std::string v) { r.script = convert_file_name(v); });
            c.attribute(key_display_name,   [](Recipe & r, const std::string v) { r.display_name = QString::fromStdString(v); });
            c.attribute(key_type,           [](Recipe & r, const std::string v) { r.type = convert_target_type(v); });
            c.attribute(key_build_target,   [](Recipe & r, const std::string v) { r.build_target = convert_file_name(v); });

            {
                FixedStructureElement<FileInfo> & f = c.composite_node<FileInfo>(tag_file, [](Recipe & r, const FileInfo & f) { r.files.append(f); });
                f.attribute(key_type, [](FileInfo & f, const std::string & v) { f.type = convert_file_type(v); });
                f.attribute(key_path, [](FileInfo & f, const std::string & v) { f.file = convert_file_name(v); });

            }

            {
                FixedStructureElement<QString> & f = c.composite_node<QString>(tag_include_path, [](Recipe & r, const QString & f) { r.include_paths.append(f); });
                f.attribute(key_path, [](QString & f, const std::string & v) { f = QString::fromStdString(v); });
            }
        }

        this->set_root(&element);
    }

    bool parse(Cook & structure, const QByteArray & input)
    {
        bool ret_val = gubg::parse::polymorphic_tree::Parser::process(input.toStdString());
        if (ret_val)
            structure = element.value();

        return ret_val;
    }

    virtual void handle_error(gubg::parse::polymorphic_tree::ReturnCode code) override
    {
        QString path;


        for(auto it = current_path().begin(); it != current_path().end(); ++it)
        {
            QString t = QString::fromStdString(*it);
            if(it != current_path().begin())
                path.append("/");

            path.append(t);
        }

        QString error = QString("Badly formatted tree at [%1] (%2)").arg(path).arg(QString::fromStdString(to_string(code)));
        parser_->emit_error_occured(error);
    }

    project::Parser * parser_;
    FixedStructureElement<Cook> element;
};


Parser::Parser()
    : d_(new ParserInt(this))
{
}

Parser::~Parser()
{
    delete d_;
}

bool Parser::parse(Cook & structure, const QByteArray & input)
{
    return d_->parse(structure, input);
}

void Parser::emit_error_occured(const QString & error)
{
    emit error_occurred(error);
}



} }

