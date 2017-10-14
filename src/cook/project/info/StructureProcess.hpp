#ifndef HEADER_cook_project_info_StructureProcess_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_StructureProcess_hpp_ALREADY_INCLUDED

#include "cook/project/info/CookProcess.hpp"

namespace cook { namespace project { namespace info {

class StructureParser;
class Recipes;

class StructureProcess : public CookProcess
{
    Q_OBJECT

public:
    explicit StructureProcess(const toolset::CookTool * tool, QObject *parent = nullptr);

    bool start_blocking(const QString &recipe_file, const QString &build_directory, int msecs = 3000);
    const Recipes & associated_recipes() const;

private:
    StructureParser * parser_;
};

} } }

#endif
