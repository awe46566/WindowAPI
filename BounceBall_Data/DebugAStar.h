#pragma once

struct PQNode
{
	bool operator>(const PQNode& other) const
	{
		int f = g + h;
		int otherF = other.g + other.h;

		if (f == otherF)
			return g > other.g;  // f가 같다면, 현재까지 비용 (g) 값이 작을수록 좋다.

		return f > otherF;
	}

	int g;
	int h;
	Cell pos;
};

class DebugAStar
{
public:
	void Update(float deltaTime);
	void Render(HDC renderTarget);


	map<Cell, int32> best;
	map<Cell, Cell> parent;
	set<Cell> closedList;
	map<Cell, PQNode> openList;

	bool _drawDebug = false;
};

