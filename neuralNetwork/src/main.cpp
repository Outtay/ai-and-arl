#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <random>
#include <stack>
#include <cmath>
#include <random>
#include <algorithm>
#include <ctime>


//Index:
//0 -> Iris setosa
//1 -> Iris versicolor
//2 -> Iris virginica

struct flowerData{
    float v1;
    float v2;
    float v3;
    float v4;
    int index;
};

typedef struct matrix_t{
    size_t rows;
    size_t cols;
    float** data;
} matrix_t;

float sigmoid (float x) {
    return 1/(1+std::exp(-x));
}
float sigmoidDerivative (float x){
    return sigmoid(x)*(1-sigmoid(x));
}


void Parser(const std::string &fileName, std::vector<flowerData> &dataVector);
void SeparateData(const std::vector<flowerData> &dataVector, std::vector<flowerData> &trainingVector, std::vector<flowerData> &validationVector);
void InitMatrix(matrix_t &matrix, size_t row, size_t col);
void FillMatrix(matrix_t &matrix);
void MatrixMult(matrix_t &A, matrix_t &B, matrix_t &C);
void Activation(const matrix_t &matrix, matrix_t &result);
void SoftMax(const matrix_t & matrix, matrix_t &result);
void HadamardProduct(const matrix_t &A, const matrix_t &B, matrix_t &result);
void Transpose(const matrix_t &A, matrix_t &result);
void OutputErrorFunction(const matrix_t &matrix, matrix_t &result, int correctIndex);
void DerivativeActivation(const matrix_t &matrix, matrix_t &result);
void UpdateWeightMatrix(const matrix_t &deltaMatrix, matrix_t &weights);

std::default_random_engine GEN;
std::string fileName = "iris-z-score-normalized.csv";
std::string seedStr = "tttet";

float g_learningRate = 0.01;

//Input is 4 neurons
//Weight Matrix must be 20x4
//Hidden is 20 neurons
//Weight Matrix must be 3x20
//Output is 3 neurons


