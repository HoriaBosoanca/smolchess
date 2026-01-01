#include <cstdint>
#include <iostream>
#include "board.h"

#include <bitset>

#include "board_helpers.h"

// optimized

Move::Move() : move(0), pieces(0) {}

Move::Move(const uint8_t from, const uint8_t to, const Piece from_piece, const Piece to_piece) {
    move = from | (to << 6); // 16 - 6x2 = 4 bits left
    pieces = from_piece | (to_piece << 3); // 8 - 3x2 = 2 bits left
}

uint8_t Move::get_from() const {
    return move & 0b111111;
}

uint8_t Move::get_to() const {
    return (move >> 6) & 0b111111;
}

uint8_t Move::get_from_piece() const {
    return pieces & 0b111;
}

uint8_t Move::get_to_piece() const {
    return (pieces >> 3) & 0b111;
}

void Move::print() const {
    std::cout << piece_map[get_from_piece()] << " " << file(get_from()) << rank(get_from()) << file(get_to()) << rank(get_to()) << " " << piece_map[get_to_piece()] << "\n";
}

void Board::add_move(const Move move) {
    moves[turn][move_count[turn]++] = move;
}

Piece Board::get_piece_by_color(const uint64_t pos, const Color color) const {
    for (int p = PAWN; p <= KING; p++)
        if (bitboard[color][p] & pos)
            return Piece(p);
    return NONE;
}

void Board::add_continuous_move(const uint8_t i, const uint64_t pos, const Piece piece, const uint64_t* occupied,
    const int file_increase, const int rank_increase) {
    for (int file_offset = file_increase, rank_offset = rank_increase; continuous_move_condition(i, file_offset, rank_offset, file_increase, rank_increase);
        file_offset += file_increase, rank_offset += rank_increase) {
        if (const uint64_t new_pos = offset_pos(pos, file_offset, rank_offset);
            new_pos & (occupied[WHITE] | occupied[BLACK])) {
            if (new_pos & occupied[!turn]) {
                add_move(Move(i, offset_idx(i, file_offset, rank_offset), piece, get_piece_by_color(new_pos, Color(!turn))));
            }
            break;
        }
        add_move(Move(i, offset_idx(i, file_offset, rank_offset), piece, NONE));
    }
}

void Board::make_move(const Move move) {
    const uint64_t from = 1ULL << move.get_from(), to = 1ULL << move.get_to();
    const uint8_t from_piece = move.get_from_piece(), to_piece = move.get_to_piece();
    bitboard[turn][from_piece] = (bitboard[turn][from_piece] & ~from) | to;
    if (to_piece != NONE) {
        bitboard[Color(!turn)][to_piece] = bitboard[Color(!turn)][to_piece] & ~to;
    }
    turn = Color(!turn);
}

uint64_t Board::get_occupied(const Color color) const {
    const uint64_t* c = bitboard[color];
    return c[PAWN] | c[KNIGHT] | c[BISHOP] | c[ROOK] | c[QUEEN] | c[KING];
}

