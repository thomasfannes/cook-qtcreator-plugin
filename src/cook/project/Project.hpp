#ifndef HEADER_cook_project_Project_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_Project_hpp_ALREADY_INCLUDED

#include "cook/project/InfoManagerType.hpp"
#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <cpptools/cppprojectupdater.h>

namespace cook { namespace project {

class BuildConfiguration;
class Cook;
class CookBuildTarget;
class CookManager;

class Project : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit Project(const Utils::FileName & filename);
    virtual ~Project() override;

    virtual bool supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const override;
    virtual bool needsConfiguration() const;

    BuildConfiguration * active_build_configuration() const;

signals:
    void recipes_available();
    void detailed_recipes_available();

public slots:
    void refresh_all();
    void refresh(RequestFlags flags);

private slots:
    void active_target_changed(ProjectExplorer::Target *target);

protected:
    virtual RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) final;
    virtual bool setupTarget(ProjectExplorer::Target *t) override;

private:
    friend class BuildConfiguration;

//    void update_nodes(const Cook & cook);
    void handle_parsing_started_(BuildConfiguration * configuration, RequestFlags flags);
    void handle_parsing_finished_(BuildConfiguration * configuration, RequestFlags succeeded, RequestFlags failed);
    void handle_sub_parsing_finished(BuildConfiguration * configuration, InfoRequestType request, bool success);
    void handle_build_recipes_available_();

    ProjectExplorer::Target * connected_target_;
    BuildConfiguration * current_build_config_;
    QList<const ProjectExplorer::FileNode *> all_files_;
    CppTools::CppProjectUpdater *cpp_code_model_updater_ = nullptr;
};


} }

#endif
