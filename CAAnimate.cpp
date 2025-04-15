#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "emp/math/math.hpp"

emp::web::Document doc{"target"};

class CAAnimator : public emp::web::Animate
{

    // grid width and height
    const int num_h_boxes = 10;
    const int num_w_boxes = 15;
    const double RECT_SIDE = 25;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};

    // some vectors to hold information about the CA
    std::vector<std::vector<float>> cells;

    // some vectors to hold information about the next screen
    std::vector<std::vector<float>> next;

    // where we'll draw
    emp::web::Canvas canvas{width, height, "canvas"};

public:
    CAAnimator()
    {
        // shove canvas into the div
        // along with some control buttons
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        // fill the vectors with 1.0 (white=dead) to start
        cells.resize(num_w_boxes, std::vector<float>(num_h_boxes, 1.0));
        next.resize(num_w_boxes, std::vector<float>(num_h_boxes, 1.0));


        // showing how to set a cell to 'alive'
        cells[0][0] = 0.0;
        cells[2][0] = 0.0;
        cells[1][1] = 0.0;
        cells[2][1] = 0.0;
        cells[1][3] = 0.0;
        
    }

    int NumLiveNeighbors(int x, int y)
    {
        int count = 0;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0)
                    continue;
                int neighborX = emp::Mod((x + i), num_w_boxes);
                int neighborY = emp::Mod((y + j), num_h_boxes);
                if (cells[neighborX][neighborY] == 0.0)
                {
                    count++;
                }
            }
        }
        return count;
    }

    void UpdateDeadCell(int x, int y, int numLiveNeighbors)
    {
        if (numLiveNeighbors == 3)
        {
            next[x][y] = 0.0;
        }
        else
        {
            next[x][y] = std::min(1.0, cells[x][y] + 0.1);
        }
    }

    void UpdateLiveCell(int x, int y, int numLiveNeighbors)
    {
        if (numLiveNeighbors < 2 || numLiveNeighbors > 3)
        {
            next[x][y] = cells[x][y] + 0.1;
        }
    }


    void UpdateCell(int x, int y){
        int numLiveNeighbors = this->NumLiveNeighbors(x, y);
        if (cells[x][y] == 0){
            this->UpdateLiveCell(x, y, numLiveNeighbors);
        }
        else{
            this->UpdateDeadCell(x, y, numLiveNeighbors);
        }
    }

    void UpdateBoard()
    {
        for (int x = 0; x < num_w_boxes; x++)
        {
            for (int y = 0; y < num_h_boxes; y++)
            {
                cells[x][y] = next[x][y];
            }
        }
    }

    void DoFrame() override
    {
        canvas.Clear();

        for (int x = 0; x < num_w_boxes; x++)
        {
            for (int y = 0; y < num_h_boxes; y++)
            {

                if (cells[x][y] == 0)
                {
                    // Draw a rectangle on the canvas filled with white and outlined in black
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                    
                }
                else
                {
                    // Draw a rectangle filled with black
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "black", "black");
                }
                this->UpdateCell(x,y);
            }
        }

        // Place to update the values of all the cells based on CA rules

        this->UpdateBoard();
    }
};

CAAnimator animator;

int main()
{
    // Have animator call DoFrame once to start
    animator.Step();
}