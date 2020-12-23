#ifndef CG_INSPECT_H
#define CG_INSPECT_H

#include <array>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <sstream>

#include "chord.hpp"
#include "finger.hpp"
#include "helpers.hpp"

namespace inspect {
	enum class Key {Fingering, FingeringScore, Intervals, NoteCount};

	const std::vector<Key> ALL_KEYS = {Key::FingeringScore, Key::Fingering, Key::Intervals, Key::NoteCount};

	const std::unordered_map<std::string, Key> KEY_NAMES = {{"fingering", Key::Fingering},
								{"fingering-score", Key::FingeringScore},
								{"intervals", Key::Intervals},
								{"note-count", Key::NoteCount}};

	typedef std::unordered_map<Key, std::string> Details;

	std::string to_string(Key key) {
		for (auto [k, v] : KEY_NAMES) if (v == key) return k;
		throw std::runtime_error("Invalid key");
	}

	Key from_string(const char * s) {
		if (KEY_NAMES.find(s) == KEY_NAMES.end()) throw std::runtime_error("Invalid key string");
		return KEY_NAMES.at(s);
	}

	Details inspect(chord::Chord c, std::vector<Key>& keys) {
		Details details;

		auto intervals = chord::to_intervals(c);
		details[Key::Intervals] = helpers::fmt_vector(intervals);

		for (auto k : keys) {
			if (k == Key::Fingering || k == Key::FingeringScore) {
				if (details.find(Key::Fingering) != details.end()) continue;
				
				auto [score, fing] = finger::finger(intervals, 0, finger::EMPTY_FING, 0);
				details[Key::FingeringScore] = std::to_string(score);
				std::ostringstream oss;
				finger::print(oss, fing);
				details[Key::Fingering] = oss.str();
			} else if (k == Key::NoteCount) {
				auto count = intervals.size();
				details[Key::NoteCount] = std::to_string(count);
			} else if (k == Key::Intervals) {
			} else throw std::runtime_error("Unknown key");
		}

		return details;
	}
}

#endif // CG_INSPECT_H
