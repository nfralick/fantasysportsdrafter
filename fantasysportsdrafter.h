#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <array>
#include <algorithm>
#include <cassert>

using namespace std;

const int SALARY_MAX = 50000;
const double PERCENTAGE_TO_PRUNE = 0.01;
const int NUM_TEAMS = 5000;

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
		return "SP";
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
	if (position == "SP")
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

int NumPlayersInPosition(BaseballPositionsEnum position)
{
	switch (position)
	{
	case BaseballPositionsEnum::PITCHER:
		return 2;
	case BaseballPositionsEnum::CATCHER:
		return 1;
	case BaseballPositionsEnum::OUTFIELDER:
		return 3;
	case BaseballPositionsEnum::SHORTSTOP:
		return 1;
	case BaseballPositionsEnum::FIRSTBASE:
		return 1;
	case BaseballPositionsEnum::SECONDBASE:
		return 1;
	case BaseballPositionsEnum::THIRDBASE:
		return 1;
	default:
		throw 1;
	}
}

// Generates all subsets of size r from n
// When finished, call Reset() to generate all subsets again
class NChooseRClass
{
	std::vector<bool> m_selectionsVector;
	int m_n = 0, m_r = 0;
	bool m_isFinished = false;

public:

	void Initialize(int n, int r)
	{
		m_n = n;
		m_r = r;

		m_selectionsVector.resize(m_n);
		std::fill(m_selectionsVector.begin() + m_n - m_r, m_selectionsVector.end(), true);
	}

	std::vector<int> GetNextPermutation()
	{
		assert(m_n != 0 && m_r != 0);

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
	double m_points;
	int m_salary;

	double m_pointsPerSalary;

	BaseballPositionsEnum m_position;

	std::string m_name;

public:
	BaseballPlayerClass(const std::string &name, double points, int salary, BaseballPositionsEnum position) :
		m_name(name), m_points(points), m_salary(salary), m_position(position), m_pointsPerSalary(points / salary) { }

	double GetPoints() const { return m_points; }
	int GetSalary() const { return m_salary; }
	double GetPointsPerSalary() const { return m_pointsPerSalary; }
	BaseballPositionsEnum GetPositionFromPlayer() const { return m_position; }
	std::string GetName() const { return m_name; }
};

bool operator < (const BaseballPlayerClass& lhs, const BaseballPlayerClass &rhs)
{
	if (lhs.GetPositionFromPlayer() < rhs.GetPositionFromPlayer())
		return true;
	
	else if (lhs.GetPositionFromPlayer() == rhs.GetPositionFromPlayer())
		return lhs.GetName() < rhs.GetName();

	return false;
}

bool operator == (const BaseballPlayerClass& lhs, const BaseballPlayerClass &rhs)
{
	return lhs.GetPositionFromPlayer() == rhs.GetPositionFromPlayer() &&
		   lhs.GetName() == rhs.GetName();
}

bool operator > (const BaseballPlayerClass& lhs, const BaseballPlayerClass &rhs)
{
	return !(lhs < rhs) && !(lhs == rhs);
}

// Holds results of a player subset from a position
struct PositionResultsClass
{
private:
	std::vector<BaseballPlayerClass> m_players;
	double points = 0;
	int salary = 0;

public:
	PositionResultsClass(const std::vector<BaseballPlayerClass> &players)
		: m_players(players) 
		{
			for (const auto &player : players)
			{
				points += player.GetPoints();
				salary += player.GetSalary();
			}
		}

	double GetPoints() const { return points; }
	int GetSalary() const { return salary; }
	std::vector<BaseballPlayerClass> GetPlayers() const { return m_players; }
};

// Holds all possible players in a position
// Returns subsets of players from GetNextPermutation()
class BaseballPositionClass
{
	NChooseRClass m_chooser;
	vector<BaseballPlayerClass> m_players; // MAINTAIN THE ORDER OF THIS YOU SHIT (once permutations have started)
	bool m_isFinished;

	BaseballPositionsEnum m_position;

	double m_maxPoints = 0;
	int m_minSalary = 0;

	int m_numPlayersFromPositionInFinalTeam;

