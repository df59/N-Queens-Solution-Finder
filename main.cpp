#include <cassert>
#include <cmath>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int getFileSize();
int* getRawInput(int*);
int resizeArray();
int* fillResized(int*, int*, int);


class Board {
  private:
	std::vector<std::byte> state;
	std::size_t m_width = 0U;

  public:
	explicit Board(std::vector<std::byte>& in_Board, std::size_t const width)
		: state(in_Board), m_width(width) {
		assert(m_width * m_width == state.size());
	}

	explicit Board() = default;

	[[nodiscard]] std::size_t
	Width() const {
		return m_width;
	}

	void
	clear() {
		state.clear();
		m_width = 0U;
	}

	std::byte&
	operator[](unsigned int index) {
		return state[index];
	}

	std::byte
	operator[](unsigned int index) const {
		return state[index];
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
				board.m_width = board.state.size();
				size_Of_Board = board.m_width * board.m_width;
				board.state.reserve(board.m_width * board.m_width);
			}
			continue;
		}

		if (c != '0' && c != '1') {
			is.setstate(std::ios::failbit);
			return is;
		}
		board.state.push_back(static_cast<std::byte>(c != '0'));
		if (board.state.size() == size_Of_Board) {
			break;
		}
	}

	return is;
}

std::ostream&
operator<<(std::ostream& stream, const Board& board) {
	stream << "output stream of Board.state: \n";
	stream << "Board.state size: \n";
	stream << board.state.size() << '\n';
	for (auto c : board.state) {
		stream << static_cast<int>(c) << ' ';
	}
	stream << '\n';
	for (auto i = 0U; i < board.Width(); i++) {
		for (auto j = board.Width() * i; j < board.Width() * (i + 1); j++) {
			stream << static_cast<int>(board.state[j]) << " | ";
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
		std::cout << "stringstream output: " << content_Of_File.str() << '\n';
		auto board = Board{};
		content_Of_File >> board;
		std::cout << "======================================== \n";
		std::cout << board;
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}
}