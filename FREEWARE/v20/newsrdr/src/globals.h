/*
** NEWSRDR globals
*/
    EXTERN unsigned int (*default_action)();
    EXTERN unsigned int (*cleanup_action)();
    EXTERN struct PROF news_prof;
    EXTERN struct CFG  news_cfg;
    EXTERN unsigned int image_privs[2];
