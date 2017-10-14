#ifndef HEADER_cook_project_InfoManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_InfoManager_hpp_ALREADY_INCLUDED

#include "cook/project/InfoManagerType.hpp"
#include "cook/project/info/Types.hpp"
#include "cook/project/info/StructureManager.hpp"
#include <QFuture>
#include <QFlags>

namespace cook { namespace project {

class BuildConfiguration;

class InfoManager : public QObject
{
    Q_OBJECT

public:
    InfoManager(BuildConfiguration * config);
    ~InfoManager();

    info::StructureManager & recipes()                { return *recipes_mgr_; }

signals:
    void started(InfoRequestType type);
    void error_occurred(const QString & error, InfoRequestType type);
    void finished(bool ok, InfoRequestType type);

private:
    using ParserFlags = QFlags<InfoRequestType>;
    info::StructureManager * recipes_mgr_;
};

} }

#endif
