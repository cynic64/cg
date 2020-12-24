#ifndef CG_FINGER_H
#define CG_FINGER_H

#include <array>
#include <vector>
#include <sstream>
#include <cstdint>
#include <map>
#include <utility>

#include "chord.hpp"

namespace finger {
	const auto STRINGS = 6;

	// [0] = low E, [5] = high E. -1 means a string is not played, otherwise
	// the value represents the fret.
	typedef std::array<int, STRINGS> Fingering;

	// Low E to high E
	const Fingering EMPTY_FING = {-1, -1, -1, -1, -1, -1};

	// Offsets from C1 (3rd fret on the A string)
	const std::array<int, STRINGS> OFFSETS = {-8, -3, 2, 7, 11, 16};

	const int HIGHEST_FRET = 24;
	// Thumb doesn't count!
	const auto FINGERS = 4;

	namespace scoring {
		const auto WORST = INT32_MAX;
		// A positive number is a penalty, a negative number is a bonus
		// (remember, lower is better)
		const auto STRETCH = 10;             // Added for every absolute
					             // difference to the lowest note
		const auto OPEN_STRING = -5;         // Added for every open string
		const auto REGISTER = 1;             // Added for every fret (so lower
					             // frets are preferred)
		const auto TOO_MANY_FINGERS = 40;    // Added for every finger
						     // over the max necessary
		const auto DOWN_FROM_A_BARRE = 40;   // Fretting a lower fret
						     // after a barré is hard
		const auto MUTE_IN_THE_MIDDLE = 50;  // Having to skip a string
						     // in between a bunch being
						     // played sucks
	}

	// The string is the reason (only written to if justify is set)
	std::pair<int, std::string> calc_score(Fingering fing, bool justify = false) {
		auto score = 0;
		auto string0_fret = fing[0];
		auto fingers_used = 0;
		auto last_fret = -1;
		auto barre = false;
	        auto have_played = false;
		std::map<std::string, int> counts;
		for (auto fret : fing) {
			if (fret == -1) { last_fret = fret; continue; }

			if (have_played && last_fret == -1) {
				score += scoring::MUTE_IN_THE_MIDDLE;
				if (justify) counts["mute-in-the-middle"] += scoring::MUTE_IN_THE_MIDDLE;
			}

			have_played = true;
			if (barre && fret < last_fret) {
				score += scoring::DOWN_FROM_A_BARRE;
				if (justify) counts["down-from-a-barré"] += scoring::DOWN_FROM_A_BARRE;
			}

			if (fret == 0) {
				score += scoring::OPEN_STRING;
				if (justify) counts["open-string"] += scoring::OPEN_STRING;
				continue;
			}

			// If you can barré it, it doesn't count
			if (fret == last_fret) {
				barre = true;
				continue;
			}

			auto stretch = abs(fret-string0_fret) * scoring::STRETCH;
			score += stretch;
			if (justify) counts["stretch"] += stretch;

			score += fret * scoring::REGISTER;
			if (justify) counts["register"] += fret * scoring::REGISTER;
			fingers_used++;
			if (fingers_used > FINGERS) {
				score += scoring::TOO_MANY_FINGERS;
				if (justify) counts["too-many-fingers"] += scoring::TOO_MANY_FINGERS;
			}

			last_fret = fret;
		}

		if (justify) {
			std::vector<std::string> parts;
			for (auto [name, val] : counts) parts.push_back(name + "=" + std::to_string(val));
			auto reason = "[" + helpers::fmt_vector(parts) + "]";

			return {score, reason};
		} else {
			return {score, ""};
		}
	}

	// Returns (score, fingering, reason). Lower score is
	// better. scoring::WORST is returned if there are too many notes to fit
	// on a fretboard.
	std::pair<int, Fingering> finger(chord::Chord chord, int start,
					 Fingering fing, int taken_ct) {
		int N = chord.notes.size();

		// Edge case: won't have enough strings
		if (chord.notes.size() > STRINGS) return {scoring::WORST, EMPTY_FING};
		// Edge case: done arranging
		if (start == N) {
			auto score = calc_score(fing);
			return {score.first, fing};
		}

		// Otherwise, put the next note on every string and return the
		// best score (recursive)
		std::pair<int, Fingering> best = {scoring::WORST, EMPTY_FING};
		auto note = chord.root + chord.notes[start];
		for (auto string = 0; string < STRINGS; ++string) {
			auto string_already_taken = fing[string] != -1;
			if (string_already_taken) continue;

			auto fret = note - OFFSETS[string];
			if (fret < 0 || fret > HIGHEST_FRET) continue;

			auto f = fing;
			f[string] = fret;

			auto child = finger(chord, start+1, f, taken_ct+1);
			if (child.first < best.first) best = child;
		}

		return best;
	}

	std::string fmt(Fingering fing) {
		std::vector<std::string> parts;
		for (auto fret : fing) parts.push_back(fret < -1 ? "x" : std::to_string(fret));
		return helpers::fmt_vector(parts);
	}
}

#endif // CG_FINGER_H
