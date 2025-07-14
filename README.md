# Profiling
- [x] Generate (x,y) pairs to compute Haversine distance store the points in a JSON file
- [x] Also compute a reference Haversine distance (we will compare the results with this to make sure our JSON parser impl is correct)
- [x] Write a Simple JSON Parser
- [x] Parse the JSON file and compute the Haversine distance
- [x] Confirm that the Haversine distance is the same as the reference implementation

## How to run
To generate sample data:
```bash
g++ -o haversine_point_generator haversine_point_generator.cpp
./haversine_point_generator -h # to see the usage
./haversine_point_generator uniform 1234567890 1000000 > data_5_flex.json # to generate 1000000 (x,y) pairs
```

To parse the JSON file and compute the Haversine distance:
```bash
g++ -o main main.cpp json/json_parser.cpp
./main <json_file>
```

Compare the two sums to make sure the JSON parser is correct.

## Profiling Result (Very Primitive Profiling)
I used the RDTSC instruction to measure the time elapsed in critical sections of the code. 

- **Total Time:** 0.00176518 seconds (CPU Freq: 3 GHz)
- **Read:** 174016 TSC ticks (3.09% of total)
- **Misc Setup:** 44320 TSC ticks (0.79% of total)
- **Parse JSON:** 4691296 TSC ticks (83.20% of total)
- **Sum:** 676512 TSC ticks (12.00% of total)
- **Misc Output:** 30624 TSC ticks (0.54% of total)

As can be seen the JSON parsing is the most time consuming part of the code. I am not saying JSON is bad, but clearly we can do better :)