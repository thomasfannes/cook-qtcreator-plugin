#ifndef HEADER_qook_toolset_Tool_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_Tool_hpp_ALREADY_INCLUDED

#include <QFileInfo>
#include <utils/synchronousprocess.h>
//#include <utils/fileutils.h>
#include <coreplugin/id.h>

namespace qook { namespace toolset {

class Tool
{
public:
    struct Version
    {
        unsigned int major = 0;
        unsigned int minor = 0;
        unsigned int patch = 0;
    };

    enum Detection
    {
        AutoDetected,
        ManualSpecified
    };

    Tool();
    Tool(Detection d, const Core::Id & id);

    static Core::Id generate_id();
    static std::pair<QString, bool> test_cook_executable(const QFileInfo & exec_file);
    static Tool * generate_from_map(const QVariantMap & map);

    bool is_auto_detected() const               { return d_ == AutoDetected; }
    bool is_valid() const                       { return is_valid_; }
    Version version() const                     { return version_; }
    const Core::Id & id() const                 { return id_; }

    const QString & display_name() const        { return display_name_; }
    const QFileInfo & exec_file() const         { return exe_file_; }
    QString user_file_name() const              { return exe_file_.absoluteFilePath(); }

    QVariantMap to_map() const;

    void set_display_name(const QString & display_name);
    void set_exec_file(const QFileInfo & exec_file);

    QString version_string() const;
    Utils::SynchronousProcessResponse run(const QStringList &args) const;

private:
    static Utils::SynchronousProcessResponse run_(const QFileInfo & exec, const QStringList &args);
    static std::pair<Tool::Version, bool> get_version_(const QFileInfo & exec);
    static QString version_string_(const Version & version, bool is_valid);

    void extract_version_info_();
    void clear_test_settings_();

    Detection d_;
    QString display_name_;
    QFileInfo exe_file_;
    Core::Id id_;

    bool is_valid_;
    Version version_;
    struct RunDetails
    {
        bool attempted_run = false;
        bool did_run = false;
    };
    mutable RunDetails run_details;
};

} }

#endif
