# cg

So many chords, so little time. What to do? Brute-force the search space! Now with 700% more unnecessary multithreading. `cg` is the tool for the job, with the elegance of COBOL, the comprehensibility of boolean algebra and the beauty of a Soviet housing block.

## Examples
```shell
$ echo "(-3) 0 11 16 18" | ./cg inspect fingering
fingering: 5 x 6 6 4 x, chord: (-3) 0 11 16 18
```
Finds and prints the best fingering for Amaj7#11 (more on the chord notation later)

```shell
$ echo "(0) 0 11 16 18" | ./cg transpose | ./cg inspect fingering-score fingering | sort -n -k2,2
No shifts given, will try -8 to 12.
21 transpositions generated.
Inspecting: fingering-score fingering 
fingering-score: 89, fingering: 8 x x 11 0 0, chord: (0) 0 11 16 18
fingering-score: 98, fingering: 1 x 2 2 0 x, chord: (-7) 0 11 16 18
fingering-score: 98, fingering: 3 x 4 6 0 x, chord: (-5) 0 11 16 18
fingering-score: 104, fingering: 13 x x 14 12 0, chord: (5) 0 11 16 18
fingering-score: 116, fingering: 2 x 3 3 1 x, chord: (-6) 0 11 16 18
fingering-score: 118, fingering: 6 x 7 7 x 0, chord: (-2) 0 11 16 18
fingering-score: 122, fingering: 4 x 5 5 3 x, chord: (-4) 0 11 16 18
fingering-score: 125, fingering: 5 x 6 6 4 x, chord: (-3) 0 11 16 18
...
```
Finds and prints the best fingering scores for 20 7#11 chords where the intervals appear in the given order, sorting by which has the best fingering (lower score is better).

`cg` can also generate chords, but has no sense of what chords actually sound good. By restricting the generator with rules, however, you can nudge it in a less (or more) jazzy direction!
```shell
$ ./cg gen "lydian && d5 && M7 && ! M2 && ! M6 && low-root" | ./cg transpose | ./cg inspect | grep -Ev 'note-count: (1|2|3)' | sort -n -k2,2 | less
No shifts given, will try -8 to 12.
No inspection keys given.
Inspecting: fingering-score fingering root note-count intervals fingering-reason 
Rule interpreted as: lydian d5 M7 M2 ! M6 ! low-root && && && && &&
Conditions:
070707003030
000000700000
000000000003
007000000000
000000000300
100000000000
Truth table has size 64
5376 chord types found.
112896 transpositions generated.
fingering-score: 5, fingering: 3 2 0 0 2 2, root: -5, note-count: 6, intervals: 0 4 7 12 18 23, fingering-reason: [open-string=-10 register=5 stretch=10], chord: (-5) 0 4 7 12 18 23
fingering-score: 7, fingering: 4 3 0 0 3 3, root: -4, note-count: 6, intervals: 0 4 6 11 18 23, fingering-reason: [open-string=-10 register=7 stretch=10], chord: (-4) 0 4 6 11 18 23
fingering-score: 7, fingering: 4 3 0 0 3 x, root: -4, note-count: 5, intervals: 0 4 6 11 18, fingering-reason: [open-string=-10 register=7 stretch=10], chord: (-4) 0 4 6 11 18
fingering-score: 7, fingering: 4 3 0 0 x x, root: -4, note-count: 4, intervals: 0 4 6 11, fingering-reason: [open-string=-10 register=7 stretch=10], chord: (-4) 0 4 6 11
fingering-score: 11, fingering: 4 3 0 0 3 4, root: -4, note-count: 6, intervals: 0 4 6 11 18 24, fingering-reason: [open-string=-10 register=11 stretch=10], chord: (-4) 0 4 6 11 18 24
fingering-score: 11, fingering: 4 3 0 0 4 4, root: -4, note-count: 6, intervals: 0 4 6 11 19 24, fingering-reason: [open-string=-10 register=11 stretch=10], chord: (-4) 0 4 6 11 19 24
fingering-score: 11, fingering: 4 3 0 0 4 x, root: -4, note-count: 5, intervals: 0 4 6 11 19, fingering-reason: [open-string=-10 register=11 stretch=10], chord: (-4) 0 4 6 11 19
fingering-score: 13, fingering: 1 2 2 2 x x, root: -7, note-count: 4, intervals: 0 6 11 16, fingering-reason: [register=3 stretch=10], chord: (-7) 0 6 11 16
...
```
That generates all chords using only the notes from Lydian that have a dimished 5th (sharp 4, really), major 7th, no major 2nd, no major 6th and with the root being the lowest note, eliminates all that have less than 4 notes and sorts the results by easiest-to-finger. OK, a lot of them are still pretty avant-garde, but you get the idea.

## Installation
```shell
$ git clone https://github.com/cynic64/cg
$ cd cg
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./cg [...]
```

## Usage
`cg` has only 3 subcommands: `gen`, `transpose` and `inspect`.