void Board::generate_moves() {
    move_count[turn] = 0; // reset moves

    const uint64_t occupied[2] = {get_occupied(WHITE), get_occupied(BLACK)};
    const uint64_t any_occupied = occupied[WHITE] | occupied[BLACK];

    for (uint8_t i = 0; i < 64; i++) {
        if (const uint64_t pos = 1ULL << i; bitboard[turn][PAWN] & pos) { // pawns
            if (turn == WHITE) {
                if (rank(i) == 8) { // if it's rank 8
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, 1) & any_occupied)) {
                    add_move(Move(i, offset_idx(i, 0, 1), PAWN, NONE));
                    if (!(offset_pos(pos, 0, 2) & any_occupied) && rank(i) == 2) {
                        add_move(Move(i, offset_idx(i, 0, 2), PAWN, NONE));
                    }
                }
                if (file(i) > 'a') { // if it has a piece ahead-left
                    if (const uint64_t new_pos = offset_pos(pos, -1, 1); new_pos & occupied[BLACK]) {
                        add_move(Move(i, offset_idx(i, -1, 1), PAWN, get_piece_by_color(new_pos, BLACK)));
                    }
                }
                if (file(i) < 'h') { // if it has a piece ahead-right
                    if (const uint64_t new_pos = offset_pos(pos, 1, 1); new_pos & occupied[BLACK]) {
                        add_move(Move(i, offset_idx(i, 1, 1), PAWN, get_piece_by_color(new_pos, BLACK)));
                    }
                }
            } else {
                if (rank(i) == 1) { // if it's rank 1
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, -1) & any_occupied)) { // if it has space ahead
                    add_move(Move(i, offset_idx(i, 0, -1), PAWN, NONE));
                    if (!(offset_pos(pos, 0, -2) & any_occupied) && rank(i) == 7) {
                        add_move(Move(i, offset_idx(i, 0, -2), PAWN, NONE));
                    }
                }
                if (file(i) > 'a') { // if it has a piece ahead-left
                    if (const uint64_t new_pos = offset_pos(pos, -1, -1); new_pos & occupied[WHITE]) {
                        add_move(Move(i, offset_idx(i, -1, -1), PAWN, get_piece_by_color(new_pos, WHITE)));
                    }
                }
                if (file(i) < 'h') { // if it has a piece ahead-right
                    if (const uint64_t new_pos = offset_pos(pos, 1, -1); new_pos & occupied[WHITE]) {
                        add_move(Move(i, offset_idx(i, 1, -1), PAWN, get_piece_by_color(new_pos, WHITE)));
                    }
                }
            }
        } else if (bitboard[turn][KNIGHT] & pos) { // knights
            if (const uint64_t new_pos = offset_pos(pos, -1, -2);
                rank(i) >= 3 && file(i) >= 'b' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -1, -2), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, -2);
                rank(i) >= 3 && file(i) <= 'g' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 1, -2), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 2, -1);
                rank(i) >= 2 && file(i) <= 'f' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 2, -1), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 2, 1);
                rank(i) <= 7 && file(i) <= 'f' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 2, 1), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, 2);
                rank(i) <= 6 && file(i) <= 'g' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 1, 2), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 2);
                rank(i) <= 6 && file(i) >= 'b' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -1, 2), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -2, 1);
                rank(i) <= 7 && file(i) >= 'c' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -2, 1), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -2, -1);
                rank(i) >= 2 && file(i) >= 'c' && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -2, -1), KNIGHT, get_piece_by_color(new_pos, Color(!turn))));
            }
        } else if (bitboard[turn][KING] & pos) { // kings
            const bool not_max_rank = rank(i) < 8, not_min_rank = rank(i) > 1, not_max_file = file(i) < 'h', not_min_file = file(i) > 'a';
            if (const uint64_t new_pos = offset_pos(pos, 1, 1);
                not_max_rank && not_max_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 1, 1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 1);
                not_max_rank && not_min_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -1, 1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, -1);
                not_min_rank && not_max_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 1, -1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, -1);
                not_min_rank && not_min_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -1, -1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 0, -1);
                not_min_rank && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 0, -1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 0);
                not_min_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, -1, 0), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 0, 1);
                not_max_rank && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 0, 1), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, 0);
                not_max_file && !(occupied[turn] & new_pos)) {
                add_move(Move(i, offset_idx(i, 1, 0), KING, get_piece_by_color(new_pos, Color(!turn))));
            }
        } else {
            if (const Piece diagonal_piece = ((bitboard[turn][BISHOP] & pos) ? BISHOP : QUEEN); bitboard[turn][diagonal_piece] & pos) { // bishops or queens
                add_continuous_move(i, pos, diagonal_piece, occupied, 1, 1);
                add_continuous_move(i, pos, diagonal_piece, occupied, -1, 1);
                add_continuous_move(i, pos, diagonal_piece, occupied, -1, -1);
                add_continuous_move(i, pos, diagonal_piece, occupied, 1, -1);
            }
            if (const Piece straight_piece = (bitboard[turn][ROOK] & pos) ? ROOK : QUEEN; bitboard[turn][straight_piece] & pos) { // rooks or queens
                add_continuous_move(i, pos, straight_piece, occupied, 1, 0);
                add_continuous_move(i, pos, straight_piece, occupied, 0, 1);
                add_continuous_move(i, pos, straight_piece, occupied, -1, 0);
                add_continuous_move(i, pos, straight_piece, occupied, 0, -1);
            }
        }
    }
}

// user input / initialization

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            add_piece(1ULL << (8 * i    ), i == 0 ? WHITE : BLACK, ROOK);
            add_piece(1ULL << (8 * i + 1), i == 0 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 2), i == 0 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 3), i == 0 ? WHITE : BLACK, QUEEN);
            add_piece(1ULL << (8 * i + 4), i == 0 ? WHITE : BLACK, KING);
            add_piece(1ULL << (8 * i + 5), i == 0 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 6), i == 0 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 7), i == 0 ? WHITE : BLACK, ROOK);
        } else if (i == 1 || i == 6) {
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), i == 1 ? WHITE : BLACK, PAWN);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const Color color, const int piece) {
    bitboard[color][piece] |= pos;
}

bool Board::check_move_legality(const Move move) const {
    for (int i = 0; i < move_count[turn]; i++)
        if (move.get_from() == moves[turn][i].get_from() &&
            move.get_to() == moves[turn][i].get_to() &&
            move.get_from_piece() == moves[turn][i].get_from_piece() &&
            move.get_to_piece() == moves[turn][i].get_to_piece())
            return true;
    std::cout << "Illegal move!\n";
    return false;
}

Board::Board() : bitboard{}, moves{}, move_count{}, turn(WHITE) {
    setup_normal();
}

Color Board::get_turn() const {
    return turn;
}

bool Board::make_move_str(std::string& move_str) {
    if (!parse_move_format(move_str)) {
        return false;
    }
    const uint8_t from = (move_str[1]-'1')*8+move_str[0]-'a', to = (move_str[3]-'1')*8+move_str[2]-'a';
    Piece from_piece; Color from_color;
    get_piece_and_color(1ULL << from, from_piece, from_color);
    if (from_piece == NONE || from_color != turn) {
        std::cout << "Not your piece!\n";
        return false;
    }
    const Move move = Move(from, to, from_piece, get_piece_by_color(1ULL << to, Color(!turn)));
    if (!check_move_legality(move)) {
        return false;
    }
    make_move(move);
    return true;
}

void Board::get_piece_and_color(const uint64_t pos, Piece& piece, Color& color) const {
    for (int c = WHITE; c <= BLACK; c++)
        for (int p = PAWN; p <= KING; p++)
            if (bitboard[c][p] & pos) {
                piece = Piece(p);
                color = Color(c);
                return;
            }
    piece = NONE;
}

void Board::print_board() const {
    for (int i = 0; i < 8; i++) {
        std::cout << i+1 << " ";
        for (int j = 0; j < 8; j++) {
            Piece piece; Color color;
            get_piece_and_color(1ULL << (8 * i + j), piece, color);
            std::cout << (piece == NONE ? ' ' : char(piece_map[piece] + (color ? '\0' : ' '))) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << (char)('A' + i) << " ";
    }
    std::cout << "\n";
}

void Board::print_moves() const {
    for (int i = 0; i < move_count[turn]; i++) {
        moves[turn][i].print();
    }
}