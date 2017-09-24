#ifndef HEADER_qook_project_Project_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_Project_hpp_ALREADY_INCLUDED

#include "qook/project/Structure.hpp"
#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <cpptools/cppprojectupdater.h>

namespace qook { namespace project {

class BuildConfiguration;
class Cook;
class CookBuildTarget;

class Project : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit Project(const Utils::FileName & filename);
    virtual ~Project() override;

//    void refresh();

    virtual bool supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const override;
    virtual bool needsConfiguration() const;
    bool has_built_target(const CookBuildTarget & tgt) const;

    QStringList build_target_titles() const;

public slots:
    void refresh();

private slots:
    void active_target_changed(ProjectExplorer::Target *target);

protected:
    virtual RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) final;

private:
    friend class BuildConfiguration;

//    void update_nodes(const Cook & cook);
    void handle_parsing_started_(BuildConfiguration * configuration);
    void handle_parsing_error_(BuildConfiguration * configuration);
    void update_project_data_(BuildConfiguration * configuration, const Cook & cook_info);
    void generate_project_tree_();

    void refresh_cpp_code_model_();

    ProjectExplorer::Target * connected_target_;
    BuildConfiguration * current_build_config_;
    QList<const ProjectExplorer::FileNode *> all_files_;
    CppTools::CppProjectUpdater *cpp_code_model_updater_ = nullptr;
    Cook cook_info_;
};

} }

#endif
