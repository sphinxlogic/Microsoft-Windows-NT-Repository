/* ************************************************************************
*   File: house.c                                       Part of CircleMUD *
*  Usage: Handling of player houses                                       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"
#include "house.h"
#include "constants.h"

extern struct room_data *world;
extern struct index_data *obj_index;


struct obj_data *Obj_from_store(struct obj_file_elem object, int *location);
int Obj_to_store(struct obj_data * obj, FILE * fl, int location);

struct house_control_rec house_control[MAX_HOUSES];
int num_of_houses = 0;

/* local functions */
int House_get_filename(int vnum, char *filename);
int House_load(room_vnum vnum);
int House_save(struct obj_data * obj, FILE * fp);
void House_restore_weight(struct obj_data * obj);
void House_delete_file(int vnum);
int find_house(room_vnum vnum);
void House_save_control(void);
void hcontrol_list_houses(struct char_data * ch);
void hcontrol_build_house(struct char_data * ch, char *arg);
void hcontrol_destroy_house(struct char_data * ch, char *arg);
void hcontrol_pay_house(struct char_data * ch, char *arg);
ACMD(do_hcontrol);
ACMD(do_house);


/* First, the basics: finding the filename; loading/saving objects */

/* Return a filename given a house vnum */
int House_get_filename(int vnum, char *filename)
{
  if (vnum == NOWHERE)
    return (0);

  sprintf(filename, LIB_HOUSE"%d.house", vnum);
  return (1);
}


/* Load all objects for a house */
int House_load(room_vnum vnum)
{
  FILE *fl;
  char fname[MAX_STRING_LENGTH];
  struct obj_file_elem object;
  room_rnum rnum;
  int i;

  if ((rnum = real_room(vnum)) == NOWHERE)
    return (0);
  if (!House_get_filename(vnum, fname))
    return (0);
  if (!(fl = fopen(fname, "r+b")))	/* no file found */
    return (0);
  while (!feof(fl)) {
    fread(&object, sizeof(struct obj_file_elem), 1, fl);
    if (ferror(fl)) {
      perror("SYSERR: Reading house file in House_load");
      fclose(fl);
      return (0);
    }
    if (!feof(fl))
      obj_to_room(Obj_from_store(object, &i), rnum);
  }

  fclose(fl);

  return (1);
}


/* Save all objects for a house (recursive; initial call must be followed
   by a call to House_restore_weight)  Assumes file is open already. */
int House_save(struct obj_data * obj, FILE * fp)
{
  struct obj_data *tmp;
  int result;

  if (obj) {
    House_save(obj->contains, fp);
    House_save(obj->next_content, fp);
    result = Obj_to_store(obj, fp, 0);
    if (!result)
      return (0);

    for (tmp = obj->in_obj; tmp; tmp = tmp->in_obj)
      GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
  }
  return (1);
}


/* restore weight of containers after House_save has changed them for saving */
void House_restore_weight(struct obj_data * obj)
{
  if (obj) {
    House_restore_weight(obj->contains);
    House_restore_weight(obj->next_content);
    if (obj->in_obj)
      GET_OBJ_WEIGHT(obj->in_obj) += GET_OBJ_WEIGHT(obj);
  }
}


/* Save all objects in a house */
void House_crashsave(room_vnum vnum)
{
  int rnum;
  char buf[MAX_STRING_LENGTH];
  FILE *fp;

  if ((rnum = real_room(vnum)) == NOWHERE)
    return;
  if (!House_get_filename(vnum, buf))
    return;
  if (!(fp = fopen(buf, "wb"))) {
    perror("SYSERR: Error saving house file");
    return;
  }
  if (!House_save(world[rnum].contents, fp)) {
    fclose(fp);
    return;
  }
  fclose(fp);
  House_restore_weight(world[rnum].contents);
  REMOVE_BIT(ROOM_FLAGS(rnum), ROOM_HOUSE_CRASH);
}


/* Delete a house save file */
void House_delete_file(int vnum)
{
  char fname[MAX_INPUT_LENGTH];
  FILE *fl;

  if (!House_get_filename(vnum, fname))
    return;
  if (!(fl = fopen(fname, "rb"))) {
    if (errno != ENOENT)
      log("SYSERR: Error deleting house file #%d. (1): %s", vnum, strerror(errno));
    return;
  }
  fclose(fl);
  if (remove(fname) < 0)
    log("SYSERR: Error deleting house file #%d. (2): %s", vnum, strerror(errno));
}


