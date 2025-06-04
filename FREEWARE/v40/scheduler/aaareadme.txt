How to Win the CPU Battle with Class Scheduling for OpenVMS

This article is based on session VS059 presented at DECUS '95 in San
Francisco and has also been published in the Canadian DECUS magazine,
DECUScope.

Abstract

CPU class scheduling has long been available on IBM operating systems. This
capability was implemented as of OpenVMS V6.0 within the OpenVMS scheduler
although it has not received the amount of attention that it deserves. Hooks
are available for customers to write their own CPU class scheduling policies. 
This article discusses some potential uses of class scheduling and the callouts
available to system programmers for writing a class scheduler policy.

About the author

Curtis Rempel is an OpenVMS systems programmer.  He has managed OpenVMS systems
for over 12 years and specializes in OpenVMS internals and system performance
issues.


                                     ---


Imagine for a moment the following scenario. You arrive at the office and your
phone is already ringing. You answer it and discover that the caller is another
user calling to complain about poor response time.   The boss has been on your
case to "fix it" for what seems like an eternity and now he is standing in
front of your desk leading an angry mob of users.  You've got a problem.  What
do you do?  Tempting though it might be, hiding under your desk and calling 911
is not the solution.  Then you suddenly remember reading something somewhere
about CPU class scheduling and wonder if it might be able to calm the natives
and reduce the amount of coffee you've been drinking lately.

Fact or fiction?  Perhaps you have actually experienced this situation
yourself. Is your system response time less than adequate?  Are your users
constantly complaining that the system is slow?  Is productivity going down the
drain?  If you answered "yes" to any of these questions, read on.  Help may be
available.

First, let's take a look at some of the common potential causes of poor system
response time.

Incompatible job mix:

The jobs you run on your system may be competing against each other for
resources.  One classic example of an incompatible job mix is production versus
development work.  Production jobs need to be completed within a specified
time, meanwhile, your developers are banging the heck out of the CPU debugging
their code.  Development work typically starts with "make it work" and then
"make it efficient".  It's the "make it work" part consisting of repeated
compilation and unoptimized code that eats away at your CPU resources in a
hurry. Obviously this behavior is going to have an affect on your production
work.

Another example of an incompatible job mix is keystroke sensitive applications
versus compute bound jobs. Are you running timing sensitive applications like
editors and data entry/update forms which require quick cursor response while
at the same time you have number crunchers, report generators, and other
compute bound jobs running?  Setting different base priorities can help to a
degree but as we'll see later, this doesn't always work.  You can sometimes
fool users into perceiving snappier keyboard response by playing around with
quantum but there are tradeoffs and we'll discuss this later as well.

Insufficient CPU capacity

You workload may simply be too much for the CPU to properly handle.  Have you
outgrown your CPU capacity?  If you have, class scheduling probably will not be
of any value to you.  Useful tools like Digital's POLYCENTER Performance
Advisor can help you determine if you are in fact out of capacity.


Now let's take a look at some possible solutions to the problem.


Priority Based Scheduling

The most common method of priority based scheduling used on OpenVMS systems is
"interactive" and "batch" priority.  Interactive priority is typically set at
the default base value of 4 while batch priority is lower, and varies in value
depending on the value of PRIORITY_OFFSET in SYSGEN and the number and types of
batch queues that you have.

With priority based scheduling, higher priority jobs will pre-empt lower
priority jobs. While this may seem to be adequate, batch jobs tend to take up
the "slack" CPU time, which can be as much as 90% or more if you have only a
few small activity interactive keystroke type jobs.  The problem here is that
although the interactive jobs will pre-empt the lower priority batch jobs, the
system is perceived by the users to be sluggish.  Adjusting quantum to favor
interactive users may be all that is required to satisfy your users. There are
caveats however.  More on this later.  


Job Mix Realignment

Job mix realignment involves separating jobs on separate CPU's (i.e.
development machines & production machines). Note that there are some potential
costs involved and include such things as additional hardware, licenses,
administration, and support.   This solution is optimal if you can afford the
extra costs involved.


Working Hours Realignment

Altering the working hours of your employees is another possible solution,
however, you can count on it being met with varying degrees of opposition. What
impact would a different work schedule have on your customers?  Could they get
the information/goods they need when they need it?  Other costs involved with
changing the work schedule could include building expenses (utility costs),
employee productivity and morale, and of course, maintaining contact with the
other vendors that you may depend upon.  If your particular application is not
affected by these variables, simply distributing your workload throughout the
day might be all that is required to solve the problem.


Adjusting CPU Quantum

