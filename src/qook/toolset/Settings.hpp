#ifndef HEADER_qook_toolset_Settings_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_Settings_hpp_ALREADY_INCLUDED

#include <coreplugin/dialogs/ioptionspage.h>

namespace qook { namespace toolset {

class SettingsWidget;

class Settings : public Core::IOptionsPage
{
public:
    Settings();

    QWidget * widget() override;
    void apply() override;
    void finish() override;

private:
    SettingsWidget * widget_ = 0;
};

} }

#endif