/* List all objects in a house file */
void House_listrent(struct char_data * ch, room_vnum vnum)
{
  FILE *fl;
  char fname[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  struct obj_file_elem object;
  struct obj_data *obj;
  int i;

  if (!House_get_filename(vnum, fname))
    return;
  if (!(fl = fopen(fname, "rb"))) {
    sprintf(buf, "No objects on file for house #%d.\r\n", vnum);
    send_to_char(buf, ch);
    return;
  }
  *buf = '\0';
  while (!feof(fl)) {
    fread(&object, sizeof(struct obj_file_elem), 1, fl);
    if (ferror(fl)) {
      fclose(fl);
      return;
    }
    if (!feof(fl) && (obj = Obj_from_store(object, &i)) != NULL) {
      sprintf(buf + strlen(buf), " [%5d] (%5dau) %s\r\n",
	      GET_OBJ_VNUM(obj), GET_OBJ_RENT(obj),
	      obj->short_description);
      free_obj(obj);
    }
  }

  send_to_char(buf, ch);
  fclose(fl);
}




/******************************************************************
 *  Functions for house administration (creation, deletion, etc.  *
 *****************************************************************/

int find_house(room_vnum vnum)
{
  int i;

  for (i = 0; i < num_of_houses; i++)
    if (house_control[i].vnum == vnum)
      return (i);

  return (NOWHERE);
}



/* Save the house control information */
void House_save_control(void)
{
  FILE *fl;

  if (!(fl = fopen(HCONTROL_FILE, "wb"))) {
    perror("SYSERR: Unable to open house control file.");
    return;
  }
  /* write all the house control recs in one fell swoop.  Pretty nifty, eh? */
  fwrite(house_control, sizeof(struct house_control_rec), num_of_houses, fl);

  fclose(fl);
}


/* call from boot_db - will load control recs, load objs, set atrium bits */
/* should do sanity checks on vnums & remove invalid records */
void House_boot(void)
{
  struct house_control_rec temp_house;
  room_rnum real_house, real_atrium;
  FILE *fl;

  memset((char *)house_control,0,sizeof(struct house_control_rec)*MAX_HOUSES);

  if (!(fl = fopen(HCONTROL_FILE, "rb"))) {
    if (errno == ENOENT)
      log("   House control file '%s' does not exist.", HCONTROL_FILE);
    else
      perror("SYSERR: " HCONTROL_FILE);
    return;
  }
  while (!feof(fl) && num_of_houses < MAX_HOUSES) {
    fread(&temp_house, sizeof(struct house_control_rec), 1, fl);

    if (feof(fl))
      break;

    if (get_name_by_id(temp_house.owner) == NULL)
      continue;			/* owner no longer exists -- skip */

    if ((real_house = real_room(temp_house.vnum)) == NOWHERE)
      continue;			/* this vnum doesn't exist -- skip */

    if (find_house(temp_house.vnum) != NOWHERE)
      continue;			/* this vnum is already a house -- skip */

    if ((real_atrium = real_room(temp_house.atrium)) == NOWHERE)
      continue;			/* house doesn't have an atrium -- skip */

    if (temp_house.exit_num < 0 || temp_house.exit_num >= NUM_OF_DIRS)
      continue;			/* invalid exit num -- skip */

    if (TOROOM(real_house, temp_house.exit_num) != real_atrium)
      continue;			/* exit num mismatch -- skip */

    house_control[num_of_houses++] = temp_house;

    SET_BIT(ROOM_FLAGS(real_house), ROOM_HOUSE | ROOM_PRIVATE);
    SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
    House_load(temp_house.vnum);
  }

  fclose(fl);
  House_save_control();
}



/* "House Control" functions */

const char *HCONTROL_FORMAT =
"Usage: hcontrol build <house vnum> <exit direction> <player name>\r\n"
"       hcontrol destroy <house vnum>\r\n"
"       hcontrol pay <house vnum>\r\n"
"       hcontrol show\r\n";

void hcontrol_list_houses(struct char_data * ch)
{
  int i;
  char *timestr, *temp;
  char built_on[128], last_pay[128], own_name[128];

  if (!num_of_houses) {
    send_to_char("No houses have been defined.\r\n", ch);
    return;
  }
  strcpy(buf, "Address  Atrium  Build Date  Guests  Owner        Last Paymt\r\n");
  strcat(buf, "-------  ------  ----------  ------  ------------ ----------\r\n");
  send_to_char(buf, ch);

  for (i = 0; i < num_of_houses; i++) {
    /* Avoid seeing <UNDEF> entries from self-deleted people. -gg 6/21/98 */
    if ((temp = get_name_by_id(house_control[i].owner)) == NULL)
      continue;

    if (house_control[i].built_on) {
      timestr = asctime(localtime(&(house_control[i].built_on)));
      *(timestr + 10) = '\0';
      strcpy(built_on, timestr);
    } else
      strcpy(built_on, "Unknown");

    if (house_control[i].last_payment) {
      timestr = asctime(localtime(&(house_control[i].last_payment)));
      *(timestr + 10) = '\0';
      strcpy(last_pay, timestr);
    } else
      strcpy(last_pay, "None");

    /* Now we need a copy of the owner's name to capitalize. -gg 6/21/98 */
    strcpy(own_name, temp);

    sprintf(buf, "%7d %7d  %-10s    %2d    %-12s %s\r\n",
	    house_control[i].vnum, house_control[i].atrium, built_on,
	    house_control[i].num_of_guests, CAP(own_name), last_pay);

    send_to_char(buf, ch);
    House_list_guests(ch, i, TRUE);
  }
}



void hcontrol_build_house(struct char_data * ch, char *arg)
{
  char arg1[MAX_INPUT_LENGTH];
  struct house_control_rec temp_house;
  room_vnum virt_house, virt_atrium;
  room_rnum real_house, real_atrium;
  sh_int exit_num;
  long owner;

  if (num_of_houses >= MAX_HOUSES) {
    send_to_char("Max houses already defined.\r\n", ch);
    return;
  }

  /* first arg: house's vnum */
  arg = one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  virt_house = atoi(arg1);
  if ((real_house = real_room(virt_house)) == NOWHERE) {
    send_to_char("No such room exists.\r\n", ch);
    return;
  }
  if ((find_house(virt_house)) != NOWHERE) {
    send_to_char("House already exists.\r\n", ch);
    return;
  }

  /* second arg: direction of house's exit */
  arg = one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((exit_num = search_block(arg1, dirs, FALSE)) < 0) {
    sprintf(buf, "'%s' is not a valid direction.\r\n", arg1);
    send_to_char(buf, ch);
    return;
  }
  if (TOROOM(real_house, exit_num) == NOWHERE) {
    sprintf(buf, "There is no exit %s from room %d.\r\n", dirs[exit_num],
	    virt_house);
    send_to_char(buf, ch);
    return;
  }

  real_atrium = TOROOM(real_house, exit_num);
  virt_atrium = GET_ROOM_VNUM(real_atrium);

  if (TOROOM(real_atrium, rev_dir[exit_num]) != real_house) {
    send_to_char("A house's exit must be a two-way door.\r\n", ch);
    return;
  }

  /* third arg: player's name */
  one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((owner = get_id_by_name(arg1)) < 0) {
    sprintf(buf, "Unknown player '%s'.\r\n", arg1);
    send_to_char(buf, ch);
    return;
  }

  temp_house.mode = HOUSE_PRIVATE;
  temp_house.vnum = virt_house;
  temp_house.atrium = virt_atrium;
  temp_house.exit_num = exit_num;
  temp_house.built_on = time(0);
  temp_house.last_payment = 0;
  temp_house.owner = owner;
  temp_house.num_of_guests = 0;

  house_control[num_of_houses++] = temp_house;

  SET_BIT(ROOM_FLAGS(real_house), ROOM_HOUSE | ROOM_PRIVATE);
  SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
  House_crashsave(virt_house);

  send_to_char("House built.  Mazel tov!\r\n", ch);
  House_save_control();
}



void hcontrol_destroy_house(struct char_data * ch, char *arg)
{
  int i, j;
  room_rnum real_atrium, real_house;

  if (!*arg) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((i = find_house(atoi(arg))) == NOWHERE) {
    send_to_char("Unknown house.\r\n", ch);
    return;
  }
  if ((real_atrium = real_room(house_control[i].atrium)) == NOWHERE)
    log("SYSERR: House %d had invalid atrium %d!", atoi(arg),
	house_control[i].atrium);
  else
    REMOVE_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);

  if ((real_house = real_room(house_control[i].vnum)) == NOWHERE)
    log("SYSERR: House %d had invalid vnum %d!", atoi(arg), house_control[i].vnum);
  else
    REMOVE_BIT(ROOM_FLAGS(real_house),
	       ROOM_HOUSE | ROOM_PRIVATE | ROOM_HOUSE_CRASH);

  House_delete_file(house_control[i].vnum);

  for (j = i; j < num_of_houses - 1; j++)
    house_control[j] = house_control[j + 1];

  num_of_houses--;

  send_to_char("House deleted.\r\n", ch);
  House_save_control();

  /*
   * Now, reset the ROOM_ATRIUM flag on all existing houses' atriums,
   * just in case the house we just deleted shared an atrium with another
   * house.  --JE 9/19/94
   */
  for (i = 0; i < num_of_houses; i++)
    if ((real_atrium = real_room(house_control[i].atrium)) != NOWHERE)
      SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
}


