#ifndef HEADER_cook_project_CookNinjaStepFactory_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_CookNinjaStepFactory_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>

namespace cook { namespace project {

class CookNinjaStepFactory  : public ProjectExplorer::IBuildStepFactory
{
    Q_OBJECT

public:
    explicit CookNinjaStepFactory(QObject *parent = nullptr);

    virtual QList<ProjectExplorer::BuildStepInfo> availableSteps(ProjectExplorer::BuildStepList *parent) const override;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, Core::Id id) override;
    virtual ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) override;
};

} }

#endif
