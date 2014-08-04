#ifndef _UPPUTILS_H_INCLUDED_
#define _UPPUTILS_H_INCLUDED_

#include <QString>

inline std::string qs2utf8s(const QString& qs)
{
    return std::string((const char *)qs.toUtf8());
}


#endif /* _UPPUTILS_H_INCLUDED_ */
