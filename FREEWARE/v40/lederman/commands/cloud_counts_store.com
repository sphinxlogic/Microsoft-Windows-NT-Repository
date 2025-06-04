$ RUN SYS$SYSTEM:RDO
invoke database filename 'user0:[accounting]qna'
start_transaction read_write reserving cloud_counts for shared write
store D in cloud_counts using
    d.direction = "IN ";
    d.date_time = " 31-JUL-1992 10:13:05";
    d.AVAILABLE = Noinformati;
end_store
store D in cloud_counts using
    d.direction = "OUT";
    d.date_time = " 31-JUL-1992 10:13:29";
    d.SECONDS_SINCE_LAST_ZEROED = 6293;
    d.TERMINATING_PACKETS_RECEIVED = 344;
    d.ORIGINATING_PACKETS_SENT = 281;
    d.TERMINATING_CONGESTION_LOSS = 0;
    d.TRANSIT_PACKETS_RECEIVED = 0;
    d.TRANSIT_PACKETS_SENT = 0;
    d.TRANSIT_CONGESTION_LOSS = 0;
    d.CIRCUIT_DOWN = 0;
    d.INITIALIZATION_FAILURE = 0;
    d.ADJACENCY_DOWN = 0;
    d.PEAK_ADJACENCIES = 1;
end_store
commit
exit
$ EXIT
