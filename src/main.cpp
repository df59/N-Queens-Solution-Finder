#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
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
	std::vector<std::vector<unsigned char>> m_state_stack;
	QueenStack m_queen_stack;
	std::size_t m_width = 0UL;
	std::size_t m_num_start_queens = 0UL;
	std::size_t i = 0UL;


  public:
	explicit Board() = default;

	void
	clear() {
		m_state_stack.clear();
		m_queen_stack.clear();
		m_state_stack.emplace_back();
		m_width = 0UL;
		m_num_start_queens = 0UL;
		i = 0UL;
	}

	[[nodiscard]] std::size_t
	Width() const {
		return m_width;
	}

	[[nodiscard]]
	std::vector<unsigned char>&
	get_state() {
		assert(!m_state_stack.empty());
		return m_state_stack.back();
	}

	[[nodiscard]]
	std::vector<unsigned char> const&
	get_state() const {
		assert(!m_state_stack.empty());
		return m_state_stack.back();
	}

	[[nodiscard]]
	unsigned char&
	operator[](Point const point) {
		assert(point.x < Width());
		assert(point.y < Width());
		return get_state()[(m_width * point.y) + point.x];

	}

	[[nodiscard]]
	unsigned char
	operator[](Point const point) const {
		assert(point.x < Width());
		assert(point.y < Width());
		return get_state()[(m_width * point.y) + point.x];

	}


	[[nodiscard]]
	std::size_t
	number_of_queens () const {
		return m_queen_stack.size();
	}


	// [[nodiscard]]
	// bool
	// fully_attacked() const {
	// 	return std::end(get_state()) == std::find(std::begin(get_state()), std::end(get_state()), false);
	// }

	[[nodiscard]]
	bool
	findQueens() {
		for (auto y = 0U; y < Width(); y++) {
			for (auto x = 0U; x < Width(); x++) {
				auto const p = Point{x, y};
				if (operator[](p) == false) {
					//std::cout << "\t --------->" << p << '\n'; 
					//check square on board
					addQueen(p);
					if (findQueens()) {
						return true;
					}
					if ((number_of_queens() == Width())) {
						return true;
					}
					//std::cout << "removing queen: " << m_queen_stack.back() << '\n';
					m_queen_stack.pop_back();
					m_state_stack.pop_back();
					// print_debug();
				}
			}
		}

		return false;
	}

	void addQueen(Point const queen) {
		assert(!m_queen_stack.empty());
		assert(queen.x < Width());
		assert(queen.y < Width());
		assert(m_queen_stack.back().x < Width());
		assert(m_queen_stack.back().y < Width());

		m_queen_stack.push_back(queen);
		//copy and push last state onto stack
		m_state_stack.push_back(m_state_stack.back());
		assert(m_queen_stack.size() == m_state_stack.size());


		populate_new_attacks(queen);
		// std::cout << "adding queen at position " << queen << '\n';
		// print_debug();
	}

	void
	populate_first_queen_attacks() {
		for(auto const& queen : m_queen_stack) {
			populate_new_attacks(queen);
		}
	}



	void
	populate_new_attacks(Point const queen) {
		i++;
		//  vertical
		for (auto x = 0U; x < Width(); x++) {
			auto const point = Point{x, queen.y};
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point)= true;
		}

		// horizontal
		for (auto y = 0U; y < Width(); y++) {
			auto const point = Point{queen.x, y};
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
		}

		// top left to bottom right diagonal
		auto const m = std::min(queen.x, queen.y);
		auto point = Point{queen.x - m, queen.y - m};
		while (point.x < Width() && point.y < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			point.x++;
			point.y++;
		}

		// towards top right
		point = Point{queen.x, queen.y};
		while (point.x < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			point.x++;
			if (point.y == 0) break;
			point.y--;
		}

		// towards bottom left
		point = Point{queen.x, queen.y};
		while (point.y < Width()) {
			assert(point.x < Width());
			assert(point.y < Width());
			operator[](point) = true;
			if (point.x == 0) break;
			point.x--;
			point.y++;
		}
	}

	void print_debug() {
		std::cout << "print debug: \n";
		for(auto& j : m_queen_stack) {
			std::cout << j << ' ';
			operator[](j) = 2;
		}
		//std::cout << "\n fully attacked = " << fully_attacked() << '\n';
		std::cout << '\n' << i << '\n';
		for (auto y = 0U; y < Width(); y++) {
			for (auto x = 0U; x < Width(); x++) {
				std::cout << static_cast<int>(operator[](Point{x, y}));
				if (x < Width() - 1) std::cout << ',';
			}
			std::cout << '\n';
		}	
	}


	friend std::istream& operator>>(std::istream& is, Board& board);
	friend std::ostream& operator<<(std::ostream& stream, Board& board);
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
				board.m_width = board.get_state().size();
				size_Of_Board = board.m_width * board.m_width;
				board.get_state().reserve(board.m_width * board.m_width);
			}
			continue;
		}

		if (c != '0' && c != '1') {
			is.setstate(std::ios::failbit);
			return is;
		}
		//FIXME: loads improper input
		board.get_state().push_back(c != '0');
		if (board.get_state().size() == size_Of_Board) {
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
	board.populate_first_queen_attacks();
	assert(board.get_state().size() == (board.Width()*board.Width()));
	return is;
}

std::ostream&
operator<<(std::ostream& stream, Board& board) {
	auto state = board.get_state();
	std::fill(state.begin(), state.end(), 0);
	for(auto& i : board.m_queen_stack) {
		auto index = i.y * board.Width() + i.x;
		state[index] = 1;
	}
	for (auto y = 0U; y < board.Width(); y++) {
		for (auto x = 0U; x < board.Width(); x++) {
			auto index = y * board.Width() + x;
			stream << static_cast<int>(state[index]);
			if (x < board.Width() - 1) stream << ',';
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


int
main() {
	try {
		auto content_Of_File = getFile("input.csv");
		auto board = Board{};
		content_Of_File >> board;
		std::cout << "======================================== \n";
		std::cout << board;
		if(!board.findQueens()) {
			std::cerr << "No solution found. \n";
			return EXIT_FAILURE;
		}
		auto ofs = std::ofstream{"solution.csv"};
		ofs << board;
		std::cout << "========================================" << '\n' << board << '\n';
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

}