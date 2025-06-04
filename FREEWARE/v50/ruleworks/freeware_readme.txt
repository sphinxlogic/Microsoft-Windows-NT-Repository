RULEWORKS, LANGUAGES, RuleWorks Language Environment

RuleWorks is a language, compiler (with debugger), and Run-Time Library for
constructing high performance, modular, object-oriented, forward-chaining,
rules-based applications.  (RuleWorks is an evolutionary step past OPS5.)
It provides portability by producing ANSI C sources which can then be 
compiled with any of the leading C or C++ compilers. There is a Run-Time 
Library for each supported platform and C compiler.

Prerequisites

Before building RuleWorks in an OpenVMS environment, be certain that you have 
the following software already installed:

     OpenVMS Alpha V6.2 or greater or OpenVMS VAX V6.0 or greater 
     DEC C V4.0 or greater or VAX C V3.2

Kit Format:

     RULSVMS.ZIP - All RuleWorks sources and build script. 
     RULKAVMS.ZIP - RuleWorks compiler, run-time library and examples. (Alpha) 
     RULKVVMS.ZIP - RuleWorks compiler, run-time library and examples. (VAX) 

Extracting the source files will create a directory structure as follows: 

 []           - Contains The General Public License (HTML format)
 [.BIN]       - Contains compiler and Help file when build process is complete.
 [.BUILD]     - Contains build file and intermediate files during build process.
 [.EXAMPLES]  - Contains example files.
 [.INCLUDE]   - Contains header files.
 [.LIBRARY]   - Contains run-time library when build process is complete.
 [.SOURCE]    - Contains sources required for build.

To build, unzip the source kit and invoke: @RULE_VMS.COM KIT 


