#ifndef HEADER_cook_plugin_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_plugin_Manager_hpp_ALREADY_INCLUDED

#include <QObject>
#include <QAction>

namespace cook { namespace plugin {

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager();

private:
    void update_actions_();
    void rescan_active_project_();

    QAction * rescan_project_;
    QAction * goto_recipe_;
};

} }

#endif
