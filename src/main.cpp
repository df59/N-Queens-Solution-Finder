#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>
#include <stack>


struct Point {
	std::size_t x;
	std::size_t y;
};

using QueenStack = std::vector<Point>;

std::ostream&
operator<<(std::ostream& stream, Point const& point) {
	stream << '(' << point.x << ", " << point.y << ')';
	return stream;
}


class Board {
  private:
	std::vector<unsigned char> m_state;
	QueenStack m_queen_stack;
	std::size_t m_width = 0UL;
	std::size_t m_num_start_queens = 0UL;


  public:
	explicit Board(std::vector<unsigned char>& in_Board, std::size_t const width)
		: m_state(in_Board), m_queen_stack(), m_width(width) {
		assert(m_width * m_width == m_state.size());
	}

	explicit Board() = default;

	[[nodiscard]] std::size_t
	Width() const {
		return m_width;
	}

	void
	clear() {
		m_state.clear();
		m_width = 0U;
	}

	void
	reset() {
		std::fill(std::begin(m_state), std::end(m_state), false);
	}


	[[nodiscard]]
	unsigned char&
	operator[](Point const point) {
		assert(point.x < Width());
		assert(point.y < Width());
		return m_state[(m_width * point.y) + point.x];

	}

	[[nodiscard]]
	unsigned char
	operator[](Point const point) const {
		assert(point.x < Width());
		assert(point.y < Width());
		return m_state[(m_width * point.y) + point.x];

	}

	void addQueen(Point queen) {
	std::cout << "adding queen at position " << queen << '\n';
	// attempt to fix bug m_queen_stack.reserve(m_queen_stack.capacity()+1);
	assert(queen.x < Width());
	assert(queen.y < Width());
	//debug std::cout << m_queen_stack.back().x;
	//debug std::cout << m_queen_stack.back().y;
	m_queen_stack.push_back(queen);
	//debug std::cout << "debug pushback worked \n";
	assert(m_queen_stack.back().x < Width());
	assert(m_queen_stack.back().y < Width());
	//debug std::cout << m_queen_stack.back().x;
	//debug std::cout << m_queen_stack.back().y;
	updateAttacks();
	//debug std::cout << "debug update Attacks was called \n";
	//debug std::cout << board;
}


void
updateAttacks() {
	reset();
	for (auto const& recentQueen : m_queen_stack) {
		//  vertical
		for (auto x = 0U; x < Width(); x++) {
			auto const point = Point{x, recentQueen.y};
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point)= true;
		}

		// horizontal
		for (auto y = 0U; y < Width(); y++) {
			auto const point = Point{recentQueen.x, y};
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
		}

		// top left to bottom right diagonal
		auto const m = std::min(recentQueen.x, recentQueen.y);
		auto point = Point{recentQueen.x - m, recentQueen.y - m};
		while (point.x < Width() && point.y < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			point.x++;
			point.y++;
		}

		// towards top right
		point = Point{recentQueen.x, recentQueen.y};
		while (point.x < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			point.x++;
			if (point.y == 0) break;
			point.y--;
		}

		// towards bottom left
		point = Point{recentQueen.x, recentQueen.y};
		while (point.y < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			if (point.x == 0) break;
			point.x--;
			point.y++;
		}
	}
}


	friend std::istream& operator>>(std::istream& is, Board& board);
	friend std::ostream& operator<<(std::ostream& stream, const Board& board);
};

std::istream&
operator>>(std::istream& is, Board& board) {
	board.clear();
	auto line = std::string{};
	assert(board.m_width == 0U);
	auto size_Of_Board = 0UL;

	while (!is.eof()) {
		auto c = is.get();
		if (c == ',') continue;
		if (c == '\n' || c == '\r') {
			if (board.m_width == 0U) {
				board.m_width = board.m_state.size();
				size_Of_Board = board.m_width * board.m_width;
				board.m_state.reserve(board.m_width * board.m_width);
			}
			continue;
		}

		if (c != '0' && c != '1') {
			is.setstate(std::ios::failbit);
			return is;
		}
		board.m_state.push_back(c != '0');
		if (board.m_state.size() == size_Of_Board) {
			break;
		}
	}

	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			auto const p = Point{x, y};
			if(board[p] == true) {
				board.m_queen_stack.push_back(p);
				board.m_num_start_queens++;
			}
		}

	}
	return is;
}

std::ostream&
operator<<(std::ostream& stream, const Board& board) {

	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			stream << static_cast<int>(board[Point{x, y}]);
			if (x < board.Width() - 1) std::cout << ',';
		}
		stream << '\n';
	}
	return stream;
}

[[nodiscard]] std::stringstream
getFile(std::string const input_file) {
	auto i_filestream = std::ifstream{input_file.c_str()};

	if (!i_filestream.is_open()) {
		throw std::runtime_error(
			"File did not open. Make sure it is in the project directory \n");
	}
	auto ss = std::stringstream();
	i_filestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	ss << i_filestream.rdbuf();

	return ss;
}


void printQueens(QueenStack m_queen_stack) {
	std::cout << "current queen stack: \n";
	for(auto i : m_queen_stack) {
		std::cout << '(' << i.x << ',' << i.y << ')' << '\n';
	}
}

int
main() {
	try {
		auto content_Of_File = getFile("input.csv");
		auto board = Board{};
		content_Of_File >> board;
		std::cout << "======================================== \n";
		std::cout << board;
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

}