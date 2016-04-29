#include "fantasysportsdrafter.h"
#include <fstream>
#include <sstream>

vector<BaseballPositionClass> ReadFromCsv(std::string filepath);

void PrettyPrint(const BestNTeamsClass &bestNTeams, const vector<BaseballPositionClass> &positions)
{
	ofstream file("C:\\Users\\Nathan\\Downloads\\DKSalaries (5) team output.txt");
	auto bestTeamsInOrder = bestNTeams.GetBestTeams();

	auto GetPlayerNameFromPositionAndIndex = [&](BaseballPositionsEnum position, int index)
	{
		std::string name;
		float expectedPoints;
		int salary;
		float pointsPerSalary;
		for (auto iter = positions.begin(); iter != positions.end(); iter++)
		{
			if (iter->GetPosition() == position)
			{
				name = iter->IndexToPlayerName(index);
				expectedPoints = iter->IndexToPlayerPoints(index);
				salary = iter->IndexToPlayerSalary(index);
				pointsPerSalary = iter->IndexToPlayerPointsPerSalary(index);

				file << BaseballPositionEnumToString(position) << ": " << name << " AvgPts: " << expectedPoints << " Salary: " << salary << endl;
				return;
			}
		}
	};

	int i = 0;
	while (bestTeamsInOrder.size())
	{
		file << "Team: " << ++i << endl;
		auto bestTeamCandidate = bestTeamsInOrder.top();

		auto bestPlayers = bestTeamCandidate.GetPlayers();

		for (const auto &positionPlayerSetPair : bestPlayers)
		{
			for (const auto& playerIndex : positionPlayerSetPair.second)
				GetPlayerNameFromPositionAndIndex(positionPlayerSetPair.first, playerIndex);

		}
		
		file << "\n------------------------------------------------------\n\n";

		bestTeamsInOrder.pop();
	}


}

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

BestNTeamsClass PositionRecurser(vector<BaseballPositionClass> &positions)
{
	// sort positions by...something?
	// average salary?
	// max salary?
	// num players chosen from position?

	// compute minimum salaries for all position
	// compute maximum points for all position
	vector<int> minSalaryFromIndex(positions.size() + 1);
	for (int i = positions.size() - 1; i >= 0; i--)
		minSalaryFromIndex[i] = positions[i].GetMinSalary() + minSalaryFromIndex[i + 1];

	vector<float> maxPointsFromIndex(positions.size() + 1);
	for (int i = positions.size() - 1; i >= 0; i--)
		maxPointsFromIndex[i] = positions[i].GetMaxPoints() + maxPointsFromIndex[i + 1];

	// make team results thing
	BestNTeamsClass bestNTeams(NUM_TEAMS);
	TeamClass currentTeam;

	PositionRecurserHelper(positions, 0, bestNTeams, currentTeam, minSalaryFromIndex, maxPointsFromIndex);

	return bestNTeams;
}

int main()
{
	auto positionList = ReadFromCsv("C:\\Users\\Nathan\\Downloads\\DKSalaries (5).csv");

	auto bestNTeams = PositionRecurser(positionList);

	PrettyPrint(bestNTeams, positionList);
}



vector<BaseballPositionClass> ReadFromCsv(std::string filepath)
{
	auto RemoveQuotes = [](std::string &inputString)
		{
			inputString.erase(inputString.begin());
			inputString.erase(inputString.end() - 1);
		};

	unordered_map<BaseballPositionsEnum, vector<BaseballPlayerClass>> players;
	ifstream csvfile(filepath);

	string line;
	getline(csvfile, line);
	/*
	BaseballPlayerClass(const std::string &name, float points, int salary, BaseballPositionsEnum position) :
	m_name(name), m_points(points), m_salary(salary), m_position(position), m_pointsPerSalary(points / salary) { }*/

	string position;
	string name;
	int salary;
	string skip;
	float points;

	while (getline(csvfile, line))
	{

		stringstream linestream(line);
		string cell;
		int i = 0;
		while (getline(linestream, cell, ','))
		{
			//if (i != 2 && i != 4) RemoveQuotes(cell);
			stringstream cellstream(cell);
			if (i == 0)
				cellstream >> position;
			else if (i == 1)
			{
				string asdf;
				cellstream >> asdf;
				name = asdf + " ";
				cellstream >> asdf;
				name += asdf;
			}
			else if (i == 2)
				cellstream >> salary;
			else if (i == 4)
				cellstream >> points;

			i++;
		}
		try
		{
			auto positionEnum = StringToBaseballPositionEnum(position);
			BaseballPlayerClass player(name, points, salary, positionEnum);
			players[positionEnum].push_back(std::move(player));
		}
		catch (...)
		{

		}
	}

	vector<BaseballPositionClass> returnValue;
	for (auto& playersPerPosition : players)
	{
		auto positionEnum = playersPerPosition.second.begin()->GetPositionFromPlayer();
		BaseballPositionClass baseballPosition(std::move(playersPerPosition.second), positionEnum);
		returnValue.push_back(std::move(baseballPosition));
	}

	return returnValue;
}