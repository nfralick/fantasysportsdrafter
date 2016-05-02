#include "fantasysportsdrafter.h"
#include <fstream>
#include <sstream>

std::set<std::string> GetPlayerNamesFromFile(std::string filepath);

vector<BaseballPositionClass> ReadFromCsv(std::string filepath, std::set<std::string> playersInLineup);

void PrettyPrint(const BestNTeamsClass &bestNTeams)
{
	ofstream file("C:\\Users\\Nathan\\Downloads\\DKSalaries (6) team output.txt lineup 430617");
	auto bestTeamsInOrder = bestNTeams.GetBestTeams();

	int i = 0;
	while (bestTeamsInOrder.size())
	{
		file << "Team: " << ++i << endl;
		auto bestTeamCandidate = bestTeamsInOrder.top();

		auto bestPlayers = bestTeamCandidate.GetPlayers();

		double totalTeamPoints = 0;
		for (const auto &player : bestPlayers)
		{
			file << BaseballPositionEnumToString(player.GetPositionFromPlayer()) << ": " << player.GetName() << " AvgPts: " << player.GetPoints() << " Salary: " << player.GetSalary() << endl;
			totalTeamPoints += player.GetPoints();
		}
		file << "TOTAL POINTS: " << totalTeamPoints << endl;

		file << "\n------------------------------------------------------\n\n";

		bestTeamsInOrder.pop();
	}
}

void PrettyPrintTeamsWithDifferences(const BestNTeamsClass &bestNTeams, int numDifferentPlayers)
{
	std::set<TeamClass> bestTeams;
	auto bestTeamsCopy = bestNTeams.GetBestTeams();

	std::set<TeamClass> bestDifferentTeams;
	while (bestTeamsCopy.size())
	{
		auto team = bestTeamsCopy.top();
		team.GetPlayers();

		bestTeamsCopy.pop();
		auto bestTeamsPlayers = team.GetPlayers();

		bool different = true;
		for (auto &differentTeam : bestDifferentTeams)
		{
			auto differentTeamPlayers = differentTeam.GetPlayers();

			std::vector<BaseballPlayerClass> setdifference;
			std::set_difference(differentTeamPlayers.begin(), differentTeamPlayers.end(), bestTeamsPlayers.begin(), bestTeamsPlayers.end(), std::inserter(setdifference, setdifference.begin()));

			if (setdifference.size() < numDifferentPlayers)
				different = false;
			
			if (!different)
				break;
		}

		if (different)
			bestDifferentTeams.insert(team);
	}

	BestNTeamsClass bestNdifferentTeams(NUM_TEAMS);

	for(auto &team : bestDifferentTeams)
		bestNdifferentTeams.AddTeamIfGood(team);

	PrettyPrint(bestNdifferentTeams);
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
	// Get pruning information
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
	auto positionList = ReadFromCsv("C:\\Users\\Nathan\\Downloads\\DKSalaries (8).csv", playerNames);

	auto bestNTeams = PositionRecurser(positionList);

	//PrettyPrint(bestNTeams);
	PrettyPrintTeamsWithDifferences(bestNTeams, 6);
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
			if (i != 2 && i != 4) RemoveQuotes(cell);
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

	// Sort so that highest salaries are at the beginning of the vector
	// This helps with pruning (test with sample size of 1 showed ~322x speed increase vs the reverse)
	sort(returnValue.begin(), returnValue.end(),
		[](const BaseballPositionClass& lhs, const BaseballPositionClass &rhs)
		{
			return lhs.GetMinSalary() > rhs.GetMinSalary();
		});

	return returnValue;
}