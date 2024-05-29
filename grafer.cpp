#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <windows.h>
#include <ctime>
#include <string>
#include <thread>

//this program lets user insert polynomial equations and graph them in cartesian plain

//global variables
const int WIDTH=1000, HEIGHT=1000;
const char *windowTitle="Grafer";
double xScale = 0.01, yScale=10, yScaleChange =1, xScaleChange=0.0001, centerChange=10;

//grids center
std::pair<int , int> center = {WIDTH/2, HEIGHT/2};

//window and renderer pointers
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
//bools
bool addPoly = true;
//cls
void cls(){
    system("cls");
}
//instructions
void Instructions(){
    std::cout<<"LEFT/RIGHT - change yScale\nUP/DOWN - change xScale\nA/D S/W - move center\nMINUS/EQUALS - zoom out/in\nQ - add another polynomial\nX - change scales\nR - reset center position and scales\n";
}
//horners method
double Horners(std::vector<double> &polynomial, double x){
    double result = 0;
    for(int i=0; i<polynomial.size(); i++){
        result = (result*x)+polynomial[i];
    }
    return result;
}
//scale operations
void ChangeScale(){
    cls();
    std::cout<<"New xScale:  "<<"\n";
    std::cin>>xScale;
    cls();
    std::cout<<"New yScale:  "<<"\n";
    std::cin>>yScale;
    cls();
    Instructions();
}
void CheckScales(){
    if(xScale<=0){
        xScale=0.000000001;
    }

    if(yScale<=0){
        yScale=0.000000001;
    }
    //setting scale changes
    xScaleChange=xScale/10;
    yScaleChange=yScale/10;
}

