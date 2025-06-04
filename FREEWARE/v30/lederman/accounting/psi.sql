set verify
set language ENGLISH
set date format DATE 001, TIME 001
--
--                         Physical Database Definition
--
--------------------------------------------------------------------------------
create SCHEMA filename 'PSI.RDB'
--    
    dictionary is NOT REQUIRED
    protection is ACL
    number of users 50
    number of VAXcluster nodes 16
    buffer size is 6 blocks
    number of buffers 20
    number of recovery buffers 20
    adjustable lock granularity ENABLED
    snapshot is ENABLED IMMEDIATE
    page size is 2 blocks
    allocation is 512 pages
    extent is (minimum 99, maximum 9999, percent growth 20)
    snapshot allocation is 100 pages
    snapshot extent is (minimum 99, maximum 9999, percent growth 20)
; -- end create database
--
--                              Domain Definitions
--

create domain PROTOCOL_DOM  is char(4);		-- PROTOCOL_ID

create domain NAME_DOM	     is varchar (12);	-- job_name, username
create domain DTE_DOM        is varchar (14);	-- remote_dte, local_dte
create domain FACILITIES_DOM is varchar (16);
create domain ID1_DOM        is varchar (16);	-- network, remote_id
create domain DEST_DOM	     is varchar (32);	-- DESTINATION, QUEUE
create domain ID2_DOM	     is varchar (6);	-- terminal, PVC, node, unit
create domain ACCOUNT_DOM    is varchar (8);

create domain TYPE_DOM		is tinyint;	-- TYPE, SUB_TYPE
create domain CLEARING_DOM	is tinyint;	-- diagnostic, reason, cause
create domain PRIORITY_DOM	is tinyint edit string is "Z9";

create domain NEGOTIATED_DOM	is smallint;   -- packet, window, thruput
create domain ID3_DOM		is smallint;   -- requested, lcn, cug, node_address

create domain STAT_DOM	is integer edit string is "z,zz9";  -- messages, segments, packets, bytes
create domain ID4_DOM	is integer;  -- PIDs, JOB_ID, UIC, Privileges

create domain DATE_DOM	DATE;		-- START_DATE, SYSTEM_DATE

commit work;
--
--                              Table Definitions
--

create table PSI_CALLS (
    TYPE		TYPE_DOM,
    SUB_TYPE		TYPE_DOM,
    SYSTEM_DATE		DATE_DOM,
    PID			ID4_DOM,
    PID_OWNER		ID4_DOM,
    UIC			ID4_DOM,
    PRIVILEGE_1		ID4_DOM,
    PRIVILEGE_2		ID4_DOM,
    PRIORITY		PRIORITY_DOM,
    USERNAME		NAME_DOM,
    ACCOUNT		ACCOUNT_DOM,
    NODE		ID2_DOM,
    TERMINAL		ID2_DOM,
    JOB_NAME		NAME_DOM,
    JOB_ID		ID4_DOM,
    QUEUE		DEST_DOM,
    NODE_ADDRESS	ID3_DOM,
    REMOTE_ID		ID1_DOM,
    START_DATE		DATE_DOM,
    DESTINATION		DEST_DOM,
    REMOTE_DTE		DTE_DOM,
    LOCAL_DTE		DTE_DOM,
    NETWORK		ID1_DOM,
    PVC			ID2_DOM,
    LCN			ID3_DOM edit string is "zz9",
    BYTES_SENT		STAT_DOM edit string is "ZZZ,ZZ9",
    BYTES_RECEIVED	STAT_DOM edit string is "ZZZ,ZZ9",
    SEGMENTS_SENT	STAT_DOM,
    SEGMENTS_RECEIVED	STAT_DOM,
    PACKETS_SENT	STAT_DOM,
    PACKETS_RECEIVED	STAT_DOM,
    MESSAGES_SENT	STAT_DOM,
    MESSAGES_RECEIVED	STAT_DOM,
    THRUPUT_IN		NEGOTIATED_DOM,
    THRUPUT_OUT		NEGOTIATED_DOM,
    WINDOW_IN		NEGOTIATED_DOM,
    WINDOW_OUT		NEGOTIATED_DOM,
    PACKET_IN		NEGOTIATED_DOM,
    PACKET_OUT		NEGOTIATED_DOM,
    CLOSED_USER_GROUP	ID3_DOM,
    REQUESTED		ID3_DOM,
    FACILITIES_CLEARING  FACILITIES_DOM,
    FACILITIES_CALLING   FACILITIES_DOM,
    FACILITIES_ACCEPTING FACILITIES_DOM,
    CLEARING_REASON	CLEARING_DOM,
    CLEARING_CAUSE	CLEARING_DOM,
    CLEARING_DIAGNOSTIC	CLEARING_DOM,
    UNIT		ID2_DOM,
    PROTOCOL		PROTOCOL_DOM);
 
commit work;

--
--                              Index Definitions
--

-- create index R_COMMENT_SID_REV
--     on R_COMMENT (
--     SID
--         asc,
--     REV
--         asc)
--     type is SORTED;
-- commit work;
--  
-- create index SUB_PREV_SID
--     on SUB_PREV (
--     SID
--         asc)
--     type is SORTED;
-- commit work;
 
GRANT ALL PRIVILEGES ON SCHEMA RDB$DBHANDLE TO PUBLIC;

commit work;

