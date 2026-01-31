#include "math.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define GRID_STARTING_X 50
#define GRID_STARTING_Y 50
#define GRID_CELL_SIZE 37
#define GRID_CELL_COUNT 19

bool    update_menu();
bool    update_grid(int *grid, int *grid_cpy, int grid_size);
bool    draw_menu();
bool    draw_grid(int *grid, int grid_size, int x, int y, int grid_cell_size, int grid_cell_count);
void    draw_grid_lines(int x, int y, int grid_cell_size, int grid_cell_count);
Vector2 get_snapped_mouse_position(int x, int y, int grid_cell_size, int grid_cell_count);
void    switch_player();
void    reset_game(int *grid, int *grid_cpy, int grid_size);

typedef enum game_state
{
    GAME_STARTING_MENU = 0,
    GAME_PLAYING       = 1,
} game_state;

typedef struct player
{
    int   is_turn;
    Color color;
    int   game_pieces[5];
} player;

typedef struct game
{
    game_state state;

    player  player_one;
    player  player_two;
    player *curr_player;
    player *who_won;

    // if a player won then show its line
    Vector2 start_line;
    Vector2 end_line;

} game;

static game *global_state;

int main(void)
{
    int grid_size = GRID_CELL_COUNT * GRID_CELL_COUNT;

    int *grid     = calloc(grid_size, sizeof(int));
    int *grid_cpy = calloc(grid_size, sizeof(int));

    global_state = calloc(1, sizeof(game));

    global_state->player_one.is_turn = true;
    global_state->player_one.color   = GREEN;

    global_state->player_two.is_turn = false;
    global_state->player_two.color   = RED;

    global_state->curr_player = &global_state->player_one;
    global_state->state       = GAME_STARTING_MENU;

    for (int i = 0; i < 5; i++)
    {
        global_state->player_one.game_pieces[i] = 5;
        global_state->player_two.game_pieces[i] = 5;
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Calculation Connect Five");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (global_state->state)
        {
        case GAME_STARTING_MENU: {
            update_menu();
        }
        break;
        case GAME_PLAYING: {
            update_grid(grid, grid_cpy, grid_size);
        }
        break;
        }
        // menu
        int key = GetKeyPressed();
        switch (key)
        {
        case 'R': {
            reset_game(grid, grid_cpy, grid_size);
        }
        break;
        case 'P': {
            global_state->state = GAME_PLAYING;
        }
        break;
        case 'M': {
            global_state->state = GAME_STARTING_MENU;
            reset_game(grid, grid_cpy, grid_size);
        }
        break;
        }

        BeginDrawing();
        switch (global_state->state)
        {
        case GAME_STARTING_MENU: {
            draw_menu();
        }
        break;
        case GAME_PLAYING: {
            draw_grid(grid, grid_size, GRID_STARTING_X, GRID_STARTING_Y, GRID_CELL_SIZE, GRID_CELL_COUNT);
        }
        break;
        }
        EndDrawing();
    }

    CloseWindow();

    free(grid);
    free(grid_cpy);
    free(global_state);

    return 0;
}
void reset_game(int *grid, int *grid_cpy, int grid_size)
{
    // reset game
    memset(grid, 0, grid_size * sizeof(int));
    memset(grid_cpy, 0, grid_size * sizeof(int));
    global_state->who_won = NULL;
    for (int i = 0; i < 5; i++)
    {
        global_state->player_one.game_pieces[i] = 5;
        global_state->player_two.game_pieces[i] = 5;
    }
    global_state->player_one.is_turn = true;
    global_state->player_two.is_turn = false;
}
void switch_player()
{
    if (global_state->curr_player == NULL)
    {
        printf("Curr_player is null\n");
        exit(1);
    };

    if (global_state->player_one.is_turn == true)
    {
        global_state->player_two.is_turn = true;
        global_state->player_one.is_turn = false;
        global_state->curr_player        = &global_state->player_two;
    }
    else
    {
        global_state->player_one.is_turn = true;
        global_state->player_two.is_turn = false;
        global_state->curr_player        = &global_state->player_one;
    }
}