void hcontrol_pay_house(struct char_data * ch, char *arg)
{
  int i;

  if (!*arg)
    send_to_char(HCONTROL_FORMAT, ch);
  else if ((i = find_house(atoi(arg))) == NOWHERE)
    send_to_char("Unknown house.\r\n", ch);
  else {
    sprintf(buf, "Payment for house %s collected by %s.", arg, GET_NAME(ch));
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);

    house_control[i].last_payment = time(0);
    House_save_control();
    send_to_char("Payment recorded.\r\n", ch);
  }
}


/* The hcontrol command itself, used by imms to create/destroy houses */
ACMD(do_hcontrol)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

  half_chop(argument, arg1, arg2);

  if (is_abbrev(arg1, "build"))
    hcontrol_build_house(ch, arg2);
  else if (is_abbrev(arg1, "destroy"))
    hcontrol_destroy_house(ch, arg2);
  else if (is_abbrev(arg1, "pay"))
    hcontrol_pay_house(ch, arg2);
  else if (is_abbrev(arg1, "show"))
    hcontrol_list_houses(ch);
  else
    send_to_char(HCONTROL_FORMAT, ch);
}


/* The house command, used by mortal house owners to assign guests */
ACMD(do_house)
{
  int i, j, id;

  one_argument(argument, arg);

  if (!ROOM_FLAGGED(IN_ROOM(ch), ROOM_HOUSE))
    send_to_char("You must be in your house to set guests.\r\n", ch);
  else if ((i = find_house(GET_ROOM_VNUM(IN_ROOM(ch)))) == NOWHERE)
    send_to_char("Um.. this house seems to be screwed up.\r\n", ch);
  else if (GET_IDNUM(ch) != house_control[i].owner)
    send_to_char("Only the primary owner can set guests.\r\n", ch);
  else if (!*arg)
    House_list_guests(ch, i, FALSE);
  else if ((id = get_id_by_name(arg)) < 0)
    send_to_char("No such player.\r\n", ch);
  else if (id == GET_IDNUM(ch))
    send_to_char("It's your house!\r\n", ch);
  else {
    for (j = 0; j < house_control[i].num_of_guests; j++)
      if (house_control[i].guests[j] == id) {
	for (; j < house_control[i].num_of_guests; j++)
	  house_control[i].guests[j] = house_control[i].guests[j + 1];
	house_control[i].num_of_guests--;
	House_save_control();
	send_to_char("Guest deleted.\r\n", ch);
	return;
      }
    if (house_control[i].num_of_guests == MAX_GUESTS) {
      send_to_char("You have too many guests.\r\n", ch);
      return;
    }
    j = house_control[i].num_of_guests++;
    house_control[i].guests[j] = id;
    House_save_control();
    send_to_char("Guest added.\r\n", ch);
  }
}



