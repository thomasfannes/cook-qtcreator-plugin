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

    Tool(const Core::Id & type_id);
    Tool(const Core::Id & type_id, Detection d, const Core::Id & id);

    virtual ~Tool() = default;

    static Core::Id generate_id();

    bool is_auto_detected() const               { return d_ == AutoDetected; }
    bool is_valid() const                       { return is_valid_; }
    virtual bool is_default() const = 0;
    Version version() const                     { return version_; }
    const Core::Id & id() const                 { return id_; }
    const Core::Id & type_id() const            { return type_id_; }

    const QString & display_name() const        { return display_name_; }
    const QFileInfo & exec_file() const         { return exe_file_; }
    QString user_file_name() const              { return exe_file_.absoluteFilePath(); }

    virtual QVariantMap to_map() const;
    virtual bool from_map(const QVariantMap & map);

    void set_display_name(const QString & display_name);
    void set_exec_file(const QFileInfo & exec_file);

    QString version_string() const;
    Utils::SynchronousProcessResponse run(const QStringList &args) const;

protected:
    static Utils::SynchronousProcessResponse run_(const QFileInfo & exec, const QStringList &args);

private:
    virtual std::pair<Tool::Version, bool> get_version_() const = 0;
    void extract_version_info_();
    void clear_test_settings_();

    Detection d_;
    QString display_name_;
    QFileInfo exe_file_;
    Core::Id type_id_;
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

class ToolFactoryInterface
{
public:
    virtual ~ToolFactoryInterface() = default;

    virtual Tool * construct() const = 0;
    virtual Tool * construct(const Tool & rhs) const = 0;
    virtual Tool * construct(Tool::Detection d, const Core::Id & id) const = 0;
    virtual QString name() const = 0;

    virtual QStringList executable_names() const = 0;
};

} }

#endif
