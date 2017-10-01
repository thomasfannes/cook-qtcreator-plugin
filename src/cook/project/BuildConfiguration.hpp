#ifndef HEADER_cook_project_BuildConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_BuildConfiguration_hpp_ALREADY_INCLUDED

#include "cook/project/info/Types.hpp"
#include "cook/project/CookBuildTarget.hpp"
#include "cook/project/ParseRequestRun.hpp"
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>
#include <cpptools/cppprojectupdater.h>

namespace cook {

namespace toolset { class CookTool; }

namespace project {

class InfoManager;
class CookNode;
class Project;
class Cook;
class CookBuildStep;

class BuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT

public:
    explicit BuildConfiguration(ProjectExplorer::Target * parent, const BuildType & build_type = BuildType::Unknown);
    ~BuildConfiguration();

    virtual ProjectExplorer::NamedWidget *createConfigWidget() override;
    virtual BuildType buildType() const override;
    virtual QVariantMap toMap() const;

    QStringList all_recipes_options() const;
    QStringList recipe_detail_options(const QString & uri) const;
    QStringList build_options(const QString & uri) const;

    const info::Recipes & recipes_info() const;
    const info::BuildRecipes & build_recipes_info() const;
    const info::Recipe * find_recipe(const QString & uri) const;
    const info::BuildRecipe * find_build_recipe(const QString & uri) const;
    QList<CookBuildTarget> all_build_targets() const;
    QList<CookBuildTarget> all_run_targets() const;


    QString target_uri() const;
    bool is_valid_uri(const QString & uri) const;
    const toolset::CookTool * tool() const;
    void set_build_target(const CookBuildTarget & target);
    const CookBuildTarget & build_target() const;

    bool refresh(QFlags<InfoRequestType> flags);
    const QString & error() const { return error_; }

    ProjectExplorer::ProjectNode * generate_tree() const;
    void refresh_cpp_code_model(CppTools::CppProjectUpdater * cpp_updater);

signals:
    void error_occured(const QString & error);

    void build_targets_changed();
    void build_target_changed();

protected:
    virtual bool fromMap(const QVariantMap &map);
    BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source);

private:
    friend class BuildConfigurationFactory;

    QList<CookBuildTarget> special_targets_() const;

    void ctor();
    void set_error_(const QString & error);
    void clear_error_();
    void start_refresh_(InfoRequestType type);
    Project * project_() const;

    void handle_request_started(InfoRequestType type);
    void handle_request_finished(bool success, InfoRequestType type);
    void handle_error_occured(const QString & error, InfoRequestType type);

    BuildType type_;
    QString error_;
    CookBuildTarget target_;
    InfoManager * info_mngr_;
    ParseRequestRun request_;
};

} // namespace prokect
} // namespace qook

#endif

