#pragma once
#include "Header.h"

void PrettyPrint(const BestNLineupsClass &bestNTeams)
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

void PrettyPrintTeamsWithDifferences(const BestNLineupsClass &bestNTeams, int numDifferentPlayers)
{
	std::set<LineupClass> bestTeams;
	auto bestTeamsCopy = bestNTeams.GetBestTeams();

	std::set<LineupClass> bestDifferentTeams;
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

	BestNLineupsClass bestNdifferentTeams(NUM_TEAMS);

	for (auto &team : bestDifferentTeams)
		bestNdifferentTeams.AddTeamIfGood(team);

	PrettyPrint(bestNdifferentTeams);
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