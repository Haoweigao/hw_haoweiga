# CMake generated Testfile for 
# Source directory: /home/cs104/hw_haoweiga/hw1_check/teams_tests
# Build directory: /home/cs104/hw_haoweiga/hw1_check/teams_tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Teams.TwoPeople "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=Teams.TwoPeople")
add_test(Teams.FourPeople "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=Teams.FourPeople")
add_test(Teams.EightPeople "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=Teams.EightPeople")
add_test(Teams.TwelvePeople "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=Teams.TwelvePeople")
add_test(TeamsError.NoNames "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=TeamsError.NoNames")
add_test(TeamsError.TooFewNames_Even "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=TeamsError.TooFewNames_Even")
add_test(TeamsStress.4x12Person "/home/cs104/hw_haoweiga/hw1_check/teams_tests/teams_tests" "--gtest_filter=TeamsStress.4x12Person")
