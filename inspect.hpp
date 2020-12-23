#ifndef CG_INSPECT_H
#define CG_INSPECT_H

#include <array>
#include <vector>
#include <cstdint>
#include <cstdlib>

#include "chord.hpp"

namespace inspect {
	const auto STRINGS = 6;

	// [0] = low E, [5] = high E. -1 means a string is not played, otherwise
	// the value represents the fret.
	typedef std::array<int, STRINGS> Fingering;

	const Fingering EMPTY_FING = {-1, -1, -1, -1, -1, -1};

	// Offsets from C1 (3rd fret on the A string)
	const std::array<int, STRINGS> OFFSETS = {-8, -3, 2, 7, 11, 16};

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
	std::pair<int, Fingering> finger(std::vector<int> notes, int start,
					 Fingering fing, int taken_ct) {
		int N = notes.size();
		// Edge case: done arranging or all strings used
		if (start == N || taken_ct == STRINGS) {
			if (start < N) return {scoring::WORST, fing};
			else return {calc_score(fing), fing};
		}

		// Otherwise, put the next note on every string and return the
		// best score
		auto best_score = scoring::WORST;
		auto best_fing = EMPTY_FING;
		auto note = notes[start];
		for (auto string = 0; string < STRINGS; ++string) {
			auto string_already_taken = fing[string] != -1;
			if (string_already_taken) continue;

			auto fret = note - OFFSETS[string];
			if (fret < 0) continue;

			auto f = fing;
			f[string] = fret;

			auto [score, complete_fing] = finger(notes, start+1, f, taken_ct+1);
			if (score < best_score) {
				best_score = score;
				best_fing = complete_fing;
			}
		}

		return {best_score, best_fing};
	}

	void print_fingering(Fingering fing, bool newline = true) {
		for (auto n : fing) {
			if (n < 0) std::cout << "x ";
			else std::cout << n << " ";
		}
		if (newline) std::cout << std::endl;
	}

	void inspect(chord::Chord c, bool compact = false) {
		auto intervals = chord::to_intervals(c);
		auto [score, fing] = inspect::finger(intervals, 0, inspect::EMPTY_FING, 0);

		if (compact) {
			std::cout << "score: " << score << ", fingering: ";
			print_fingering(fing, false);
		} else {
			std::cout << "Best fingering score: " << score << std::endl;
			std::cout << "Fingering:" << std::endl;
			inspect::print_fingering(fing);
		}
	}
}

#endif // CG_INSPECT_H