//polynomial class - vector of coefficients and graphs color
class Polynomial{
    private:
        //vector of coefficients
        std::vector <double> coefficients;
        //color
        int R, G, B;
    public:
        void PolynomialSetup(){
            //seed
            srand(time(0));
            double input;
            while(input!=2005){
                cls();
                int deg=coefficients.size()-1;
                if(coefficients.size()==0){
                    deg=0;
                }
                std::cout<<"Current degree: "<<deg<<" Provide another coefficient or 2005 to finish\n";
                std::cin>>input;
                if(input!=2005){
                    coefficients.push_back(input);
                }
            }
            //random color
            R=rand()%256;
            G=rand()%256;
            B=rand()%256;
            cls();
            
        }
        void DrawPolynomial(){
            //line drawing helpers
            std::pair<std::pair<int, int> , std::pair<int, int>> points;   
            
            //choosing color
            SDL_SetRenderDrawColor(renderer, R, G, B, 255);
            
            points.first.first =0;
            //scaled y
            points.first.second = -yScale*((Horners(coefficients, -center.first*xScale)))+center.second;
            //not drawing off screen bounds
            if(points.first.second<0){
                points.first.second = -1;
            }
            else if(points.first.second>HEIGHT){
                points.first.second = HEIGHT+1;
            }
            for(double i=1.0; i<=WIDTH; i++){
                points.second.first = i;
                points.second.second = -yScale*((Horners(coefficients, (i-center.first)*xScale)))+center.second;
                //not drawing off screen bounds
                if(points.second.second<0){
                    points.second.second =-1;
                }
                else if(points.second.second>HEIGHT){
                    points.second.second=HEIGHT+1;
                }
                //drawing line from points.first to points.second
                SDL_RenderDrawLine(renderer, points.first.first, points.first.second, points.second.first, points.second.second);
                //preparing for next iteration of the loop
                points.first = points.second;
            }
    
        }
};
//vector of polynomials
std::vector<Polynomial> polynomials;
//drawing the frame
void DrawAll(){
    //drawing grid
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 0,center.second, WIDTH,center.second);
    SDL_RenderDrawLine(renderer, center.first,0, center.first,HEIGHT);
    //setting up scales
    double xDist = 1.0/xScale;
    double yDist = 1.0*yScale;
    while(xDist<=10){
        xDist*=10;
    }
    while(yDist<=10){
        yDist*=10;
    }
    //drawing x axis markers
    for(double i=0; (center.first+i*xDist<=WIDTH)||(center.first-i*xDist>=0); i++){
        if(center.second>=0&&center.second<=HEIGHT){
            if(center.first<=WIDTH){
                SDL_RenderDrawLine(renderer, center.first+(i*xDist),center.second-2, center.first+(i*xDist),center.second+2);
            }
            if(center.first>=0){
                SDL_RenderDrawLine(renderer, center.first-(i*xDist),center.second-2, center.first-(i*xDist),center.second+2);
            }
        }
    }
    //drawing y axis markers
    for(double i=0; (center.second+i*yDist<=HEIGHT)||(center.second-i*yDist>=0); i++){
        if(center.first>=0&&center.first<=WIDTH){
            if(center.second<=HEIGHT){
                SDL_RenderDrawLine(renderer, center.first-2,center.second+(i*yDist), center.first+2,center.second+(i*yDist));
            }
            if(center.second>=0){
                SDL_RenderDrawLine(renderer, center.first-2,center.second-(i*yDist), center.first+2,center.second-(i*yDist)); 
            }
        }
    }
    //drawing all polynomials
    for(Polynomial p:polynomials){
        p.DrawPolynomial();
    }
}
//adding polynomial
void AddPolynomial(){
    
    Polynomial p;      
    p.PolynomialSetup();
    polynomials.push_back(p);
    cls();
    Instructions();
    addPoly = false;
    
} 
//processing events
void EventProcesser(SDL_Event &event){
    switch(event.key.keysym.sym){
                    //yScale changes
                    case SDLK_LEFT: 
                            yScale-=yScaleChange;
                            break;
                    case SDLK_RIGHT: 
                            yScale+=yScaleChange;
                            break;
                    //xScale changes
                    case SDLK_DOWN: 
                            xScale+=xScaleChange;
                            break;
                    case SDLK_UP: 
                            xScale-=xScaleChange;
                            break;
                    //zoom in
                    case SDLK_EQUALS:
                            xScale-=xScaleChange;
                            yScale+=0.1;
                            break;
                    //zoom out
                    case SDLK_MINUS:
                            xScale+=xScaleChange;
                            yScale-=0.1;
                            break;
                    //moving grid center
                    case SDLK_w: 
                            center.second+=centerChange;
                            break;
                    case SDLK_s: 
                            center.second-=centerChange;
                            break;
                    case SDLK_a: 
                            center.first+=centerChange;
                            break;
                    case SDLK_d: 
                            center.first-=centerChange;
                            break;
                    case SDLK_r: 
                            center.first=WIDTH/2;
                            center.second=HEIGHT/2;
                            xScale=1;
                            yScale=1;
                            break;
                    //changing scales
                    case SDLK_x: 
                            ChangeScale();
                            break;
                    //adding polynomial  
                    case SDLK_q: 
                            addPoly = true;
                            break;     
                }
                //std::cout<<xScale<<"    "<<yScale<<"\n";

}
//setting up window and renderer
void WindowSetup(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_RenderSetScale(renderer , 1, 1);
    SDL_SetWindowTitle(window, windowTitle);
}
void PolynomialLoop(){
    while(true){
        if(addPoly){
            AddPolynomial();
        }
    }
}
void Frame(){
    while(true){
        SDL_Event event;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        if(SDL_PollEvent(&event)){
            if(event.type==SDL_KEYDOWN){
                EventProcesser(event);
                if(event.key.keysym.sym==SDLK_ESCAPE){
                    break;
                }
            }
            if(event.type == SDL_QUIT){
                break;
            }
        }
        CheckScales();
        DrawAll();
        SDL_RenderPresent(renderer);
    }
}

//main
int main(int argc, char*argv[]){
    
    
    WindowSetup();
    std::thread t1(PolynomialLoop);
    Frame();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;

    
}