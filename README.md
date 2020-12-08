# Cellular Automata Pseudoranom Number Generator Analyzer (CAPY)

ECE 3710 Project by Ava Decker  
10730830

# Usage

```
Cellular Automata Pseudorandom number generator analYzer (CAPY)

Simulates a CA PRNG and maps arbitrary output streams to all possible walks
through the machines internal state. Determines the total number of distinct
output streams and distinct internal states the machine can be determined to
exist in as the output stream grows in length.

Usage:
capy.exe [options] <output file>

Options:
-h          Displays this message.
-r <rule>   Specifies the elementary cellular automata rule to use.
            Supported rules are 30, 45, 54, 90, 105, and 124.
-s <size>   Uses an automata of <size> cells. Must be between 1 and 64.
-o <cell>   Read output from the <cell>th cell. Must be between 0 and <size>.
            Cells are counted from right to left.
            Default: 0

```

# TODO

- Build environment setup guide.
