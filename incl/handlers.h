#ifndef HANDLERS_H
# define HANDLERS_H

typedef struct  s_tab_entry
{
    char *      name;
    void        (*function)();
}               t_tab_entry;

typedef struct t_tab_entry t_tab;

/* Dispatch TABle */

# define DTAB_ENTRIES 2

t_tab DTAB[DTAB_ENTRIES] = {
        { "-ip", h_ip },
        { "-iL", h_file }
};

/* LEXicon for
 * file parsing
 */

# define LEX_ENTRIES 1

t_tab LEX[LEX_ENTRIES][] = {
        { "#IP", GENERIC }
};


#endif