# cg

So many chords, so little time. What to do? Brute-force the search space! Now with 700% more unnecessary multithreading. `cg` is the tool for the job, with the elegance of COBOL, the comprehensibility of boolean algebra and the beauty of a Soviet housing block.

## Examples
### Find optimum fingering
```shell
$ echo "(-3) 0 11 16 18" | ./cg inspect fingering
fingering: 5 x 6 6 4 x, chord: (-3) 0 11 16 18
```
Finds and prints the best fingering for Amaj7#11 (more on the chord notation later)

### Find optimum fingering in every position
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
