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