// Terminal app - same shell as text mode (shellExecute) but inside a window

#include <stddef.h>
#include "apps.h"
#include "graphics.h"
#include "debug.h"

#define TERM_COLS 70
#define TERM_ROWS 20
#define TERM_PADDING 8
#define INPUT_MAX 64

extern void shellExecute(const char *str);
extern uint32_t timer;
extern uint16_t current_tps;

struct window * terminal_window = NULL;

char term_lines[TERM_ROWS][TERM_COLS+1]; //history (last line is just above prompt)
int term_line_count = 0;

char term_input[INPUT_MAX+1];
int term_input_len = 0;

int cursor_visible = 1;
uint32_t last_blink = 0;

void terminal_draw(struct window * w){
    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,w->height,15,15,20);

    //history
    int y = TERM_PADDING;
    for(int i=0;i<term_line_count;i++){
        vbe_draw_string(TERM_PADDING,y,term_lines[i],220,220,220);
        y += FONT_HEIGHT;
    }

    //prompt + input
    vbe_draw_string(TERM_PADDING,y,"PenguOS>> ",80,220,100);
    int input_x = TERM_PADDING + 10*FONT_WIDTH;
    vbe_draw_string(input_x,y,term_input,255,255,255);

    //cursor
    if(cursor_visible){
        vbe_draw_rect_solid(input_x + term_input_len*FONT_WIDTH,y+2,FONT_WIDTH,FONT_HEIGHT-4,200,200,200);
    }

    window_draw_end(w);
}

void terminal_new_line(const char * str){
    if(term_line_count >= TERM_ROWS-1){ //scroll up (keep 1 row for prompt)
        for(int i=1;i<term_line_count;i++){
            for(int j=0;j<=TERM_COLS;j++) term_lines[i-1][j] = term_lines[i][j];
        }
        term_line_count--;
    }

    int i=0;
    for(;str[i] != '\0' && i<TERM_COLS;i++){
        term_lines[term_line_count][i] = str[i];
    }
    term_lines[term_line_count][i] = '\0';
    term_line_count++;
}

//print text in terminal (splits on \n and long lines)
void terminal_print(const char * str){
    if(terminal_window == NULL){
        serial_print(str);
        return;
    }

    char line[TERM_COLS+1];
    int len = 0;
    for(int i=0;;i++){
        if(str[i] == '\0' || str[i] == '\n' || len == TERM_COLS){
            line[len] = '\0';
            terminal_new_line(line);
            len = 0;
            if(str[i] == '\0') break;
            if(str[i] == '\n') continue;
        }
        line[len] = str[i];
        len++;
    }
    terminal_draw(terminal_window);
}

void terminal_clear(){
    term_line_count = 0;
}

void terminal_key(struct window * w,char c){
    if(c == '\n'){ //Enter
        char line[TERM_COLS+1] = "PenguOS>> ";
        int i=0;
        for(;term_input[i] != '\0';i++) line[10+i] = term_input[i];
        line[10+i] = '\0';
        terminal_new_line(line);

        shellExecute(term_input); //execute the command

        term_input_len = 0;
        term_input[0] = '\0';
    }
    else if(c == '\b'){ //Backspace
        if(term_input_len > 0){
            term_input_len--;
            term_input[term_input_len] = '\0';
        }
    }
    else if(c >= 32 && c <= 126){
        if(term_input_len < INPUT_MAX){
            term_input[term_input_len] = c;
            term_input_len++;
            term_input[term_input_len] = '\0';
        }
    }

    cursor_visible = 1;
    last_blink = timer;
    terminal_draw(w);
}

void terminal_update(struct window * w){
    if(timer >= last_blink + current_tps/2){ //blink cursor every half second
        cursor_visible = !cursor_visible;
        last_blink = timer;
        terminal_draw(w);
    }
}

void terminal_close(struct window * w){
    (void)w;
    terminal_window = NULL;
}

void open_terminal(){
    if(terminal_window != NULL){ //only one terminal for now
        window_focus(terminal_window);
        return;
    }

    terminal_window = create_window("Terminal",160,90,TERM_COLS*FONT_WIDTH + 2*TERM_PADDING,TERM_ROWS*FONT_HEIGHT + 2*TERM_PADDING);
    if(terminal_window == NULL) return;

    terminal_window->on_draw = terminal_draw;
    terminal_window->on_key = terminal_key;
    terminal_window->on_update = terminal_update;
    terminal_window->on_close = terminal_close;

    term_line_count = 0;
    term_input_len = 0;
    term_input[0] = '\0';
    terminal_print("Welcome to PenguOS terminal! type 'help' for commands");
}
