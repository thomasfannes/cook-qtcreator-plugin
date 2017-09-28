#ifndef HEADER_qook_project_BuildConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_BuildConfiguration_hpp_ALREADY_INCLUDED

#include "qook/project/info/Types.hpp"
#include "qook/project/CookBuildTarget.hpp"
#include "qook/project/ParseRequestRun.hpp"
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>
#include <cpptools/cppprojectupdater.h>

namespace qook {

namespace toolset { class Tool; }

namespace project {

class InfoManager;
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

    QStringList all_recipes_options() const;
    QStringList recipe_detail_options(const QString & uri) const;
    QStringList build_options(const QString & uri) const;

    BuildType buildType() const override;

    const info::Recipes & recipes_info() const;
    const info::DetailedRecipes & detailed_recipes_info() const;
    bool knows_uri(const QString & uri) const;

    bool refresh(QFlags<InfoRequestType> flags);
    const QString & error() const { return error_; }

    ProjectExplorer::ProjectNode * generate_tree() const;
    void refresh_cpp_code_model(CppTools::CppProjectUpdater * cpp_updater);

    QString target_uri() const;

    const toolset::Tool * tool() const;

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
    void start_refresh_(InfoRequestType type);
    Project * project_() const;

    void handle_request_started(InfoRequestType type);
    void handle_request_finished(bool success, InfoRequestType type);
    void handle_error_occured(const QString & error, InfoRequestType type);

    friend class GenericBuildSettingsWidget;
    friend class BuildConfigurationFactory;

    BuildType type_;
    QString error_;
    CookBuildTarget target_;
    InfoManager * info_mngr_;
    ParseRequestRun request_;
};

} // namespace prokect
} // namespace qook

#endif

