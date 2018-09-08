#ifndef HANDLERS_H
# define HANDLERS_H

typedef struct  s_tab_entry
{
    char *      name;
    void        (*function)();
}               t_tab_entry;

typedef struct t_tab_entry t_tab;

#define DTAB_ENTRIES 2

/* Dispatch TABle */
t_tab DTAB[256] = {
        { '-ip', h_ip },
        { '-iL', h_file }
};

#endif