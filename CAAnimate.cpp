#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "emp/math/math.hpp"

emp::web::Document doc{"target"};

// templates for the CA
// this is the Gosper glider gun from https:/playgameoflife.com/lexicon/Gosper_glider_gun
std::vector<std::tuple<int, int>> gosper_glider_gun { 
    {0, 4}, {0, 5}, 
    {1, 4}, {1, 5}, 
    {10, 4}, {10, 5}, {10, 6}, 
    {11, 3}, {11, 7},
    {12, 2}, {12, 8},
    {13, 2}, {13, 8},
    {14, 5},
    {15, 3}, {15, 7},
    {16, 4}, {16, 5}, {16, 6}, 
    {17, 5},
    {20, 2}, {20, 3}, {20, 4}, 
    {21, 2}, {21, 3}, {21, 4}, 
    {22, 1}, {22, 5},
    {24, 0}, {24, 1}, {24, 5}, {24, 6}, 
    {34, 2}, {34, 3},
    {35, 2}, {35, 3}
};

// the glider was copied from Anya's example at https://anyaevostinar.github.io/CA-Assignment/ 
std::vector<std::tuple<int, int>> basic_glider {
    {0, 0}, 
    {1, 1}, {1, 2}, 
    {2, 0}, {2, 1}
};

// the unnamed ship, code 64P2H1V0 from https://conwaylife.com/wiki/64P2H1V0
std::vector<std::tuple<int, int>> unnamed_ship {
    {0, 7}, 
    {1, 4}, {1, 5}, {1, 6}, {1, 7}, 
    {2, 4}, {2, 5},
    {3, 2},
    {4, 2}, {4, 3}, {4, 4}, {4, 5}, 
    {5, 1},
    {6, 0}, {6, 1}, {6, 2}, {6, 5}, 
    {7, 1}, {7, 2}, {7, 3},
    {8, 2},
    {9, 3}, {9, 4}, {9, 5}, {9, 7},
    {10, 3}, {10, 6}, {10, 7},
    {11, 4}, {11, 5}, {11, 6},
    {12, 6}, 
    {13, 4}, 
    {14, 4}, {14, 6},
    {15, 3}, 
    {16, 4}, {16, 6},    
    {17, 4},               
    {18, 6},               
    {19, 4}, {19, 5}, {19, 6},
    {20, 3}, {20, 6}, {20, 7},
    {21, 3}, {21, 4}, {21, 5}, {21, 7},  
    {22, 2}, 
    {23, 1}, {23, 2}, {23, 3},
    {24, 0}, {24, 1}, {24, 2}, {24, 5},  
    {25, 1},    
    {26, 2}, {26, 3}, {26, 4}, {26, 5},  
    {27, 2},    
    {28, 4}, {28, 5},   
    {29, 4}, {29, 5}, {29, 6}, {29, 7}, 
    {30, 7}    
};

class CAAnimator : public emp::web::Animate {

    // grid management variables
    const int num_h_boxes = 40;
    const int num_w_boxes = 40;
    const double RECT_SIDE = 20;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};
    
    // choose the starting template from the ones above
    std::vector<std::tuple<int, int>> init_cells = unnamed_ship;

    // state management variables
    // Visually, a live cell is black, and when it dies, it changes to gray, then slowly fades to white in a fixed amount of time
    const double life_value = 0.0;
    const double fade_value = 0.5;
    const double dead_value = 1.0;
    const double fade_time = 10.0;
    const double fade_increment = (dead_value - fade_value) / fade_time;

    // vectors to hold information about the CA
    std::vector<std::vector<double>> cells;

    // vectors to hold information about the next screen
    std::vector<std::vector<double>> next;

    // where we'll draw
    emp::web::Canvas canvas{width, height, "canvas"};

