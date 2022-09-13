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
#include <string>
#include <vector>
#include <stack>

int getFileSize();
int* getRawInput(int*);
int resizeArray();
int* fillResized(int*, int*, int);

enum BitTypes : char {
	QUEEN = 1U,
	ATTACKED = 2U,
};

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
	std::vector<std::byte> m_state;
	std::size_t m_width = 0U;
	Point m_start_queen;


  public:
	explicit Board(std::vector<std::byte>& in_Board, std::size_t const width)
		: m_state(in_Board), m_width(width) {
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
		for(auto i = 0U; i < (m_width*m_width); i++){
			m_state[i] >>= 8;
		}
	}

	[[nodiscard]] Point
	StartQueen() {
		return m_start_queen;
	}

	[[nodiscard]]
	Point findFirstQueen(Board const& board) {
	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			auto const p = Point{x, y};
			if(static_cast<unsigned>(board[p]) & BitTypes::QUEEN) {
				m_start_queen = p;
				return p;
			}
		}
	}	
}


	[[nodiscard]]
	std::byte&
	operator[](Point const point) {
		return m_state[(m_width * point.y) + point.x];
	}

	[[nodiscard]]
	std::byte
	operator[](Point const point) const {
		return m_state[(m_width * point.y) + point.x];
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
		board.m_state.push_back(static_cast<std::byte>(c != '0'));
		if (board.m_state.size() == size_Of_Board) {
			break;
		}
	}

	return is;
}

std::ostream&
operator<<(std::ostream& stream, const Board& board) {
	stream << "Board.m_state size:" << board.m_state.size() << '\n';
	stream << "output stream of Board.m_state: \n";

	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			stream << static_cast<int>(board[Point{x, y}]) << " | ";
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

void updateAttacks(Board& board, QueenStack const& queen_stack) {
	board.reset();
	for(auto i : queen_stack) {
	std::cout << "updating attacks for queen at: " << i.x << ',' << i.y << '\n';
	auto const recentQueen = i;
	//debug std::cout << "recent queen is at " << recentQueen << '\n';

	for(auto x = 0U; x < board.Width();x++) {
		auto const point = Point{x, recentQueen.y};
		board[point] = static_cast<std::byte>(static_cast<unsigned>(board[point]) | BitTypes::ATTACKED);

	}

	for(auto y = 0U; y < board.Width();y++) {
		auto const point = Point{recentQueen.x, y};
		board[point] = static_cast<std::byte>(static_cast<unsigned>(board[point]) | BitTypes::ATTACKED);

	}

	auto const m = std::min(recentQueen.x, recentQueen.y);
	auto point = Point{recentQueen.x - m, recentQueen.y - m};

	while(point.x < board.Width() && point.y < board.Width()) {
		board[point] = static_cast<std::byte>(static_cast<unsigned>(board[point]) | BitTypes::ATTACKED);
		point.x++;
		point.y++;
	}

	point = Point{recentQueen.x - m, recentQueen.y + m};

	while(point.x < board.Width()) {
		//debug std::cout << point << '\n';
		board[point] = static_cast<std::byte>(static_cast<unsigned>(board[point]) | BitTypes::ATTACKED);
		point.x++;
		if(point.y == 0) break;
		point.y--;
	}
	}

	//debug std::cout << "board after updating attacks: \n" << board;
	
}

void addQueen(Board& board, QueenStack& queen_stack, Point queen) {
	std::cout << "adding queen at position " << queen << '\n';
	queen_stack.push_back(queen);
	updateAttacks(board, queen_stack);
	std::cout << "current board: \n" << board;
}

void findNextQueen(Board& board, QueenStack& queen_stack, Point starting_point) {
	// if(queen_stack.empty()) {
	// 	std::cout << "all queens were popped off the stack. There must be no solution. \n";
	// 	return;
	// }
	for(auto y = starting_point.y; y < board.Width(); y++) {
		for(auto x = starting_point.x; x < board.Width(); x++) {
			auto const point = Point{x, y};
			if(static_cast<unsigned>(board[point]) == 0U) {
				addQueen(board, queen_stack, point);
				break;
				//findNextQueen(board, queen_stack);
				//add basecase for findNextQueen
			} else if (static_cast<size_t>(point.x == (board.Width()-1)) && point.y != board.StartQueen().y) {
				std::cout << "no valid space in this row. need to pop the stack to a previous state. \n";
				queen_stack.pop_back();
				if(queen_stack.empty()) {
					std::cout << "all queens were popped off the stack. There must be no solution. \n";
					return;
				}
				updateAttacks(board, queen_stack);
				Point next_start = queen_stack.back();
				next_start.x++;
				findNextQueen(board, queen_stack, next_start);
				// if(next_start.x == (board.Width()-1)) {
				// 	std::cout << "this was the last column in the row. need to pop back further. \n";
				// 	next_start = queen_stack.back();
				// 	next_start.x++;
				// 	queen_stack.pop_back();
				// 	findNextQueen(board, queen_stack, next_start);
				// 	break;
				// } else 
				if(next_start.y == (board.Width()-1)) {
					std::cout << "this was the last row. No solution. \n";
					break;
				} else {
				//findNextQueen(board, queen_stack, next_start);
				break;
				}
			}
			
		}
	}
}

int
main() {
	try {
		auto content_Of_File = getFile("input.csv");
		std::cout << "stringstream output: " << content_Of_File.str() << '\n';
		auto board = Board{};
		content_Of_File >> board;
		std::cout << "======================================== \n";
		std::cout << board;
		auto queen_Stack = QueenStack{};
		queen_Stack.push_back(board.findFirstQueen(board));
		std::cout << "first queen is at " << queen_Stack.back() << '\n';
		updateAttacks(board, queen_Stack);
		std::cout << board;
		std::cout << "from function findNextQueen: \n";
		findNextQueen(board, queen_Stack, Point{0, 0});

	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

}