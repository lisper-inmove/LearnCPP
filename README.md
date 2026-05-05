# Support for googletest

    1. git clone git@github.com:google/googletest.git
    2. cd googletest
    3. mkdir build && cd build && cmake .. && make && sudo make install

# use makefile

    1. make list-all-tests: Display all test-cases
       ```$ make list-all-tests
       All test cases in LearnCPP_Test:
       ========================================
       Tester.
       QtSupportTest
       ```

    2. run all test-cases in file
       ```shell
       $ make CH1_basic_operation        <- filename in tests directory
       ========================================
       Running all tests in CH1_basic_operation.cc
       ========================================
       Note: Google Test filter = *NamedWindowTest*:*ReadWriteTest*:*OpenVideo*
       [==========] Running 3 tests from 1 test suite.
       [----------] Global test environment set-up.
       [----------] 3 tests from Tester
       [ RUN      ] Tester.NamedWindowTest
       [       OK ] Tester.NamedWindowTest (976 ms)
       [ RUN      ] Tester.ReadWriteTest
       [       OK ] Tester.ReadWriteTest (837 ms)
       [ RUN      ] Tester.OpenVideo
       Video Info:
           FPS: 29
           Total Frames: 713
           Resolution: 720x540
           Press 'q' or 'ESC' to quit
           [       OK ] Tester.OpenVideo (1353 ms)
           [----------] 3 tests from Tester (3167 ms total)
       [----------] Global test environment tear-down
       [==========] 3 tests from 1 test suite ran. (3167 ms total)
       [  PASSED  ] 3 tests.
       ```
    3. run a test-case

    ```
    $ make test-pattern-NamedWindowTest
    Running tests matching pattern: NamedWindowTest
    Note: Google Test filter = *NamedWindowTest*
    [==========] Running 1 test from 1 test suite.
    [----------] Global test environment set-up.
    [----------] 1 test from Tester
    [ RUN      ] Tester.NamedWindowTest
    [       OK ] Tester.NamedWindowTest (781 ms)
    [----------] 1 test from Tester (781 ms total)

    [----------] Global test environment tear-down
    [==========] 1 test from 1 test suite ran. (781 ms total)
    [  PASSED  ] 1 test.

    ```
