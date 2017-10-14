#ifndef HEADER_cook_project_info_StructureParser_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_StructureParser_hpp_ALREADY_INCLUDED

#include <QObject>
#include "cook/project/info/Types.hpp"

namespace cook { namespace project { namespace info {

class CookProcess;

class StructureParser : public QObject
{
    Q_OBJECT

    struct P;

public:
    using value_type = Recipes;

    StructureParser(QObject * parent = nullptr);
    ~StructureParser();

    void attach(CookProcess * process);

    bool is_finished() const;
    bool was_succesful() const;
    const Recipes & data() const;

signals:
    void error_occured(const QString & error);
    void finished(bool succesful);

private:
    void close_();
    void process_(const QByteArray & array);

    StructureParser(const StructureParser &) = delete;
    StructureParser & operator=(const StructureParser &) = delete;
    StructureParser(StructureParser &&) = delete;
    StructureParser & operator=(StructureParser &&) = delete;

    P * p_;
    CookProcess * cook_process_;
    Recipes recipes_;
};

} } }

#endif
