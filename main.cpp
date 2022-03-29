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
  int start_door;
  int end_door;
  wall *walls;
  room *next;
  room *prev;
  // da aggiungere: nemici, artefatti ecc...
};
typedef room *p_room;

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
} // funzione che stampa i muri

class Personaggio{
protected:
  int current_x;
  int current_y;
  int max_y;
  int max_x;
  room *room_head;
public:
  Personaggio(room *head, int max_y, int max_x, int current_x = 1, int current_y = 1){
    this->max_y = max_y;
    this->max_x = max_x;
    this->current_x = current_x;
    this->current_y = current_y;
    this->room_head = head;
    this->init();
  }

  bool check_for_h_walls(wall *head, int direction){    // the "h" stands for "horizontal"
    bool flag = true;
    switch(direction){
      case 1: {
        while(head != NULL){
          if(this->current_y - 1 == head->start_y && !(this->current_x >= head->start_door && this->current_x <= head->end_door)){
            flag = false;
          }
          head = head->next;
        }
        break;
      };
      case 2: {
        while(head != NULL){
          if(this->current_y + 1 == head->start_y && !(this->current_x >= head->start_door && this->current_x <= head->end_door)){
            flag = false;
          }
          head = head->next;
        }
        break;
      };
      case 3: {
        while(head != NULL){
          if(this->current_y == head->start_y && !(this->current_x - 1 >= head->start_door && this->current_x - 1 <= head->end_door)){
            flag = false;
          }
          head = head->next;
        }
        break;
      };
      case 4: {
        while(head != NULL){
          if(this->current_y == head->start_y && !(this->current_x + 1 >= head->start_door && this->current_x + 1 <= head->end_door)){
            flag = false;
          }
          head = head->next;
        }
        break;
      };
      default: flag = false; break;
    }
    return flag;
  }

  // bool check_for_v_walls(wall *head, int direction);

  void move_up(wall *head){
    if(this->current_y > 1 && this->check_for_h_walls(room_head->walls, 1)){ // a function that checks wheter there's a wall or not  wether
      this->current_y = this->current_y - 1;
    }
  }

  void move_down(wall *head){
    if(this->next_room()){
      this->current_y += 1;
      this->go_ahead();
    }
    else if(this->current_y < max_y - 2 && check_for_h_walls(room_head->walls, 2)){
      this->current_y = this->current_y + 1;
    }
  }

  void move_left(wall *head){
    if(this->current_x > 1 && check_for_h_walls(room_head->walls, 3)){
      this->current_x = this->current_x - 1;
    }
  }

  void move_right(wall *head){
    if(this->current_x < max_x - 2 && check_for_h_walls(room_head->walls, 4)){
      this->current_x = this->current_x + 1;
    }
  }

  void move(int direction, wall *head){
    mvwprintw(room_head->win, current_y, current_x, " ");
    switch(direction){
      case 1: this->move_up(head); break;
      case 2: this->move_down(head); break;
      case 3: this->move_left(head); break;
      case 4: this->move_right(head); break;
      default: break;
    }
    this->update();
  }

  bool next_room(){
    if((this->current_x >= room_head->start_door && this->current_x <= room_head->end_door) && (this->current_y + 1 == room_head->max_y-1 )){
      return true;
    }
    return false;
  }

  void update(){
    mvwprintw(room_head->win, current_y, current_x, "P");
    wrefresh(room_head->win);
    refresh();
  }

  void go_ahead(){
    this->room_head = this->room_head->next;
    this->init();
  }

  void go_backwards(){
    this->room_head = this->room_head->prev;
    this->init();
  }

  void reset(){
    this->current_x = 1;
    this->current_y = 1;
  }

  void init(){
    clear();
    stampa_muri(room_head->walls, room_head->win);
    refresh();
    reset();
    box(room_head->win, 0, 0);
    for(int i = room_head->start_door; i <= room_head->end_door; i++){
      mvwprintw(room_head->win, max_y-1, i, " ");
    }
    wrefresh(this->room_head->win);
    update();
  }
};

int main(){
  initscr();
  noecho();
  curs_set(0);

  WINDOW *win = newwin(20, 40, 5, 10);
  WINDOW *win2 = newwin(20, 40, 5, 10);

  wall *head = new wall;
  wall *second = new wall;
  wall *third = new wall;

  head->way = true;
  head->start_x = 1;
  head->end_x = 39;
  head->start_y = 5;
  head->start_door = 9;
  head->end_door = 12;

  second->way = true;
  second->start_x = 1;
  second->end_x = 39;
  second->start_y = 10;
  second->start_door = 25;
  second->end_door = 28;

  third->way = true;
  third->start_x = 1;
  third->end_x = 39;
  third->start_y = 15;
  third->start_door = 15;
  third->end_door = 18;

  head->next = NULL;
  second->next = third;
  third->next = NULL;

  room *room1 = new room;
  room *room2 = new room;

  room1->win = win;
  room1->max_x = 40;
  room1->max_y = 20;
  room1->walls = head;
  room1->start_door = 10;
  room1->end_door = 13;
  room1->prev = NULL;
  room1->next = room2;

  room2->win = win2;
  room2->max_x = 40;
  room2->max_y = 20;
  room2->start_door = 33;
  room2->end_door = 36;
  room2->walls = second;
  room2->prev = room1;
  room2->next = NULL;

  refresh();

  stampa_muri(head, win);

  Personaggio *fc = new Personaggio(room1, 20, 40);

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
      // delwin(win);
      // touchwin(stdscr);
      // clear();
      // refresh();
      stampa_muri(second, win2);
      break;
    }
  }

  getch();

  endwin();
}
