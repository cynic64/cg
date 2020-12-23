#ifndef CG_FINGER_H
#define CG_FINGER_H

#include <array>
#include <vector>
#include <sstream>
#include <cstdint>

#include "chord.hpp"

namespace finger {
	const auto STRINGS = 6;

	// [0] = low E, [5] = high E. -1 means a string is not played, otherwise
	// the value represents the fret.
	typedef std::array<int, STRINGS> Fingering;

	const Fingering EMPTY_FING = {-1, -1, -1, -1, -1, -1};

	// Offsets from C1 (3rd fret on the A string)
	const std::array<int, STRINGS> OFFSETS = {-8, -3, 2, 7, 11, 16};

	const int HIGHEST_FRET = 24;

	namespace scoring {
		const auto WORST = INT32_MAX;
		// A positive number is a penalty, a negative number is a bonus
		// (remember, lower is better)
		const auto STRETCH = 10;     // Added for every absolute
					     // difference to the lowest note
		const auto OPEN_STRING = -1; // Added for every open string
		const auto REGISTER = 1;     // Added for every fret (so lower
					     // frets are preferred)
	}

	int calc_score(Fingering fing) {
		auto string0_fret = fing[0];
		auto score = 0;
		for (auto fret : fing) {
			if (fret == 0) { score += scoring::OPEN_STRING; continue; }
			score += abs(fret-string0_fret) * scoring::STRETCH;
			score += fret * scoring::REGISTER;
		}

		return score;
	}

	// Returns (score, fingering). Lower score is better. scoring::WORST is
	// returned if there are too many notes to fit on a fretboard. Lower
	// score is better.
	std::pair<int, Fingering> finger(chord::Chord chord, int start,
					 Fingering fing, int taken_ct) {
		int N = chord.notes.size();
		// Edge case: done arranging or all strings used
		if (start == N || taken_ct == STRINGS) {
			if (start < N) return {scoring::WORST, fing};
			else return {calc_score(fing), fing};
		}

		// Otherwise, put the next note on every string and return the
		// best score (recursive)
		auto best_score = scoring::WORST;
		auto best_fing = EMPTY_FING;
		auto note = chord.root + chord.notes[start];
		for (auto string = 0; string < STRINGS; ++string) {
			auto string_already_taken = fing[string] != -1;
			if (string_already_taken) continue;

			auto fret = note - OFFSETS[string];
			if (fret < 0 || fret > HIGHEST_FRET) continue;

			auto f = fing;
			f[string] = fret;

			auto [score, complete_fing] = finger(chord, start+1, f, taken_ct+1);
			if (score < best_score) {
				best_score = score;
				best_fing = complete_fing;
			}
		}

		return {best_score, best_fing};
	}

	void print(std::ostream& out, Fingering fing) {
		auto first = true;
		for (auto n : fing) {
			if (!first) out << " ";
			first = false;
			if (n < 0) out << "x";
			else out << n;
		}
	}
}

#endif // CG_FINGER_H
