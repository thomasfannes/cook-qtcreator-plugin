#ifndef HEADER_qook_project_BuildStep_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_Buildstep_hpp_ALREADY_INCLUDED

#include <projectexplorer/abstractprocessstep.h>

namespace qook { namespace project {

class BuildConfiguration;
class RunConfiguration;
class CookStepConfigWidget;

class CookBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT

//    friend class BuildStepConfigWidget;
//    friend class BuildStepFactory;

public:
    explicit CookBuildStep(ProjectExplorer::BuildStepList *parent);

    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

//    bool init(QList<const BuildStep *> &earlierSteps) override;
//    void run(QFutureInterface<bool> &fi) override;

//    ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;
//    bool immutable() const override;
//    bool buildsTarget(const QString &target) const;
//    void setBuildTarget(const QString &target, bool on);
//    QString allArguments() const;
//    QString makeCommand(const Utils::Environment &environment) const;

//    void setClean(bool clean);
//    bool isClean() const;

//    QVariantMap toMap() const override;

//protected:
//    BuildStep(ProjectExplorer::BuildStepList *parent, GenericMakeStep *bs);
//    BuildStep(ProjectExplorer::BuildStepList *parent, Core::Id id);
//    bool fromMap(const QVariantMap &map) override;

//private:
//    void ctor();

//    QStringList m_buildTargets;
//    QString m_makeArguments;
//    QString m_makeCommand;
//    bool m_clean = false;
};


} }

#endif
