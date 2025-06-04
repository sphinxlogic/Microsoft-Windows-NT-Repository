  /* Formatted by text2c V01-120 (Flex V2.5). Last update 23-Oct-2000 */
  printf("\n Syntax is : \n    $ DELTREE[/qualifier] DirectorySpecifica");
  printf("tion\n\n DirectorySpecification can be of any of the followin");
  printf("g form\n    DirName[.dir]\tlocal directory (.dir is optional)");
  printf("\n    [.XXX.YYY]\t\trelative path syntax. Deletes [.XXX]YYY.D");
  printf("IR\n    DISK:[XXX.YYY]\twould remove DISK:[XXX]YYY.DIR \n    ");
  printf("x,y,z\t\ta comma separated list of the above\n \t\n Qualifier");
  printf("s are :\n   /HELP\t\tPrint this help.\t\n   /VERSION\t\tShows");
  printf(" the version only.\n   /[NO]ALLDIR\t\tConfirm before each sub");
  printf("-directory deletion.\n   /[NO]CONFIRM\t\tDeletion confirmatio");
  printf("n. Default is /CONFIRM.\n   /[NO]LOG\t\tShow operation failur");
  printf("e LOG and final statistics.\n   /[NO]TOP\t\tDelete or not the");
  printf(" top directory. Default is /TOP.\n\n   /DETACH\t\tDetach DELT");
  printf("REE command line.\n   /PRIORITY[=n]\tUses priority 'n' for th");
  printf("e detached process. \n   /PRIVILEGES\t\tEnable specific privi");
  printf("lege for a detached process.\n   /NOTIFY={MAIL|SEND}\tNotifie");
  printf("s user upon completion. See help on SEND method.\n\n");