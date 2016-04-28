#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <set>
#include <functional>
#include <array>
#include <algorithm>

using namespace std;

const int SALARY_MAX = 50000;

enum class BaseballPositionsEnum
{
	PITCHER,
	CATCHER,
	OUTFIELDER,
	SHORTSTOP,
	FIRSTBASE,
	SECONDBASE,
	THIRDBASE,
	MAX
};

std::string BaseballPositionEnumToString(BaseballPositionsEnum position)
{
	switch (position)
	{
	case BaseballPositionsEnum::PITCHER:
		return "P";
	case BaseballPositionsEnum::CATCHER:
		return "C";
	case BaseballPositionsEnum::OUTFIELDER:
		return "OF";
	case BaseballPositionsEnum::SHORTSTOP:
		return "SS";
	case BaseballPositionsEnum::FIRSTBASE:
		return "1B";
	case BaseballPositionsEnum::SECONDBASE:
		return "2B";
	case BaseballPositionsEnum::THIRDBASE:
		return "3B";
	default:
		throw 1;
	}
}

BaseballPositionsEnum StringToBaseballPositionEnum(std::string position)
{
	// @TODO consider players with multiple positions
	if (position == "P")
		return BaseballPositionsEnum::PITCHER;
	else if (position == "C")
		return BaseballPositionsEnum::CATCHER;
	else if (position == "OF")
		return BaseballPositionsEnum::OUTFIELDER;
	else if (position == "SS")
		return BaseballPositionsEnum::SHORTSTOP;
	else if (position == "1B")
		return BaseballPositionsEnum::FIRSTBASE;
	else if (position == "2B")
		return BaseballPositionsEnum::SECONDBASE;
	else if (position == "3B")
		return BaseballPositionsEnum::THIRDBASE;
	else
		throw 1;
}

// Generates all subsets of size r from n
// When finished, call Reset() to generate all subsets again
class NChooseRClass
{
	std::vector<bool> m_selectionsVector;
	int m_n, m_r;
	bool m_isFinished;

public:
	NChooseRClass(int _n, int _r)
		: m_n(_n), m_r(_r), m_isFinished(false)
	{
		m_selectionsVector.resize(m_n);
		std::fill(m_selectionsVector.begin() + m_n - m_r, m_selectionsVector.end(), true);
	}

	std::vector<int> GetNextPermutation()
	{
		if (m_isFinished)
			throw 1;

		std::vector<int> selections;

		for (int i = 0; i < m_n; i++)
		{
			if (m_selectionsVector[i])
				selections.push_back(i);
		}

		m_isFinished = !std::next_permutation(m_selectionsVector.begin(), m_selectionsVector.end());
		return selections;
	}

	bool IsFinished() const
	{
		return m_isFinished;
	}

	void Reset()
	{
		m_isFinished = false;
	}
};

class BaseballPlayerClass
{
	float m_points;
	int m_salary;

	float m_pointsPerSalary;

	BaseballPositionsEnum m_position;

	std::string m_name;

public:
	BaseballPlayerClass();
	BaseballPlayerClass(const std::string &name, float points, int salary, BaseballPositionsEnum position) :
		m_name(name), m_points(points), m_salary(salary), m_position(position), m_pointsPerSalary(points / salary) { }

	float GetPoints() const { return m_points; }
	int GetSalary() const { return m_salary; }
	float GetPointsPerSalary() const { return m_pointsPerSalary; }
	BaseballPositionsEnum GetPositionFromPlayer() const { return m_position; }
};

// Holds results of a player subset from a position
struct PositionResultsStruct
{
	float points = 0;
	int salary = 0;
	vector<int> selections;
	BaseballPositionsEnum position;

	PositionResultsStruct(float _points, int _salary, const vector<int> &_selections, BaseballPositionsEnum _position)
		: points(_points), salary(_salary), selections(_selections), position(_position) { }
};

// Holds all possible players in a position
// Returns subsets of players from GetNextPermutation()
class BaseballPositionClass
{
	// order matters here in case of std::move of players
	NChooseRClass m_chooser;
	vector<BaseballPlayerClass> m_players;
	bool m_isFinished;

	BaseballPositionsEnum m_position;

	float m_maxPoints = 0;
	int m_minSalary = 0;

	const int m_numPlayersFromPositionInFinalTeam;

