//standard c/c++ headers
#include <list>
#include <iostream>
#include <math.h>
#include <string>
#include <array>
#include <vector>
#include <stdio.h>

//raylib headers
#include "raylib.h"




//bredden och höjden på skärmen
const int screenWidth = 1080;
const int screenHeight = 820;

bool paused = true;
Color betterwhite = GetColor(0xf4f3f2ff);
int updateFrequency = 20;
int framecount = 0;
int generations = 0;
int currentPattern = 0;

std::array<int, 12150> grid = {0};
std::array<int, 12150> updateGrid = {0};

struct Pattern
{
    int width;
    int height;
    int xOffset;
    int yOffset;
    std::vector<int> data;

    Pattern(int _width, int _height, int _xOffset, int _yOffset, std::string _data)
    {
        width = _width;
        height = _height;
        xOffset = _xOffset;
        yOffset = _yOffset;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                data.push_back((int)_data[x + (y * width)] - 48);
            }
        }
        
    }
};

Pattern gosperGliderGun = Pattern(36, 9, 5, 5, "000000000000000000000000100000000000000000000000000000000010100000000000000000000000110000001100000000000011000000000001000100001100000000000011110000000010000010001100000000000000110000000010001011000010100000000000000000000010000010000000100000000000000000000001000100000000000000000000000000000000110000000000000000000000");

Pattern lilypad = Pattern(7, 9, 64, 41, "010001010101011001001100000110000011000001010001000101000001000");

Pattern rPentomino = Pattern(3, 3, 65, 43, "011110010");

Pattern beehiveGenerators = Pattern(16, 16, 59, 37, "0000000010000000011100011000111101000001000000000000000000000000000000000000000000100001000000000000000000001000011000011000011001000001000000010000000000000000000000000000000000000000000000001010000010000100001000101000010000010000010001000000000000000001");

Pattern switchEngine = Pattern(16, 4,100,70, "0000000000010100110000000010000011000000000100100000000000000111");


void setPattern(Pattern p)
{
    grid.fill(0);
    for (int y = 0; y < p.height; y++)
    {
        for (int x = 0; x < p.width; x++)
        {
            grid[(p.xOffset + x) + (p.yOffset + y)*135] = (int)p.data[x + y*p.width];
        }
        
    }
}

void saveGrid()
{
    FILE *fp;
    char cell;
    fp = fopen("grid.conway", "w");
    for (int y = 0; y < 90; y++)
    {
        for (int x = 0; x < 135; x++)
        {
            cell = (char)grid[x + y*135];
            putc(cell, fp);
        }
    }
    fclose(fp);
}

void loadSavedGrid()
{
    FILE *fp;
    int cell;
    fp = fopen("grid.conway", "r");

    if(fp != NULL)
    {    
        for (int y = 0; y < 90; y++)
        {
            for (int x = 0; x < 135; x++)
            {
                cell = getc(fp);
                if(cell == EOF){break;}
                grid[x + y*135] = cell;
            }
        }
        fclose(fp);

    }
}

