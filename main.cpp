#include "fantasysportsdrafter.h"

void PositionRecurserHelper(vector<BaseballPositionClass> &positions, int currentPositionIndex, BestNTeamsClass &bestNTeams, TeamClass &currentTeam, const vector<int> &minSalaryFromIndex, const vector<float> maxPointsFromIndex)
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

void PositionRecurser(vector<BaseballPositionClass> &positions)
{
	// sort positions by...something?
	// average salary?
	// max salary?
	// num players chosen from position?

	// compute minimum salaries for all position
	// compute maximum points for all position
	vector<int> minSalaryFromIndex(positions.size());
	for (auto i = positions.size() - 1; i >= 0; i--)
	{
		auto minSalary = (i == positions.size() - 1) ? positions[i].GetMinSalary() : positions[i].GetMinSalary() + positions[i + 1].GetMinSalary();
		minSalaryFromIndex[i] = minSalary;
	}
	minSalaryFromIndex.push_back(0);

	vector<float> maxPointsFromIndex(positions.size());
	for (auto i = positions.size() - 1; i >= 0; i--)
	{
		auto maxPoints = (i == positions.size() - 1) ? positions[i].GetMaxPoints() : positions[i].GetMaxPoints() + positions[i + 1].GetMaxPoints();
		maxPointsFromIndex[i] = maxPoints;
	}
	maxPointsFromIndex.push_back(0);

	// make team results thing
	BestNTeamsClass bestNTeams(100);
	TeamClass currentTeam;

	PositionRecurserHelper(positions, 0, bestNTeams, currentTeam, minSalaryFromIndex, maxPointsFromIndex);
}

int main()
{

}