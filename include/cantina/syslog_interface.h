/*
 *  syslog_interface.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      This defines a SyslogInterface class, which was originally written to
 *      facilitate unit testing. It still serves that purpose, but also
 *      simplifies interacting with alternate system logging functions
 *      on different platforms.
 *
 *  Portability Issues:
 *      None.
 *
 */

#pragma once

#ifndef _WIN32
#include <syslog.h>
#endif

namespace cantina
{

// Syslog interface declaration
class SyslogInterface
{
    public:
        virtual ~SyslogInterface() {}

        virtual void openlog(const char *ident, int option, int facility)
        {
#ifndef _WIN32
            ::openlog(ident, option, facility);
#endif
        }

        virtual void closelog(void)
        {
#ifndef _WIN32
            ::closelog();
#endif
        }

        template<typename ...Args> void syslog(int priority,
                                               const char *format,
                                               Args ...args)
        {
#ifndef _WIN32
            ::syslog(priority, format, args...);
#endif
        }
};

} // namespace cantina
