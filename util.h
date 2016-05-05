#pragma once
#include "Header.h"

enum class BaseballHandedness
{
	LEFT,
	RIGHT,
	SWITCH,
	UNKNOWN
};

enum class BaseballPositionsEnum
{
	PITCHER,
	CATCHER,
	OUTFIELDER,
	SHORTSTOP,
	FIRSTBASE,
	SECONDBASE,
	THIRDBASE,
	DH,
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
	if (position == "SP" || position == "P")
		return BaseballPositionsEnum::PITCHER;
	else if (position == "C")
		return BaseballPositionsEnum::CATCHER;
	else if (position == "OF" || position == "LF" || position == "RF" || position == "CF")
		return BaseballPositionsEnum::OUTFIELDER;
	else if (position == "SS")
		return BaseballPositionsEnum::SHORTSTOP;
	else if (position == "1B")
		return BaseballPositionsEnum::FIRSTBASE;
	else if (position == "2B")
		return BaseballPositionsEnum::SECONDBASE;
	else if (position == "3B")
		return BaseballPositionsEnum::THIRDBASE;
	else if (position == "DH")
		return BaseballPositionsEnum::DH;
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

double GetPitcherPoints(int games, int IP, int SO, int win, int ERA, int HA, int BBA, int HB, int complete, int shutout, int nohitter)
{
	return IP * 2.25 + SO * 2 + win * 4 + ERA * -2 + HA * -0.6 + BBA * -0.6 + HB * -0.6 + complete * 2.5 + shutout * 2.5 + nohitter * 5;
}

double GetHitterPoints(int singles, int doubles, int triples, int HR, int RBI, int runs, int SB, int HPB, int BoB)
{
	return singles * 3 + doubles * 5 + triples * 8 + HR * 10 + RBI * 2 + runs * 2 + SB * 5 + HPB * 2 + SB * 5 + BoB * 2;
}

double GetPitcherPoints(Json::Value vsBatterHandJson)
{
	// TODO some needed stats aren't in the thing, ike IP, wins, games complete, shutouts, nohitters, 
	// IP and wins are most important, probably don't need the others (well okay maybe games complete)
	// so we need to tell tim "hey buddy"
	int games = vsBatterHandJson.get("G", 0).asInt();
	int IP = vsBatterHandJson.get("IP", 0).asInt();
	int SO = vsBatterHandJson.get("SO", 0).asInt();
	int ERA = vsBatterHandJson.get("R", 0).asInt();
	int HA = vsBatterHandJson.get("H", 0).asInt();
	int BBA = vsBatterHandJson.get("BB", 0).asInt();
	int HB = vsBatterHandJson.get("HPB", 0).asInt();
	//int SO = vsBatterHandJson.get("IP", -1).asInt();
	//int SO = vsBatterHandJson.get("IP", -1).asInt();
	//int SO = vsBatterHandJson.get("IP", -1).asInt();
	//int SO = vsBatterHandJson.get("IP", -1).asInt();


	return 0;
	//return GetPitcherPoints(vsBatterHandJson.get("IP", 0).asInt(), vsBatterHandJson.get("IP", 0).asInt(), vsBatterHandJson.get("IP", 0).asInt(), )
}



double GetHitterPoints(Json::Value vsPitcherHandJson)
{
	int singles = vsPitcherHandJson.get("H", 0).asInt() - vsPitcherHandJson.get("2B", 0).asInt() - vsPitcherHandJson.get("3B", 0).asInt() - vsPitcherHandJson.get("HR", 0).asInt();
	return GetHitterPoints(singles, vsPitcherHandJson.get("2B", 0).asInt(), vsPitcherHandJson.get("3B", 0).asInt(), vsPitcherHandJson.get("HR", 0).asInt(),
		vsPitcherHandJson.get("RBI", 0).asInt(), vsPitcherHandJson.get("R", 0).asInt(), 
		vsPitcherHandJson.get("SB", 0).asInt(), vsPitcherHandJson.get("HPB", 0).asInt(), vsPitcherHandJson.get("BB", 0).asInt());
}