	void PruneLowestPointsPerSalary(double percentageOfAverageToPrune)
	{
		double positionSalaryTotal = 0, positionPointsTotal = 0;
		for (auto iter = m_players.begin(); iter != m_players.end(); iter++)
		{
			positionSalaryTotal += iter->GetSalary();
			positionPointsTotal += iter->GetPoints();
		}

		const double averagePointsPerSalary = positionPointsTotal / positionSalaryTotal;

		std::remove_if(m_players.begin(), m_players.end(),
			[&](const BaseballPlayerClass &player)
		{
			return player.GetPointsPerSalary() < 0.5 * percentageOfAverageToPrune;
		});
	}

public:
	BaseballPositionClass(const std::vector<BaseballPlayerClass> &players, int r)
		: m_players(players), m_chooser(players.size(), r), m_numPlayersFromPositionInFinalTeam(r)
	{
		PruneLowestPointsPerSalary(0.5);

		std::priority_queue<float> highestPointsPerPlayer;
		for (const auto &player : m_players)
			highestPointsPerPlayer.push(player.GetPoints());

		for (int i = 0; i < r; i++)
		{
			m_maxPoints += highestPointsPerPlayer.top();
			highestPointsPerPlayer.pop();
		}

		std::priority_queue<int, std::vector<int>, std::greater<int>> lowestSalaryPerPlayer;
		for (int i = 0; i < r; i++)
		{
			m_minSalary += lowestSalaryPerPlayer.top();
			lowestSalaryPerPlayer.pop();
		}
	}

	PositionResultsStruct GetNextPermutation()
	{
		if (m_isFinished)
			throw 1;

		float points = 0;
		int salary = 0;

		auto playerSelection = m_chooser.GetNextPermutation();
		for (int i = 0; i < playerSelection.size(); i++)
		{
			points += m_players[playerSelection[i]].GetPoints();
			salary += m_players[playerSelection[i]].GetSalary();
		}

		m_isFinished = m_chooser.IsFinished();

		return PositionResultsStruct(points, salary, std::move(playerSelection), m_position);
	}

	float GetMaxPoints() const { return m_maxPoints; }
	int GetMinSalary() const { return m_minSalary; }
	BaseballPositionsEnum GetPosition() const { return m_position; }

	bool IsFinished() const { return m_isFinished; }

	void Reset()
	{
		m_isFinished = false;
		m_chooser.Reset();
	}

};

class TeamClass
{
	std::unordered_map<BaseballPositionsEnum, std::set<int>>  playerIndices;

	int m_teamSalary;
	float m_teamPoints;

public:
	void AddPlayers(PositionResultsStruct positionResults)
	{
		m_teamSalary += positionResults.salary;
		m_teamPoints += positionResults.points;

		auto &positionPlayerSet = playerIndices[positionResults.position];
		for (auto selection : positionPlayerSet)
			positionPlayerSet.insert(selection);
	}

	void RemovePlayers(PositionResultsStruct positionResults)
	{
		playerIndices.erase(positionResults.position);

		m_teamSalary -= positionResults.salary;
		m_teamPoints -= positionResults.points;
	}

	int GetTeamSalary() const { return m_teamSalary; }
	float GetTeamPoints() const { return m_teamPoints; }
};

bool operator < (const TeamClass &lhs, const TeamClass &rhs)
{
	return lhs.GetTeamPoints() < rhs.GetTeamPoints();
}

bool operator > (const TeamClass &lhs, const TeamClass &rhs)
{
	return lhs.GetTeamPoints() > rhs.GetTeamPoints();
}

class BestNTeamsClass
{
	int m_numTeams;
	std::priority_queue<TeamClass, std::vector<TeamClass>, std::greater<TeamClass>> bestTeams;

public:
	BestNTeamsClass(int n) :
		m_numTeams(n) { }

	void AddTeamIfGood(const TeamClass &team)
	{
		if (bestTeams.size() < m_numTeams)
			bestTeams.push(team);

		if (bestTeams.top() < team)
		{
			bestTeams.pop();
			bestTeams.push(team);
		}
	}

	float GetWorstPointsFromBestTeams() const
	{
		if (bestTeams.size() < m_numTeams)
			return 0;

		return bestTeams.top().GetTeamPoints();
	}
};