### `./cg gen`
Expects exactly 1 argument, the rule that the generated chords should match. `gen` will only output chords with a root of C1 (that's the 3rd fret on the A string), you can shift this around with `./cg transpose`. What `gen` really generates is "chord types", which means a root and a list of intervals present from that root. Here's a line of example output from `gen`:
`(0) 0 4 7 12`
That's a chord with a root of C1 ( `(0)` ) and notes offset 0, 4, 7 and 12 semitones above that. In other words, a C major triad with the root repeated one octave higher. If the root note isn't the lowest of the bunch, negative offsets will be output to represent a note offset _below_ the root:
`(0) -5 0 4`
Like I said, elegant as COBOL. The good thing is that you don't have to read these, they're just a useful representation to pipe into the other tools.

Now, onto the rule syntax. There is a list of pre-defined rules in rules.txt that you can combine however you want, and look at as examples. Rules are combined through 3 operators: `&&`, `||`, and `!`. `&&` and `||` are binary operators and do what you'd expect, and `!` is a unary operator that inverts its operand. Some examples:

- `m2` matches all chords with a minor 2nd
- `ionian && ! ( M6 && M7 )` matches all chords that only have notes from the Ionian scale and don't have both a major 6th and major 7th
- `phrygian-dom && ! m2 && ( M3 || m6 ) && ! 4 && low-root` matches all chords that only have notes from Phrygian dominant with no minor 2nd, either a major 3rd or minor 6th, no 4th and that have the root note as the lowest note

**All operators MUST be space-separated!**. Sorry

There _are_ rules for order of precedence and all that, but I've kind of forgotten them. Be generous with the parentheses! You can also define your own rules in a slightly more powerful way, but that requires an even more arcane syntax we'll get to later.

### `./cg transpose`
Takes whatever chords come in stdin and outputs transposed versions of each, one for each command-line argument. `./cg transpose 0 1 2 -5` will transpose every input chord 0, 1, 2 and -5 semitones up. If no arguments are given, will transpose to everything in the range -8..12, which is totally arbitrary but also close enough to "everything that fits on the fretboard".

### `./cg inspect`
Inspects every chord read from stdin. There are 6 things you can inspect: `root`, `intervals`, `note-count`, `fingering`, `fingering-score`, and `fingering-reason`. Inspects whatever you give it, or everything if no arguments are passed. Examples:

- `./cg inspect root intervals` print the root note and intervals for each chord in the input
- `./cg inspect` print everything `inspect` knows how to inspect

`inspect` outputs its results one-line-per-chord, which does make it a little hard to read. However, it has the advantage of being easily pipeable. Try these out for fun:
```shell
$ ./cg gen 'low-root && harmonic-minor && ! m6 && m3 && ! 4 && 5 && ( M7 || M2 )' | ./cg transpose | ./cg inspect | grep 'note-count: 6' | sort -n -k2,2 | head -n 100  # Easiest-to-finger 6-note harmonic minor chords
$ ./cg gen 'low-root && harmonic-minor && ! m6 && m3 && ! 4 && 5 && ( M7 || M2 )' | ./cg transpose | ./cg inspect | perl -ne '$_=~/note-count: (\d+)/;print if($1>=3&&$1<=5);' | shuf | head -n 50  # 50 random 3- to 5- note harmonic minor chords (most of them impossible to play on a guitar)
$ ./cg gen 'ionian && ! M2 && ! 4 && 5 && high-root && ! M6' | ./cg transpose | ./cg inspect | grep -Ev 'note-count: (1|2|3)' | grep -Ev 'fingering-score: 2147' | grep -Eo --color=never 'fingering: ([x0-9]+ ?){6}'  # just the fingerings
```

Hours of joy!

## Details on Rule Syntax
Have a look at rules.txt and you'll see how the basic rules, like `m6` and `root` are defined:
```
low-root: 100000000000

root: 700000000000
m2:   070000000000
M2:   007000000000
m3:   000700000000
...
```

The generator works representing (almost) every possible chord as a 32-bit integer. The numbers in rules.txt are in octal and are in the same format as the 32-bit integers used by the generator. Each bit represents in which octaves that a is present. Looking at the individual octal numbers in binary might make more sense:
- `0`: `000` --> This note is not present in any octave
- `1`: `001` --> Present 0 octaves above the root
- `2`: `010` --> Present 1 octave above the root
- `3`: `011` --> Present 0 and 1 octaves above the root
- `4`: `100` --> Present 2 octaves above the root
- `5`: `101` --> Present 0 and 2 octaves above the root
- `6`: `110` --> Present 1 and 2 octaves above the root
- `7`: `111` --> Present 0, 1, and 2 octaves above the root
The first octal number represents the interval 0, which is the root note. The second octal represents the interval 1, which is 1 semitone above the root (a minor 2nd). The 3rd octal number represents the interval 2, etc. all the way up to interval 11, which is a major 7th. This:
`730100000002`

Represents a chord with the root note in octaves 0, 1 and 2 (`7`), a minor second present 0 and 1 octaves of the root (`3`), a minor 3rd present 0 octaves above the root (`0`), and a major 7th present 1 octave above the root (plus another 11 semitones, so almost 2 octaves) (`4`). That's an ugly chord, but hopefully it makes _slightly_ more sense now.

The first 8 octal numbers can go up to 7, but the last 4 only go up to 4 (2 bits) to be able to squeeze everything into 32 bits.

A rule "matches" a chord when AND-ing a chord with the rule's bitmask returns a positive result. Let's look at the "root" rule:
`700000000000` a.k.a `11100000000000000000000000000000` in binary
So, "root" matches whenever the root note is present in any octave.

Good luck!
