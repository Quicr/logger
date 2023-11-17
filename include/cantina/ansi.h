/*
 *  ansi.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This module will insert ANSI escape sequences into an output stream,
 *      allowing for colored text, bold text, italics, etc.
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

#pragma once

#include <ostream>

namespace cantina::ansi
{
    std::ostream & fg_black(std::ostream &os);
    std::ostream & fg_red(std::ostream &os);
    std::ostream & fg_green(std::ostream &os);
    std::ostream & fg_yellow(std::ostream &os);
    std::ostream & fg_blue(std::ostream &os);
    std::ostream & fg_magenta(std::ostream &os);
    std::ostream & fg_cyan(std::ostream &os);
    std::ostream & fg_white(std::ostream &os);
    std::ostream & fg_reset(std::ostream &os);
    std::ostream & bg_black(std::ostream &os);
    std::ostream & bg_red(std::ostream &os);
    std::ostream & bg_green(std::ostream &os);
    std::ostream & bg_yellow(std::ostream &os);
    std::ostream & bg_blue(std::ostream &os);
    std::ostream & bg_magenta(std::ostream &os);
    std::ostream & bg_cyan(std::ostream &os);
    std::ostream & bg_white(std::ostream &os);
    std::ostream & bg_reset(std::ostream &os);
    std::ostream & reset(std::ostream &os);
    std::ostream & bold_on(std::ostream &os);
    std::ostream & bold_off(std::ostream &os);
    std::ostream & dim_on(std::ostream &os);
    std::ostream & dim_off(std::ostream &os);
    std::ostream & italics_on(std::ostream &os);
    std::ostream & italics_off(std::ostream &os);
    std::ostream & underline_on(std::ostream &os);
    std::ostream & underline_off(std::ostream &os);
    std::ostream & blink_on(std::ostream &os);
    std::ostream & blink_off(std::ostream &os);
    std::ostream & inverse_on(std::ostream &os);
    std::ostream & inverse_off(std::ostream &os);
    std::ostream & invisible_on(std::ostream &os);
    std::ostream & invisible_off(std::ostream &os);
    std::ostream & strikethrough_on(std::ostream &os);
    std::ostream & strikethrough_off(std::ostream &os);
} // namespace cantina::ansi
