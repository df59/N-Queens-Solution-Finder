/*

N-Queens Solution Finder

Dustin Franklin
df59@zips.uakron.edu
https://github.com/df59/N-Queens-Solution-Finder


Given a N*N board with the inital position of one queen, finds a possible solution for the arrangement of N queens
such that no queen is currently threatened by a direct attack from another queen. If no solution exists, outputs
"No solution". Input is a file in the working directory of main.cpp called "input.csv" with rigid format. Output is
in the same directory as "output.csv".

This is a recursive method of the backtracking algorithm, solving quickly when the stack does not overflow. 
Future consideration will be to use internal memory handling rather than keeping track of previous board states
with recursive function calls.

*/

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
	Point findFirstQueen(Board const& board) { // returns the first queen in the input board
	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			auto const p = Point{x, y};
			if(static_cast<unsigned>(board[p]) & 1) {
				m_start_queen = p;
				return p;
			}
		}
	}	
	assert(false);
}

	[[nodiscard]]
	std::byte&
	operator[](Point const point) { // using y = mx + b, returns the value at the 1D Vector index given a 2D index format
		assert(point.x < Width());
		assert(point.y < Width());
		return m_state[(m_width * point.y) + point.x];

	}

	[[nodiscard]]
	std::byte
	operator[](Point const point) const { // using y = mx + b, returns the value at the 1D Vector index given a 2D index format
		assert(point.x < Width());
		assert(point.y < Width());
		return m_state[(m_width * point.y) + point.x];

	}

	friend std::istream& operator>>(std::istream& is, Board& board);
	friend std::ostream& operator<<(std::ostream& stream, const Board& board);
};

std::istream&
operator>>(std::istream& is, Board& board) { // inputs rigidly formatted board of 0 for open space and 1 for queens separated by commas
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
operator<<(std::ostream& stream, const Board& board) { // outputs formatted board state

	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			stream << static_cast<int>(board[Point{x, y}]);
			if (x < board.Width() - 1) stream << ',';
		}
		stream << '\n';
	}
	return stream;
}

[[nodiscard]] 
std::stringstream
getFile(std::string const input_file) { // takes in file as input
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

void
updateAttacks(Board& board, QueenStack const& queen_stack) { // places "1" on all spaces of the board which are invalid for future queens
	board.reset();
	for (auto const& recentQueen : queen_stack) {
		//  vertical
		for (auto x = 0U; x < board.Width(); x++) {
			auto const point = Point{x, recentQueen.y};
			assert(point.x < board.Width());
			assert(point.y < board.Width());
			board[point] = static_cast<std::byte>(1U);
		}

		// horizontal
		for (auto y = 0U; y < board.Width(); y++) {
			auto const point = Point{recentQueen.x, y};
			assert(point.x < board.Width());
			assert(point.y < board.Width());
			board[point] = static_cast<std::byte>(1U);
		}

		// top left to bottom right diagonal
		auto const m = std::min(recentQueen.x, recentQueen.y);
		auto point = Point{recentQueen.x - m, recentQueen.y - m};
		while (point.x < board.Width() && point.y < board.Width()) {
			assert(point.x < board.Width());
			assert(point.y < board.Width());
			board[point] = static_cast<std::byte>(1U);
			point.x++;
			point.y++;
		}

		// towards top right
		point = Point{recentQueen.x, recentQueen.y};
		while (point.x < board.Width()) {
			assert(point.x < board.Width());
			assert(point.y < board.Width());
			board[point] = static_cast<std::byte>(1U);
			point.x++;
			if (point.y == 0) break;
			point.y--;
		}

		// towards bottom left
		point = Point{recentQueen.x, recentQueen.y};
		while (point.y < board.Width()) {
			assert(point.x < board.Width());
			assert(point.y < board.Width());
			board[point] = static_cast<std::byte>(1U);
			if (point.x == 0) break;
			point.x--;
			point.y++;
		}
	}
}

void
addFinalQueens(Board& board, QueenStack& queen_stack) { // adds the final stack of queens to the board without assigning attack vectors
		board.reset();
		while(!queen_stack.empty()) {
		board[queen_stack.back()] = static_cast<std::byte>(1U);
		queen_stack.pop_back();
		}

}

void
addQueen(Board& board, QueenStack& queen_stack, Point queen) { // adds a queen to the queen stack and calls updateAttacks
	assert(queen.x < board.Width());
	assert(queen.y < board.Width());
	queen_stack.push_back(queen);
	assert(queen_stack.back().x < board.Width());
	assert(queen_stack.back().y < board.Width());
	updateAttacks(board, queen_stack);
}

void
printQueens(QueenStack queen_stack) { // prints the contents of the current stack of valid queens
	std::cout << "current queen stack: \n";
	for(auto i : queen_stack) {
		std::cout << '(' << i.x << ',' << i.y << ')' << '\n';
	}

}

void
findNextQueen(Board& board, QueenStack& queen_stack, Point starting_point) { // iterates across x and recurses across y
		for(auto x = starting_point.x; x != board.StartQueen().x; x = (x + 1) % (board.Width())) {
			auto point = Point{x, starting_point.y};
			if(static_cast<unsigned>(board[point]) == 0U) {
				addQueen(board, queen_stack, point);
				point.x = static_cast<std::size_t>((board.StartQueen().x+1) % board.Width());
				point.y = static_cast<std::size_t>((queen_stack.back().y+1) % board.Width());
				if(queen_stack.size() == board.Width()){
					std::cout <<"Solution found! \n";
					return;
				} else {
				findNextQueen(board, queen_stack, point); // recurse at next y if valid queen is found and is not the final queen to the solution
				}
			} else if((x + 1) % board.Width() == board.StartQueen().x && (queen_stack.size() != board.Width() && !queen_stack.empty()) ) {
						std::cout << "no valid space in this row. need to pop the stack to a previous state. \n";
						Point next_start;
						next_start.x = (queen_stack.back().x + 1) % board.Width();
						next_start.y = queen_stack.back().y;
						queen_stack.pop_back();
						if(queen_stack.empty()) {
							std::cout << "all queens were popped off the stack. There must be no solution. \n";
							return;
						}
						updateAttacks(board, queen_stack);
						findNextQueen(board, queen_stack, next_start);  // if this y value has been exhausted for valid x positions for the queen
															   			// and "no solution" hasn't yet been determined, then
															    		// recurse from the previous state, popping off the recent queen and
															    		// checking a new x at the previous y
						return;	
		}			
		}
		return;
}

int
main() {
	try {
		auto content_Of_File = getFile("input.csv");
		auto board = Board{};
		content_Of_File >> board;
		std::cout << "======================================== \n";
		std::cout << "Starting board:\n" << board;
		auto queen_stack = QueenStack{};
		queen_stack.push_back(board.findFirstQueen(board));
		std::cout << "first queen is at " << queen_stack.back() << '\n';
		std::cout << "running solution... \n";
		updateAttacks(board, queen_stack); // initial queen attack vectors
		findNextQueen(board, queen_stack, Point{(board.StartQueen().x+1)%(board.Width()), (board.StartQueen().y+1)%(board.Width())}); // recursive function to solve
		printQueens(queen_stack);
		addFinalQueens(board, queen_stack);
		std::cout << board;
		auto ofs = std::ofstream{"solution.csv"};
		ofs << board;
		std::cout << "Solution written to solution.csv\n";
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

}