bool player_won_condition(int pos_sum, int neg_sum)
{

    if (pos_sum >= 16)
    {
        global_state->who_won = &global_state->player_one;
        return true;
    }
    else if (neg_sum <= -16)
    {
        global_state->who_won = &global_state->player_two;
        return true;
    }
    return false;
}

bool update_menu()
{
    return true;
}

bool update_grid(int *grid, int *grid_cpy, int grid_size)
{
    // copy the state of the grid to the grid_cpy
    memcpy(grid_cpy, grid, grid_size * sizeof(int));

    // check if a player has won
    // horizontal
    int num     = 0;
    int sum     = 0;
    int neg_sum = 0;

    for (int y = 0; y < GRID_CELL_COUNT; y++)
    {
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        for (int x = 0; x < GRID_CELL_COUNT; x++)
        {
            num                    = grid_cpy[(y * GRID_CELL_COUNT) + x];
            global_state->end_line = (Vector2){x, y};

            if (num != 0)
            {
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x, y};
                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }
                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }
        }
        if (player_won_condition(sum, neg_sum))
        {
            global_state->end_line.x += 1;
            return true;
        }
    }
    // vertical
    num     = 0;
    sum     = 0;
    neg_sum = 0;

    for (int x = 0; x < GRID_CELL_COUNT; x++)
    {
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        for (int y = 0; y < GRID_CELL_COUNT; y++)
        {
            num                    = grid_cpy[(y * GRID_CELL_COUNT) + x];
            global_state->end_line = (Vector2){x, y};

            if (num != 0)
            {
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x, y};
                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }

                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }
        }
        if (player_won_condition(sum, neg_sum))
        {
            global_state->end_line.y += 1;
            return true;
        }
    }

    // diagonals
    /*
        start from v and end at <-. in this case we are going the front slash '/' way
         0 1 2 3
          v v v v
      0  / / / /
    1 <- / / /  *
    2 <- / /  * *
    3 <- /  * * *
    */
    num     = 0;
    sum     = 0;
    neg_sum = 0;

    for (int i = 0; i < GRID_CELL_COUNT; i++)
    {
        int y                    = 0;
        int x                    = GRID_CELL_COUNT - i;
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        while (x >= 0 && y >= 0)
        {
            num = grid_cpy[(y * GRID_CELL_COUNT) + x];

            if (num != 0)
            {
                // we are doing x + 1 and y + 1 is because when we want the grid lines for the diagonals to start over
                // from the next grid index
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x + 1, y};
                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }

                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }

            global_state->end_line = (Vector2){x, y + 1};

            x -= 1;
            y += 1;
        }
        // this is an edge case because when a value of the grid which is at any x = 0, y = grid_count-1 is set and we
        // add the sum. In the next iteration we go out of bounds and we never check it.
        if (player_won_condition(sum, neg_sum))
        {
            return true;
        }
    }

    /*
        start from v and end at <-. in this case we are going the front slash '/' way
         0 1 2 3
       0 * * * / ->   in this case we are starting from y = grid_size - 1 , x = 0 and at each iteration we are moving to
       the next grid cell at y - 1 && x + 1 1 * * / / -> 2 * / / / -> 3 / / / / -> ^ ^ ^ ^
    */
    for (int i = 0; i < GRID_CELL_COUNT; i++)
    {
        int y                    = GRID_CELL_COUNT - 1;
        int x                    = i;
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        while (x >= 0 && x < GRID_CELL_COUNT && y >= 0 && y < GRID_CELL_COUNT)
        {
            num = grid_cpy[(y * GRID_CELL_COUNT) + x];

            if (num != 0)
            {
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x, y};
                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }

                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }
            // we are doing x + 1 and y + 1 is because when we want the grid lines for the diagonals to end at the next
            // grid index in this case it's flipped from its upper counterpart
            global_state->end_line = (Vector2){x + 1, y + 1};

            x += 1;
            y += 1;
        }
        // boundry check
        if (player_won_condition(sum, neg_sum))
        {
            return true;
        }
    }
    /*    0 1 2 3
          v v v v
     * 0  \ \ \ \ ->
     * 1  * \ \ \ ->
     * 2  * * \ \ ->
     * 3  * * * \ ->
    */
    num     = 0;
    sum     = 0;
    neg_sum = 0;

    for (int i = 0; i < GRID_CELL_COUNT; i++)
    {
        int y                    = 0;
        int x                    = i;
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        while (x < GRID_CELL_COUNT && y < GRID_CELL_COUNT)
        {
            num = grid_cpy[(y * GRID_CELL_COUNT) + x];

            if (num != 0)
            {
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x, y};
                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }
                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }

            global_state->end_line = (Vector2){x + 1, y + 1};

            x += 1;
            y += 1;
        }
        // boundry check
        if (player_won_condition(sum, neg_sum))
        {
            return true;
        }
    }
    /*
         0 1 2 3
       0 \ * * *
       1 \\ * *
       2 \\\ *
       3 \\\\
         ^ ^ ^ ^
    */
    for (int i = 0; i < GRID_CELL_COUNT; i++)
    {
        int y                    = GRID_CELL_COUNT - 1;
        int x                    = GRID_CELL_COUNT - i;
        global_state->start_line = (Vector2){0, 0};
        global_state->end_line   = (Vector2){0, 0};

        while (x >= 0 && x < GRID_CELL_COUNT && y >= 0 && y < GRID_CELL_COUNT)
        {
            num = grid_cpy[(y * GRID_CELL_COUNT) + x];

            if (num != 0)
            {
                if (global_state->start_line.x == 0 && global_state->start_line.y == 0)
                    global_state->start_line = (Vector2){x, y + 1};

                sum     += num;
                neg_sum += num;
            }
            else
            {
                if (player_won_condition(sum, neg_sum))
                {
                    return true;
                }
                global_state->start_line = (Vector2){0, 0};

                sum     = 0;
                neg_sum = 0;
            }

            global_state->end_line = (Vector2){x + 1, y};

            x -= 1;
            y -= 1;
        }
        // boundry check
        if (player_won_condition(sum, neg_sum))
        {
            return true;
        }
    }
    // Check if a number has been pressed
    // reset game

    Vector2 snapped_mouse =
        get_snapped_mouse_position(GRID_STARTING_X, GRID_STARTING_Y, GRID_CELL_SIZE, GRID_CELL_COUNT);

    if (snapped_mouse.x == 0 && snapped_mouse.y == 0)
    {
        return false;
    }

    snapped_mouse.x -= GRID_STARTING_X;
    snapped_mouse.y -= GRID_STARTING_Y;
    int x            = snapped_mouse.x / GRID_CELL_SIZE;
    int y            = snapped_mouse.y / GRID_CELL_SIZE;

    // might be terrible
    int key_pressed = GetKeyPressed();

    if (key_pressed != 0 && grid_cpy[(y * GRID_CELL_COUNT) + x] == 0)
    {
        switch (key_pressed)
        {
        case 49: { // 1
            if (global_state->player_one.is_turn && global_state->player_one.game_pieces[0])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = 1;
                global_state->player_one.game_pieces[0]--;
                switch_player();
            }
            else if (global_state->player_two.is_turn && global_state->player_two.game_pieces[0])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = -1;
                global_state->player_two.game_pieces[0]--;
                switch_player();
            }
        }
        break;
        case 50: { // 2
            if (global_state->player_one.is_turn && global_state->player_one.game_pieces[1])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = 2;
                global_state->player_one.game_pieces[1]--;
                switch_player();
            }
            else if (global_state->player_two.is_turn && global_state->player_two.game_pieces[1])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = -2;
                global_state->player_two.game_pieces[1]--;
                switch_player();
            }
        }
        break;
        case 51: { // 3
            if (global_state->player_one.is_turn && global_state->player_one.game_pieces[2])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = 3;
                global_state->player_one.game_pieces[2]--;
                switch_player();
            }
            else if (global_state->player_two.is_turn && global_state->player_two.game_pieces[2])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = -3;
                global_state->player_two.game_pieces[2]--;
                switch_player();
            }
        }
        break;
        case 52: { // 4
            if (global_state->player_one.is_turn && global_state->player_one.game_pieces[3])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = 4;
                global_state->player_one.game_pieces[3]--;
                switch_player();
            }
            else if (global_state->player_two.is_turn && global_state->player_two.game_pieces[3])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = -4;
                global_state->player_two.game_pieces[3]--;
                switch_player();
            }
        }
        break;
        case 53: { // 5
            if (global_state->player_one.is_turn && global_state->player_one.game_pieces[4])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = 5;
                global_state->player_one.game_pieces[4]--;
                switch_player();
            }
            else if (global_state->player_two.is_turn && global_state->player_two.game_pieces[4])
            {
                grid_cpy[(y * GRID_CELL_COUNT) + x] = -5;
                global_state->player_two.game_pieces[4]--;
                switch_player();
            }
        }
        break;
        default: {
        }
        break;
        }
    }

    memcpy(grid, grid_cpy, grid_size * sizeof(int));

    return false;
}

