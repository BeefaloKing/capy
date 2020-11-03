# Cellular Automata Pseudoranom Number Generator Analyzer (CAPY)

ECE 3710 Project by Ava Decker  
10730830

# Usage

```
Cellular Automata Pseudorandom number generator analYzer (CAPY).

Simulates a CA PRNG to and maps arbitrary output streams to all possible walks
through the machines internal state. Determines the total number of distinct
output streams and distinct internal states (or superpositions of states) the
machine can be determined to exist as the output stream grows in length.

Usage:
capy.exe [options] <file>

Stores analysis in <file>.
-s <size>   Uses an automata with <size> cells. Must be between 1 and 64.
-o <cell>   Read output from the <cell>th cell. Must be less than <size>. Cells
            are counted from right to left.
            Default: 0
```

# TODO

- Better progress.
- Output data records.
