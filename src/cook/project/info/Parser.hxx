#ifndef HEADER_cook_project_info_Parser_hxx_ALREADY_INCLUDED
#define HEADER_cook_project_info_Parser_hxx_ALREADY_INCLUDED

#include "cook/project/info/Parser.hpp"
#include "gubg/parse/polymorphic_tree/Parser.hpp"


namespace cook { namespace project { namespace info {


template <typename Info, typename Derived>
class ParserInternal : public gubg::parse::polymorphic_tree::Parser
{
public:
    ParserInternal(info::Parser<Info, Derived> * p) : p_(p) {}

    QString get_error() const
    {
        QString path;

        for(auto it = current_path().begin(); it != current_path().end(); ++it)
        {
            QString t = QString::fromStdString(*it);
            if(it != current_path().begin())
                path.append("/");

            path.append(t);
        }

        return QString("Badly formatted tree at [%1] (%2)").arg(path).arg(QString::fromStdString(gubg::parse::polymorphic_tree::to_string(this->error_code())));
    }

    void handle_error(gubg::parse::polymorphic_tree::ReturnCode code)
    {
        QString path;

        for(auto it = current_path().begin(); it != current_path().end(); ++it)
        {
            QString t = QString::fromStdString(*it);
            if(it != current_path().begin())
                path.append("/");

            path.append(t);
        }

        QString error = QString("Badly formatted tree at [%1] (%2)").arg(path).arg(QString::fromStdString(gubg::parse::polymorphic_tree::to_string(code)));
        p_->emit_error_occurred(error);
    }

     info::Parser<Info, Derived> * p_;
};

template <typename Info, typename Derived>
bool Parser<Info, Derived>::parse(Info & info, const QByteArray & input)
{
    ParserInternal<Info, Derived> p(this);

    static_cast<Derived *>(this)->initialize_(p, info);

    if (!p.process(input.toStdString()))
    {
        QString error = p.get_error();
        emit error_occured(error);

        return false;
    }

    return true;
}

} } }

#endif
