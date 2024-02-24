run_test file_does_not_exist
chmod 000 no_open
run_test no_open
chmod 444 no_open
run_test_stdin simple_echo
