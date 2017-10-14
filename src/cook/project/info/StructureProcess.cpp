#include "cook/project/info/StructureProcess.hpp"
#include "cook/project/info/CookProcess.hpp"
#include "cook/project/info/StructureParser.hpp"

namespace cook { namespace project { namespace info {

StructureProcess::StructureProcess(const toolset::CookTool *tool, QObject *parent)
    : CookProcess(tool, parent),
      parser_(new StructureParser(this))
{
    parser_->attach(this);

    connect(parser_, &StructureParser::error_occured,           this, &StructureProcess::error_occured);
}

const Recipes & StructureProcess::associated_recipes() const
{
    return parser_->data();
}

bool StructureProcess::start_blocking(const QString & recipe_file, const QString & build_directory, int msecs)
{
    QStringList args = { "-g", "recipes.tree", "-f", recipe_file, "-b", build_directory };
    return start_blocking_(args, msecs);
}

} } }

