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

enum BitTypes : char {
	QUEEN = 1U,
	ATTACKED = 1U,
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
		//std::cout << "from [] overloaded operator &ref in Board class: \n point.x: " << point.x << "\n point.y: " << point.y << "\n index: " << ((m_width * point.y) + point.x);
		// return m_state.at((m_width * point.y) + point.x);
		return m_state[(m_width * point.y) + point.x];

	}

	[[nodiscard]]
	std::byte
	operator[](Point const point) const {
		//std::cout << "from [] overloaded operator in Board class: \n point.x: " << point.x << "\n point.y: " << point.y << "\n index: " << ((m_width * point.y) + point.x);
		// return m_state.at((m_width * point.y) + point.x);
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
	stream << "Current board: \n";

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
	//std::cout << "updating attacks \n";
	for(auto i : queen_stack) {
	//debug std::cout << "updating attacks for queen at: " << i.x << ',' << i.y << '\n';
	auto const recentQueen = i;
	//debug std::cout << "recent queen is at " << recentQueen << '\n';

	for(auto x = 0U; x < board.Width();x++) {
		//debug std::cout << "first for loop\n";
		auto const point = Point{x, recentQueen.y};
		assert(point.x < board.Width());
		assert(point.y < board.Width());
		//debug std::cout << "setting board[ " << point.x << ' ' << point.y << "] to true \n"; 
		board[point] = static_cast<std::byte>(1U);
	}

	for(auto y = 0U; y < board.Width();y++) {
		//debug std::cout << "second for loop\n";
		auto const point = Point{recentQueen.x, y};
		assert(point.x < board.Width());
		assert(point.y < board.Width());
		//debug std::cout << "setting board[ " << point.x << ' ' << point.y << "] to true \n"; 
		board[point] = static_cast<std::byte>(1U);
	}

	auto const m = std::min(recentQueen.x, recentQueen.y);
	auto point = Point{recentQueen.x - m, recentQueen.y - m};

	while(point.x < board.Width() && point.y < board.Width()) {
		//debug std::cout << "first while loop\n" << "m value " << m << '\n';
		assert(point.x < board.Width());
		assert(point.y < board.Width());
		//debug std::cout << "setting board[ " << point.x << ' ' << point.y << "] to true \n"; 
		board[point] = static_cast<std::byte>(1U);
		point.x++;
		point.y++;
	}

	//auto const big = std::max(recentQueen.x, recentQueen.y);
	point = Point{recentQueen.x - m, recentQueen.y + m};
	

	while(point.x < board.Width() && point.y < board.Width()) {
		//debug std::cout << "second while loop\n" << "m value " << m << '\n';// << "recentQueen.x value " << recentQueen.x << '\n' << "recentqueen.y value " << recentQueen.y << '\n';
		assert(point.x < board.Width());
		assert(point.y < board.Width());
		//debug //debug std::cout << point << '\n';
		//debug std::cout << "setting board[ " << point.x << ' ' << point.y << "] to true \n"; 
		board[point] = static_cast<std::byte>(1U);
		point.x++;
		if(point.y == 0) break;
		point.y--;
	}

	auto const big = std::max(recentQueen.x, recentQueen.y);
	point = Point{recentQueen.x - big, recentQueen.y + big};
	

	while(point.x < board.Width() && point.y < board.Width()) {
		//debug std::cout << "third while loop\n" << "big value " << big << '\n';
		assert(point.x < board.Width());
		assert(point.y < board.Width());
	// 	//debug std::cout << point << '\n';
	// 	std::cout << "setting board[ " << point.x << ' ' << point.y << "] to true \n"; 
		board[point] = static_cast<std::byte>(1U);
		point.x++;
		if(point.y == 0) break;
		point.y--;
	}

	
	 }

	//debug std::cout << "board after updating attacks: \n" << board;
	
}

void addQueen(Board& board, QueenStack& queen_stack, Point queen) {
	std::cout << "adding queen at position " << queen << '\n';
	// attempt to fix bug queen_stack.reserve(queen_stack.capacity()+1);
	assert(queen.x < board.Width());
	assert(queen.y < board.Width());
	//debug std::cout << queen_stack.back().x;
	//debug std::cout << queen_stack.back().y;
	queen_stack.push_back(queen);
	//debug std::cout << "debug pushback worked \n";
	assert(queen_stack.back().x < board.Width());
	assert(queen_stack.back().y < board.Width());
	//debug std::cout << queen_stack.back().x;
	//debug std::cout << queen_stack.back().y;
	updateAttacks(board, queen_stack);
	//debug std::cout << "debug update Attacks was called \n";
	std::cout << board;
}

void printQueens(QueenStack queen_stack) {
	std::cout << "current queen stack: \n";
	for(auto i : queen_stack) {
		std::cout << '(' << i.x << ',' << i.y << ')' << '\n';
	}
}


void findNextQueen(Board& board, QueenStack& queen_stack, Point starting_point) {
	printQueens(queen_stack);
	//debug std::cout << "debug findNextQueen called \n";
	//debug std::cout << "debug starting queen value is " << board.StartQueen().x << ',' << board.StartQueen().y << '\n';
	//debug std::cout << "debug startingpoint.x: " << starting_point.x << "\n debug startingpoint.y: " << starting_point.y << '\n';
	
	//while(starting_point.y != board.StartQueen().y){
	//for(auto y = starting_point.y; y != board.StartQueen().y; y = ((y + 1) % board.Width())) {
		for(auto x = starting_point.x; x != board.StartQueen().x; x = (x + 1) % (board.Width())) {
			auto point = Point{x, starting_point.y};
			if(static_cast<unsigned>(board[point]) == 0U) {
				//debug std::cout << "debug board[point]: " << static_cast<int>(board[point]) << '\n';
				//std::cout << "debug static_cast<unsigned>(board[point]) == 0U returned true \n";
				addQueen(board, queen_stack, point);
				point.x = static_cast<std::size_t>((board.StartQueen().x+1) % board.Width());
				point.y = static_cast<std::size_t>((queen_stack.back().y+1) % board.Width());
				//std::cout << "debug point.x: " << point.x << "\n debug point.y: " << point.y << '\n';
				if(queen_stack.size() == board.Width()){
					std::cout <<"Solution found! \n";
					return;
				} else {
				findNextQueen(board, queen_stack, point);
				}
			} else if((x + 1) % board.Width() == board.StartQueen().x) {
						if(queen_stack.size() != board.Width() && !queen_stack.empty()) {
			//old comparison statement (static_cast<size_t>(point.x == (board.Width()-1)) && point.y != board.StartQueen().y) {
				std::cout << "no valid space in this row. need to pop the stack to a previous state. \n";
				Point next_start;
				next_start.x = (queen_stack.back().x + 1) % board.Width();
				next_start.y = queen_stack.back().y;
				queen_stack.pop_back();
				if(queen_stack.empty()) { //needs changed due to circular iterating
					std::cout << "all queens were popped off the stack. There must be no solution. \n";
					return;
				}
				updateAttacks(board, queen_stack);
				findNextQueen(board, queen_stack, next_start);
				// if(next_start.y == (board.Width()-1)) {
				// 	std::cout << "this was the last row. No solution. \n";
				// 	break;
				// }
			}
		}
			
		}


	//}
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
		findNextQueen(board, queen_Stack, Point{(board.StartQueen().x+1)%(board.Width()), (board.StartQueen().y+1)%(board.Width())});
		//updateAttacks(board, queen_Stack);
		//debug std::cout << queen_Stack.empty();
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

}