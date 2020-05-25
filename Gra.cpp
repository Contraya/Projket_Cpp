#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>
int h, w, z = 10, k, punkty;// z - życie, k - znak z klawiatury
bool wyjscie = false; // wyjście z gry
WINDOW* wnd;

typedef struct {
    int_fast8_t x;
    int_fast8_t y;
} vek2p;

typedef struct {
    vek2p poz;
    char znak = '>';
}pocisk;

typedef struct {
    vek2p poz;
    char znak = '#';
}obcy;

struct {
    vek2p poz;
    char gracz;
} gracz;

pocisk* magazynek = new pocisk[10];
obcy* kosmici = new obcy[30];

void obromowka(){
	/*
    Wyświetlanie obramówki pola
    ---------------------
    */
    init_pair(1, COLOR_BLACK, 35);// stworzenie schematu kolorów pod nr.1
    init_pair(2, 238, 35);
    init_pair(3, COLOR_RED, 35);
    wbkgd(wnd, COLOR_PAIR(1));//kolor tła
    attron(A_BOLD);
    box(wnd, 0, 0);
    attroff(A_BOLD);
    //-------------------
}
// ustawainie pocisku, że wyleciał z statku
void strzal(int x, int y){
	for(int i = 0; i < 10; i++){
		if(magazynek[i].poz.x == -1 || magazynek[i].poz.y == -1){
			magazynek[i].poz.x = x;
			magazynek[i].poz.y = y;
			break;
		}
	}
}
// wyświetlanie tabeli z życiem i punktami
void tabela(){
	for(int i = 1; i <= w-2; i++){
		attron(COLOR_PAIR(2));
		mvaddch(2, i, '=');
		attroff(COLOR_PAIR(2));
		//mvaddch(2, i, '=');
	}
	attron(COLOR_PAIR(3));
	mvprintw(1, 1, "Życie : %i",z);
	attroff(COLOR_PAIR(3));
	
	mvprintw(1,15,"Punkty : %i", punkty);
}
//sprawdzenie czy jest naciśnięty klawisz
int kbhit(void){
    k = getch();
    if (k != ERR) return 1;
    else return 0;
}
//sprawdzenie jaki klawisz został naciśnięty
int ruch(){
	int next;
	if(k == 'q'){
		wyjscie = true;
		return 1;
	}else if(k == KEY_UP || k == 'w'){
		next = gracz.poz.y - 1;
        if(next > 2)
        	gracz.poz.y -= 1;
        return 1;
	}else if(k == KEY_DOWN || k == 's'){
		next = gracz.poz.y + 1;
    	if(next < h - 1)
    		gracz.poz.y += 1;
		return 1;
	}else if(k == KEY_LEFT || k == 'a'){
		next = gracz.poz.x - 1;
        if(next > 0)
        	gracz.poz.x -= 1;
		return 1;
	}else if(k == KEY_RIGHT || k == 'd'){
		next = gracz.poz.x + 1;
        if(next < w - 1)
        	gracz.poz.x += 1;
        return 1;		
	}else if(k == ' '){
		strzal(gracz.poz.x, gracz.poz.y);
	}
	return 0;
}
// poruszanie pocisku jeśli został wystrzelony
void lotPocisku(){
	for(int i = 0; i < 10; i++){
		if(magazynek[i].poz.x > w - 2){
			magazynek[i].poz.x = -1;
			magazynek[i].poz.y = -1;
		}else if(magazynek[i].poz.x != -1){
			mvaddch(magazynek[i].poz.y,magazynek[i].poz.x, magazynek[i].znak);
			magazynek[i].poz.x += 1;
		}
	}
}
void tworzenieKosmici(int i){
	if(kosmici[i].poz.y == -1 ){
		kosmici[i].poz.y = rand() % 30 + 2;
	}else if(kosmici[i].poz.x < 2){
		kosmici[i].poz.y = -1;
	}
}
void ruchKosmitow(){
	for(int i = 0; i < 30; i++){
		if(kosmici[i].poz.y != -1){
			mvaddch(kosmici[i].poz.y,kosmici[i].poz.x, kosmici[i].znak);
			kosmici[i].poz.x -= 1;
		}
	}
}

void sprawdzanieKolizjiKosmici(){
	for(int i = 0; i < 30; i++){
		if(kosmici[i].poz.y != -1)
			if(gracz.poz.y == kosmici[i].poz.y && gracz.poz.x == kosmici[i].poz.x) z -=1;
	}
}
void sprawdzanieKolizjiPociski(){
	for(int j = 0; j < 30; j++){
		if(kosmici[j].poz.y != -1)
			for(int k = 0; k < 10; k++)
							if (kosmici[j].poz.y == magazynek[k].poz.y && kosmici[j].poz.x == magazynek[k].poz.x){
								kosmici[j].poz.x = w - 2;
								kosmici[j].poz.y = -1;
								magazynek[k].poz.x = -1;
								magazynek[k].poz.y = -1; 
								punkty += 10;
								}
	}
}
void Koniec(){
	mvprintw(10,45,"Zdobyles %i punktow", punkty);
	mvprintw(12,45,"za 2 s sie wylaczy");
	refresh();
	sleep(2);
}

//główna pętla gry
void poruszanie(){
	int i = 0;
    while(true) {
    	if(!wyjscie && z != 0){
    	if(kbhit()) ruch();
    	tworzenieKosmici(i);
    	
    	if(i < 30) i++;
    	else i = 0;
    	
    	ruchKosmitow();
    	lotPocisku();
    	
    	sprawdzanieKolizjiKosmici();
    	sprawdzanieKolizjiPociski();
    	
		mvaddch(gracz.poz.y, gracz.poz.x, gracz.gracz);
		obromowka();
        tabela();
		refresh();
    	usleep(10000);
        clear();
        //if(wyjscie || z <= 0) break; 
        }else{ 
        	Koniec();
        	break;
        }
	}
  
}

int main(int argv, char** argc) {
	for(int i = 0; i < 10; i++){
		magazynek[i].poz.x = -1;
		magazynek[i].poz.y = -1;
	}
	for(int i = 0; i < 30; i++){
		kosmici[i].poz.x = w - 2;
		kosmici[i].poz.y = -1;
	}
	
	wnd = initscr();
	keypad(stdscr, TRUE);
	cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    scrollok(stdscr, TRUE);
    start_color();
    getmaxyx(stdscr, h, w);
    if(!has_colors()) {
        endwin();
        std::cout << "Błąd. Twój terminal nie obsługuje kolorów.\n";
        exit(-1);
    }
    curs_set(FALSE);
    obromowka();
    mvprintw(10,40,"Poruszanie W,A,S,D lub strzalki, strzal SPACJA.");
    mvprintw(11,50,"Wyjscie klawisz Q");
    refresh();
    sleep(2);
    tabela();
    
    gracz.gracz = '0';// wygląd
    gracz.poz = {10, 5};// pozycja startowa
    
	poruszanie();
	std::cout << "Zdobyłeś : " << punkty << "punktów" << std::endl;
	
    //Czyszczenie
	curs_set(true);
	nocbreak();
	echo();
	refresh();
	delwin(wnd);
	endwin();
    return 0;
}
