#ifndef HEADER_qook_project_InfoManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_InfoManager_hpp_ALREADY_INCLUDED

#include "qook/project/InfoManagerType.hpp"
#include "qook/project/info/Types.hpp"
#include "qook/project/info/RecipesManager.hpp"
#include "qook/project/info/BuildRecipesManager.hpp"
#include <QFuture>
#include <QFlags>

namespace qook { namespace project {

namespace info {

class RecipesManager;
class BuildRecipesManager;

}

class BuildConfiguration;

class InfoManager : public QObject
{
    Q_OBJECT

public:
    InfoManager(BuildConfiguration * config);
    ~InfoManager();

    info::RecipesManager & recipes()                { return *recipes_mgr_; }
    info::BuildRecipesManager & build_recipes()     { return *build_recipes_mgr_; }

signals:
    void started(InfoRequestType type);
    void error_occurred(const QString & error, InfoRequestType type);
    void finished(bool ok, InfoRequestType type);

private:
    using ParserFlags = QFlags<InfoRequestType>;
    info::RecipesManager * recipes_mgr_;
    info::BuildRecipesManager * build_recipes_mgr_;
};

} }

#endif
