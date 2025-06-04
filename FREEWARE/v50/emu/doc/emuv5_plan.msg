    EMUV5 Plan

    Introduction.

    In the beginning there was the plan. And the plan was the word.
    And the plan was good for about 2 minutes when after much
    blaspheming and quaffing of wine was condemned to the fires of
    hell. And the planners were pleased. And the new plan became the
    new word and was codified and implemented. And the plan was again
    blasphemed and burnt and scorned was heaped unto the heavens upon
    it. It then came to pass that a certain programmer was on his way to
    Jerico and was accosted. And it was discovered he was a man with a
    plan. The plan was then nailed to the temple door and in their
    anguish the planners said 'fuck all this planning and let us
    implement before we are old and grey'. And the man was also nailed
    to the door as a warning to all concerned lest they would ignore
    the plan next to him. 

    Summary.
    This is the high level list of things to do with some ordering
    imposed. Detailed discussion of each activity is in the next
    section.

    Phase 1. - Initial

    High level design , infrastructure and initial implementation.
    This can be considered as done.

    Phase 2  - Core

    Convert existing PSRs to new scheme
    Add Relater facility
    Rewrite NAMER facility
    Define and implement Datatyping scheme.
    Simple (internal use only) display mechanism.
    Implement ageout

    Phase 3. - Management

    Define and implement system management
    Comprehensive display mechanism
    Phase 1 Alert mechanism
    Formal Documentation

    Phase 4. - 1st 'layered' applications

    Planned expansion of PSRs
    Counter processing
    WANPROBE
    SETFUNC

    Phase 5. Database

    Define and implement the RDB for each PSR
    EMU to RDB interface
    PSR Pollers
    
    Phase 6. User interface

    Define and implement
    
    Phase 7 - To market

    Select, prepare and issue 'freeware'

    Phase 8. - SNMP full integration

    Include MIB compilers and walker
    Define and implement EMU interface to same

    Phase 9. Applications

    Integrate bridge functions

    Phase 10 - Locator

    Define and implement node locator
    Define and implement nettrace


    Phase 11. Multiple servers.

    Inaugurate EMU to EMU connectivity and shared databases.

    Detailed plan.

    Phase 1.

    In as much as we have a working system that does some useful work,
    this phase is considered done. 

    Phase 2.
    This is completion of the base system - termed the core. The end
    of this phase is when we can:
    Load all existing PSR databases with verifiable data
    Relate all addresses together correctly
    See name to address (and reverse) translation
    Display database contents using defined mechanisms.
    Ageout (delete) addresses no longer in use.

    Essentially what we have is a verifiably correct database at any
    time regardless of network conditions - addresses changing,
    appearing or disappearing, names changing etc.

    PSR notes.
    A fundamental difference between V4 and V5 is that the PSR
    database was all that existed or was intended. In V5 we are moving
    to an external (RDB) database and this has allowed a number of
    desirable features:
    PSR database now contains ONLY that data that we can receive
    gratuitously from the network. That is, it is broadcast
    regularly on some network. This is not to say the EMU would always
    receive this directly - nodes that are not on the LAN we are
    attached to will broadcast info and we will find them via
    WANPROBE. None the less, when this info is found it is defined as
    'broadcast data' and stored as such  in the PSR database. 
    PSR headers now contain 1st heard and last heard times. That is
    the absolute time we 1st heard from this node and the last time we
    heard (confirmed the protocol still active). The PSRs are
    individually responsible for ageing out any address not heard from
    for some time. This has to be protocol and node specific. Each
    node is flagged with the number of seconds between expected
    updates. In the case of directly connected (same LAN) this will be
    set by the node's broadcast timer on that protocol. Some examples
    are:
    Vaxes broadcast MOP sysid every 16 min
    DECservers broadcast MOP every 8 min
    DECnet (phase IV) has a settable hello timer
    So does LAT.
    IP routers send RIP, OSPF or IGRP on some regular basis
    IPX Broadcasts regularly 
    And so on.
    In these cases we simply determine and store the timer values as
    part of the PSR database. Note this is an exception to the rule above; 
    in some cases we will have to query the node to get the value.

    In the case where the node is not directly connected the timer is
    set by the interval we poll the node (or router) at.

    In either case we take the 'next expected interval' and if we
    haven't heard, or contacted the node in (user settable, def = 3x) 
    interval, the address is deleted and the relater is informed if
    this is the only address on that protocol for this BOXID. Note
    that if multiple addresses on this protocol exist and other
    addresses still respond, there is no relater change. 
    The relater then does a reverse (unrelate) function by deleting
    references to this protocol from all addresses with this BOXID and
    finally, the relater deletes the record that sent the message.
     
    Display. What is wanted at this stage is the definition and
    implementation of the so called 'map record'. In whatever form,
    once this is available, an internal 'dump' type routine to: 1.
    test the mechanism and 2. view the contents according to design
    should be written. Note this routine is not intended for release -
    it is simply a development tool.

    NAMER. Currently we have an Ethernet to name database. This is
    nowhere near adequate for the intended purpose. 
    Some enhancements:
    Multiple keys necessary to allow lookups via BOXID, Protocol
    specific address and further key when single protocol has multiple
    names. 
    Name type - or similar. In the 1st instance we want node names. In
    near future we may want to add any name to address xlation such as
    LAT service names, name server locations etc. Essentially any
    xlation of a user provided name to location should be allowed.
    Validation that any name accepted for storage contains only
    printable characters. This is a real problem in current
    implementation.
    Reverse xlate. Most lookups will provide an address, boxid or
    similar to retrieve the name(s). The reverse should also be true.
    User inputs name to find any boxes matching.

    Relater. This is defined and 1st part implemented. A discussion on
    finer points to complete the definition is necessary and an
    implementation.

    Phase 3.
    System management is simply our ability to run routines that
    display the system operation. Briefly if you think of what VMS
    provides via SDA and Monitor the idea is more or less complete.
    Necessary inclusions:
    For each processor (not a definite term):
    Count of messages received
    Count of messages sent
    Count of messages with format errors
    Count of messages ignored (Intended)
    Count of non fatal errors encountered.
    In the case where the processor allocates mem:
    Initial allocation
    Current utilisation

    It is thought that this is a good stage to formalise and implement
    this as it will become crucial as we add to the system to see the
    effects. The data collected has 2 purposes: Allows us (and
    possibly users) to see in simple terms how well the system is
    using resources and 2. allows the possibility of automated system
    tuning. Note that to be truly effective each counter held must be
    precisely defined and utilised.

    Comprehesive Display mechanism. This is the search engine and
    needs to accommodate (within some reasonable limits) very complex
    queries. At this point only the interface is needed. It is upon
    this that applications will be developed. Note that the query
    mechanism will include RDB access.

    Alert mechanism. Finish definition and early implementation. Only
    a basic working system is required; PSRs will detect alertable
    events, send alerts with limited number of params and a display
    receives, formats and prints the result. Note this relies on the
    Datatyping mechanism to have been finished.

    Formal documentation. This is required and at this stage the
    system is complex and developed enough to warrant the effort. Most
    importantly, there are multiple persons contributing to this and
    all hands have need to understand the entire system. It has been
    decided to VAX document the words and I will undertake to produce
    the doc with input from the developers. Format is essentially
    copied from DEC (for a really boring but excellent example read
    System Services reference)

    Phase 4.

    PSR expansion. It has been planned for some time to expand the
    ability of some PSRs. This is in line with the principle that EMU
    extracts the maximum information it can by simply listening to the
    net. Planned:
    DN4: Decode routing updates to detect other nodes that are part of
    the net but not on this LAN.
    IP: Decode RIP, OSPF, IGRP for the same purpose.
    DN5: Decoding of routing update is done. Implement ability to add
    nodes to the PSR database (probably via LocateRecord).
    There are other minor changes planned - detail to be supplied
    closer to time.

    Counter processing. The theory is essentially proven in EMU V. We
    can determine normal operation of any node in any area that
    provides a consistently available counter. This version has major
    expansion in 2 areas: Counters are now 64 bits wide - it
    simplifies the implementation if all counters are the same width.
    Both DN5 and soon SNMP will provide 64 bit counters and it make
    sense to convert all others to this format as part of the
    gathering process. Previously we only monitored MOP counts. In
    this version any counter or set of counters is included. 
    The process is in 3 parts:
    Routine request and receive counters via specific protocol.
    Routine to check the counts for validity and format them into frames 
    suitable for input to the processor.
    To format:
    Determine the difference in time since last sample, subtracts the 
    new samples from the previous, converts 'ratio' counts to ratios
    and passes each count to the processor. Store current counts and
    time.
    The processor stores (for each counter) the previous x samples,
    calculates the average over both short (limited by number of
    previous samples stored) and long term, determines normal range
    of this counter and determines if this is outside normal range.
    Alerts if so.
    Essentially this is a total re-write. While much of the idea and
    even algorithm remains unchanged, the changes are fundamental and
    is probably best to re-write (in C).

    WANPROBE. Rewrite the existing module stripping out some functions
    for simplicity and modularity reasons. There is on purpose to this
    module - probe the wide are networks with all probes we have and
    find other nodes that exist but do not appear on our LAN. Add new
    nodes to the database. The current module does this and (too) much
    more.  

    SETFUNC. Probes all nodes using appropriate protocols and
    determines the management functions the node supports. Indicates
    this in the Relater record. Later modules make use of this is
    gathering more info and monitoring. 

    Phase 5. Database.

    This is the RDB (or similar) database required for expansion of
    EMU. In simple terms, the RDB contains parameter data we can poll
    for and receive off the network. Each PSR will define it's own set
    of relations and tables with the BOXID as the main foreign key.
    Secondary (and Tertiary) keys to be defined as necessary. Some
    examples to scope this:
    A DECnet phase IV node has something like 300 - 700 unique params
    divided into  about 8 tables
    DECnet V is closer to 1000 params in around 10 tables. Most of the
    10 tables have at least 4 'subtables'
    A generic SNMP node will have 4 (ish) tables containing something
    like 100 - 200 params.
    A bridge will have 2 tables and 25 (ish) params.
    Most of the rest (LAT, MOP, SCS ,IPX are small)
    A multiport router will be combinations of the above. For example,
    a cisco router implementing DECnet, IP and bridging will have all
    of the above.
    There are 3 main parts to this phase:
    Database definition - Define the tables, relationships and
    parameters for each section (PSR). I expect this will fall to me
    and it is proposed to use RDO type defs and RDB as the database
    for the simple reason that it is available and native to VMS. It
    is assumed that if as and when we move to other operating systems
    that a relatively simple conversion would take place.

    EMU to RDB interface. A mechanism to allow EMU routines to
    write/query the RDB. I have no thoughts beyond some preliminary
    and obvious ideas at this time.

    Pollers. EMU routines that acquire the data from the net, format 
    it suitably for the RDB and passing it in via the interface.

    The current state of the above is quite simple. 
    RDB Definitions
    Much of DECnet IV is ready for the interface. 
    Other than that it is , with some exceptions,
    starting from scratch. A complete suite is not necessary to be
    presentable.
    EMU to RDB interface. Just started. Ron has been working on this
    and has made some preliminary progress. Finer design and
    implementation is next
    Pollers. Maybe 60 - 80% done:
    Decnet phase IV is complete
    Decnet Phase V is well under way
    SNMP is well under way
    The rest are small.

    Phase 6. - User Interface

    We have decided to explore Intranet - that is EMU is a web server
    accessible from any standard web browser. Ron has located server
    software for AXP VMS and soon we should be able to determine it's
    suitability. Assuming all is well a high level design defining the
    menus, processes and links will be needed followed by a design and
    implementation. I will undertake the high level bit to be used as
    discussion doc. It is this change in direction that allows us to 
    revive EMU as a viable seller.

    Phase 7.
    At this point we can market EMU. Not having much of either
    experience or success here, I leave the discussion to those more
    capable. I would like however to make sure we get on the DEC ( and
    possibly other's) freeware disks with:
    EMU V4.3  - we should release this and give it away. It is a good
    prototype for EMU 5 and (I expect) would attract some attention.
    MIB tools. I have developed and got working (better than hoped) a
    suite of tools that compile MIBs and take the results into a menu
    driven application (MIBWALKER) that queries SNMP nodes and
    formats the results. These are standard tools used in this area
    and this version has some very nice enhancements (automatic help
    files, logical ordering of tables, etc) and as far as I know the
    only ones available for VMS. 

    Phase 8.
    Essentially this is what the MIB tools were developed for. Allow
    the user to take any MIB (MIBs are the definitions of what param
    and counter data is available from any specific hardware, the
    codes to get it and the format it returns in), compile it into EMU
    and thus EMU can monitor ANY SNMP device without further code
    change. With the enhancements above, this provides what is
    considered 'standard' tools but in our case is better.

    Phase 9. Applications

    These are endless, and is where EMU can shine. Applications in
    this context, analyse data we have (or can get) and produce
    network specific monitors, reports and such. 
    Examples that exist:
    Bridge monitor that (via SNMP) locates documents and monitors an
    unlimited number of bridges in unlimited Spanning trees. No other
    tool I know of can monitor in multiple spanning trees.
    DECHUB900 monitor - Locates, documents and monitors these devices 
    Examples that should exist.
    Analyser for each PSR database that compares parameters on the
    nodes with each other and recommended settings producing a report
    showing bad settings and such.

    Phase 10. Locator

    In a word, physically locate each node on the network and record
    it. This is very difficult but possible. Some work on this done.
    Nettrace - Ability to trace the path any transmission follows and
    show each device it transverses. Again - very difficult but
    possible.  The combination of these two facilities working would
    sell many EMU copies by themselves.

    Phase 11. Multiple EMU servers

    This is the ability to have many servers that cooperate in all of
    the above, share data and responsibilities. There has been much
    talk about this and am inclined to leave detailed discussion until
    a later date. The only comment now is that this is required
    functionality and should be planned for during the implementation.
     




         

      

