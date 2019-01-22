//
// CS104 HW1 wordsearch test suits
//

#include <gtest/gtest.h>

#include <user_code_runner.h>
#include <random_generator.h>
#include <misc_utils.h>

#include <kwsys/SystemTools.hxx>
#include <kwsys/RegularExpression.hxx>

#include <string>
#include <fstream>

// constants for directions -- must match output in skeleton code
const std::string DIR_UP = "up";
const std::string DIR_DOWN = "down";
const std::string DIR_LEFT = "left";
const std::string DIR_RIGHT = "right";

// struct to represent a match for a word
struct WordMatch
{
	size_t _row, _col;
	std::string _direction;

	WordMatch(size_t row, size_t col, std::string const & direction):
	_row(row),
	_col(col),
	_direction(direction)
	{

	}

	bool operator<(WordMatch const & other) const
	{
		if(_row != other._row)
		{
			return _row < other._row;
		}
		else if(_col != other._col)
		{
			return _col < other._col;
		}
		else
		{
			return _direction < other._direction;
		}
	}
};

// make WordMatch printable
std::ostream & operator<<(std::ostream & ostream, WordMatch const & wordMatch)
{
	return ostream << "(" << wordMatch._row << ", " << wordMatch._col << ") -> " << wordMatch._direction;
}

// regular expressions for reading output
kwsys::RegularExpression wordMatchRegex(
		R"(([a-z]+) starts at \(row,col\) = \(([0-9]+),([0-9]+)\) and proceeds ([a-z]+))");
kwsys::RegularExpression doesNotOccurRegex("[a-z]+ does NOT occur");


// Tries running the user's wordsearch program on the given input, and reports whether it produced the expected output
testing::AssertionResult checkSearch(std::string const & testName, bool expectValid, std::set<WordMatch> expectedMatches, std::string const & search, std::string const & word, bool useValgrind = true)
{
	bool noMatches = expectedMatches.empty();

	// set up file structure
	std::string executablePath = WORDSEARCH_EXECUTABLE;
	std::string testFolder =  TEST_BINARY_DIR "/wordsearch_tests/testFiles/" + testName;
	kwsys::SystemTools::MakeDirectory(testFolder);
	std::string inputFilePath = testFolder + "/input.txt";
	std::string outputFilePath = testFolder + "/output.txt";

	// write wordsearch to file
	std::ofstream inputWriter(inputFilePath);
	inputWriter << search << std::endl;

	// run the program
	UserCodeRunner runner(testFolder, executablePath, {inputFilePath, word}, outputFilePath, useValgrind);
	testing::AssertionResult result = runner.execute();

	if(!expectValid)
	{
		if(!result)
		{
			// correctly detected as invalid
			return testing::AssertionSuccess();
		}
		else
		{
			return testing::AssertionFailure() << "User program reports grid is valid, when it is actually invalid";
		}
	}
	else if(!result)
	{
		return result;
	}

	// read output
	{
		std::ifstream outputFile(outputFilePath);

		if(!outputFile)
		{
			return testing::AssertionFailure() << "Couldn't open output file " << outputFilePath;
		}

		std::string outputLine;
		while(std::getline(outputFile, outputLine))
		{
			if(wordMatchRegex.find(outputLine))
			{
				//std::cout << "Parsing match: " << outputLine << std::endl;
				size_t detectedRow = static_cast<size_t>(std::stoi(wordMatchRegex.match(2)));
				size_t detectedCol = static_cast<size_t>(std::stoi(wordMatchRegex.match(3)));
				std::string detectedDirection = wordMatchRegex.match(4);
				//std::cout << "Parsed match at (" << detectedRow << ", " << detectedCol << ") with direction " << detectedDirection << std::endl;

				WordMatch detectedMatch(detectedRow, detectedCol, detectedDirection);

				std::set<WordMatch>::iterator matchIter = expectedMatches.find(detectedMatch);
				if(matchIter == expectedMatches.end())
				{
					// this match was not expected
					return testing::AssertionFailure() << "User program reported a match " << detectedMatch << ", which was not expected!";
				}
				else
				{
					// match was expected, remove from list of matches we're waiting for.
					expectedMatches.erase(matchIter);
				}

			}
			else if(doesNotOccurRegex.find(outputLine))
			{
				if(noMatches)
				{
					// correctly detected no matches
					return testing::AssertionSuccess();
				}

			}
			else if(!outputLine.empty())
			{
				std::cout << "Warning: unrecognized program output: " << outputLine;
			}

		}

		// finally, check that all the matches we expected have been printed.
		if(!expectedMatches.empty())
		{
			return testing::AssertionFailure() << "User program failed to find " << expectedMatches.size() << " correct matches: " << setToString(expectedMatches);
		}

	}

	return testing::AssertionSuccess();

};

