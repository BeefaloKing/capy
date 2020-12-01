# Cellular Automata Pseudoranom Number Generator Analyzer (CAPY)

ECE 3710 Project by Ava Decker  
10730830

# Usage

```
capy.exe [options] <directory>
Simulates a CA PRNG to and maps arbitrary output streams to all possible walks
through the machines internal state. Sate/output mapping will be stored in
<directory> which must be empty.

Generate Mode:
-g              Enables generate mode. If used with -a, will generate first.
-s <cells>      Uses an automata of size <cells>. Must be between 1 and 64.
                Default: 8
-o <bits>       Read output of size <bits> from each state. Must be smaller
                than <cells> specified from -s.
                Default: 1

Analyze Mode:
-a              Enables analyze mode. if used with -g, will generate first.
-l <value>      Looks ahead <value> number of states in each read. Does bigger
                sequential reads but may increase total I/O.
                Default: 1
```

# TODO

- Use human readable sizes.
- Output progress.
- Preallocate index files.
- Check performance of buffering file writes.
- Check performance of only writing indexes to disk.