int main(int argc, char *argv[]){
    std::srand ( unsigned ( std::time(0) ) );
    
    std::seed_seq seed (seedStr.begin(), seedStr.end());
    GEN.seed(seed);

    if (argc < 5){
        //std::cout << "Not enough arguments" << std::endl;
        //return -1;
    }
    for (int i = 1; i < argc; i++){
        if (std::string(argv[i]).compare("-file") == 0){
            fileName = argv[++i];
        }


    }

    std::vector<flowerData> dataVector;
    Parser(fileName, dataVector);
    
    std::vector<flowerData> trainingVector;
    std::vector<flowerData> validationVector;

    SeparateData(dataVector, trainingVector, validationVector);

    //Initialize our Network
    matrix_t InputMatrix;
    InitMatrix(InputMatrix, 4, 1);

    matrix_t Input_Hidden1_WeightMatrix;
    InitMatrix(Input_Hidden1_WeightMatrix, 20, 4);
    FillMatrix(Input_Hidden1_WeightMatrix);

    matrix_t Hidden1Matrix;
    //InitMatrix(Hidden1Matrix, 20, 1);

    matrix_t Hidden1_Output_WeightMatrix;
    InitMatrix(Hidden1_Output_WeightMatrix, 3, 20);
    FillMatrix(Hidden1_Output_WeightMatrix);
    
    matrix_t OutputMatrix;
    //InitMatrix(OutputMatrix, 3, 1);

    //Training the network
    for (size_t i = 0; i < trainingVector.size(); i++){
        //Init the Input layer
        InputMatrix.data[0][0] = trainingVector[i].v1;
        InputMatrix.data[1][0] = trainingVector[i].v2;
        InputMatrix.data[2][0] = trainingVector[i].v3;
        InputMatrix.data[3][0] = trainingVector[i].v4;

        //Feedforward
        MatrixMult(Input_Hidden1_WeightMatrix, InputMatrix, Hidden1Matrix);
        matrix_t ActivatedHidden1Matrix;
        Activation(Hidden1Matrix, ActivatedHidden1Matrix);
        
        MatrixMult(Hidden1_Output_WeightMatrix, Hidden1Matrix, OutputMatrix);
        matrix_t ActivatedOutputMatrix;
        SoftMax(OutputMatrix, ActivatedOutputMatrix);
        //std::cout << OutputMatrix.data[0][0] << ", " << OutputMatrix.data[1][0] << ", " << OutputMatrix.data[2][0] << std::endl;
        
        //Backpropagation
        //
        //Last Layer
        matrix_t ErrorFunctionOutputMatrix;
        OutputErrorFunction(ActivatedOutputMatrix, ErrorFunctionOutputMatrix, trainingVector[i].index);

        matrix_t DerivativeOutputMatrix;
        DerivativeActivation(OutputMatrix, DerivativeOutputMatrix);

        matrix_t DeltaOutputMatrix;
        HadamardProduct(ErrorFunctionOutputMatrix, DerivativeOutputMatrix, DeltaOutputMatrix);

        UpdateWeightMatrix(DeltaOutputMatrix, Hidden1_Output_WeightMatrix);

        //HiddenLayer
        matrix_t Hidden1_Output_WeightMatrixTransposed;
        Transpose(Hidden1_Output_WeightMatrix, Hidden1_Output_WeightMatrixTransposed);

        //replaces the error term of the output
        matrix_t Hidden1WeightDeltaOutputMultMatrix;
        MatrixMult(Hidden1_Output_WeightMatrixTransposed, DeltaOutputMatrix, Hidden1WeightDeltaOutputMultMatrix);

        matrix_t DerivativeHidden1Matrix;
        DerivativeActivation(Hidden1Matrix, DerivativeHidden1Matrix);

        matrix_t DeltaHidden1Matrix;
        HadamardProduct(Hidden1WeightDeltaOutputMultMatrix, DerivativeHidden1Matrix, DeltaHidden1Matrix);

        UpdateWeightMatrix(DeltaHidden1Matrix, Input_Hidden1_WeightMatrix); 

    }


    //Validating the Network
    float numOfCorrect = 0.0;
    for (size_t i = 0; i < validationVector.size(); i++){
        //Init the Input layer
        InputMatrix.data[0][0] = validationVector[i].v1;
        InputMatrix.data[1][0] = validationVector[i].v2;
        InputMatrix.data[2][0] = validationVector[i].v3;
        InputMatrix.data[3][0] = validationVector[i].v4;

        //Feedforward
        MatrixMult(Input_Hidden1_WeightMatrix, InputMatrix, Hidden1Matrix);
        matrix_t ActivatedHidden1Matrix;
        Activation(Hidden1Matrix, ActivatedHidden1Matrix);

        MatrixMult(Hidden1_Output_WeightMatrix, Hidden1Matrix, OutputMatrix);
        matrix_t ActivatedOutputMatrix;
        SoftMax(OutputMatrix, ActivatedOutputMatrix);

        float max = 0.0;
        int bestIndex = 0;
        for (size_t j = 0; j < ActivatedOutputMatrix.rows; j++){
            if (ActivatedOutputMatrix.data[j][0] > max){
                max = ActivatedOutputMatrix.data[j][0];
                bestIndex = j;
            }
        }
        if (bestIndex == validationVector[i].index)
            numOfCorrect += 1.0;
    }

    std::cout << numOfCorrect/((float) validationVector.size()) << std::endl;
    

    //MatrixMult(aMatrix, bMatrix, cMatrix);

    //std::cout << cMatrix.data[0][0] << std::endl;
    //std::cout << cMatrix.data[0][1] << std::endl;
    //std::cout << cMatrix.data[1][0] << std::endl;
    //std::cout << cMatrix.data[1][1] << std::endl;

    
    
    return 0;

}

void Parser(const std::string &fileName, std::vector<flowerData> &dataVector){
    std::ifstream file(fileName);
    while(true){
        flowerData result;
        std::string tmp;

        getline(file, tmp, ',');
        result.v1 = std::stof(tmp);
        getline(file, tmp, ',');
        result.v2 = std::stof(tmp);
        getline(file, tmp, ',');
        result.v3 = std::stof(tmp);
        getline(file, tmp, ',');
        result.v4 = std::stof(tmp);
        getline(file, tmp, '\n');
        result.index = std::stoi(tmp);
        dataVector.push_back(result);

        if(file.peek() == -1)
            break;

    }
}

