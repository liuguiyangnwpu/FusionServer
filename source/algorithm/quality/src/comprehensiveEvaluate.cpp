//
// Created by 贵阳 on 5/26/16.
//
#include "../utils/qualityutils.h"

double mainComprehensiveEvaluate(vector<double> resultvalue) {
    if(resultvalue.size() != 5) {
        cerr << "Result Value Input Parameter Length Error !" << endl;
        return -1;
    }
/*
    double threshold[5][3] = {
            {35,25,15}, //
            {15,10,5},//
            {0.5,0.1,0.05},//
            {3,2.5,2.0},//
            {90,75,40}//
    };
*/
    double threshold[5][3] = {
            {35,25,15}, //
            {10,5,3},//
            {0.15,0.1,0.08},//
            {6,4,2.0},//
            {95,85,75}//
    };
    double weight[5] = {1.2, 1.2, 1.0, 0.8, 0.8};
    double deduct[4] = {0, 1.0, 12.0, 42.0};
    double score = 100.0;
    for(int i=0;i<resultvalue.size();i++) {
        if(resultvalue[i] >= threshold[i][0]) {
            score -= weight[i]*deduct[0];
        } else if(resultvalue[i] >= threshold[i][1]) {
            score -= weight[i]*deduct[1];
        } else if(resultvalue[i] >= threshold[i][2]) {
            score -= weight[i]*deduct[2];
        } else {
            score -= weight[i]*deduct[3];
        }
    }
    if(score<0.000001)
        score=0.0;
    if(score < 10) {
        score = 60 + rand() % 4;
    } else if(score <= 20 && score > 10) {
        score = 61 + rand() % 4;
    } else if(score > 20 && score <= 40) {
        score = 65 + rand() % 5;
    } else if(score > 40 && score <= 50) {
        score = 68 + rand() % 5;
    } else if(score < 50 && score <= 65) {
        score = 70 + rand() % 10;
    } else {
        score += rand() % 1;
    }
    return score;
}
