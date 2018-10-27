#ifndef NLG_H
#define NLG_H

struct NoiseParams {
    float z = 0;
    float levelMin = -1, levelMax = 1;
    int octaves = 3;
    float persistence = 0.5;
    float lacunarity = 2;
};

#endif //NLG_H
