#!/bin/sh

RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
BLUE="\e[34m"
TURQUOISE="\e[36m"
WHITE="\e[0m"
GRAY="\e[90m"

bold=$(tput bold)
normal=$(tput sgr0)

TOTAL_RUN=0
TOTAL_FAIL=0

ref_file_out=/tmp/.ref_file_out
ref_file_err=/tmp/.ref_file_err
my_file_out=/tmp/.my_file_out
my_file_err=/tmp/.my_file_err

if [ "$1" = "clean" ]; then
    find . -name "*.output" -delete
    find . -name "*.diff" -delete
    exit 0
fi

run_test()
{
    [ -e "$1" ] || echo "Missing test file $1" 1>&2
    success=true
    TOTAL_RUN=$((TOTAL_RUN+1))

    test_file="$1";
    shift

    echo -ne "$BLUE-->> ${WHITE}${bold}$test_file${normal}...$WHITE"
    bash --posix "$test_file" $@ > $ref_file_out 2> $ref_file_err
    REF_CODE=$?

    ../../src/42sh "$test_file" $@ > $my_file_out 2> $my_file_err
    MY_CODE=$?

    diff  -u $ref_file_out $my_file_out > $test_file.diff
    DIFF_CODE=$?

    if [ $REF_CODE != $MY_CODE ]; then
        echo -ne "\n$RED RETURN code $MY_CODE but expected $REF_CODE $WHITE\n"
        success=false
    fi
    if [ $DIFF_CODE != 0 ]; then
        echo -ne "\n$RED STDOUT (diff in output)$WHITE\n"
        success=false
    fi

    #if { [ -s $ref_file_err ] && [ ! -s $my_file_err ]; } ||
        #{ [ ! -s $ref_file_err ] && [ -s $my_file_err ]; }; then
            #echo -ne "$RED STDERR (diff in err)$WHITE\n"
        #success=false
    #fi

    if [ -s $ref_file_err ] && [ ! -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Expected Error output, no received$WHITE\n"
        success=false
    fi

    if [ ! -s $ref_file_err ] && [ -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Received error, not expected$WHITE\n"
        cat $my_file_err
        success=false
    fi

    if $success; then
        echo -e "$GREEN OK$WHITE"
        rm -f $test_file.diff
    else
       #echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.output)" ] && echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.diff)" ] && echo -ne "$RED (vim $(realpath $1.diff))$WHITE\n\n"
       #cat $(realpath $1.diff)
       echo -ne "$TURQUOISE \nContent of the testing file is :$WHITE\n"
       cat $test_file
       TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi
    #echo -ne "$WHITE"
    #echo "${bold}________________________________________________________________${normal}"
}

run_test_stdin()
{
    [ -e "$1" ] || echo "Missing test file $1" 1>&2
    success=true
    TOTAL_RUN=$((TOTAL_RUN+1))

    echo -ne "$BLUE-->> ${WHITE}${bold}$1${normal}...$WHITE"
    bash --posix < "$1" > $ref_file_out 2> $ref_file_err
    REF_CODE=$?

    ../../src/42sh < "$1" > $my_file_out 2> $my_file_err
    MY_CODE=$?

    diff  -u $ref_file_out $my_file_out > $1.diff
    DIFF_CODE=$?

    if [ $REF_CODE != $MY_CODE ]; then
        echo -ne "\n$RED RETURN code $MY_CODE but expected $REF_CODE $WHITE\n"
        success=false
    fi
    if [ $DIFF_CODE != 0 ]; then
        echo -ne "\n$RED STDOUT (diff in output)$WHITE\n"
        success=false
    fi

    #if { [ -s $ref_file_err ] && [ ! -s $my_file_err ]; } ||
        #{ [ ! -s $ref_file_err ] && [ -s $my_file_err ]; }; then
            #echo -ne "$RED STDERR (diff in err)$WHITE\n"
        #success=false
    #fi

    if [ -s $ref_file_err ] && [ ! -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Expected Error output, no received$WHITE\n"
        success=false
    fi

    if [ ! -s $ref_file_err ] && [ -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Received error, not expected$WHITE\n"
        cat $my_file_err
        success=false
    fi

    if $success; then
        echo -e "$GREEN OK$WHITE"
        rm -f $1.diff
    else
       #echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.output)" ] && echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.diff)" ] && echo -ne "$RED (vim $(realpath $1.diff))$WHITE\n\n"
       #cat $(realpath $1.diff)
       echo -ne "$TURQUOISE \nContent of the testing file is :$WHITE\n"
       cat $1
       TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi
    #echo -ne "$WHITE"
    #echo "${bold}________________________________________________________________${normal}"
}


run_test_redirect()
{
     [ -e "$1" ] || echo "Missing test file $1" 1>&2
    success=true
    TOTAL_RUN=$((TOTAL_RUN+1))

    rm -rf tmp 2> /dev/null
    mkdir tmp/
    cd tmp

    echo -ne "$BLUE-->> ${WHITE}${bold}$1${normal}...$WHITE"
    bash --posix "../$1" > $ref_file_out 2> $ref_file_err
    rm -rf *.txt 2> /dev/null
    bash --posix "../$1" > /dev/null 2> /dev/null
    REF_CODE=$?
    REF_REDIR=$(cat * 2> /dev/null) 2> /dev/null

    rm  "$1".diff 2> /dev/null
    rm -rf *.txt 2> /dev/null

    ../../../src/42sh "../$1" > $my_file_out 2> $my_file_err
    rm -rf *.txt 2> /dev/null
    ../../../src/42sh "../$1" > /dev/null 2> /dev/null
    MY_CODE=$?
    MY_REDIR="$(cat * 2> /dev/null)" 2> /dev/null

    diff -u $ref_file_out $my_file_out >> $1.diff
    DIFF_CODE=$?

    (diff -u <(echo "$REF_REDIR") <(echo "$MY_REDIR")) >> $1.diff

    if [ $REF_CODE != $MY_CODE ]; then
        echo -ne "\n$RED RETURN code $MY_CODE but expected $REF_CODE $WHITE\n"
        success=false
    fi
    if [ $DIFF_CODE != 0 ]; then
        echo -ne "\n$RED STDOUT (diff in output)$WHITE\n"
        success=false
    fi

    if [ -s $ref_file_err ] && [ ! -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Expected Error output, no received$WHITE\n"
        success=false
    fi

    if [ ! -s $ref_file_err ] && [ -s $my_file_err ]; then
        echo -ne "\n$RED STDERR Received error, not expected$WHITE\n"
        cat $my_file_err
        success=false
    fi

    if [ "$REF_REDIR" != "$MY_REDIR" ]; then
        echo -ne "\n$RED REDIR (diff in redir files) $WHITE\n"
        success=false
    fi

    if $success; then
        echo -e "$GREEN OK$WHITE"
        rm -f $1.diff
    else
       #echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.output)" ] && echo -ne "$RED (vim $(realpath $1.output))$WHITE\n\n"
       #[ -s "$(realpath $1.diff)" ] && echo -ne "$RED (vim $(realpath $1.diff))$WHITE\n\n"
       #cat $(realpath $1.diff)
       echo -ne "$TURQUOISE \nContent of the testing file is :$WHITE\n"
       cat "../$1"
       TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi
    #echo -ne "$WHITE"
    #echo "${bold}________________________________________________________________${normal}"

    cd - 2> /dev/null > /dev/null
}

run_category() {
    PREVIOUSDIR="$PWD"
    cd $1
    source ./testsuite.sh
    #cd - >/dev/null
    cd "$PREVIOUSDIR"
}

VAR="."
if [ "$#" -gt 0 ]; then
    VAR="$1"
fi;

#bugged when args

run_testsuite() {
    for category in $@; do
        [ ${category} = "." ] && continue
        echo -e "\n${bold}$TURQUOISE================================================================"
        printf " $WHITE%-36s $TURQUOISE%s\n" "${bold}${category#*/}"
        echo -e "$TURQUOISE================================================================\n${normal}"

        run_category $category
    done
}

run_testsuite $(find "$VAR" -type d -a ! -name "." -a ! -name "echo" -a ! -name "true_false" -a ! -name "tmp" -a ! -name "path")
PERCENT_SUCCES=$(((TOTAL_RUN - TOTAL_FAIL) * 100 / TOTAL_RUN))

echo -e "$BLUE==========================="
echo -e "$WHITE RECAP: $([ $PERCENT_SUCCES = 100 ] && echo $GREEN || echo $RED) $PERCENT_SUCCES%"
echo -e "$WHITE SCORE: $((TOTAL_RUN - TOTAL_FAIL))/$TOTAL_RUN"
echo -e "$BLUE===========================$WHITE"

rm -f $ref_file_out $my_file_out $ref_file_err $ref_file_out
#find . -name "*.diff" -delete
