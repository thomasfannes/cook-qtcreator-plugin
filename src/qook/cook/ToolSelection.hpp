#ifndef HEADER_qook_cook_ToolSelection_hpp_ALREADY_INCLUDED
#define HEADER_qook_cook_ToolSelection_hpp_ALREADY_INCLUDED

#include <QWidget>
#include <QFileInfo>
#include <qook/cook/Tool.hpp>

namespace qook { namespace cook {

namespace Ui {
class ToolSelection;
}

class ToolSelection : public QWidget
{
    Q_OBJECT

public:
    explicit ToolSelection(QWidget *parent = 0);
    ~ToolSelection();

    const Tool & tool() const                       { return tool_; }
    void set_executable(const Tool & tool);

private:
    void update_tool_ui_();

private slots:
    void on_browse_clicked();
    void on_filename_textChanged(const QString &arg1);

private:
    Ui::ToolSelection *ui;
    Tool tool_;
};

} }

#endif
