#ifndef HEADER_qook_cook_ToolSettings_hpp_ALREADY_INCLUDED
#define HEADER_qook_cook_ToolSettings_hpp_ALREADY_INCLUDED

#include <coreplugin/dialogs/ioptionspage.h>
#include <utils/pathchooser.h>


namespace qook { namespace cook {

class ToolSelection;

class ToolSettings : public Core::IOptionsPage
{
public:
    ToolSettings();

    QWidget * widget() override;
    void apply() override;
    void finish() override;

private:
    ToolSelection * widget_ = 0;
};

} }

#endif
