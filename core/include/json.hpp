//
// Created by taylor-santos on 1/4/2022 at 20:30.
//

#pragma once

#include "location.hh"

void
pos_to_json(const yy::position &pos, std::ostream &os);

void
loc_to_json(const yy::location &loc, std::ostream &os);

std::string
escape_string(const std::string &str);
