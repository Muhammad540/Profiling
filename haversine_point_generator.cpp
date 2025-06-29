#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "haversine_formula.cpp"

typedef uint32_t u32;
typedef uint64_t u64;
#define U64Max UINT64_MAX

struct random_series {
    u64 A, B, C, D;
};

static u64 RotateLeft(u64 Value, int Shift) {
    u64 Result = ((Value << Shift) | (Value >> (64 - Shift)));
    return Result;
}

static u64 RandomU64(random_series *Series) {
    u64 A = Series->A;
    u64 B = Series->B;
    u64 C = Series->C;
    u64 D = Series->D;
    
    u64 E = A - RotateLeft(B, 27);
    
    A = (B ^ RotateLeft(C, 17));
    B = (C + D);
    C = (D + E);
    D = (E + A);
    
    Series->A = A;
    Series->B = B;
    Series->C = C;
    Series->D = D;
    
    // D is the random 64 bit number
    return D;
}

static random_series Seed(u64 Value) {
    random_series Series = {};

    Series.A = 0xf1ea5eed;
    Series.B = Value;
    Series.C = Value;
    Series.D = Value;
    
    u32 Count = 20;
    while(Count--){
        RandomU64(&Series);
    }

    return Series;
}

static double RandomInRange(random_series *Series, double Min, double Max){
    double t = (double)RandomU64(Series) / (double)U64Max;
    double Result = (1.0 - t)*Min + t*Max;
    
    return Result;
}

static FILE* Open(long long unsigned PairCount, const char *Label, const char *Extension){
    char Temp[256];
    sprintf(Temp, "data_%llu_%s.%s", PairCount, Label, Extension);
    FILE *Result = fopen(Temp, "wb");
    if(!Result)
    {
        fprintf(stderr, "Unable to open \"%s\" for writing.\n", Temp);
    }
    
    return Result;
}

static double RandomDegree(random_series *Series, double Center, double Radius, double MaxAllowed){
    double MinVal = Center - Radius;
    if (MinVal < -MaxAllowed){
        MinVal = -MaxAllowed;
    }

    double MaxVal = Center + Radius;
    if (MaxVal > MaxAllowed){
        MaxVal = MaxAllowed;
    }

    double Result = RandomInRange(Series, MinVal, MaxVal);
    return Result;
}

int main(int ArgCount, char **Args) {
    if (ArgCount == 4){
        u64 ClusterCountLeft = U64Max;
        double MaxAllowedX = 180.0;
        double MaxAllowedY = 90.0;

        double XCenter = 0.0;
        double YCenter = 0.0;
        double XRadius = MaxAllowedX;
        double YRadius = MaxAllowedY;

        const char *MethodName = Args[1];
        if(strcmp(MethodName, "cluster") == 0){
            ClusterCountLeft = 0;
        } else if(strcmp(MethodName, "uniform") != 0){
            MethodName = "uniform";
            fprintf(stderr, "Warning: %s is not a valid method name, using %s instead\n", Args[1], MethodName);
        }
        
        u64 SeedValue = atoll(Args[2]);
        random_series series = Seed(SeedValue);

        u64 MaxPairCount = (1ULL << 34);
        u64 PairCount = atoll(Args[3]);
        if (PairCount < MaxPairCount){
            u64 ClusterCountMax = 1 + (PairCount / 64);

            FILE *FlexJSON = Open(PairCount, "flex", "json");
            FILE *HaverAnswers = Open(PairCount, "haveranswer", "json");
            if (FlexJSON && HaverAnswers){
                fprintf(FlexJSON, "{\"pairs\":[\n");
                double Sum = 0;
                double sumCoef = 1.0/(double)PairCount;
                for(u64 PairIndex = 0; PairIndex < PairCount; ++PairIndex){
                    if (ClusterCountLeft-- == 0){
                        ClusterCountLeft = ClusterCountMax;
                        XCenter = RandomInRange(&series, -MaxAllowedX, MaxAllowedX);
                        YCenter = RandomInRange(&series, -MaxAllowedY, MaxAllowedY);
                        XRadius = RandomInRange(&series, 0.0, MaxAllowedX);
                        YRadius = RandomInRange(&series, 0.0, MaxAllowedY);
                    }

                    double X0 = RandomDegree(&series, XCenter, XRadius, MaxAllowedX);
                    double Y0 = RandomDegree(&series, YCenter, YRadius, MaxAllowedY);
                    double X1 = RandomDegree(&series, XCenter, XRadius, MaxAllowedX);
                    double Y1 = RandomDegree(&series, YCenter, YRadius, MaxAllowedY);

                    double EarthRadius = 6371.8;
                    double HaversineDistance = ReferenceHaversine(X0, Y0, X1, Y1, EarthRadius);

                    Sum += sumCoef * HaversineDistance;
                    char const *JSONSep = (PairIndex == (PairCount - 1)) ? "\n" : ",\n";
                    fprintf(FlexJSON, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}%s", X0, Y0, X1, Y1, JSONSep);
                    
                    fwrite(&HaversineDistance, sizeof(HaversineDistance), 1, HaverAnswers);
                }
                fprintf(FlexJSON, "]}\n");
                fwrite(&Sum, sizeof(Sum), 1, HaverAnswers);
        
                fprintf(stdout, "Method: %s\n", MethodName);
                fprintf(stdout, "Random seed: %llu\n", (long long unsigned)SeedValue);
                fprintf(stdout, "Pair count: %llu\n", (long long unsigned)PairCount);
                fprintf(stdout, "Expected sum: %.16f\n", Sum);
            }
            
            if(FlexJSON) fclose(FlexJSON);
            if(HaverAnswers) fclose(HaverAnswers);

        } else {
            fprintf(stderr, "To avoid accidentally generating massive files, number of pairs must be less than %llu.\n", MaxPairCount);
        }
    } else {
        fprintf(stderr, "Usage: %s [uniform/cluster] [random seed] [number of coordinate paris to generate]\n", Args[0]);
    }

    return 0;
}