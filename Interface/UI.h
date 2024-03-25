#ifndef PROJETO_UI_H
#define PROJETO_UI_H

#include "../Logic/LoadingFunctions.h"
#include "../Logic/stdafx.h"


class UI {
public:
    UI();

    void menu_start();

    static void clear_screen();

    void loading_stuff(UI &ui);

    bool validate_input(char &op, const char lower_bound, const char upper_bound);

private:
    Graph<DeliverySite> g;

};


#endif //PROJETO_UI_H