bool checkNeighbors(int x, int y)
{
    int totalNeighbors = 0;


    //horizontal and vertical
    if(grid[(x + 1) + y * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[(x - 1) + y * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[x + (y + 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[x + (y - 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }

    //diagonal
    if(grid[(x - 1) + (y - 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[(x + 1) + (y - 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[(x + 1) + (y + 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }
    if(grid[(x - 1) + (y + 1) * 135] == 1)
    {
        totalNeighbors += 1;
    }
    

    //rules
    if(grid[x + y * 135] == 1)
    {
        if(totalNeighbors < 2)
        {
            return false;
        }
        if(totalNeighbors == 2 || totalNeighbors == 3)
        {
            return true;
        }
        if(totalNeighbors > 3)
        {
            return false;
        }
    }
    else
    {
        if(totalNeighbors == 3)
        {
            return true;
        }
    }

    return false;
}

int main()
{
    
    //öppna ett nytt fönster
    InitWindow(screenWidth, screenHeight, "IsometriCraft");
    InitAudioDevice();
    SetTargetFPS(120);

    
    

    //huvudloop
    while (!WindowShouldClose())
    {
        
        if(IsKeyPressed(KEY_SPACE)){paused = !paused;}

        if(IsKeyDown(KEY_UP)){updateFrequency += 5;}
        if(IsKeyDown(KEY_DOWN)){updateFrequency -= 5;}
        if(updateFrequency < 5){updateFrequency = 5;}
        if(updateFrequency > 120){updateFrequency = 120;}

        if(IsKeyPressed(KEY_LEFT)){currentPattern -= 1;}
        if(IsKeyPressed(KEY_RIGHT)){currentPattern += 1;}
        if(currentPattern > 4){currentPattern = 4;}
        if(currentPattern < 0){currentPattern = 0;}

        if(IsKeyPressed(KEY_S)){saveGrid();}
        if(IsKeyPressed(KEY_L)){loadSavedGrid();}

        if(IsKeyPressed(KEY_ENTER))
        {
            switch (currentPattern)
            {
            case 0:
                setPattern(gosperGliderGun);
                break;
            case 1:
                setPattern(lilypad);
                break;
            case 2:
                setPattern(rPentomino);
                break;
            case 3:
                setPattern(beehiveGenerators);
                break;
            case 4:
                setPattern(switchEngine);
            default:
                break;
            }
        }


        BeginDrawing();
        ClearBackground(BLACK);

        if(paused == false)
        {
            if(framecount >= 120/updateFrequency)
            {
                framecount = 0;
                updateGrid = grid;

                for (int y = 1; y < 89; y++)
                {
                    for (int x = 1; x < 134; x++)
                    {

                        updateGrid[x + y * 135] = (int)checkNeighbors(x, y);

                        if(grid[x + y * 135] == 1)
                        {
                            DrawRectangle(x * 8, y * 8, 8, 8, betterwhite);
                        }
                    }
                }
                
                DrawRectangleLines(0,0,1080,720, GREEN);
                grid = updateGrid;
                generations += 1;
            }
            else
            {

                for (int y = 1; y < 89; y++)
                {
                    for (int x = 1; x < 134; x++)
                    {


                        if(grid[x + y * 135] == 1)
                        {
                            DrawRectangle(x * 8, y * 8, 8, 8, betterwhite);
                        }
                    }
                }
                
                DrawRectangleLines(0,0,1080,720, GREEN);
                
                framecount += 1;
            }
            
        }
        else if(paused == true)
        {
            

            Vector2 mousePos = GetMousePosition();

            mousePos.x = floor(mousePos.x/8);
            mousePos.y = floor(mousePos.y/8);

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                grid[mousePos.x + mousePos.y * 135] = 1;
            }
            else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                grid[mousePos.x + mousePos.y * 135] = 0;
            }

            for (int y = 1; y < 89; y++)
            {
                for (int x = 1; x < 134; x++)
                {
                    DrawRectangleLines(x * 8, y * 8, 8, 8, betterwhite);

                    if(grid[x + y * 135] == 1)
                    {
                        DrawRectangle(x * 8, y * 8, 8, 8, betterwhite);
                    }
                }
            }

            DrawRectangleLines(mousePos.x * 8, mousePos.y * 8, 8, 8, RED);
            DrawRectangleLines(0,0,1080,720,RED);
            
        }
        DrawFPS(10, 790);
        DrawText(TextFormat("Update frequency: %d", updateFrequency), 10, 730, 24, GREEN);
        DrawText(TextFormat("Generations: %d", generations), 10, 760, 24, GREEN);

        std::string patternText;

        switch (currentPattern)
        {
        case 0:
            patternText = "Gosper glider gun";
            break;
        case 1:
            patternText = "Lilypad";
            break;
        case 2:
            patternText = "R pentomino";
            break;
        case 3:
            patternText = "Beehive generators";
            break;
        case 4:
            patternText = "Switch engine";
        default:
            break;
        }

        DrawText("Press enter to load:", 700 - MeasureText("Press enter to load:", 24)/2, 750, 24, GREEN);
        DrawText(TextFormat("<- %s ->", patternText.c_str()), 700 - MeasureText(TextFormat("<- %s ->", patternText.c_str()), 24)/2, 780, 24, GREEN);

        EndDrawing();
    }


    CloseWindow();
    
}