Quantum, simply stated, is the CPU "time slice" that you are allocated before
you must give up control of the CPU.  The default value for quantum is normally
set at 200 ms.  Jobs may voluntarily or involuntarily give up control of the
CPU before quantum end due to such events as waiting for I/O to complete or
because a higher priority process has become computable.

Process quantum is controlled by the SYSGEN parameter QUANTUM. A short quantum
favors interactive response time while long quantum favors batch job
throughput. 

How can you compromise on the value of quantum? One way is to have two values
for quantum, a short quantum for prime time jobs (interactive users) and a long
quantum for non-prime time jobs (batch jobs).  A recurring batch job could be
used to lower and raise quantum automatically since it is a dynamic parameter
(it does not require a system reboot to take effect).  Note that only processes
created after quantum is changed will receive the new quantum value.

There is of course a trade-off with lowering quantum.  Jobs execute for shorter
periods of time before being pre-empted by quantum end processing which adds a
certain degree of overhead.  You end up with extra overhead (primarily observed
through a higher level of interrupt stack activity) but the system appears to
be snappier to interactive keystroke applications although you are really not
getting any more work done.


Finally, let's look at how CPU class scheduling may be of help to us.

With class scheduling, CPU time is allocated to job "classes" at regular
intervals instead of individual processes. Processes in the same class share
the CPU time allocated to the class which means that CPU bound jobs can impact
"normal" jobs if grouped into the sames class.  Thus, it is important to give
some thought to how you want to group jobs into scheduling classes. Each class
cannot exceed the CPU time allocated to that class by the scheduler policy.  Be
careful not confuse this CPU time limit with with the AUTHORIZE /CPUTIME
qualifier which limits the amount of process CPU time per session.
 
Implementing class scheduling typically costs next to nothing (some programming
is required, your mileage may vary). It is important to remember that class
scheduling does NOT give you increased capacity but rather it can help to you
better manage and control your CPU resources and possibly prevent workloads
from interfering with each other. In the course of implementation, you may find
that the productivity of some workloads is actually lowered by class
scheduling. Thus, the actual implementation becomes a process of making an
initial estimate of job classes and assigning processes to those classes,
measuring and evaluating the results, and repeating the procedure again until
you are satisfied.


Defining scheduling classes

The algorithm for grouping jobs into scheduling classes is up to you. You may
wish to consider grouping jobs by workload (developers versus production), by
work group (sales, accounting, etc.), or by application type. Any other way you
can think of to distinguish separate job classes according to your particular
requirements is your decision.  However, be sure to assign similar jobs to the
same scheduling class to prevent one or more jobs from monopolizing the class
CPU time.

Some sample job class definitions are shown in Figure 1, 2, and 3.


Implementation

Class scheduling is implemented under OpenVMS through the use of an executive
loadable image.

By default, this image is not loaded when OpenVMS boots.  To make it available,
use SYSMAN and then reboot:

  $ MCR SYSMAN SYS_LOADABLE ADD OPENVMS CLASS_SCHEDULER
  $ @SYS$UPDATE:VMS$SYSTEM_IMAGES
  $ @SYS$SYSTEM:SHUTDOWN

Once you have the class scheduler execlet loaded, you need to write a program
which calls the SYS$SCHED system service.


Overview of SYS$SCHED

Format: SYS$SCHED (func,p1,p2,p3)

    func = function code specifying action to perform
    p1, p2, p3 = arguments depending on the function code specified

Privileges required:

  ALTPRI         required to affect processes
  GROUP or WORLD required to affect processes in the same UIC group or in 
                 different UIC groups
  SYSPRV         required to set the timeout value


SYS$SCHED Function Codes

CSH$_READ_ALL - Returns a buffer containing information for all processes

CSH$_READ_NEW - Returns a buffer containing information for all processes that
have not been placed into a scheduling class

CSH$_READ_QUANT - Returns a buffer containing information about how many ticks
are left for each class

CSH$_SET_CLASS - Places processes in classes with or without windfall
capability

CSH$_SET_NEW - Indicates to the class scheduler that the next READ_NEW will
return information about the calling process

CSH$_SET_QUANT - Establishes class quantum and enables class scheduling

CSH$_SET_TIMEOUT - Establishes a nonstandard timeout


A Real Life Situation ...

To cut costs, management insists that developers and production users share the
same machine (a recipe for disaster).  Thus, two different workloads are
competing for CPU time and interfering with each other.  An upgrade to Alpha in
the next budget year is planned.  Until then a solution must be found.

Goal #1: Minimize the impact on production users
Goal #2: Solution must be quick and cost little or nothing 
Goal #3: Get the users and the boss off your back (you might want to make
         this goal #1).


