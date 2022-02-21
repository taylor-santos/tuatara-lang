//
// Created by taylor-santos on 2/12/2022 at 23:42.
//

#pragma once

#define POS(FILE, LINE, COL)  \
    yy::position {            \
        (FILE), (LINE), (COL) \
    }
#define LOC(FILE, BEGIN_LINE, BEGIN_COL, END_LINE, END_COL)                        \
    yy::location {                                                                 \
        POS((FILE), (BEGIN_LINE), (BEGIN_COL)), POS((FILE), (END_LINE), (END_COL)) \
    }