	void PruneLowestPointsPerSalary()
	{
		double positionSalaryTotal = 0, positionPointsTotal = 0;
		for (auto iter = m_players.begin(); iter != m_players.end(); iter++)
		{
			positionSalaryTotal += iter->GetSalary();
			positionPointsTotal += iter->GetPoints();
		}

		const double averagePointsPerSalary = positionPointsTotal / positionSalaryTotal;

		auto new_end = 
			std::remove_if(m_players.begin(), m_players.end(),
				[averagePointsPerSalary](const BaseballPlayerClass &player)
				{
					bool asdf = player.GetPointsPerSalary() < PERCENTAGE_TO_PRUNE * averagePointsPerSalary;
					return asdf;
				});

		m_players.erase(new_end, m_players.end());
		m_players.shrink_to_fit();
	}

public:
	BaseballPositionClass(const std::vector<BaseballPlayerClass> &players, BaseballPositionsEnum position)
		: m_players(players), m_position(position), m_numPlayersFromPositionInFinalTeam(NumPlayersInPosition(position))
	{
		PruneLowestPointsPerSalary();

		sort(m_players.begin(), m_players.end(), [](const BaseballPlayerClass & lhs, const BaseballPlayerClass& rhs) {return lhs.GetPoints() < rhs.GetPoints(); });

		m_chooser.Initialize(m_players.size(), m_numPlayersFromPositionInFinalTeam);

		std::priority_queue<double> highestPointsPerPlayer;
		for (const auto &player : m_players)
			highestPointsPerPlayer.push(player.GetPoints());

		for (int i = 0; i < m_numPlayersFromPositionInFinalTeam; i++)
		{
			m_maxPoints += highestPointsPerPlayer.top();
			highestPointsPerPlayer.pop();
		}

		std::priority_queue<int, std::vector<int>, std::greater<int>> lowestSalaryPerPlayer;
		for (const auto &player : m_players)
			lowestSalaryPerPlayer.push(player.GetSalary());

		for (int i = 0; i < m_numPlayersFromPositionInFinalTeam; i++)
		{
			m_minSalary += lowestSalaryPerPlayer.top();
			lowestSalaryPerPlayer.pop();
		}
	}

	PositionResultsClass GetNextPermutation()
	{
		if (m_isFinished)
			throw 1;

		std::vector<BaseballPlayerClass> permutationOfPlayers;

		auto playerSelection = m_chooser.GetNextPermutation();
		for (int i = 0; i < playerSelection.size(); i++)
			permutationOfPlayers.push_back(m_players[playerSelection[i]]);

		m_isFinished = m_chooser.IsFinished();

		return PositionResultsClass(std::move(permutationOfPlayers));
	}

	double GetMaxPoints() const { return m_maxPoints; }
	int GetMinSalary() const { return m_minSalary; }
	BaseballPositionsEnum GetPosition() const { return m_position; }

	// these can be prettier but w/e i'm a genius
	std::string IndexToPlayerName(int index) const { return m_players[index].GetName(); }
	double IndexToPlayerPoints(int index) const { return m_players[index].GetPoints(); }
	int IndexToPlayerSalary(int index) const { return m_players[index].GetSalary(); }
	double IndexToPlayerPointsPerSalary(int index) const { return m_players[index].GetPointsPerSalary(); }

	bool IsFinished() const { return m_isFinished; }

	void Reset()
	{
		m_isFinished = false;
		m_chooser.Reset();
	}

};

auto BaseballPlayerClassHasher = [](const BaseballPlayerClass& player) {return std::hash<std::string>()(player.GetName());  };

class TeamClass
{
	std::set<BaseballPlayerClass> m_players;

	int m_teamSalary = 0;
	double m_teamPoints = 0;

public:
	void AddPlayers(const PositionResultsClass &positionResults)
	{
		m_teamSalary += positionResults.GetSalary();
		m_teamPoints += positionResults.GetPoints();

		for (const auto &player : positionResults.GetPlayers())
			m_players.insert(player);
	}

	void RemovePlayers(const PositionResultsClass &positionResults)
	{
		for (const auto &player : positionResults.GetPlayers())
			m_players.erase(player);

		m_teamSalary -= positionResults.GetSalary();
		m_teamPoints -= positionResults.GetPoints();
	}

	int GetTeamSalary() const { return m_teamSalary; }
	double GetTeamPoints() const { return m_teamPoints; }
	std::set<BaseballPlayerClass> GetPlayers() const { return m_players; }
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

		else if (bestTeams.top() < team)
		{
			bestTeams.pop();
			bestTeams.push(team);
		}
	}

	double GetWorstPointsFromBestTeams() const
	{
		if (bestTeams.size() < m_numTeams)
			return 0;

		return bestTeams.top().GetTeamPoints();
	}

	std::priority_queue<TeamClass> GetBestTeams() const
	{
		auto bestTeamsCopy = bestTeams;

		std::priority_queue<TeamClass> returnSet;
		while (bestTeamsCopy.size())
		{
			returnSet.push(std::move(bestTeamsCopy.top()));
			bestTeamsCopy.pop();
		}

		return returnSet;
	}
};