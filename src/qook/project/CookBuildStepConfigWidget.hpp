#ifndef HEADER_qook_project_CookStepConfigWidget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookStepConfigWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>
#include <QListWidget>

namespace qook { namespace project {

class CookBuildStep;

class CookBuildStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
public:
    explicit CookBuildStepConfigWidget(CookBuildStep * buildStep);

    virtual QString summaryText() const { return summary_text_; }
    virtual QString displayName() const;

private slots:
    void build_targets_changed();
    void item_changed(QListWidgetItem * item);
    void update_details();
    void build_target_changed();

private:
    CookBuildStep * build_step_;
    QListWidget * build_targets_;
    QString summary_text_;
};

} }

#endif
