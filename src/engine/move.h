#pragma once

#include <cstdint>

enum Piece : uint8_t {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	NONE,
};
enum MoveType : uint8_t {
	REGULAR,
	EN_PASSANT,
	QUEEN_CASTLING,
	KING_CASTLING,
	KNIGHT_PROMOTION,
	BISHOP_PROMOTION,
	ROOK_PROMOTION,
	QUEEN_PROMOTION,
};
class Move {
	uint16_t move;
	uint8_t pieces;
	bool initialized = false;
public:
	Move();
	Move(uint8_t from, uint8_t to, Piece from_piece, Piece to_piece = NONE, MoveType move_type = REGULAR);
	uint8_t from() const;
	uint8_t to() const;
	MoveType move_type() const;
	uint8_t from_piece() const;
	uint8_t to_piece() const;
	bool operator==(const Move& other) const;
};