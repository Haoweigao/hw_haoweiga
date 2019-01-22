//
// Test suite for HW1 Teams
//

#include "teams_checker.h"

#include <user_code_runner.h>
#include <random_generator.h>

#include <kwsys/SystemTools.hxx>

#include <fstream>

TEST(Teams, TwoPeople)
{
	ASSERT_TRUE(checkTeams("TwoPeople",
							true,
					        std::set<std::string>({"Fred", "James"}),
							"2\n"
							"Fred James "));
}


TEST(Teams, FourPeople)
{
	ASSERT_TRUE(checkTeams("FourPeople",
	                       true,
	                       std::set<std::string>({"Fred", "James", "Lisa", "Jane"}),
	                       "4\n"
	                       "Fred Lisa James Jane"));
}

TEST(Teams, EightPeople)
{
	ASSERT_TRUE(checkTeams("EightPeople",
	                       true,
	                       std::set<std::string>({"Fred", "James", "Lisa", "Jane", "Cain", "Ed", "Ned", "Frieda"}),
	                       "8\n"
	                       "Fred Frieda Lisa James Jane\n"
						   "Ned Ed Cain"));
}


TEST(Teams, TwelvePeople)
{
	ASSERT_TRUE(checkTeams("TwelvePeople",
	                       true,
	                       std::set<std::string>({"Fred", "James", "Lisa", "Jane", "Cain", "Ed", "Ned", "Frieda", "Bob", "Marlene", "Alf", "Skylar"}),
	                       "12\n"
	                       "Skylar\n"
	                       "Fred Frieda Lisa James Jane\n"
	                       "Ned Ed Cain\n"
	                       "Bob Marlene\n"
	                       "Alf\n"));
}


TEST(TeamsError, NoNames)
{
	ASSERT_TRUE(checkTeams("NoNames",
	                       false,
	                       std::set<std::string>({}),
	                       "2\n"));
}

TEST(TeamsError, TooFewNames_Even)
{
	ASSERT_TRUE(checkTeams("TooFewNames_Even",
	                       false,
	                       std::set<std::string>({}),
	                       "4\n"
						   "A B"));
}


TEST(TeamsStress, 4x12Person)
{
	size_t numTrials = 4;
	size_t numPeople = 12;
	size_t nameLength = 15;
	RandomSeed masterSeed = 454;

	std::vector<RandomSeed> trialSeeds = makeRandomSeedVector(numTrials, masterSeed);

	for(RandomSeed seed : trialSeeds)
	{
		std::set<std::string> names = makeRandomAlphaStringSet(numPeople, seed, nameLength);

		std::ostringstream inputFile;
		inputFile << names.size() << std::endl;
		for(std::string name : names)
		{
			inputFile << " " << name;
		}
		inputFile << std::endl;

		ASSERT_TRUE(checkTeams("10x5Person", true, names, inputFile.str()));
	}
}