void SeparateData(const std::vector<flowerData> &dataVector, std::vector<flowerData> &trainingVector, std::vector<flowerData> &validationVector){
    std::vector<int> randomIndex(dataVector.size());
    for (size_t i = 0; i < dataVector.size(); i++){
        randomIndex[i] = i;
    }
    std::random_shuffle(randomIndex.begin(), randomIndex.end());

    int numberOfValidations = dataVector.size()/4;

    for (int i = 0; i < 150 - numberOfValidations; i++){
        trainingVector.push_back(dataVector[randomIndex[i]]);
    }
    for (int i = 150 - numberOfValidations; i < 150; i++){
        validationVector.push_back(dataVector[randomIndex[i]]);
    }

}


void MatrixMult(matrix_t &A, matrix_t &B, matrix_t &C){
    InitMatrix(C, A.rows, B.cols);

    for (size_t i = 0; i < C.rows; i++){
        for (size_t j = 0; j < C.cols; j++){
            C.data[i][j] = 0.0;
        }
    }

    for (size_t i = 0; i < C.rows; i++){
        for (size_t j = 0; j < C.cols; j++){
            for (size_t k = 0; k < A.cols; k++){
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
}


void InitMatrix(matrix_t &matrix, size_t row, size_t col){
    matrix.cols = col;
    matrix.rows = row;

    matrix.data = new float *[row];
    for (size_t i = 0; i < row; i++){
        matrix.data[i] = new float[col];
    }
}

void FillMatrix(matrix_t &matrix){
    std::uniform_real_distribution<float> distr(-2.0, 2.0);

    for (size_t i = 0; i < matrix.rows; i++){
        for (size_t j = 0; j < matrix.cols; j++){
            matrix.data[i][j] = distr(GEN);
        }
    }

}

void Activation (const matrix_t &matrix, matrix_t &result){
    InitMatrix(result, matrix.rows, matrix.cols);
    for (size_t i = 0; i < matrix.rows; i++){
        result.data[i][0] = sigmoid(matrix.data[i][0]);
    }
}

void SoftMax (const matrix_t &matrix, matrix_t &result){
    InitMatrix(result, matrix.rows, matrix.cols);
    float sum = 0.0;
    for (size_t i = 0; i < matrix.rows; i++){
        sum += std::exp(matrix.data[i][0]);
    }
    
    for (size_t i = 0; i < matrix.rows; i++){
        result.data[i][0] = std::exp(matrix.data[i][0]) / sum;
    }
}

//Only works for vectors
void HadamardProduct(const matrix_t &A, const matrix_t &B, matrix_t &result){
    
    InitMatrix(result, A.rows, A.cols);
    for (size_t i = 0; i < A.rows; i++){
        result.data[i][0] = A.data[i][0] * B.data[i][0];
    }
}

void Transpose(const matrix_t &A, matrix_t &result){
    
    InitMatrix(result, A.cols, A.rows);
    for (size_t i = 0; i < A.rows; i++){
        for (size_t j = 0; j < A.cols; j++){
            result.data[j][i] = A.data[i][j];
        }
    }

}

void DerivativeActivation (const matrix_t &matrix, matrix_t &result){
    InitMatrix(result, matrix.rows, matrix.cols);
    for (size_t i = 0; i < matrix.rows; i++){
        result.data[i][0] = sigmoidDerivative(matrix.data[i][0]);
    }
}

void OutputErrorFunction (const matrix_t &matrix, matrix_t &result, int correctIndex){
    InitMatrix(result, matrix.rows, matrix.cols);
    for (size_t i = 0; i < matrix.rows; i++){
        if ((int)i == correctIndex)
            result.data[i][0] = matrix.data[i][0] - 1.0;
        else 
            result.data[i][0] = matrix.data[i][0] - 0.0;
    }
}
void UpdateWeightMatrix(const matrix_t &deltaMatrix, matrix_t &weights){
    
    for (size_t i = 0; i < weights.rows; i++){
        for (size_t j = 0; j < weights.cols; j++){
            weights.data[i][j] -= deltaMatrix.data[i][0] * g_learningRate;
        }
    }

}
