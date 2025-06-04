PQM, Utilities, Print Queue Manager for Users and Operators

       Written by Jeremy Begg, VSM Software Services Pty Ltd.
          Copyright © 2000, 2001 VSM Software Development.

PQM is a screen-based utility which presents a user-friendly interface
to the OpenVMS print queue system.  It is designed for ease-of-use by
users and operators who do not wish to learn, or have no direct access
to, the DCL commands which are relevant to printer queues.

It is intended for use in environments where there is a heavy printer
load (many queues and/or many jobs) and individual users wish to have
some control over their jobs and the queues those jobs are processed
in.

PQM does not attempt to display or control batch queues.  Allowing
inexperienced users to manipulate batch queues can easily lead to
major application problems and therefore most sites find that this is
best left to those who know and understand the relevant DCL commands.

Release Notes, Documentation, and a command procedure for building PQM
can be found in the [.SRC] directory.
