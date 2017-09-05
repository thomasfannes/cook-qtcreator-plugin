#ifndef HEADER_qook_cook_Tool_hpp_ALREADY_INCLUDED
#define HEADER_qook_cook_Tool_hpp_ALREADY_INCLUDED

#include <QFileInfo>
#include <utils/synchronousprocess.h>
#include <utils/fileutils.h>
#include <coreplugin/id.h>

namespace qook { namespace cook {

class Tool
{
public:
    struct Version
    {
        unsigned int major = 0;
        unsigned int minor = 0;
        unsigned int patch = 0;
    };

    Tool(const Core::Id & id, const QString & name = QString());

    static Core::Id generate_id();

    void set_name(const QString & name)         { name_ = name; }
    const QString & name() const                { return name_; }
    bool is_valid() const                       { return valid_; }
    Version version() const                     { return version_; }
    const Core::Id & id() const                 { return id_; }
    QString filename() const                    { return exec_; }

    void set_filename(const QString & filename);
    QString version_string() const;
    Utils::SynchronousProcessResponse run(const QStringList &args) const;

private:
    void extract_version_info_();
    void clear_test_settings_();

    QString exec_;
    QString name_;
    bool valid_;
    Version version_;

    struct RunDetails
    {
        bool attempted_run = false;
        bool did_run = false;
    };
    mutable RunDetails run_details;
    Core::Id id_;
};

} }

#endif
