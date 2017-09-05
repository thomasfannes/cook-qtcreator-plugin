#ifndef HEADER_qook_plugin_QookPluginGlobal_hpp_ALREADY_INCLUDED
#define HEADER_qook_plugin_QookPluginGlobal_hpp_ALREADY_INCLUDED

#include <QtGlobal>

#if defined(QOOKPLUGIN_LIBRARY)
#  define QOOKPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QOOKPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
