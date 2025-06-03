/* V22$MACROS.H
 *
 * Utility & storage class definitions
 * (used for cross-referential or multiple-instance structures)
 *
 */

# define GOOD				1
# define BAD				0
# define valid( function_result )	((function_result) & GOOD)
# define odd( expression )		((expression) & 1)
# define get_data( target,count )	((read(fileno(sample),target,count) == count) ? GOOD : BAD)

# define D_CONSTANT( name,string )	struct dsc$descriptor_s name = { sizeof(string)-1, DSC$K_DTYPE_T, DSC$K_CLASS_S, string }
# define D_VARIABLE( name,string )	struct dsc$descriptor_d name = { sizeof(string)-1, 0, 0, string }

typedef struct cycle_block
   {
   struct cycle_block *next;
   struct cycle_block *previous;
   unsigned long reference_count;
   union
      {
      struct prb$c_header
	 {
	 unsigned long max_id;
	 unsigned long creation_time[2];
	 unsigned long activity;
	 unsigned long standby;
	 } sample;
      struct prb$c_link
	 {
	 unsigned long id;
	 unsigned long start_time[2];
	 unsigned long stop_time[2];
	 } object;
      } block;
   } C_BLOCK;

typedef struct node_block
   {
   struct node_block *next;
   unsigned long reference_count;
   struct prb$node
      {
      unsigned char address[6];
      unsigned char name[rpl$object_name_size+1];
      } object;
   struct prb$n_flags
      {
      unsigned is_rejected : 1;
      unsigned : 0;
      } control;
   } N_BLOCK;

typedef struct protocol_block
   {
   struct protocol_block *next;
   unsigned long reference_count;
   struct prb$protocol
      {
      unsigned short id;
      unsigned char  value[2];
      unsigned char  name[rpl$object_name_size+1];
      } object;
   struct prb$p_flags
      {
      unsigned is_rejected : 1;
      unsigned : 0;
      } control;
   } P_BLOCK;

