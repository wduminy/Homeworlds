#pragma once

#include "PieceCollection.h"
#include "global.h"
#include <stdio.h>
#include <string>
#include <vector>

class StarSystem {
public:
    std::string name;
    PieceCollection star;
    PieceCollection ships[NUMPLAYERS];
    int homeworldOf;

public:
    /* Each system must have a valid name; a name is valid iff it is composed
     * of valid characters. Valid characters are alphanumeric plus some
     * punctuation, but whitespace is never allowed in system names. */
    static bool is_valid_name(const char *name);

public:
    enum { MAXSTRLEN = 2 + 24*(NUMPLAYERS+1) };

    explicit StarSystem(): homeworldOf(-1) { }
    explicit StarSystem(const char *n): name(n), homeworldOf(-1) { }
    int numberOfShips() const;
    bool hasNoShips() const;
    int numberOf(Color c) const;
    int number() const { return numberOfShips() + star.number(); }
    bool canCatastropheStar() const;
    bool containsOverpopulation(Color c) const { return (this->numberOf(c) >= 4); }
    bool containsOverpopulation() const;
    void performCatastrophe(Color c, PieceCollection &stash);
    bool playerHasAccessTo(int player, Color c) const {
        return (star.numberOf(c) > 0) || (ships[player].numberOf(c) > 0);
    }

    bool isAdjacentTo(const StarSystem &that) const {
        return this->star.isAdjacentTo(that.star);
    }
    PieceCollection pieceCollection() const {
        PieceCollection pc = star;
        for (const auto& fleet : ships) {
            pc += fleet;
        }
        return pc;
    }

    std::string toString() const;
    std::string toComparableString() const;
    char *toComparableString(char buffer[MAXSTRLEN+1]) const;
    bool scan(const char *text);

    friend void operator+=(PieceCollection& pc, const StarSystem& ss) {
        pc += ss.star;
        for (const auto& ships : ss.ships) {
            pc += ships;
        }
    }

    friend void operator-=(PieceCollection& pc, const StarSystem& ss) {
        pc -= ss.star;
        for (const auto& ships : ss.ships) {
            pc -= ships;
        }
    }
};

class GameState {
public:
    std::vector<StarSystem> stars;
    PieceCollection stash;

public:
    const StarSystem *systemNamed(const char *name) const;
    StarSystem *systemNamed(const char *name);
    const StarSystem *homeworldOf(int player) const;
    StarSystem *homeworldOf(int player);
    bool containsOverpopulation() const;
    void performAllCatastrophes();

    void removeSystemNamed(const char *name);
    void removeSystem(StarSystem &star);
    void newGame();
    bool hasLost(int who) const;
    bool gameIsOver() const { return hasLost(0) || hasLost(1); }

    /* Return a human-readable representation of this game state. */
    std::string toString() const;

    /* Return a compact representation of this game state that is guaranteed to be
     * identical to the compact representation of any equivalent game state, and
     * distinct from the compact representation of any non-equivalent game state.
     * In other words, two GameStates are "basically the same position" iff
     * g1.toComparableString() == g2.toComparableString(). */
    std::string toComparableString() const;

    /* Scan a human-readable representation of this game state, as produced
     * by toString(). Since the representation may span multiple lines, return
     * the first unparseable line as a string (so that we don't lose it). */
    std::string scan(FILE *fp);

    /* Scan a human-readable representation of this game state, as produced
     * by toString(). Return `true` if scanning succeeded and `false` if it
     * failed. Having extra text left at the end of the input counts as failure.
     */
    bool scan(const char *text);

    /* Flip the players: make player 0 player 1 and vice versa. */
    GameState mirror() const;
};

/* Inline definitions for the toComparableString methods. */
#include "ToComparableString.h"
