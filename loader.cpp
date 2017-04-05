#include <iostream>
#include <string>
#include <vector>

#include "boilerplate.cpp"

using std::string;
using std::printf;
using std::strcmp;
using std::vector;

class Loader {
    string text;
    VertexArray* va;

public:
    Loader(string textToDisplay, VertexArray * vArray){
        text = textToDisplay;
        va = vArray;
    }

    void load() {

        string path = "cmuntt/gly_";

        vector<float> points;

        int pos = 0;
        int length = text.length();
        for (; pos < length; pos++) {
            int letter = text[pos];

            if (letter == 32) continue;

            float startPos[2];

            float previousEndPoint[2];

            string letterPath = path + letter;

            FILE * file = fopen(letterPath,"r");

            bool openFile = true;
            if (file == NULL) {
                printf("Impossible to open the file, " + letterPath + "\n");
                openFile = false;
            }

            while (openFile) {
                char lineType[2];
                int res = fscanf(file, "%s", lineType);

                if (res == EOF) {
                    break;
                }
                if (strcmp(lineType, "M") == 0) {
                    float coords[2];
                    fscanf(file, "%f %f\n", coords[0], coords[1]);
                    startPos = coords;
                    previousEndPoint = coords;
                } else if (strcmp(lineType, "C") == 0) {
                    float point1[2];
                    float point2[2];
                    float point3[2];
                    fscanf(file, "%f %f %f %f %f %f",
                        point1[0], point1[1],
                        point2[0], point2[1],
                        point3[0], point3[1]);


                    points.push_back(previousEndPoint[0]);
                    points.push_back(previousEndPoint[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                    points.push_back(point2[0]);
                    points.push_back(point2[1]);
                    points.push_back(point3[0]);
                    points.push_back(point3[1]);

                    previousEndPoint = point3;
                } else if (strcmp(lineType, "L") == 0) {
                    float point0[2] = previousEndPoint;
                    float point1[2];

                    fscanf(file, "%f %f", point1[0], point1[1]);

                    float middle1[2];
                    float middle2[2];

                    middle1[0] = point0[0] * 0.75 + point1[0] * 0.25;
                    middle1[1] = point0[1] * 0.75 + point1[1] * 0.25;

                    middle2[0] = point0[0] * 0.25 + point1[0] * 0.75;
                    middle2[1] = point0[1] * 0.25 + point1[1] * 0.75;

                    points.push_back(point0[0]);
                    points.push_back(point0[1]);
                    points.push_back(middle1[0]);
                    points.push_back(middle1[1]);
                    points.push_back(middle2[0]);
                    points.push_back(middle2[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                } else if (strcmp(lineType, "Z") == 0) {
                    float point0[2] = previousEndPoint;
                    float point1[2] = startPos;

                    float middle1[2];
                    float middle2[2];

                    middle1[0] = point0[0] * 0.75 + point1[0] * 0.25;
                    middle1[1] = point0[1] * 0.75 + point1[1] * 0.25;

                    middle2[0] = point0[0] * 0.25 + point1[0] * 0.75;
                    middle2[1] = point0[1] * 0.25 + point1[1] * 0.75;

                    points.push_back(point0[0]);
                    points.push_back(point0[1]);
                    points.push_back(middle1[0]);
                    points.push_back(middle1[1]);
                    points.push_back(middle2[0]);
                    points.push_back(middle2[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                }
            }

        }

        va->addBuffer("v", 0, points);

    }
};
