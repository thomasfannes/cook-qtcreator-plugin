#ifndef HEADER_qook_project_NinjaBuildStepFactory_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_NinjaBuildStepFactory_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>

namespace qook { namespace project {

class NinjaBuildStepFactory  : public ProjectExplorer::IBuildStepFactory
{
    Q_OBJECT

public:
    explicit NinjaBuildStepFactory(QObject *parent = nullptr);

    virtual QList<ProjectExplorer::BuildStepInfo> availableSteps(ProjectExplorer::BuildStepList *parent) const override;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, Core::Id id) override;
    virtual ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) override;
};

} }

#endif
