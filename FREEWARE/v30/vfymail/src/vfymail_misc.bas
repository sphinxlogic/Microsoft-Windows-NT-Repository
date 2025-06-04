%TITLE "CHKSTS"
%SBTTL "Check Error Status"
%IDENT "V02.00"

FUNCTION LONG chksts (LONG stat)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    EXTERNAL SUB lib$signal (LONG BY VALUE)

    CALL lib$signal (stat)  IF (stat AND 1) = 0

END FUNCTION  stat  ! End of FUNCTION chksts

%TITLE "ALLOCATE_NODE"
%SBTTL "Allocate Tree Node"
%IDENT "V02.00"

FUNCTION LONG allocate_node &
	    (STRING sym_str, LONG node_addr, LONG user_data)

![function-header]
    
    OPTION TYPE=EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    !+
    ! External Routines
    !-
     
    EXTERNAL LONG FUNCTION chksts, &
			   lib$movc3, &
			   lib$get_vm

    !+
    ! Local Variables
    !-
     
    DECLARE WORD	sym_len, &
	    LONG	stat

    %PAGE
 
    WHEN ERROR IN
	sym_len = LEN (sym_str)

	stat = chksts (lib$get_vm ( &
			   sym_len+12, &
			   node_addr))

	CALL lib$movc3 ( &
		 2, &
		 sym_len, &
		 node_addr+10 BY VALUE)
	CALL lib$movc3 ( &
		 sym_len, &
		 sym_str BY REF, &
		 node_addr+12 BY VALUE)
    USE
	EXIT HANDLER
    END WHEN

END FUNCTION  1  ! End of FUNCTION ALLOCATE_NODE

%TITLE "COMPARE_NODE"
%SBTTL "Compare node in tree"
%IDENT "V02.00"

FUNCTION LONG compare_node ( &
		      STRING sym_str, &
		      node_type node, &
		      LONG user_data)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR, &
	   INACTIVE = SETUP
 
    %SBTTL "Declarations"
     
    %INCLUDE "vfycommon"
    
    !+
    ! External Routines
    !-

    EXTERNAL LONG FUNCTION compare_node1

    !+
    ! Local Variables
    !-
     
    compare_node = compare_node1 (sym_str, node)

END FUNCTION   ! End of FUNCTION COMPARE_NODE

%TITLE "COMPARE_NODE1"
%SBTTL "Compare node in tree"
%IDENT "V02.00"

FUNCTION LONG compare_node1 ( &
		      STRING sym_str, &
		      node_type node)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR
 
    %SBTTL "Declarations"
     
    %INCLUDE "str$routines" %FROM %LIBRARY
    %INCLUDE "vfycommon"

    !+
    ! MAP Declarations
    !-

    MAP (compare1_local) node_text_type	node_rec

    !+
    ! Local Variables
    !-
     
    DECLARE string  node_key, &
		    temp
     
    WHEN ERROR IN
	LSET node_rec::textrec = SEG$ (node::text, 1, node::text_len)
	node_key = TRM$ (node_rec::filename)

	compare_node1 = str$compare ( &
			    sym_str, &
			    node_key)
    USE
	EXIT HANDLER
    END WHEN

END FUNCTION   ! End of FUNCTION COMPARE_NODE1
