/*
 *  syslog_interface.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      This defines a SyslogInterface class, which was originally written to
 *      facilitate unit testing.  It still serves that purpose, but may also
 *      simplify interacting with alternate system logging functions on
 *      different platforms.
 *
 *  Portability Issues:
 *      None.
 *
 */

#pragma once

namespace cantina
{

// Syslog interface declaration
class SyslogInterface
{
    public:
        SyslogInterface() = default;
        virtual ~SyslogInterface() = default;

        virtual void openlog(const char *ident, int option, int facility);

        virtual void closelog(void);

        virtual void syslog(int priority, const char *format, ...);
};

} // namespace cantina
