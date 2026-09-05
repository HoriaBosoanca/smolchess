#include "move.h"
#include <board.h>

Move::Move() : move(0), pieces(0) {}

Move::Move(const uint8_t from, const uint8_t to, const Piece from_piece, const Piece to_piece, const MoveType move_type) {
	move = from | (to << 6) | (move_type << 12);
	pieces = from_piece | (to_piece << 4);
}

uint8_t Move::from() const {
	return move & 0b111111;
}

uint8_t Move::to() const {
	return (move >> 6) & 0b111111;
}

MoveType Move::move_type() const {
	return static_cast<MoveType>(move >> 12 & 0b1111);
}

uint8_t Move::from_piece() const {
	return pieces & 0b1111;
}

uint8_t Move::to_piece() const {
	return (pieces >> 4) & 0b1111;
}

bool Move::operator==(const Move& other) const {
	if (from() == other.from() && to() == other.to()) {
		if (move_type() != REGULAR) { // if the move has a promotion
			if (move_type() == other.move_type()) { // pick the right one
				return true;
			}
		} else {
			return true;
		}
	}
	return false;
}

std::string Move::to_string() const {
	std::string s;
	s += file_char(from());
	s += static_cast<char>('0'+rank(from()));
	s += file_char(to());
	s += static_cast<char>('0'+rank(to()));
	if (const MoveType mt = move_type(); mt == QUEEN_PROMOTION) {
		s += 'q';
	} else if (mt == ROOK_PROMOTION) {
		s += 'r';
	} else if (mt == BISHOP_PROMOTION) {
		s += 'b';
	} else if (mt == KNIGHT_PROMOTION) {
		s += 'n';
	}
	return s;
}