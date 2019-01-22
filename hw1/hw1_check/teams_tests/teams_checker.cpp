//
// Created by cs104 on 1/13/19.
//

#include "teams_checker.h"

#include <user_code_runner.h>
#include <misc_utils.h>

#include <kwsys/SystemTools.hxx>
#include <kwsys/RegularExpression.hxx>

#include <fstream>
#include <utility>
#include <set>
#include <sstream>

// type for one possible team assignment
typedef std::pair<std::set<std::string>, std::set<std::string>> TeamAssignment;

// make TeamAssignment printable
std::ostream & operator<<(std::ostream & ostream, TeamAssignment const & teamAssignment)
{
	return ostream << "(T1->" << setToString(teamAssignment.first) << ", T2->" << setToString(teamAssignment.second) << ")";
}

// Calls itself two times, with the given person on one team or the other.
// If all players have been assigned to a team, that assignment is added.
void getTeamAssignmentsRecursive(std::set<TeamAssignment> & assignments, TeamAssignment currAssignment, std::set<std::string> namesRemaining, size_t numNames)
{
	if(namesRemaining.size() == 0)
	{
		// hit bottom of recursion
		assignments.insert(currAssignment);
		return;
	}

	std::string nextName = *namesRemaining.begin();
	namesRemaining.erase(namesRemaining.begin());

	// if more people can be added to team 2, make a case where we do that
	if(currAssignment.second.size() < numNames/2)
	{
		TeamAssignment altAssignment = currAssignment;
		altAssignment.second.insert(nextName);
		getTeamAssignmentsRecursive(assignments, altAssignment, namesRemaining, numNames);
	}

	// if more people can be added to team 1, make a case were we do that
	if(currAssignment.first.size() < numNames/2)
	{
		currAssignment.first.insert(nextName);
		getTeamAssignmentsRecursive(assignments, currAssignment, namesRemaining, numNames);
	}
}

// Get a set of all team assignments for the given set of names
std::set<TeamAssignment> getAllTeamAssignments(std::set<std::string> const & names)
{
	std::set<TeamAssignment> assignments;

	getTeamAssignmentsRecursive(assignments, TeamAssignment{}, names, names.size());

	return assignments;
}

kwsys::RegularExpression combinationNumber(R"(Combination ([0-9]+))");

kwsys::RegularExpression errorMessage("Error");

testing::AssertionResult checkTeams(std::string const &testName, bool expectValid, std::set<std::string> names, std::string const &inputFile, bool useValgrind)
{
	if(names.size() % 2 != 0)
	{
		return testing::AssertionFailure() << "Error: number of names must be divisible by 2.";
	}

	// set up file structure
	std::string executablePath = TEAMS_EXECUTABLE; // this definition points to the hw1_teams executable in the build tree
	std::string testFolder =  TEST_BINARY_DIR "/teams_tests/testFiles/" + testName;
	kwsys::SystemTools::MakeDirectory(testFolder);
	std::string inputFilePath = testFolder + "/input.txt";
	std::string outputFilePath = testFolder + "/output.txt";

	// write teams list to file
	std::ofstream inputWriter(inputFilePath);
	inputWriter << inputFile << std::endl;

	// run the program
	UserCodeRunner runner(testFolder, executablePath, {inputFilePath}, outputFilePath, useValgrind);
	runner.setCheckExitCode(false);
	testing::AssertionResult result = runner.execute();

	if(!result)
	{
		return result;
	}

	// get the correct output
	std::set<TeamAssignment> correctAssignments = getAllTeamAssignments(names);
	std::set<TeamAssignment> assignmentsRemaining = correctAssignments; // set of assignments we still need to see


	// now, check the output
	{
		std::ifstream outputFile(outputFilePath);

		if(!outputFile)
		{
			return testing::AssertionFailure() << "Couldn't open output file " << outputFilePath;
		}

		std::string outputLine;
		while(std::getline(outputFile, outputLine))
		{
			if(combinationNumber.find(outputLine))
			{
				int comboNumber = std::stoi(combinationNumber.match(1));

				// read in the next 2 lines and parse them
				std::string team1;
				std::string team2;
				std::getline(outputFile, team1);
				std::getline(outputFile, team2);
				std::set<std::string> team1Members;
				std::set<std::string> team2Members;

				std::istringstream team1Stream{ team1 };
				std::istringstream team2Stream{ team2 };

				std::string name;
				team1Stream >> name;
				if(name != "T1:")
				{
					return testing::AssertionFailure() << "At combination " << comboNumber << ": Error in formatting of team 1." << std::endl;
				}
				while(team1Stream >> name)
				{
					team1Members.emplace(name);
				}
				team2Stream >> name;
				if(name != "T2:")
				{
					return testing::AssertionFailure() << "At combination " << comboNumber << ": Error in formatting of team 2." << std::endl;
				}
				while(team2Stream >> name)
				{
					team2Members.emplace(name);
				}

				TeamAssignment thisAssignment = std::make_pair(team1Members, team2Members);

				auto searchResult = assignmentsRemaining.find(thisAssignment);

				if(searchResult == assignmentsRemaining.end())
				{
					if(correctAssignments.find(thisAssignment) == correctAssignments.end())
					{
						return testing::AssertionFailure() << "At combination " << comboNumber << ": Assignment " << thisAssignment << " is not a valid assignment, but was printed by the user program.";
					}
					else
					{
						return testing::AssertionFailure() << "At combination " << comboNumber << ": Assignment " << thisAssignment << " was printed multiple times by the user program.";
					}
				}
				else
				{
					// valid assignment, remove from list of remaining assignments
					assignmentsRemaining.erase(searchResult);
				}

			}
			else if(errorMessage.find(outputLine))
			{
				if(expectValid)
				{
					return testing::AssertionFailure() << "Program reported that the input was invalid, but it actually was valid!";
				}
				else
				{
					// correctly detected an error
					return testing::AssertionSuccess();
				}
			}
			else if(!outputLine.empty())
			{
				std::cout << "Warning: unrecognized output line \"" << outputLine << "\"" << std::endl;
			}
		}
	}

	// Now, all of the assignments should have been seen and removed from the list
	if(!assignmentsRemaining.empty())
	{
		return testing::AssertionFailure() << "Error: some team assignments were not printed: " << setToString(assignmentsRemaining);
	}

	return testing::AssertionSuccess();
}
