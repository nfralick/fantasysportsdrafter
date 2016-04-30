#include "fantasysportsdrafter.h"
#include <fstream>
#include <sstream>

std::set<std::string> GetPlayerNamesFromFile(std::string filepath);

vector<BaseballPositionClass> ReadFromCsv(std::string filepath, std::set<std::string> playersInLineup);

void PrettyPrint(const BestNTeamsClass &bestNTeams, const vector<BaseballPositionClass> &positions)
{
	ofstream file("C:\\Users\\Nathan\\Downloads\\DKSalaries (6) team output.txt lineup 429523");
	auto bestTeamsInOrder = bestNTeams.GetBestTeams();

	auto GetPlayerNameFromPositionAndIndex = [&](BaseballPositionsEnum position, int index) -> double
	{
		std::string name;
		double expectedPoints;
		int salary;
		double pointsPerSalary;
		for (auto iter = positions.begin(); iter != positions.end(); iter++)
		{
			if (iter->GetPosition() == position)
			{
				name = iter->IndexToPlayerName(index);
				expectedPoints = iter->IndexToPlayerPoints(index);
				salary = iter->IndexToPlayerSalary(index);
				pointsPerSalary = iter->IndexToPlayerPointsPerSalary(index);

				file << BaseballPositionEnumToString(position) << ": " << name << " AvgPts: " << expectedPoints << " Salary: " << salary << endl;
				return expectedPoints;
			}
		}

		return 0;
	};

	int i = 0;
	while (bestTeamsInOrder.size())
	{
		file << "Team: " << ++i << endl;
		auto bestTeamCandidate = bestTeamsInOrder.top();

		auto bestPlayers = bestTeamCandidate.GetPlayers();

		double totalTeamPoints = 0;
		for (const auto &positionPlayerSetPair : bestPlayers)
		{
			for (const auto& playerIndex : positionPlayerSetPair.second)
			{
				totalTeamPoints += GetPlayerNameFromPositionAndIndex(positionPlayerSetPair.first, playerIndex);
			}

		}
		file << "TOTAL POINTS: " << totalTeamPoints << endl;

		file << "\n------------------------------------------------------\n\n";

		bestTeamsInOrder.pop();
	}
}

void PrettyPrintTeamsWithDifferences(const BestNTeamsClass &bestNTeams, const vector<BaseballPositionClass> &positions, int numDifferentPlayers)
{
	//std::set<TeamClass> bestTeams;
	//auto bestTeamsCopy = bestNTeams.GetBestTeams();

	//while (bestTeamsCopy.size());
	//{
	//	bestTeams.insert(std::move(bestTeamsCopy.top()));
	//	bestTeamsCopy.pop();
	//}

	//std::set<TeamClass> bestDifferentTeams;
	//for (auto &team : bestTeams)
	//{
	//	bool different = true;
	//	for (auto &differentTeam : bestDifferentTeams)
	//	{
	//		std::vector<
	//		if(std::set_difference())
	//	}
	//}

}

void PositionRecurserHelper(vector<BaseballPositionClass> &positions, int currentPositionIndex, BestNTeamsClass &bestNTeams, TeamClass &currentTeam, const vector<int> &minSalaryFromIndex, const vector<double> maxPointsFromIndex)
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

	vector<double> maxPointsFromIndex(positions.size() + 1);
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
	std::set<std::string> playerNames = GetPlayerNamesFromFile("C:\\Users\\Nathan\\Downloads\\playernames429.txt");
	auto positionList = ReadFromCsv("C:\\Users\\Nathan\\Downloads\\DKSalaries (6).csv", playerNames);

	auto bestNTeams = PositionRecurser(positionList);

	PrettyPrint(bestNTeams, positionList);
}


std::set<std::string> GetPlayerNamesFromFile(std::string filepath)
{
	std::set<std::string> playernames;

	ifstream lineupfile(filepath);

	string line;
	while (getline(lineupfile, line))
	{
		stringstream linestream(line);
		string name;
		string asdf;
		linestream >> asdf >> name;
		name += " ";
		linestream >> asdf;
		name += asdf;
		playernames.insert(name);
	}

	return playernames;
}


vector<BaseballPositionClass> ReadFromCsv(std::string filepath, std::set<std::string> playersInLineup)
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
	BaseballPlayerClass(const std::string &name, double points, int salary, BaseballPositionsEnum position) :
	m_name(name), m_points(points), m_salary(salary), m_position(position), m_pointsPerSalary(points / salary) { }*/

	string position;
	string name;
	int salary;
	string skip;
	double points;

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
			if (playersInLineup.size() && playersInLineup.count(name)
				|| playersInLineup.empty())
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

	sort(returnValue.begin(), returnValue.end(),
			[](const BaseballPositionClass& lhs, const BaseballPositionClass &rhs)
			{
				return NumPlayersInPosition(lhs.GetPosition()) > NumPlayersInPosition(rhs.GetPosition());
		});

	return returnValue;
}