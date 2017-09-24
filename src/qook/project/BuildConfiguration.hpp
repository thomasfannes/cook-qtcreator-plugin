#ifndef HEADER_qook_project_BuildConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_BuildConfiguration_hpp_ALREADY_INCLUDED

#include "qook/project/Structure.hpp"
#include "qook/project/CookBuildTarget.hpp"
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>

namespace qook {

namespace toolset {

class Tool;

}

namespace project {

class CookScriptManager;
class CookNode;
class Project;
class Cook;

class BuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT


public:
    explicit BuildConfiguration(ProjectExplorer::Target * parent);
    ~BuildConfiguration();

    ProjectExplorer::NamedWidget *createConfigWidget() override;

    void set_build_type(BuildType type);
    BuildType buildType() const override;

    QStringList tree_format_options() const;
    const toolset::Tool * tool() const;
    const Cook & cook_info() const;

    void refresh();

    QList<CookBuildTarget> registered_targets() const;
    QList<CookBuildTarget> special_targets() const;
    QList<CookBuildTarget> all_targets() const;


signals:
    void error_occured(const QString & error);

protected:
    BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source);

private:
    void ctor();
    void set_error_(const QString & error);
    void clear_error_();
    Project * project_() const;

    friend class GenericBuildSettingsWidget;
    friend class BuildConfigurationFactory;

    BuildType type_;
    CookScriptManager * mngr_;
    QString error_;
};

} // namespace prokect
} // namespace qook

#endif