/* Misc. administrative functions */


/* crash-save all the houses */
void House_save_all(void)
{
  int i;
  room_rnum real_house;

  for (i = 0; i < num_of_houses; i++)
    if ((real_house = real_room(house_control[i].vnum)) != NOWHERE)
      if (ROOM_FLAGGED(real_house, ROOM_HOUSE_CRASH))
	House_crashsave(house_control[i].vnum);
}


/* note: arg passed must be house vnum, so there. */
int House_can_enter(struct char_data * ch, room_vnum house)
{
  int i, j;

  if (GET_LEVEL(ch) >= LVL_GRGOD || (i = find_house(house)) == NOWHERE)
    return (1);

  switch (house_control[i].mode) {
  case HOUSE_PRIVATE:
    if (GET_IDNUM(ch) == house_control[i].owner)
      return (1);
    for (j = 0; j < house_control[i].num_of_guests; j++)
      if (GET_IDNUM(ch) == house_control[i].guests[j])
	return (1);
  }

  return (0);
}

void House_list_guests(struct char_data *ch, int i, int quiet)
{
  int j;
  char *temp;
  char buf[MAX_STRING_LENGTH], buf2[MAX_NAME_LENGTH + 2];

  if (house_control[i].num_of_guests == 0) {
    if (!quiet)
      send_to_char("  Guests: None\r\n", ch);
    return;
  }

  strcpy(buf, "  Guests: ");

  /* Avoid <UNDEF>. -gg 6/21/98 */
  for (j = 0; j < house_control[i].num_of_guests; j++) {
    if ((temp = get_name_by_id(house_control[i].guests[j])) == NULL)
      continue;
    sprintf(buf2, "%s ", temp);
    strcat(buf, CAP(buf2));
  }

  strcat(buf, "\r\n");
  send_to_char(buf, ch);
}

