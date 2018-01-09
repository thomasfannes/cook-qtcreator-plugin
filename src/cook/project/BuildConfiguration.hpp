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

    const toolset::CookTool * tool() const;
    QStringList ninja_build_args(const QStringList &additional_arguments = QStringList()) const;
    Utils::FileName project_file() const;
    Utils::FileNameList all_script_files() const;

    const info::Recipe * find_recipe(const QString & uri) const;
    const info::Recipe & root_book() const;
    QList<CookBuildTarget> all_build_targets() const;
    QList<CookBuildTarget> all_run_targets() const;
    QList<info::Element> all_targets() const;
    QList<info::Error> all_parse_errors() const;

    void set_build_target(const CookBuildTarget & target);
    const CookBuildTarget & build_target() const;
    QString target_uri() const;

    bool refresh(QFlags<InfoRequestType> flags);
    const QString & error() const { return error_; }

    ProjectExplorer::ProjectNode * generate_linear_project() const;
    void refresh_cpp_code_model(CppTools::CppProjectUpdater * cpp_updater);

    Project * project_() const;

signals:
    void error_occured(const QString & error);
    void recipes_changed();
    void target_uri_changed();

protected:
    virtual bool fromMap(const QVariantMap &map);
    BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source);

private:
    friend class BuildConfigurationFactory;

    QList<CookBuildTarget> special_targets_() const;

    void ctor();

    const info::Recipes & recipes_info_() const;
    void set_error_(const QString & error);
    void clear_error_();
    void start_refresh_(InfoRequestType type);


    void handle_request_started();
    void handle_request_finished(bool success, InfoRequestType type);
    void handle_error_occured(const QString & error, InfoRequestType type);
    void handle_directory_changed();

    BuildType type_;
    QString error_;
    CookBuildTarget target_;
    InfoManager * info_mngr_;
    ParseRequestRun request_;
};

} // namespace prokect
} // namespace qook

#endif

