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
 */

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
