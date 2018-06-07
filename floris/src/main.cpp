#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <random>
#include <stack>


struct Production{
    int number;
    std::string output;
    float prob;
};


void Parser(const std::string &, std::string &, std::unordered_map<char, std::vector<Production>> &);
std::string EvolveString();
void DrawLSystem(sf::RenderTexture &window, const std::string &lsystem);
sf::RectangleShape MakeLine(sf::Vector2f &out_currentPos, float currentAngle);

const int WIDTH = 1280;
const int HEIGHT = 1024;
//const int AREA = WIDTH * HEIGHT;

std::string fileName;
std::string axiom;
std::unordered_map<char, std::vector<Production>> productions;
size_t generations;
std::string seedStr = "tttet";
std::string outputFile = "output.png";

float lineLength = 100.0f;
float lineWidth = 10.0f;
float angle = 21.0f;
bool everyLetterMeansDraw = false;

int main(int argc, char *argv[]){

    if (argc < 5){
        std::cout << "Not enough arguments" << std::endl;
        return -1;
    }
    for (int i = 1; i < argc; i++){
        if (std::string(argv[i]).compare("-generations") == 0){
            generations = atoi(argv[++i]);
        }
        else if (std::string(argv[i]).compare("-file") == 0){
            fileName = argv[++i];
        }
        else if (std::string(argv[i]).compare("-angle") == 0){
            angle = atof(argv[++i]);
        }
        else if (std::string(argv[i]).compare("-output") == 0){
            outputFile = argv[++i];
        } else if(std::string(argv[i]).compare("-drawEveryLetter") == 0){
            everyLetterMeansDraw = true;
        }


    }

    /*Pixel* data = new Pixel[AREA];

    for(int i = 0; i < AREA; i++){
        data[i] = {(unsigned char)254,(unsigned char) 254,(unsigned char)(i%255)};
    }

    if (stbi_write_jpg("test.jpg", WIDTH, HEIGHT, 3, data, 100))
        std::cout << "Success" << std::endl;
    */


    Parser(fileName, axiom, productions);
   
    std::string resultString = EvolveString();

    //SFML stuff
    
    sf::RenderTexture window;
    window.create(1280, 1080, false);
    //std::cout << window.getSize().x << ", " << window.getSize().y << std::endl;
    window.clear();

    DrawLSystem(window, resultString);

    //window.display();
    
    /*sf::Vector2u windowSize = window.getSize();
    sf::Texture texture;
    texture.create(windowSize.x, windowSize.y);
    texture.update(window);*/
     
    //std::cout << texture.getSize().x << ", " << texture.getSize().y <<std::endl;
    
    sf::Image screenshot = window.getTexture().copyToImage();
    

    screenshot.saveToFile(outputFile);

    
    return 0;

}

void Parser(const std::string &fileName, std::string &axiom, std::unordered_map<char, std::vector<Production>> &prods){
    std::ifstream file(fileName);

    getline(file, axiom, '\n');
    
   
    //while (!file.eof()){
    while (true){
        int num;
        char in;
        std::string outStr;
        float prob;

        std::string tmp;

        //000:F:[+F]F[-F]F:0.33
        in = file.peek();

        getline(file, tmp, ':');
        num = std::stoi(tmp, nullptr, 10);
        getline(file, tmp, ':');
        in = tmp.front();
        getline(file, outStr, ':');
        getline(file, tmp, '\n');
        prob = std::stof(tmp);

        Production tmpProd = {num, outStr, prob};
        prods[in].push_back(tmpProd);
        
        
        if(file.peek() == -1)
            break;
    }

    
}

std::string EvolveString(){
    std::string current = axiom;
    std::string tmp = "";
    
    std::default_random_engine gen;
    std::seed_seq seed (seedStr.begin(), seedStr.end());
    gen.seed(seed);
    std::uniform_real_distribution<float> distr(0.0, 1.0);

    while(generations > 0){

        for(int i = 0; i < (int)current.size(); i++){
            char c = current[i];
            std::vector<Production> currentProd = productions[c];
            if (currentProd.size() == 1){
                tmp += currentProd[0].output;
            } else if (currentProd.size() > 1) {
                
                float randomNumber = distr(gen);

                for(int j = 0; j < (int) currentProd.size(); j++){
                    randomNumber -= currentProd[j].prob;
                    if (randomNumber < 0.01f){
                        tmp += currentProd[j].output;
                        break;
                    }
                }
            } else {
                tmp += c;
            }

        }

        current = tmp;
        tmp = "";

        generations--;
    }
    

    return current;
    
}


