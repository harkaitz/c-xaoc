#!/bin/sh -e
##:
#h: Usage: ./test.sh show|test
#h:
#h: Execute tests.
#h:
#h:   2024-harka-16   : Execute Harkaitz's Tcl/Tk maze solution.
#h:
#h: Required scripts: getprj(1)
##:
xaoc_test() {
    local cmd="$1"
    shift
    case "${cmd}" in
        2024-harka-16) xaoc_test_harka_2024 "${cmd#2024-harka-}" ;;
        *) echo >&2 "error: Invalid argument: ${cmd}"; return 1  ;;
    esac
}
# --------------------------------------------------------------------
xaoc_test_harka_2024() {
    local aoc="$(getprj advent-of-code-2024-tcl)" pwd="$(pwd)"
    test -n "${aoc}"
    xaoc_get_AOC_INPUT_DIR
    cd "${aoc}"
    xaoc ./16.tcl
    cd "${pwd}"
}
# --------------------------------------------------------------------
xaoc_get_AOC_INPUT_DIR() {
    AOC_INPUT_DIR="${AOC_INPUT_DIR:-$(getprj advent-of-code-inputs)}"
    if test ! -n "${AOC_INPUT_DIR}"; then
        echo >&2 'error: Please set AOC_INPUT_DIR to a directory with AoC inputs.'
        echo >&2 '       Inputs should be named "YYYY/NN.data".'
        return 1
    fi
    export AOC_INPUT_DIR
}
# --------------------------------------------------------------------
if test -n "${BUILDDIR}"; then
    export PATH="${BUILDDIR}:${PATH}"
elif test -d .build; then
    export PATH="$(pwd)/.build:${PATH}"
fi
# --------------------------------------------------------------------
if test @"${SCRNAME:-$(basename "$0")}" = @"test.sh"; then
    case "${1}" in
        ''|-h|--help) sed -n 's/^ *#h: \{0,1\}//p' "$0";;
        *)            xaoc_test "$@"; exit 0;;
    esac
fi
