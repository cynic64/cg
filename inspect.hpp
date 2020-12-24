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
	enum class Key {Fingering, FingeringScore, FingeringReason, Intervals, NoteCount, Root};

	const std::vector<Key> ALL_KEYS = {Key::FingeringScore, Key::Fingering, Key::Root, Key::NoteCount, Key::Intervals, Key::FingeringReason};

	const std::unordered_map<std::string, Key> KEY_NAMES = {{"fingering", Key::Fingering},
								{"fingering-score", Key::FingeringScore},
								{"fingering-reason", Key::FingeringReason},
								{"intervals", Key::Intervals},
								{"note-count", Key::NoteCount},
								{"root", Key::Root}};

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


		for (auto k : keys) {
			if (k == Key::Fingering || k == Key::FingeringScore || k == Key::FingeringReason) {
				if (details.find(Key::Fingering) != details.end()) continue;
				
				auto [score, fing] = finger::finger(c, 0, finger::EMPTY_FING, 0);
				auto reason = finger::calc_score(fing, true).second;
				details[Key::FingeringScore] = std::to_string(score);
				details[Key::Fingering] = finger::fmt(fing);
				details[Key::FingeringReason] = reason;
			} else if (k == Key::NoteCount) {
				auto count = c.notes.size();
				details[Key::NoteCount] = std::to_string(count);
			} else if (k == Key::Intervals) {
				details[Key::Intervals] = helpers::fmt_vector(c.notes);
			} else if (k == Key::Root) {
				details[Key::Root] = std::to_string(c.root);
			} else throw std::runtime_error("Unknown key");
		}

		return details;
	}
}

#endif // CG_INSPECT_H