TEST(WordSearch, TwoByOne_Right)
{
	ASSERT_TRUE(checkSearch("TwoByOne_Rightward",
		true,
        {WordMatch(0, 0, DIR_RIGHT)},
		"n o",
		"no"));
}

TEST(WordSearch, TwoByOne_Left)
{
	ASSERT_TRUE(checkSearch("TwoByOne_Leftward",
        true,
        {WordMatch(0, 1, DIR_LEFT)},
        "n o",
        "on"));
}

TEST(WordSearch, TwoByTwo_No)
{
	ASSERT_TRUE(checkSearch("TwoByTwo_No",
        true,
        {WordMatch(0, 0, DIR_RIGHT), WordMatch(0, 0, DIR_DOWN), WordMatch(1, 1, DIR_LEFT), WordMatch(1, 1, DIR_UP)},
        "n o\n"
	    "o n",
        "no"));
}

TEST(WordSearch, TwoByTwo_On)
{
	ASSERT_TRUE(checkSearch("TwoByTwo_On",
        true,
        {WordMatch(0, 1, DIR_LEFT), WordMatch(0, 1, DIR_DOWN), WordMatch(1, 0, DIR_RIGHT), WordMatch(1, 0, DIR_UP)},
        "n o\n"
        "o n",
        "on"));
}


const std::string mediumGrid =
		"e d c t w p k s q x q t l u i \n"
		"c x t h u o w b q b e m g e p \n"
		"u y u q r h r o u s v l l a h \n"
		"z e z b b d b r j z o n c b q \n"
		"p t q v e m x y o o m n n e x \n"
		"p i z z l r o x c b e l r l d \n"
		"u n s j l b a s i n r u a u p \n"
		"m c v x r z b n e h u k h b n \n"
		"h r f v b i l k t n c s u e f \n"
		"s e i b b o h l w a p s n y e \n"
		"g a d a n p x i p o g h n x n \n"
		"i s d w z d e n t s x x o o i \n"
		"d e t k f l u b q v z z p n d \n"
		"d s y i d h l o j t m i l k e \n"
		"y c f y e o b e v h y g p d e ";

TEST(WordSearch, MediumGrid_Basin)
{
	ASSERT_TRUE(checkSearch("MediumGrid_Basin",
        true,
        {WordMatch(6, 5, DIR_RIGHT)},
        mediumGrid,
        "basin"));
}

TEST(WordSearch, MediumGrid_Giddy)
{
	ASSERT_TRUE(checkSearch("MediumGrid_Giddy",
        true,
        {WordMatch(10, 0, DIR_DOWN)},
        mediumGrid,
        "giddy"));
}

TEST(WordSearch, MediumGrid_Hobbies)
{
	ASSERT_TRUE(checkSearch("MediumGrid_Hobbies",
        true,
        {WordMatch(9, 6, DIR_LEFT)},
        mediumGrid,
        "hobbies"));
}

TEST(WordSearch, MediumGrid_Remove)
{
	ASSERT_TRUE(checkSearch("MediumGrid_Remove",
        true,
        {WordMatch(6, 10, DIR_UP)},
        mediumGrid,
        "remove"));
}


TEST(WordSearch, NotFound_SmallGrid)
{
	ASSERT_TRUE(checkSearch("NotFound_SmallGrid",
	                        true,
	                        {},
	                        "n o\n"
	                        "o n",
	                        "ye"));
}


TEST(WordSearch, NotFound_FirstLetterFound)
{
	ASSERT_TRUE(checkSearch("NotFound_FirstLetterFound",
	                        true,
	                        {},
	                        "n o\n"
	                        "o n",
	                        "ne"));
}