void DrawLSystem(sf::RenderTexture &window, const std::string &lsystem){

    sf::Vector2f currentPos(WIDTH/2, HEIGHT/2);
    float currentAngle = 0;
    std::stack<sf::Vector2f> stackPos;
    std::stack<float> stackAngle;
    std::vector<sf::RectangleShape> lineVector;

    float minX = currentPos.x;
    float minY = currentPos.y;
    float maxX = currentPos.x + lineWidth;
    float maxY = currentPos.y + lineLength;

    for(int i = 0; i < (int) lsystem.size(); i++){
        char currentChar = lsystem[i];
        switch (currentChar){
            case 'F':
                {
                    //currentPos is changed by MakeLine
                    lineVector.push_back(MakeLine(currentPos, currentAngle));
                    break;
                }
            case '[':
                {
                    stackPos.push(currentPos);
                    stackAngle.push(currentAngle);

                    break;
                }
            case ']':
                {
                    currentPos = stackPos.top();
                    currentAngle = stackAngle.top();
                    stackPos.pop();
                    stackAngle.pop();
                    
                    break;
                }
            case '+':
                {
                    currentAngle += angle;
                    break;
                }
            case '-':
                {
                    currentAngle -= angle;
                    break;
                }
            case 'f':
                {
                    MakeLine(currentPos, currentAngle);                    
                    break;
                }
            default:
                {
                    if (everyLetterMeansDraw){
                        lineVector.push_back(MakeLine(currentPos,currentAngle));
                    }
                    
                }
        }

        //check current Pos for max and min
        if (currentPos.x < minX)
            minX = currentPos.x;
        if (currentPos.y < minY)
            minY = currentPos.y;
        if (currentPos.x > maxX)
            maxX = currentPos.x;
        if (currentPos.y > maxY)
            maxY = currentPos.y;

    }

    //set the view to center and scale the image
    float horspace = 0.0f;
    float verspace = 0.0f;
    if (maxX - minX < WIDTH && maxY - minY < HEIGHT){
        horspace = (WIDTH-(maxX-minX))/2.0f;
        verspace = (HEIGHT-(maxY-minY))/2.0f;
    } else if (maxX-minX < WIDTH){
        //The width is too small, so has to be centered, but the height is to big.
        float newWidth = ((maxY-minY) * WIDTH)/HEIGHT;
        horspace = (newWidth - (maxX-minX))/2.0f; 

    } else if (maxY-minY < HEIGHT){
        float newHeight = (HEIGHT * (maxX-minX)) / WIDTH;
        verspace = (newHeight - (maxY-minY))/2.0f;

    } else {
        //Choose the one with the greatest distance to the desired ratio
        float xRest = maxX - minX - WIDTH;
        float yRest = maxY - minY - HEIGHT;
        if (xRest > yRest){
            //The width is farther away from the desired resolution
            //so we choose it as our new starting point to calc the new height
            float newHeight = (HEIGHT * (maxX-minX)) / WIDTH;
            verspace = (newHeight - (maxY-minY))/2.0f;
        } else {
            float newWidth = ((maxY-minY) * WIDTH)/HEIGHT;
            horspace = (newWidth - (maxX-minX))/2.0f;
        }

    }
        

    sf::View view(sf::FloatRect(minX-horspace,minY-verspace,maxX+2*horspace-minX,maxY+2*verspace-minY));
    window.setView(view);

    for (int i = 0; i < (int) lineVector.size(); i ++){
        window.draw(lineVector[i]);
        
        /*sf::CircleShape tmp(2,5);
        //tmp.setPosition(currentPos);
        //tmp.setPosition(lineVector[i].getPosition() - lineVector[i].getPoint(3));
        tmp.setPosition(lineVector[i].getTransform().transformPoint(lineVector[i].getPoint(2)));
        tmp.setFillColor(sf::Color::Red);
        window.draw(tmp);*/
    }
    

    
}

sf::RectangleShape MakeLine(sf::Vector2f &currentPos_out, float currentAngle){
    sf::RectangleShape result;
    result.setSize(sf::Vector2f(lineWidth, lineLength));
    //0,0 origin is the top-left corner
    result.setOrigin(lineWidth/2.0f, lineLength);
    result.setRotation(currentAngle);
    result.setPosition(currentPos_out);
    sf::Vector2f leftTop = result.getTransform().transformPoint(result.getPoint(0));
    sf::Vector2f rightTop = result.getTransform().transformPoint(result.getPoint(1));
    currentPos_out = (leftTop + rightTop) / 2.0f;
    


    return result;

}
