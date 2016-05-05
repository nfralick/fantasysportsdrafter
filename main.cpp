#include "Header.h"

void PositionRecurserHelper(vector<BaseballPositionClass> &positions, int currentPositionIndex, BestNLineupsClass &bestNTeams, LineupClass &currentTeam, const vector<int> &minSalaryFromIndex, const vector<double> maxPointsFromIndex)
{
	// prune!
	if (currentTeam.GetTeamSalary() + minSalaryFromIndex[currentPositionIndex] > SALARY_MAX ||
		currentTeam.GetTeamPoints() + maxPointsFromIndex[currentPositionIndex] < bestNTeams.GetWorstPointsFromBestTeams())
		return;

	// we have results!
	if (currentPositionIndex == positions.size())
	{
		bestNTeams.AddTeamIfGood(currentTeam);
		return;
	}

	while (!positions[currentPositionIndex].IsFinished())
	{
		auto positionResults = positions[currentPositionIndex].GetNextPermutation();
		currentTeam.AddPlayers(positionResults);
		PositionRecurserHelper(positions, currentPositionIndex + 1, bestNTeams, currentTeam, minSalaryFromIndex, maxPointsFromIndex);
		currentTeam.RemovePlayers(positionResults);
	}

	positions[currentPositionIndex].Reset();

	// All done!
	if (currentPositionIndex == 0)
	{
		return;
	}
}

BestNLineupsClass PositionRecurser(vector<BaseballPositionClass> &positions)
{
	// Get pruning information
	vector<int> minSalaryFromIndex(positions.size() + 1);
	for (int i = positions.size() - 1; i >= 0; i--)
		minSalaryFromIndex[i] = positions[i].GetMinSalary() + minSalaryFromIndex[i + 1];

	vector<double> maxPointsFromIndex(positions.size() + 1);
	for (int i = positions.size() - 1; i >= 0; i--)
		maxPointsFromIndex[i] = positions[i].GetMaxPoints() + maxPointsFromIndex[i + 1];

	// make team results thing
	BestNLineupsClass bestNTeams(NUM_TEAMS);
	LineupClass currentTeam;

	PositionRecurserHelper(positions, 0, bestNTeams, currentTeam, minSalaryFromIndex, maxPointsFromIndex);

	return bestNTeams;
}

int main()
{

	CreatePositionListFromFileClass asdf;
	asdf.GetPositionList();


	std::set<std::string> playerNames = GetPlayerNamesFromFile("C:\\Users\\Nathan\\Downloads\\playernames429.txt");
	auto positionList = ReadFromCsv("C:\\Users\\Nathan\\Downloads\\DKSalaries (8).csv", playerNames);

	auto bestNTeams = PositionRecurser(positionList);

	//PrettyPrint(bestNTeams);
	PrettyPrintTeamsWithDifferences(bestNTeams, 6);
}