How was this problem solved? 

Ten job classes were defined: CLASS_0 through CLASS_9 where CLASS_1 was used to
indicate a class with maximum 10% CPU time so on up to CLASS_9 with a 90% CPU
time limit.  CLASS_0 was reserved for jobs that should not be class scheduled.

The "account" field in SYSUAF.DAT was used to group users into two of these
classes.  For production users, the production job UAF records were modified to
have an account field of CLASS_0 (not class scheduled).  For developers,
development job UAF records were modified to have an account field of CLASS_2
(20% CPU). Using the account field is an easy method of grouping jobs into
classes since the account field is one of the items returned by SYS$SCHED using
the CSH$_READ_ALL and CSH$_READ_NEW function codes.  By using an account field
of CLASS_0 and CLASS_2, the scheduler policy code is able to distinguish
between the two different job classes.

The scheduling interval was then defined: 0800 - 1700 M-F (prime).

The class scheduler policy program was then written in FORTRAN, however it
could have been written in any high level language such as C such as the
example (SYS$EXAMPLES:CLASS.C) provided by Digital. FORTRAN was readily
available and some shops with "number crunchers" have FORTRAN compilers and so
this code will be easily to implement.

The class scheduler was started at the beginning of the scheduling interval and
stopped at the end of the scheduling interval using POLYCENTER Scheduler (a
recurring batch job would have worked just as well).  In this case, we weren't
concerned about about non-prime response time (no class scheduling).


Test Results

To demonstrate the effectiveness of the class scheduler, the following test
environment was configured on an idle system: A user session (BIGJOB1) was
created to run a compute bound job in the 20% CPU class:

   10     GO TO 10
    
          END
                                                       
A second session was used to run 

   $ MONITOR PROCESS /TOPCPU

A third session was used to run the class scheduler program.

The compute bound job was run with class scheduling first disabled (figure 4)
and as expected, it proceeded to consume all available CPU time.

Next, the class scheduler program was activated (figure 5) and it was observed
that the total CPU consumption by BIGJOB1 was reduced to 20% despite the fact
that no other jobs were executing, thus demonstrating that the 20% CPU class
was in effect.

A second compute bound job (BIGJOB2) was then run along with the first job in
the 20% CPU class (figure 6) to demonstrate the sharing of CPU time within the
class.  BIGJOB1 and BIGJOB2 each received 10% of the total CPU time.


Figure 4 - Class Scheduling Disabled - 1 Job

                            OpenVMS Monitor Utility 
                             TOP CPU TIME PROCESSES 
                                 on node PROD1 
                            27-OCT-1995 16:22:53.01 
 
                                     0         25        50        75       100 
                                     + - - - - + - - - - + - - - - + - - - - -+ 
 23C000E8  BIGJOB1                99  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 
                                     |         |         |         |          | 
 23C0006F  DECW$TE_006F            1 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     + - - - - + - - - - + - - - - + - - - - -+ 



Figure 5 - Class Scheduling Enabled - 1 Job

                            OpenVMS Monitor Utility 
                             TOP CPU TIME PROCESSES 
                                 on node PROD1 
                            27-OCT-1995 16:24:02.02 
 
                                     0         25        50        75       100 
                                     + - - - - + - - - - + - - - - + - - - - -+ 
 23C000E8  BIGJOB1                20  XXXXXXXX 
                                     |         |         |         |          | 
 23C0006F  DECW$TE_006F            1 
                                     |         |         |         |          | 
              
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     + - - - - + - - - - + - - - - + - - - - -+



Figure 6 - Class Scheduling Enabled - 2 Jobs

                            OpenVMS Monitor Utility 
                             TOP CPU TIME PROCESSES 
                                 on node PROD1 
                            27-OCT-1995 16:26:37.76 
 
                                     0         25        50        75       100 
                                     + - - - - + - - - - + - - - - + - - - - -+ 
 23C000E8  BIGJOB1                10  XXXX 
                                     |         |         |         |          | 
 23C00060  BIGJOB2                10  XXXX
                                     |         |         |         |          | 
 23C0006F  DECW$TE_006F            1 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     |         |         |         |          | 
 
                                     + - - - - + - - - - + - - - - + - - - - -+


Conclusion

CPU class scheduling can help you better manage your CPU resources and possibly
allow you to defer a CPU upgrade but it will NOT give you more capacity.
Implementation is relatively easy and cost effective. With class scheduling,
your phone may finally stop ringing and the boss will quit hassling you (ya,
right).


Questions?

Email: vmsguy@home.com
       curtis.rempel@shaw.ca
 
