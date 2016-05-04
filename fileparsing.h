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

class TeamHandednessClass
{
	std::unordered_map<std::string, BaseballHandedness> m_playerHandedness;
	std::string m_pitcherName = "";
	bool m_hasDH = false;

	int m_leftCount = 0, m_rightCount = 0;

public:
	void AddPlayer(std::string name, BaseballHandedness handedness, BaseballPositionsEnum position)
	{
		//assert(m_playerHandedness.count(name) == 0);
		assert(handedness != BaseballHandedness::UNKNOWN);
		assert(position != BaseballPositionsEnum::MAX);

		if (position == BaseballPositionsEnum::PITCHER)
			m_pitcherName = name;
		else if (position == BaseballPositionsEnum::DH)
		{
			assert(!m_hasDH);
			m_hasDH = true;
		}

		if (handedness == BaseballHandedness::LEFT)
			m_leftCount++;
		else
			m_rightCount++;

		m_playerHandedness[name] = handedness;
	}

	BaseballHandedness GetHandedness(std::string name) const
	{
		return m_playerHandedness.at(name);
	}

	BaseballHandedness GetPitcherHandedness() const
	{
		return m_playerHandedness.at(m_pitcherName);
	}

	int GetCountLeftHanded() const
	{
		int totalCount = m_leftCount + m_rightCount;
		assert(totalCount == 11 && m_hasDH ||
			totalCount == 10);

		if (!m_hasDH || GetPitcherHandedness() == BaseballHandedness::RIGHT)
			return m_leftCount;
		else
			return m_leftCount - 1;
	}

	int GetCountRightHanded() const
	{
		int totalCount = m_leftCount + m_rightCount;
		assert(totalCount == 11 && m_hasDH ||
			totalCount == 10);

		if (!m_hasDH || GetPitcherHandedness() == BaseballHandedness::LEFT)
			return m_rightCount;
		else
			return m_leftCount;
	}

};

// this class should maybe just be a function, not a class
class CreatePositionListFromFileClass
{
	unordered_map<std::string, TeamHandednessClass> m_teamNameToTeamMap;
	unordered_map<std::string, std::string> m_teamNameToOpponentNameMap;

public:
		void asdf()
	{

	}

	void AddOpposingTeam(std::string teamName, std::string opponentTeamName)
	{
		if (m_teamNameToOpponentNameMap.count(teamName))
		{
			assert(m_teamNameToOpponentNameMap.at(opponentTeamName) == teamName);
			assert(m_teamNameToOpponentNameMap.at(teamName) == opponentTeamName);
			return;
		}

		m_teamNameToOpponentNameMap[teamName] = opponentTeamName;
		m_teamNameToOpponentNameMap[opponentTeamName] = teamName;
	}

	void AddPlayer(std::string teamName, std::string name, BaseballHandedness handedness, BaseballPositionsEnum position)
	{
		m_teamNameToTeamMap[teamName].AddPlayer(name, handedness, position);
	}

	std::vector<BaseballPositionClass> GetPositionList()
	{
		assert(m_teamNameToTeamMap.size() == m_teamNameToOpponentNameMap.size() / 2);

		std::vector<BaseballPositionClass> positionList;

		Json::Value root;
		ifstream file("C:\\Users\\Nathan\\Documents\\GitHub\\fantasysportsdrafter\\butts", std::ifstream::binary);
		file >> root;

		auto stringtohand = [](std::string handstring) {
			assert( handstring == "R" || 
					handstring == "L" ||
					handstring == "S");
			
			if (handstring == "R")
				return BaseballHandedness::RIGHT;
			else if (handstring == "L")
				return BaseballHandedness::LEFT;
			else if (handstring == "S")
				return BaseballHandedness::SWITCH;
		};

		// First pass: Get handedness, team infromation
		for (auto& playerJson : root)
		{
			std::string name = playerJson.get("name", "").asString();
			auto opponentTeam = playerJson.get("against", "").asString();
			auto hand = stringtohand(playerJson.get("hand", "").asString());
			auto team = playerJson.get("team", "").asString();
			auto position = StringToBaseballPositionEnum(playerJson.get("position", "").asString());

			AddOpposingTeam(team, opponentTeam);
			AddPlayer(team, name, hand, position);
		}


		// Second pass: Generate player points and add to a vector of BaseballPositionClass to return
		for (auto& playerJson : root)
		{
			std::string name = playerJson.get("name", "").asString();

			auto team = playerJson.get("team", "").asString();
			auto opposingTeam = m_teamNameToOpponentNameMap.at(team);

			// pitchers are in stats twice, once without order and once with
			// we only care about the one without order
			auto position = StringToBaseballPositionEnum(playerJson.get("position", "").asString());
			if (position == BaseballPositionsEnum::PITCHER && playerJson.get("order", "") == "")
			{
				// get count of opponent lefties and righties
				// consider what to do with switch hitters -- perhaps count them as whichever the pitcher is worse against?
			}
			else if (position != BaseballPositionsEnum::PITCHER)
			{
				auto opposingPitcherHand = m_teamNameToTeamMap.at(opposingTeam).GetPitcherHandedness();
				auto vsPitcherHand = opposingPitcherHand == BaseballHandedness::LEFT ? playerJson["vsLHS"] : playerJson["vsRHS"];

				// sad story: stats only has hits, not singles, so singles have to be calculated
				//GetHitterPoints(vsPitcherHand.get("singles", 0).asInt(), vsPitcherHand.get("2B", 0).asInt(), vsPitcherHand.get("3B", 0).asInt(), vsPitcherHand.get("HR", 0).asInt(),
				//	vsPitcherHand.get("singles", 0).asInt(), vsPitcherHand.get("singles", 0).asInt(), vsPitcherHand.get("singles", 0).asInt(), vsPitcherHand.get("singles", 0).asInt())

				// these are defined in util.h
				//double GetPitcherPoints(int IP, int SO, int win, int ERA, int HA, int BBA, int HB, int complete, int shutout, int nohitter)
				//{
				//	return IP * 2.25 + SO * 2 + win * 4 + ERA * -2 + HA * -0.6 + BBA * -0.6 + HB * -0.6 + complete * 2.5 + shutout * 2.5 + nohitter * 5;
				//}

				//double GetHitterPoints(int singles, int doubles, int triples, int HR, int RBI, int runs, int SB, int HPB, int BoB)
				//{
				//	return singles * 3 + doubles * 5 + triples * 8 + HR * 10 + RBI * 2 + runs * 2 + SB * 5 + HPB * 2 + SB * 5;
				//}



			}
		}

	}
};