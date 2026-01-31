#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <conio.h>
#include <Windows.h>

#define MAX_WIDHT 98
#define MAX_HEIGHT 28


const char* const FIELD =
"##################################################################################################\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"#                                                                                                #\n"
"##################################################################################################";

enum class SnakeDirection {
	none,
	up,
	down,
	right = 4,
	left
};

struct Vec2i {
	int x, y;
};

inline static bool operator==(const Vec2i& _Left, const Vec2i& _Right) {
	return (_Left.x == _Right.x) && (_Left.y == _Right.y);
}

class Snake {
private:
	unsigned int m_Length;
	std::atomic<SnakeDirection> m_Direction;
	std::atomic<bool> m_Alive;
	std::thread m_DirectionUpdatingTheread;
	std::vector<Vec2i> m_BodyPos;
	Vec2i m_Apple;
	unsigned int m_Points;

public:
	Snake(int argc, char** argv) {
		m_Points = 0;
		m_Direction = SnakeDirection::up;
		m_Alive = true;

		m_DirectionUpdatingTheread = std::thread{ &Snake::direction_updating, this };

		for (int i = 0; i < 4; ++i) {
			m_BodyPos.push_back({0, -i});
		}

		put_new_apple();
	}

	~Snake() {
		m_DirectionUpdatingTheread.join();
	}

	void update() {
		SnakeDirection sd = m_Direction.load();
		Vec2i head_pos = m_BodyPos.at(0);
		Vec2i last_head_pos = head_pos;
		Vec2i last_pos{};

		if (std::abs(head_pos.x) == (MAX_WIDHT / 2 - 1) || std::abs(head_pos.y) == (MAX_HEIGHT / 2 - 1))
			m_Alive = false;

		if (sd == SnakeDirection::up)
			m_BodyPos.at(0).y += 1;
		else if (sd == SnakeDirection::down)
			m_BodyPos.at(0).y -= 1;
		else if (sd == SnakeDirection::right)
			m_BodyPos.at(0).x += 1;
		else if (sd == SnakeDirection::left)
			m_BodyPos.at(0).x -= 1;

		for (unsigned int i = 1; i < m_BodyPos.size(); ++i) {
			last_pos = m_BodyPos.at(i);

			if (head_pos == last_pos) {
				m_Alive = false;
			}

			m_BodyPos.at(i) = last_head_pos;
			last_head_pos = last_pos;
		}

		if (head_pos == m_Apple) {
			m_Points += 1;
			m_BodyPos.push_back(last_pos);
			put_new_apple();
		}
	}

	void draw() {
		char field[(MAX_WIDHT + 1) * MAX_HEIGHT + 1] = {};

		std::memcpy(field, FIELD, (MAX_WIDHT + 1) * MAX_HEIGHT);

		for (auto& skahe_pos : m_BodyPos) {
			field[((MAX_HEIGHT / 2) - skahe_pos.y) * (MAX_WIDHT + 1) + ((MAX_WIDHT / 2) + skahe_pos.x)] = 'O';
		}

		field[((MAX_HEIGHT / 2) - m_Apple.y) * (MAX_WIDHT + 1) + ((MAX_WIDHT / 2) + m_Apple.x)] = '@';

		system("cls");
		printf("\tLength: %llu\t|\tPoints: %ld\n", m_BodyPos.size(), m_Points);
		puts(field);
	}

	inline bool alive() {
		return m_Alive.load();
	}
	
	inline void sleep() {
		std::this_thread::sleep_for(std::chrono::milliseconds(200 / (m_Points + 1)));
	}

private:
	void direction_updating() {
		SnakeDirection sd[256] = {};

		// By default
		{
			sd['d'] = sd['D'] = SnakeDirection::right;
			sd['a'] = sd['A'] = SnakeDirection::left;
			sd['w'] = sd['W'] = SnakeDirection::up;
			sd['s'] = sd['S'] = SnakeDirection::down;
		}

		SnakeDirection sd2[256] = {};

		// Arrows: right, left, up, down
		{
			sd2[77] = SnakeDirection::right;
			sd2[75] = SnakeDirection::left;
			sd2[72] = SnakeDirection::up;
			sd2[80] = SnakeDirection::down;
		}

		while (m_Alive.load()) {
			if (_kbhit()) {
				int key0 = _getch();

				SnakeDirection new_direction = SnakeDirection::none;

				if (key0 == 224)
					new_direction = sd2[_getch()];
				else
					new_direction = sd[key0];

				if (new_direction != SnakeDirection::none) {

					// if not ((right -> left) or (left -> right) or (up -> down) or (down -> up))

					if (std::abs((int)(m_Direction.load()) - (int)new_direction) != 1)
						m_Direction = new_direction;
				}
			}
		}
	}

	void put_new_apple() {
		std::srand(std::time(nullptr));

		m_Apple.x = (rand() % (MAX_WIDHT / 2 - 1)) - (MAX_WIDHT / 2 - 1);
		m_Apple.y = (rand() % (MAX_HEIGHT / 2 - 1)) - (MAX_HEIGHT / 2 - 1);
	}
};

int main(int argc, char* argv[]) {
	Snake snake{ argc, argv };

	while (snake.alive()) {
	 	snake.update();

		snake.sleep();
	 	snake.draw();
	}

	_getch();

	return 0;
}