public:
    // constructor
    CAAnimator() {
        // shove canvas into the div
        // along with some control buttons
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        // fill the vectors with dead cells to start
        cells.resize(num_w_boxes, std::vector<double>(num_h_boxes, dead_value));
        next.resize(num_w_boxes, std::vector<double>(num_h_boxes, dead_value));

        // setting up the initial state of the cells
        for (const auto& [x, y] : init_cells) {
            cells[x][y] = life_value;
        }
    }

    /**
     * Takes in the x and y coordinates of a cell
     * and sets the next state of the cell to be alive
     * Returns nothing
     */
    void BirthNext(int x, int y) {
        next[x][y] = life_value;
    }

    /**
     * Takes in the x and y coordinates of a cell
     * and sets the next state of the cell to be half-dead (faded)
     * Returns nothing
     */
    void DieNext(int x, int y) {
        next[x][y] = fade_value;
    }

    /**
     * Takes in the x and y coordinates of a cell
     * and fades it closer to being dead,
     * keeping the value between 0 and 1 for canvas.Rect
     * Returns nothing
     */
    void FadeNext(int x, int y) {
        next[x][y] = cells[x][y] + fade_increment;
        next[x][y] = emp::Max(0.0, emp::Min(next[x][y], 1.0));
    }

    /**
     * Takes in the x and y coordinates of a cell
     * and sets the next state of the cell to stay the same
     * Returns nothing
     */
    void StayNext(int x, int y) {
        next[x][y] = cells[x][y];
    }

    /**
     * Takes in the x and y coordinates of a cell
     * and counts the number of live neighbors,
     * taking into account the toroidal spatial structure
     * Returns the number of live neighbors
     */
    int NumLiveNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;

                int neighborX = emp::Mod((x + i), num_w_boxes);
                int neighborY = emp::Mod((y + j), num_h_boxes);

                if (cells[neighborX][neighborY] == life_value) {
                    count++;
                } 
            }
        }
        return count;
    }

    /**
     * Takes in the x and y coordinates of a dead/fading cell
     * and updates the next state of the cell based on the number of live neighbors
     * Returns nothing
     */
    void UpdateDeadCell(int x, int y, int numLiveNeighbors) {
        if (numLiveNeighbors == 3) {
            this->BirthNext(x, y);
        }
        else {
            this->FadeNext(x, y);
        }
    }

    /**
     * Takes in the x and y coordinates of a live cell
     * and updates the next state of the cell based on the number of live neighbors
     * Returns nothing
     */
    void UpdateLiveCell(int x, int y, int numLiveNeighbors) {
        if (numLiveNeighbors < 2 || numLiveNeighbors > 3) {
            this->DieNext(x, y);
        }
        else {
            this->StayNext(x, y);
        }
    }

    /**
     * Takes in the x and y coordinates of a cell,
     * counts the number of live neighbors,
     * and updates the next state of the cell based on the number of live neighbors and the cell's own state
     * Returns nothing
     */
    void UpdateCell(int x, int y) {
        int numLiveNeighbors = this->NumLiveNeighbors(x, y);
        if (cells[x][y] == life_value) {
            this->UpdateLiveCell(x, y, numLiveNeighbors);
        }
        else {
            this->UpdateDeadCell(x, y, numLiveNeighbors);
        }
    }

    /**
     * Takes in nothing,
     * Updates the state of the whole board based on the next state
     * Returns nothing
     */
    void UpdateBoard() {
        for (int x = 0; x < num_w_boxes; x++) {
            for (int y = 0; y < num_h_boxes; y++) {
                cells[x][y] = next[x][y];
            }
        }
    }

    /**
     * Takes in nothing,
     * Clears the canvas and draws the current state of the board,
     * calculating the next state of each cell as it goes,
     * then updates the board based on the saved next states
     * Returns nothing
     */
    void DoFrame() override {
        canvas.Clear();

        for (int x = 0; x < num_w_boxes; x++) {
            for (int y = 0; y < num_h_boxes; y++) {
                canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, emp::ColorHSV(0, 0, cells[x][y]), "black");
                this->UpdateCell(x, y);
            }
        }

        // Place to update the values of all the cells based on CA rules,
        this->UpdateBoard();
    }
};

CAAnimator animator;

int main() {
    // Have animator call DoFrame once to start
    animator.DoFrame();
}