/*
 *  syslog_interface.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      This implements a SyslogInterface class, which was originally written to
 *      facilitate unit testing.  It still serves that purpose, but may also
 *      simplify interacting with alternate system logging functions on
 *      different platforms.
 *
 *  Portability Issues:
 *      None.
 *
 *  License:
 *      BSD 2-Clause License
 *
 *      Copyright (c) 2022, Cisco Systems
 *      All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      1. Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *
 *      2. Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *      IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *      TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *      PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//  SPDX-FileCopyrightText: 2022 Cisco Systems, Inc.
//  SPDX-License-Identifier: BSD-2-Clause

#ifdef LOGGER_SYSLOG_ENABLED
#include <syslog.h>
#include <cstdarg>
#endif
#include "cantina/syslog_interface.h"

namespace cantina
{

/*
 *  openlog()
 *
 *  Description:
 *      If syslog is enabled on the system, this function will call the
 *      system's openlog() function().
 *
 *  Parameters:
 *      ident [in]
 *          String to prepend to every syslog message.
 *
 *      option [in]
 *          Flags which control the operation of openlog() and subsequent calls
 *          to syslog().
 *
 *      facility [in]
 *          Default facility to use if not specified in subsequent calls to
 *          syslog().
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void SyslogInterface::openlog([[maybe_unused]] const char *ident,
                              [[maybe_unused]] int option,
                              [[maybe_unused]] int facility)
{
#ifdef LOGGER_SYSLOG_ENABLED
    ::openlog(ident, option, facility);
#endif
}

/*
 *  closelog()
 *
 *  Description:
 *      Closes the syslog interface opened via openlog().
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void SyslogInterface::closelog(void)
{
#ifdef LOGGER_SYSLOG_ENABLED
    ::closelog();
#endif
}

/*
 *  syslog()
 *
 *  Description:
 *      If syslog is enabled on the system, this function will call the
 *      system's openlog() function().
 *
 *  Parameters:
 *      priority [in]
 *          The priority associated with this message.
 *
 *      format [in]
 *          A format specifier like what is used with printf().
 *
 *      ... [in]
 *          Variadic arguments passed to the syslog() function.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void SyslogInterface::syslog([[maybe_unused]] int priority,
                             [[maybe_unused]] const char *format,
                             ...)
{
#ifdef LOGGER_SYSLOG_ENABLED
    std::va_list arguments;
    va_start(arguments, format);
    ::vsyslog(priority, format, arguments);
    va_end(arguments);
#endif
}

} // namespace cantina
