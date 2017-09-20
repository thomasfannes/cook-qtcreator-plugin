#ifndef HEADER_qook_project_Parser_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_Parser_hpp_ALREADY_INCLUDED

#include <QObject>
#include "qook/project/Structure.hpp"

namespace qook { namespace project {

class ParserInt;

class Parser : public QObject
{
    Q_OBJECT

public:
    Parser();
    ~Parser();

    bool parse(Cook &structure, const QByteArray & input);

signals:
    void error_occurred(QString error);

private:
    friend class ParserInt;
    void emit_error_occured(const QString & error);

    ParserInt * d_;
};

} }

#endif
