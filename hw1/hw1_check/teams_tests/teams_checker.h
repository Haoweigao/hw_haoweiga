//
// Auto-checker for HW1 Teams
//

#ifndef CS104_HW1_TEST_SUITE_TEAMS_CHECKER_H
#define CS104_HW1_TEST_SUITE_TEAMS_CHECKER_H

#include <gtest/gtest.h>

/**
 * Check that the teams program produces the right output when given a certain input file
 * @param testName Name of the test
 * @param expectValid Whether this input file is expected to produce valid output
 * @param names A set of the names in the input file
 * @param inputFile Text to pass as the input file
 * @param useValgrind Whether or not to run the program using Valgrind
 * @return
 */
testing::AssertionResult checkTeams(std::string const & testName, bool expectValid, std::set<std::string> names, std::string const & inputFile, bool useValgrind = true);

#endif //CS104_HW1_TEST_SUITE_TEAMS_CHECKER_H