bool draw_menu()
{
    ClearBackground(BLACK);
    char text1[] = "Sum Connect Five\n";
    DrawText(text1, SCREEN_WIDTH / 6, 40, 60, WHITE);
    
    char text2[] = "Press 'P' to play\n";
    DrawText(text2, SCREEN_WIDTH / 4, 180, 30, GREEN);
    
    char text3[] = "Press 'R' to Reset the game \n";
    DrawText(text3, SCREEN_WIDTH / 4, 240, 30, RED);
    
    char text4[] = "Press 'M' to go back to the menu\n";
    DrawText(text4, SCREEN_WIDTH / 4, 300, 30, BLUE);
    return true;
}

bool draw_grid(int *grid, int grid_size, int x, int y, int grid_cell_size, int grid_cell_count)
{

    int grid_width  = (grid_cell_count * grid_cell_size) + x;
    int grid_height = grid_width;

    ClearBackground(BLACK);

    draw_grid_lines(x, y, grid_cell_size, grid_cell_count);

    Vector2 snapped_mouse_pos = get_snapped_mouse_position(x, y, grid_cell_size, grid_cell_count);
    float   radius            = ((float)grid_cell_size / 2);
    float   centre_point      = (float)GRID_CELL_SIZE / 2;

    if (snapped_mouse_pos.x != 0 && snapped_mouse_pos.y != 0)
    {
        snapped_mouse_pos.x += centre_point;
        snapped_mouse_pos.y += centre_point;

        DrawCircleV(snapped_mouse_pos, radius, GRAY);
    }

    char integer[6] = {};
    for (int y = 0; y < grid_cell_count; y++)
    {
        for (int x = 0; x < grid_cell_count; x++)
        {
            int num = grid[(y * grid_cell_count) + x];
            if (num == 0)
                continue;

            // screen grid coordinates
            int pixel_x = x * GRID_CELL_SIZE + GRID_STARTING_X;
            int pixel_y = y * GRID_CELL_SIZE + GRID_STARTING_Y;

            pixel_x += centre_point / 2;
            Color color;

            if (num < 0)
            {
                num   *= -1;
                color  = RED;
            }
            else
            {
                color = GREEN;
            }

            sprintf(integer, "%d\n", num);
            DrawText(integer, pixel_x, pixel_y, GRID_CELL_SIZE, color);
        }
    }

    // draw sidelines
    int pixel_x = 2;
    int pixel_y = SCREEN_HEIGHT / 3;
    for (int i = 0; i < 5; i++)
    {
        sprintf(integer, "%d\n", (i + 1));
        DrawText(integer, pixel_x, pixel_y, GRID_CELL_SIZE, global_state->player_one.color);

        sprintf(integer, ":%d\n", global_state->player_one.game_pieces[i]);
        DrawText(integer, pixel_x + 20, pixel_y, GRID_CELL_SIZE, global_state->player_one.color);

        sprintf(integer, "%d\n", (i + 1));
        DrawText(integer, pixel_x + grid_width, pixel_y, GRID_CELL_SIZE, global_state->player_two.color);

        sprintf(integer, ":%d\n", global_state->player_two.game_pieces[i]);
        DrawText(integer, pixel_x + grid_width + 20, pixel_y, GRID_CELL_SIZE, global_state->player_two.color);

        pixel_y += GRID_CELL_SIZE;
    }

    if (global_state->player_one.is_turn)
    {
        DrawRectangle(grid_width / 2, 0, 10, 10, global_state->player_one.color);
    }
    else
    {
        DrawRectangle(grid_width / 2, 0, 10, 10, global_state->player_two.color);
    }

    if (global_state->who_won != NULL)
    {

        Vector2 pos_line_start = {(global_state->start_line.x) * GRID_CELL_SIZE + x,
                                  (global_state->start_line.y) * GRID_CELL_SIZE + y};
        Vector2 pos_line_end   = {(global_state->end_line.x) * GRID_CELL_SIZE + x,
                                  (global_state->end_line.y) * GRID_CELL_SIZE + y};

        // horizontal
        if (pos_line_start.x < pos_line_end.x && pos_line_start.y == pos_line_end.y)
        {
            pos_line_start.y += centre_point;
            pos_line_end.y   += centre_point;
        }
        else if (pos_line_start.y < pos_line_end.y && pos_line_start.x == pos_line_end.x)
        {
            pos_line_start.x += centre_point;
            pos_line_end.x   += centre_point;
        }

        DrawLineEx(pos_line_start, pos_line_end, 5, global_state->who_won->color);
    }

    return true;
}

