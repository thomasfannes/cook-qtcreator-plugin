#ifndef HEADER_qook_project_info_Parser_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_Parser_hpp_ALREADY_INCLUDED

#include <QObject>
#include <QByteArray>

namespace qook { namespace project { namespace info {

template <typename Info, typename Derived>
class ParserInternal;

class ParserBase : public QObject
{
    Q_OBJECT

signals:
    void error_occured(const QString & error);
};

template <typename Info, typename Derived>
class Parser : public ParserBase
{
public:
    using value_type = Info;

    bool parse(Info & info, const QByteArray & input);

private:
    friend class ParserInternal<Info, Derived>;
    void emit_error_occurred(const QString & error) { emit error_occured(error); }
};


} } }

#endif
