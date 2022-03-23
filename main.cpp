#include <ncurses.h>
#include <string>
using namespace std;

/*int menu(){
  initscr();
  noecho();

  WINDOW *win = newwin(5, 40, 5, 5);
  refresh();
  box(win, 0, 0);
  wrefresh(win);

  // mvwprintw(win, 1, 1, "ciao");
  // wrefresh(win);

  string array[3] = {"Cammina", "Corri", "Nuota"};
  int scelta = 0;
  keypad(win, true);

  while(true){
    for(int i = 0; i < 3; i++){
      if(i == scelta){
        wattron(win, A_REVERSE);
      }
      mvwprintw(win, i+1, 1, "%s", array[i].c_str());
      wattroff(win, A_REVERSE);
    }
    int tasto = wgetch(win);
    switch(tasto){
      case KEY_UP:
        scelta--;
        break;
      case KEY_DOWN:
        scelta++;
        break;
      default:
        break;
    }

    if(tasto == 10){
      mvprintw(0, 0, "Hai fatto una cazzata!");
      break;
    }
  }

  getch();

  endwin();
  return 0;
}*/

// Structs
struct wall{
  bool way; // if true then orizzontale else verticale
  int start_x;
  int end_x;
  int start_y;
  int end_y;
  int start_door;
  int end_door;
  wall *next;
};

struct room{
  WINDOW *win;
  int max_x;
  int max_y;
  wall *walls;
  // da aggiungere: nemici, artefatti ecc...
};

void stampa_muri(wall *walls, WINDOW *win){
  while(walls != NULL){
    if(walls->way){
      for(int i = walls->start_x; i < walls->end_x; i++){
        if(i < walls->start_door || i > walls->end_door)
          mvwprintw(win, walls->start_y, i, "-");
      }
    }else{
      for(int i = walls->start_y; i <= walls->end_y; i++){
        mvwprintw(win, i, walls->start_x, "|");
      }
    }
    walls = walls->next;
  }
}

class Personaggio{
protected:
  int current_x;
  int current_y;
  int max_y;
  int max_x;
  WINDOW *win;
public:
  Personaggio(WINDOW *win, int max_y, int max_x, int current_x = 0, int current_y = 0){
    this->max_y = max_y;
    this->max_x = max_x;
    this->current_x = current_x;
    this->current_y = current_y;
    this->win = win;
    this->update();
  }

  bool check_for_h_walls(wall *head, int direction){    // the "h" stands for "horizontal"
    switch(direction){
      case 1: return ((this->current_y - 1 != head->start_y) || (this->current_y - 1 == head->start_y && (this->current_x >= head->start_door && this->current_x <= head->end_door)));
      case 2: return ((this->current_y + 1 != head->start_y) || (this->current_y + 1 == head->start_y && (this->current_x >= head->start_door && this->current_x <= head->end_door)));
      case 3: return ((this->current_y != head->start_y) || (this->current_y == head->start_y && (this->current_x - 1 >= head->start_door && this->current_x - 1 <= head->end_door)));
      case 4: return ((this->current_y != head->start_y) || (this->current_y == head->start_y && (this->current_x + 1 >= head->start_door && this->current_x + 1 <= head->end_door)));
      default: return false;
    }
  }

  void move_up(wall *head){
    if(this->current_y > 1 && this->check_for_h_walls(head, 1)){ // a function that checks wheter there's a wall or not  wether
      this->current_y = this->current_y - 1;
    }
  }

  void move_down(wall *head){
    if(this->current_y < max_y - 2 && check_for_h_walls(head, 2)){
      this->current_y = this->current_y + 1;
    }
  }

  void move_left(wall *head){
    if(this->current_x > 1 && check_for_h_walls(head, 3)){
      this->current_x = this->current_x - 1;
    }
  }

  void move_right(wall *head){
    if(this->current_x < max_x - 2 && check_for_h_walls(head, 4)){
      this->current_x = this->current_x + 1;
    }
  }

  void move(int direction, wall *head){
    mvwprintw(win, current_y, current_x, " ");
    switch(direction){
      case 1: this->move_up(head); break;
      case 2: this->move_down(head); break;
      case 3: this->move_left(head); break;
      case 4: this->move_right(head); break;
      default: break;
    }
    this->update();
  }

  void update(){
    mvwprintw(win, current_y, current_x, "P");
    wrefresh(this->win);
  }
};

int main(){
  initscr();
  noecho();
  curs_set(0);

  WINDOW *win = newwin(20, 40, 5, 10);

  wall *head = new wall;
  head->way = true;
  head->start_x = 1;
  head->end_x = 39;
  head->start_y = 5;
  head->start_door = 9;
  head->end_door = 12;

  head->next = NULL;

  refresh();
  box(win, 0, 0);
  // wborder(win2, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win);

  stampa_muri(head, win);

  Personaggio *fc = new Personaggio(win, 20, 40, 1, 1);

  while(true){
    int c = getch();
    switch(c){
      case 'w': fc->move(1, head); break;
      case 's': fc->move(2, head); break;
      case 'a': fc->move(3, head); break;
      case 'd': fc->move(4, head); break;
      default: break;
    }
    if(c == 'l'){
      delwin(win);
      touchwin(stdscr);
      break;
    }
  }

  getch();

  endwin();
}