void draw_grid_lines(int x, int y, int grid_cell_size, int grid_cell_count)
{
    int grid_width  = grid_cell_count * grid_cell_size;
    int grid_height = grid_width;

    for (int i = x; i <= grid_width + x; i += grid_cell_size)
    {
        // horizontal
        DrawLine(x, i, grid_width + x, i, WHITE);

        // horizontal
        DrawLine(i, y, i, grid_height + y, WHITE);
    }
}

Vector2 get_snapped_mouse_position(int x, int y, int grid_cell_size, int grid_cell_count)
{
    Vector2 mouse_pos         = GetMousePosition();
    Vector2 snapped_mouse_pos = GetMousePosition();

    int grid_width  = (grid_cell_count * grid_cell_size) + x;
    int grid_height = grid_width;

    int base_x = x;
    int base_y = y;

    if (base_x > mouse_pos.x || base_y > mouse_pos.y || mouse_pos.x > grid_width || mouse_pos.y > grid_height)
    {
        return (Vector2){0, 0};
    }

    for (int i = 0; i <= grid_cell_count; i++)
    {
        if (base_x + grid_cell_size > mouse_pos.x)
            break;
        base_x += grid_cell_size;
    }

    for (int i = 0; i <= grid_cell_count; i++)
    {
        if (base_y + grid_cell_size > mouse_pos.y)
            break;
        base_y += grid_cell_size;
    }

    snapped_mouse_pos.x = base_x;
    snapped_mouse_pos.y = base_y;

    return snapped_mouse_pos;
}
