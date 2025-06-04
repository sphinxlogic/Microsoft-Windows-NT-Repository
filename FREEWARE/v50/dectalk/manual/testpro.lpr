
 
















           DECtalk Application Development Guide
           User Manual


           Order Number: EY-6727E-PR-0001






           This manual describes how to develop applications
           using the DECtalk speech synthesizer.





                                         This is a new manual.           Revision/Update Information:           Revision/Update Information:           Revision/Update Information:













           digital equipment corporation maynard, massachusetts           digital equipment corporation maynard, massachusetts           digital equipment corporation maynard, massachusetts
 




           ________________________
           February, 1987           February, 1987           February, 1987

           __________
           The information in this document is subject to change
           without notice and should not be construed as a
           commitment by Digital Equipment Corporation. Digital
           Equipment Corporation assumes no responsibility for
           any errors that may appear in this document.

           The software described in this document is furnished
           under a license and may be used or copied only in
           accordance with the terms of such license.

           No responsibility is assumed for the use or
           reliability of software on equipment that is not
           supplied by Digital Equipment Corporation or its
           affiliated companies.

           __________
           Copyright   1987 by Digital Equipment Corporation                     c                     c                     c

           All Rights Reserved.
           Printed in U.S.A.

           __________
           The postpaid READER'S COMMENTS form on the last page
           of this document requests the user's critical
           evaluation to assist in preparing future
           documentation.

           The following are trademarks of Digital Equipment
           Corporation:

           DEC             DIBOL         UNIBUS
           DEC/CMS         EduSystem     VAX
           DEC/MMS         IAS           VAXcluster
           DECnet          MASSBUS       VMS
           DECsystem-10    PDP           VT
           DECSYSTEM-20    PDT
           DECUS           RSTS
           This document was prepared using VAX DOCUMENT, Version                                         DIGITAL                                         DIGITAL                                         DIGITAL           DECwriter       RSX
           T1.0
 







           Document Structure

           The DECtalk Application Development Guide describes
           the development of applications using the DECtalk
           speech synthesizer.

           It consists of two volumes:

              User's Manual
              Sample Application Programs

           1. The first volume,              , provides relevant                                User's Manual                                User's Manual                                User's Manual
              information for application development in the
              areas of human interface, linguistics, and
              telephonics.

              o  Chapter 1, The Human Interface in Applications,
                 offers suggestions on how to create and format
                 menus for a telephone-based application.

              o  Chapter 2, Tuning the Text, offers suggestions
                 on how to make a segment of text more
                 intelligible and pleasant-sounding for the
                 end-user.

                    NOTE
                    Chapter 2 assumes that the program is
                    fully debugged and is working as it
                    should, but that the spoken (DECtalk)
                    output may have some mispronunciation or
                    may sound a bit unnatural in places.

              o  Chapter 3, DECtalk and the Telephone, focuses on
                 the telephone as an integral part of the
                 application.

              o  Chapter 4, The DECtalk Demonstration Program,
                 describes the features of the sample DECtalk
                 demonstration application program, and describes
                 how to run the VMSINSTAL procedure to install
 


                                                   Preface     vii




                 the software, how to run the program, and how to
                 terminate the program.

                    NOTE
                    If you are responsible for installing the
                    DECtalk demonstration program, you should
                    read Chapter 4 first.

              o  The Glossary defines important terms used in
                 Chapters 1 through 4.

              o  Appendix A provides details on Telephonics
                 Specifications.

              o  Appendix B describes the VAX/VMS Run-Time Library
                 Support for DECtalk, including restrictions of
                 Version 4.5 support.

              o  Appendix C provides four tables that can be used
                 to determine the number of telephone lines
                 required.

              o  The Index provides a useful tool for accessing
                 information in Chapters 1 through 4.

           2. The second volume,                            ,                                 Sample Application Programs                                 Sample Application Programs                                 Sample Application Programs
              provides the source listings of the VAX COBOL, VAX
              BASIC, and VAX C implementations of the sample
              DECtalk demonstration application program described
              in the User Manual.
 


           viii     Preface




           Intended Audience

           This guide is intended for applications developers,
           programmers, and systems analysts who develop
           applications using the DECtalk speech synthesizer.
           While it is primarily geared toward the Multi-line
           DECtalk (DTC03), it can also be used for applications
           using the Single-line DECtalk (DTC01).

           The basic demonstration program can be applied to both
           Multi-line and Single-line DECtalk, except as noted in
           the source code.


           Associated Documents

           The reader must be familiar with:

           o                                             or              _______ _____ _______ ______ _____________              DECtalk DTC01 Owner's Manual (EK-DTC01-OM)

           o              _______ _____ ______________ ______ _______ ______              DECtalk DTC03 Text-to-Speech System Owner's Manual
              _____________              (EK-DTC03-OM)

           Refer also to:

           o              _______ _____ ________ ______ ____________ ______              DECtalk Voice Response System Installation Manual
              _____________              (EK-DVRSA-IN)

           o              _______ _____ __________ _________ ______              DECtalk DTC01 Programmer Reference Manual
              _____________              (EK-DTC01-RM)

           o              _______ _____ ____________ ______ _____________              DECtalk DTC01 Installation Manual (EK-DTC01-IN)

           o              _______ _____ ____________ _________ ____              DECtalk DTC01 Programmer's Reference Card
              _____________              (EK-DTC01-RC)

           o              _______ _____ ______________ ______ ____________              DECtalk DTC03 Text-to-Speech System Installation
              ______ _____________              Manual (EK-DTC03-IN)

           o              _______ _____ ______________ ______ ______ _______              DECtalk DTC03 Text-to-Speech System Pocket Service
              _____ _____________              Guide (EK-DTC03-PS)
 


                                                    Preface     ix




           o              _______ _____ ______________ ______ ____________              DECtalk DTC03 Text-to-Speech System Programmer's
              _________ ____ _____________              Reference Card (EK-DTC03-RC)

           o              _______ ________ _______ ________ _________ ______              VAX/VMS Run-Time Library Routines Reference Manual
              ____________ ________ __________ ___________              (AD-Z502B-TI QLZ55-GZ AA-502B-TE AA-505B-TE)


           Introduction

           Voice technology is a new technology. It is a
           technology without precedent both inside or outside
           the computer industry. The DECtalk speech synthesizer
           represents state-of-the-art voice output technology.

           The value of speech synthesis as a voice output
           technology is its flexibility. Speech synthesis allows
           the reading of unlimited ASCII text. More importantly,
           it operates in          . Thus it does not require                          real time                          real time                          real time
           voice storage, or recording of speech, and does not
           depend on the availability of particular speakers.
           Moreover, there are no delays in generating new
           output; the modification of a spoken segment of text
           is as easy as modification of the file itself.

           The engineering of a speech synthesizer requires
           expertise in diverse fields: software, hardware,
           acoustic phonetics, and general linguistics. This
           broad range of fields is also reflected (although to a
           lesser extent) in writing applications programs and in
           developing general applications.

           Writing an application for a new and complex
           technology is not always trivial. Its tasks are
           occasionally non-intuitive. An application can involve
           data retrieval, real-time transactional programming,
           human factors, linguistics, and even some knowledge of
           telephony. Few application developers have this
           diverse background.
 


           x     Preface




           A successful application solves a particular problem.
           For example, data retrieval can be difficult for
           customers without terminals. However, if we turn an
           ordinary Touch-Tone telephone into a terminal, data
           from banks, airlines, weather services and the like
           become available for customers at any hour of the day
           or night. The problems are simple to state. More
           importantly, the solutions are now available.

           Let's suppose that a customer needs 24-hour access to
           data but does not have a terminal. In addition,
           suppose your company does not have sufficient
           resources to man a large number of phone lines. The
           solution is to turn the customer's telephone into a
           terminal using DECtalk's speech synthesis technology
           and appropriate, user-friendly application software.

           Let's further assume that you need to develop a
           prototype application quickly and efficiently. This
           development can be difficult and time-consuming, and,
           as stated above, requires expertise in diverse areas.
           In this case, you can use the basic application
           demonstration software and information on applications
           development contained in this guide as an aid in
           developing the application.


           Purpose

           This manual is not a hardware manual and does not
           discuss such issues as:

           o  System configuration

           o  Firmware architecture

           o  Voice quality modification.

           For an example of a configured system, refer to the
           DECtalk Voice Response System Installation Manual
           (EK-DVRSA-IN).
 


                                                    Preface     xi




           For a detailed discussion of firmware and voice
           modification, refer to the owner's manuals:

           o  DECtalk DTC01 Owner's Manual (EK-DTC01-OM)

           o  DECtalk DTC01 Programmer Reference Manual
              (EK-DTC01-RM)

           o  DECtalk DTC03 Text-to-Speech System Owner's Manual
              (EK-DTC03-OM)

           References are made in this guide to the appropriate
           sections of the DECtalk Owner's Manuals.

           This guide is geared to a specific task: the
           development of an application using the DECtalk speech
           synthesizer. As applications vary over a wide range of
           businesses, this guide does not propose the precise
           shape of an application. Rather, it provides two
           solutions:

           1. It provides sample software that serves as a model
              for a specific application.

           2. It provides relevant information for applications
              development in those specific areas that are
              particularly troublesome for the average
              application: human interface, linguistics, and
              telephonics.
 


                                                     Preface     1




           Preliminaries to Applications Development

           To develop applications for DECtalk, ensure that:

           o  DECtalk has been installed and correctly cabled and
              tested.

           o  Terminal line characteristics have been
              appropriately set. Host line speeds should match
              DECtalk line speeds.

           For information on testing the connection to the host
           computer, see the DECtalk DTC03 Text-to-Speech System
           Owner's Manual (Chapter 1), or the DECtalk Voice
           Response System Installation Manual.

           For information on setting DECtalk terminal line
           characteristics, see the DECtalk DTC01 Owner's Manual
           (Chapter 3) or the DECtalk DTC03 Text-to-Speech System
           Owner's Manual (Appendix B), or the DECtalk Voice
           Response System Installation Manual.

              NOTE
              DECtalk is a full-duplex serial device. When
              developing an application that uses DECtalk, the
              choice of host computer can often make the
              difference between success and failure.

              Personal computers, while popular and relatively
              easy to use, often do not provide functions,
              such as XON/XOFF flow control and receive
              buffering, that make driving full-duplex serial
              devices straightforward. Developing a DECtalk
              application for such an environment is possible,
              but very often most of the development effort is
              spent working out the problems associated with
              driving the serial line, rather than the
              problems associated with user interaction.
 





















           THE HUMAN INTERFACE IN APPLICATIONS
 


                       The Human Interface in Applications     1-3




           This chapter offers suggestions on how to create and
           format menus for a telephone-based application.

           The suggestions and descriptions offered in this chapter
           are based upon several years of experience with customer
           applications using the DECtalk speech synthesizer. Many
           of the problems described have been faced at one time or
           another by applications developers. Conversely, some ex-
           tremely successful applications have given rise to many
           of the suggestions in this chapter. The purpose of this
           chapter, therefore, is to provide ideas and suggestions
           on developing an application using DECtalk over telephone
           lines, and implementing this application in a timely man-
           ner.


           1.1  Creating Menus and Text

           One of the hardest tasks in applications development
           is putting yourself in the shoes of the end-user. You
           are probably familiar with computers. More
           importantly, you are familiar with the workings of the
           application. You know what constitutes an expected
           response within the context of the application, and
           what would be considered an anomalous one. However,
           the first-time caller may be a novice at computers,
           and unfamiliar with the application.

           To develop the text for your application, you need to de-
           velop a               . That is, you need to determine ex-                   model dialogue                   model dialogue                   model dialogue
           actly what you want the application to say, and how you
           want the user to interact with it. One way to develop a
           model dialogue is to have a natural language dialogue be-
           tween two individuals on the applications development team;
           one individual plays the role of a naive user, and a sec-
           ond individual plays the role of the application (the out-
           put of DECtalk). Spending time in such a dialogue often
           saves you valuable time later. It allows you to examine
           potential problems before they become bugs in the appli-
           cation and difficulties in the human interface.
 


           1-4     The Human Interface in Applications




           1.1.1  Introductory Segment

           Most applications need an introductory text or
           statement. Typically, this introduction is a polite
           greeting or welcome, followed by a statement
           identifying the business and application, and finally,
           some basic instructions or a menu.

           A typical introduction follows:

                This is the First National Bank of Timbuktu.
                Welcome to the Automatic Banking Service.
                Use the Touch-Tone keypad on your telephone to enter information.

                Now, please enter your ID number.

           [User enters the ID number.]

                Now, please enter your access code.

           [User enters the access code.]

                If you would like information on checking or savings, press 1.
                If you would like information on mortgages, press 2.
                If you would like other information, press 3.
                If you would like to speak with a customer service representative,
                                                     press the star key.


           1.1.2  Special Instructions

           Your introductory statement or following menus can
           contain special instructions for the user. For
           example, you may want to explain to the user that this
           service can be accessed by Touch-Tone telephones only.
           (It is assumed that advance publicity, marketing
           information or a user's manual already contains this
           information.)
 


                       The Human Interface in Applications     1-5




           You may want to provide a second telephone number for
           those with rotary-dial or pulse-dial telephones. (See
           "Use Timeouts Advantageously" in Section 1.1.3.)

           You may also remind the user to end the ID sequence or
           access code sequence with a string terminator such as
           a pound sign.

           All special instructions depend on the application
           itself, and on what other sources of information are
           available to the user. For example, some companies
           issue customers with a card (or even a brief user's
           manual) containing instructions on how to access the
           system and input information. Thus, the developer can
           keep such instructions to a minimum, and consequently
           shorten the time needed to develop the application.
           However, it is always better to have redundant
           information than insufficient information. (See "Use
           Timeouts Advantageously" in Section 1.1.3.)


           1.1.3  General Guidelines

           An important consideration when writing text and menus
           for an application is the difference between reading
           text from a screen and listening to that same text
           over the telephone. As the user may hear the text or
           menu only once, it is especially crucial to make the
           message as simple and clear as possible.


           Keep Menus Simple

           A user interacts with the application over a
           Touch-Tone telephone. As an applications writer, you
           are asking the user to perform two tasks:

           1. To understand what is being spoken.
 


           1-6     The Human Interface in Applications




           2. To press specified telephone keys in response.

           The following suggestions are based on customer and
           end-user experience:

           1. Keep all menus, especially the top level menu,
              simple and brief.

           2. Make all requests and commands clear and easy to
              understand.

           Most telephone keypads have only 12 keys. Representing
           the command set of an application can be difficult
           with such a limited number of keys. One way to avoid
           this limitation is to group similar functions into the
           same menu or sub-menu. For example, if an application
           provides information on both weather forecasts and ski
           reports, you could incorrectly structure the menus as
           follows:

                INCORRECT

                For the Boston weather forecast, press 1.
                For the Cape Cod weather forecast, press 2.
                For the Western Massachusetts weather forecast, press 3.
                For the New Hampshire and Vermont weather forecast, press 4.
                For the Stowe Mountain ski report, press 5.
                For the Mount Snow ski report, press 6.
                For the Stratton ski report, press 7.
                For the Jay Peak ski report, press 8.
                For the Smuggler's Notch ski report, press 9.
                For help, press 0.
                To exit, press the star key.

           However, the above menu provides too much information.
           By the time DECtalk speaks the fifth or sixth
           selection, the user will have forgotten the original
           choices.
 


                       The Human Interface in Applications     1-7




           Three or four choices within a given menu are
           sufficient if you allow simple menus to branch into
           other simple menus. Remember that, unlike using a
           terminal, the user typically has no access to visual
           information.

           Therefore a better menu structure for the previous
           example uses                  .                        logical branching                        logical branching                        logical branching

           o  The main menu gives the user the choice of either
              weather forecasts or ski conditions.

           o  Key 1 branches to a menu for weather forecasts.

           o  Key 2 branches to a menu for ski condition reports.

           In each menu or submenu, you should provide commands
           to offer help, and a way to exit the existing menu. As
           demonstrated below, these commands should, wherever
           possible, be associated with the same Touch-Tone key.

                MAIN MENU

                For weather forecasts, press 1.
                For ski condition reports, press 2.
                For help, press 0.
                To exit, press the star key.

           [User presses key  ]                             1                             1                             1

                WEATHER.

                For the Boston weather forecast, press 1.
                For the Cape Cod weather forecast, press 2.
                For the Western Massachusetts weather forecast, press 3.
                For the New Hampshire and Vermont weather forecast, press 4.
                For help, press 0.
                To exit, press the star key.
 


           1-8     The Human Interface in Applications




           [User presses key  ]                             2                             2                             2

                CAPE COD WEATHER FORECAST

                The following is the weather forecast for Cape Cod . . .

           As the user presses Touch-Tone keys, the key numbers
           should not be echoed back to the user. Verbal feedback
           confuses the user. Also, when you prompt for Touch-
           Tone key input, refer, if possible, to keypad        ,                                                         numbers                                                         numbers                                                         numbers
           not letters. It is difficult to quickly associate a
           letter with the appropriate Touch-Tone key.

           The text should also keep user input simple. Avoid
           requesting too long a string of numbers as an ID or
           access code number. (You should, however, strive for
           consistency in terms of security regulations and
           precedence.)

           For example, asking a user to enter a long sequence of
           numbers can be adifficult and time-consuming. On the
           other hand, a normal telephone number (with area code)
           is 10 digits long. There are many opportunities for
           creativity here. You may want to assign a string a
           length in inverse proportion to its frequency of use.
           That is, the more common the input, the fewer the keys
           that have to be pressed. In any event, the length of
           an input string should be dictated by tradition,
           logistics, and common sense.


           Be Consistent

           All instructions and text that the end-user hears
           should be consistent. That is, they should all have
           the same        and       . The more you vary the                    format     syntax                    format     syntax                    format     syntax
           format of a text, the greater the memory requirements
           you impose upon the end-user. Conversely, the more
           consistent a segment of text, the greater the
           redundancy in the message, and hence the greater the
           ease of comprehension and speed of input.
 


                       The Human Interface in Applications     1-9




           For example, examine the following menus:

                INCORRECT

                Good morning. Welcome to the First National Bank of Timbuktu.

                Press 1 for information on savings.
                Press 2 if you would like information on checking.
                If you wish to speak to a customer service representative, press 0.

                CORRECT

                Good morning. Welcome to the First National Bank of Timbuktu.

                For information on savings, press 1.
                For information on checking, press 2.
                To speak to a customer service representative, press 0.

           All the sentences in the second menu use the same
           format. From the end-user's point of view, the second
           menu is more predictable and therefore easier to
           understand.

           Also, note that research indicates that the
           conditional (the if-clause) should come before the
           command. Thus, especially on longer instructions, "If
           X, press Y" is generally better than "Press Y, if X."
           On brief instructions, either construction is fine.
           More importantly, the instructions should be
           consistent.

           You should also check for                    . For                                     lexical consistency                                     lexical consistency                                     lexical consistency
           example, the symbol   can be read as pound sign, pound                               #                               #                               #
           symbol, pound, number sign, number, hash symbol, cross
           hatch, box, gridlet, tic-tac-toe sign, and so on.
           Number (or number sign, or number key) and pound (or
           pound sign, or pound key) seem to be the most common.
           Whichever term you choose to use, keep it invariant.
           Never use "pound sign" in one menu and "number sign"
           in another.
 


           1-10     The Human Interface in Applications




           Anticipate User Errors

           Users occasionally make errors when inputting data or
           querying the system. These errors often fall into
           predictable categories. Your application should
           anticipate these problems before they occur.

           For example, when you dial a telephone number, you
           must dial 7 digits for a local call, 1 + 7 digits for
           a long distance call, and so on. Occasionally, you
           misdial and get a wrong number. Sometimes you miss a
           Touch-Tone and the call does not complete. Users face
           the same types of problems when entering information
           on Touch-Tone telephones.

           If you know the number of Touch-Tone signals a user
           will input, you can watch for input errors and correct
           the user if necessary. For example, if a user ID must
           be 6 digits long, anticipate the occasional error, and
           design your application accordingly. This
           consideration saves time, effort, and frustration on
           the part of the end-user.


           Be Polite

           Your application addresses and makes requests of a
           customer. While you can not always control aspects of
           courtesy and politeness in live transactions, you can
           ensure that the text of your application is courteous
           and polite, and in no way offensive or paternalistic.

           You should be especially careful when you request
           information or issue error messages. Avoid
           paternalistic phrases such as "Let's try again" or a
           brusque "Incorrect." Instead, use words like "Please"
           when requesting information, and "Thank you" when
           receiving it.
 


                      The Human Interface in Applications     1-11




                INCORRECT

                You pressed the wrong key. Try again.
                Incorrect. Let's try again.

                CORRECT

                Your entry was invalid. Please try again.
                Unknown access code. Please try again.


           Make Menus Repeatable

           As information load is an important factor for the
           user, you should have a repeat key facility to allow
           for the repetition of an instruction or an entire
           menu. For example:

                To have this message repeated, press 1.
                To continue, press 2.

           Alternatively, consider allowing the user to press a
           special key, for example, the star key ( ), to repeat                                                   *                                                   *                                                   *
           the last instruction.

           By using a particular, invariant key, some
           applications also enable users to scroll backwards
           until they reach the main menu.

           Some applications require the user to answer either
           "yes" or "no" to various prompts and questions
           contained in the menus.

           For example,

                Press 1 for yes, or 2 for no.
 


           1-12     The Human Interface in Applications




           If you use this command in your application, make
           certain that the user hears this command, or at least
           has access to a help key,             . Otherwise, the                                     in each menu                                     in each menu                                     in each menu
           user, especially the first-time user, may forget the
           keys for "yes" and "no" and may either enter the wrong
           information or be unable to enter any information at
           all.

                INCORRECT

                MENU 1
                Would you like information on our stock plan?
                Press 1 for yes or 2 for no.

           [User presses 2]

                MENU 2
                Would you like information on our mutual funds.
                Answer either yes or no.

                CORRECT

                MENU 1
                Would you like information on our stock plan?
                Press 1 for yes or 2 for no.

           [User presses 2]

                MENU 2
                Would you like information on our mutual funds?
                Press 1 for yes or 2 for no.


           Verify Information

           Always verify information entered by the user,
           especially when financial transactions are involved.
           For example, in cases of funds transfer or stock
           transactions, verify that the number the user entered
           was correct. To verify information, simply allow a
           DECtalk response like the following:
 


                      The Human Interface in Applications     1-13




                Please enter the number of shares of stock you want to sell.

           [User enters      ]                        4 5 #                        4 5 #                        4 5 #

                You want to sell 45 shares of stock. Is this correct?
                Press 1 for yes, or 2 for no.

           It is also useful to verify any keypress.

           For example,

                You pressed 1. The current stock price is . . .

           In some applications you might choose to verify
           information prior to a user's entry. For example, you
           might offer a verification facility whereby a user
           presses a sequence of keys followed by a special key,
           such as the star key ( ), and have the application                                 *                                 *                                 *
           verify the information.

           The following example illustrates a subtle way of
           verifying user input:

                Please enter the number of the geographical area you would like
                information on:

           [User presses        ]                         1 3 5 4                         1 3 5 4                         1 3 5 4

                You have entered the code for Atlanta, Georgia. The weather is . . .

           It is unwise to offer verification for a user at the
           same time the user is keying information. It is
           confusing to the user to enter data and listen to a
           string of digits at the same time.
 


           1-14     The Human Interface in Applications




           Give the User Another Chance

           People are accustomed to using the telephone keypad to en-
           ter telephone numbers (in the form of numeric information)
           in order to dial out. However, using the telephone key-
           pad to interact with a computer (that is, using the key-
           pad as a terminal keyboard) is a relatively new applica-
           tion for the telephone. Some first-time users may feel un-
           comfortable using the telephone in an interactive appli-
           cation. Therefore, when you are writing code for command
           menus, consider the end-user. You should offer the user
           a second chance at entering information. For example, as-
           sume that a given menu has four choices, as follows:

                For information on weather, press 1.
                For information on ski conditions, press 2.
                For information on storm alerts, press 3.
                To exit, press the star key.

           In this case, there is nowhere for the user to turn to
           ask for help. One way to offer assistance to the user
           is to include a help key.


           Include a Help Facility

           As the telephone is being used as a terminal, and as
           many users are first-time users, a help key is a
           useful function to include in the applications menu. A
           help key offers the user a way of accessing more
           information about the service or more assistance with
           the application. You may choose to have one help key
           in the main menu, or perhaps a help key in each
           sub-menu. Try to keep the help key invariant within a
           particular application.

           For example, the following application makes the
           mistake of inconsistency with the help key:
 


                      The Human Interface in Applications     1-15




                INCORRECT

                MENU 1
                For information on weather, press 1.
                For information on ski conditions, press 2.
                For information on storm alerts, press 3.
                For help, press 4.

           [User presses 3]

                MENU 2
                This is information on storm alerts.

                For information on the Atlantic Region, press 1.
                For information on the Pacific Region, press 2.
                For information on the Indian Ocean Region, press 3.
                For help, press the star key.

           In this example the user must press 4 for help in Menu
           1, and press the star key for help in Menu 2. Whatever
           your help key is, try to keep it invariant.

           Another type of help facility anticipates user
           problems and allows the user to call another number.
           For example, if the user enters the ID code
           incorrectly several times, there may be a problem.
           Instead of a menu-driven help facility, another
           possibility is to refer the user to an outside
           telephone number.

           For example:

                For assistance, please call 1-800-555-5555.

           You may also allow a user to press a special key (such
           as the 0 key) to speak with a customer service
           representative.
 


           1-16     The Human Interface in Applications




           Use Timeouts Advantageously

           Suppose that there is no input from the user for
           several seconds. How long should you wait before
           terminating the call? That is, how long are you
           willing to have the line tied up without input from
           the user?

           This issue is application-specific and, hence, there is
           no hard and fast answer. Timeouts can vary from several
           seconds to perhaps 10 or more seconds. There is a certain
           liability on both sides. Too short of a timeout may not
           give the user (especially a first-time user) sufficient
           time to make a decision, and a quick termination might frighten
           the caller from using the system for subsequent transac-
           tions. On the other hand, too long of a timeout has the
           liability of tying up a telephone line, thereby wasting
           valuable time and perhaps blocking a number of other calls
           from coming in.

           Another issue may arise when the user is calling for
           information from a rotary-dial or pulse-dial (that is,
           non-Touch-Tone) telephone. If the caller has neither a
           Touch-Tone telephone nor a Touch-Tone generating
           device (a device that generates tones over a
           pulse-dial telephone), then timeouts become
           particularly important.

           There are a number of ways you can use timeouts to
           both your advantage and the end-user's advantage:

           o  You can allow the initial time-out to be relatively
              long (until the application program hears its first
              Touch-Tone, that is, until the application is
              convinced that it is talking to a Touch-Tone
              telephone).
 


                      The Human Interface in Applications     1-17




           o  You can issue a prompt just before putting
              DECtalk's telephone back on-hook. This procedure
              costs little time but can save the user a second
              call.


           Allow More Time for Entering Relatively Complex
           Information

           The user interacts with the application by using a tele-
           phone keypad as a terminal keyboard. However, the keypad
           has only 12 keys (or 16 in the case of the military hand-
           set). In addition, people are used to entering numeric rather
           than alphabetic information (telephone numbers rather than
           words). This situation is beginning to change with the spread
           of non-numeric passwords in automated teller machines (ATMs)
           and mnemonic telephone "numbers" such as 1-800-MARKETS.
           Nevertheless, some people still have difficulty in enter-
           ing letters on the keypad. If the information required by
           your application is alphabetic and relatively complex (for
           example, the first four letters of a last name), consider
           allowing more time for the transaction, and make the er-
           ror correction and verification slightly more elaborate.


           Consider Experienced Users

           All applications must take into account first-time
           users of the system. However, many users access the
           system repeatedly and become familiar with the
           application and the way of interacting with the
           system. The end-user who is familiar with the system
           should not have to wait for all the menu commands to
           finish.

           An experienced user is not subject to the same type of
           general limitations as a first-time user. When writing
           code for an experienced user, you can reduce the time
           per transaction. Therefore, your application should
           allow the end-user to type ahead of the current menu.
           (To incorporate a type-ahead feature in your
 


           1-18     The Human Interface in Applications




           application, you must make sure that the operating
           system can buffer type-ahead characters.)

           If a type-ahead feature is being used, the user does
           not want to hear old menus. Therefore, you must have
           an immediate stop speaking facility whereby DECtalk
           ceases all spoken output when the user presses any
           key. For further information, see the implementations
           of the demonstration program provided in the second
           volume of this guide.

           Another way to maximize use of telephone time and
           minimize wait time is to allow a special key, for
           example, the pound sign key ( ), to be used as a                                        #                                        #                                        #
           command delimiter. For example, instead of having to
           wait for several different prompts for security code,
           access code, type of information required, and so
           forth, you could allow the experienced user to enter
           all three pieces of data with the pound sign key as a
           delimiter.

           To illustrate:

                Please enter your security code.
                End with a pound sign.

           [User presses            ]                         1 2 3 4 5 #                         1 2 3 4 5 #                         1 2 3 4 5 #

                Please enter your account number.
                End with a pound sign.

           [User presses          ]                         6 7 8 9 #                         6 7 8 9 #                         6 7 8 9 #

           Instead of the above, you can allow a user to enter
           the following after the first prompt:

           1 2 3 4 5 # 6 7 8 9 #           1 2 3 4 5 # 6 7 8 9 #           1 2 3 4 5 # 6 7 8 9 #

           where the pound sign key simply serves as a delimiter
           for different types of data entry.
 


                      The Human Interface in Applications     1-19




           Write a User Manual

           After writing the application text and menus, and the
           application is ready for the end-user, write a short
           user manual. A user manual is especially valuable for
           new users, and can also serve as a handy reference for
           all users. The User Manual, in the form of a card or
           several sheets of paper, can essentially outline the
           sequence of instructions for the user, and can:

           o  List the hours of availability of the system (for
              example, 24 hours a day, 7 days a week, or business
              hours: 9-5 Monday through Friday, and so on).

           o  Remind users to use a Touch-Tone telephone or,
              alternatively, to use a different telephone number
              with rotary-dial or pulse-dial telephones.

           o  Outline security procedures, including how to enter
              the access code and account number.

           o  Describe the menus.

           o  List all special keys (for example, repeat key,
              exit key, help key).

           o  Include examples of text that appears in the
              application.

           Placing this information on a small wallet-sized card
           provides a more portable reference for customers who
           are away from their office.
 


           1-20     The Human Interface in Applications




           1.2  Keypad Commands: Some Programming Issues

           DECtalk has opened up a new world in application
           development. It allows people to access, from any
           Touch-Tone telephone, information stored in computer
           databases.

           As the user presses Touch-Tone keys to enter valid
           keypad commands,

           1. The telephone sends information to DECtalk.

           2. DECtalk determines that keys have been pressed and
              sends the key codes (ASCII values of these keys) to
              the host application program.

           3. The host application program then translates the
              Touch-Tone key codes into commands that extract the
              desired information from the database.

           4. Once the information has been extracted from the
              database, the host application program sends the
              information to DECtalk to be spoken to the user.

           When designing a DECtalk application that accepts
           keypad commands, first make sure that the operating
           system can buffer type-ahead characters. When a user
           presses a Touch-Tone key, DECtalk sends the key code
           to the application program as soon as it has been
           detected (whether the application program is waiting
           for it or not). In addition to buffering type-ahead
           characters, the operating system should also respond
           to DECtalk's XOFFs.
 


                      The Human Interface in Applications     1-21




           Most application programs should perform the following
           procedures:

                              *Enable autostop keypad mode
                              *Enable wink detection
                               Loop
                                   Speak menu prompt
                                   Enable telephone keypad timeout
                                   Get a Touch-Tone key or escape sequence from DECtalk

                                   If (key)
                                        Cancel keypad timeout
                                        Send ']' to DECtalk (exit phonemic mode)
                                        Enable speaking
                                        Reset application-specific parameters (if necessary)
                                        Process key

                                   If (wink)
                                        Cancel keypad timeout
                                        Speak goodbye message to user
                                        Hang up DECtalk's telephone
                                        Exit loop

                                   If (timeout)
                                        Speak goodbye message to user
                                        Hang up DECtalk's telephone
                                        Exit loop
                               End loop

                *Note: The DTK$ANSWER_PHONE Run-Time Library routine automatically
                enables autostop keypad mode and wink detection.

           The following discussion focuses on some programming
           issues relating to the processing of keypad commands.

           1. Before DECtalk can receive any Touch-Tone signals,
              the telephone keypad must be enabled.

              The request to enable the telephone keypad is
              ignored if the telephone is inactive (on-hook).
 


           1-22     The Human Interface in Applications




              The telephone keypad can be enabled in:

              o  Keypad mode

              o  Autostop keypad mode

              In keypad mode, DECtalk returns the Touch-Tone key
              codes to the application program as the keys are
              pressed.

              In autostop keypad mode, DECtalk stops speaking
              text whenever it receives a Touch-Tone signal, and
              discards pending speech.

              Whenever possible, the telephone keypad should be
              enabled in autostop keypad mode. Frequent users of
              a system quickly become expert users and get
              irritated if they are forced to listen to long
              prompts.

              Note that, even if autostop mode is not enabled, a
              user can still press a Touch-Tone key in response
              before DECtalk completes the prompt. However,
              DECtalk will complete the prompt whether or not the
              user presses a Touch-Tone key.

           2. Once keypad mode or autostop keypad mode is
              enabled, wink detection should be enabled on the
              telephone line.

              On most telephone exchanges, one or more winks are
              generated when a caller hangs up. Using wink
              detection, an application program can often predict
              when a user at the other end of the telephone has
              hung up. However, a timeout must also be used to
              deal with situations where the wink is not
              available. Using timeouts on telephone keypad input
              guarantees that the application program will not
              remain off-hook indefinitely if a user fails to
              enter Touch-Tone key input.

           3. Telephone keypad timeouts should be enabled after
              DECtalk speaks the prompt for Touch-Tone key input.
 


                      The Human Interface in Applications     1-23




              When you specify a timeout value, assume that data
              entry can be slow.

              Once the telephone keypad timeout is enabled, the
              application program should wait for the user to
              press a Touch-Tone key, or for DECtalk to report
              some other event.

           4. Once the application program receives a Touch-Tone
              key code (in either keypad mode or autostop keypad
              mode), the keypad timeout should be canceled.

           5. If the keypad is enabled in autostop keypad mode,
              DECtalk may stop speaking in the middle of phonemic
              text, causing DECtalk to interpret normal ASCII
              characters as phonemes, resulting in garbled and
              unintelligible text. Therefore, the right square
              bracket, ' ', should be sent to DECtalk (prior to                        ]                        ]                        ]
              sending a command to restart speech) to cause
              DECtalk to exit phonemic mode.

           6. After the right square bracket has been sent to
              DECtalk, speaking should be enabled.

           7. After speaking has been enabled, application-
              specific parameters (such as the speaking voice and
              the speaking rate) should also be reset.

                 NOTE
                 that when the keypad is enabled in autostop
                 keypad mode, control sequences buffered in
                 the communication channel between the
                 application program and DECtalk are processed
                 even though speaking may be disabled. No text
                 is spoken until DECtalk receives a command to
                 restart speech.
 


           1-24     The Human Interface in Applications




           1.3  Representing Alphabetic and Alphanumeric Data

           In many DECtalk applications, the type of information to
           be transmitted to the application program cannot be rep-
           resented easily using the 12 telephone keypad keys. These
           applications usually require retrieving alphabetic text
           (for example, a person's name) or alphanumeric text (for
           example, part numbers) from the user. In most applications,
           existing databases do not conform to the 12-key system of-
           fered by the telephone keypad. Usually you do not have the
           option of changing the way data is represented in these
           existing systems. Therefore, you must use the keys of the
           telephone keypad to index the actual data desired.

           Two possible ways of representing alphabetic and
           alphanumeric text are:

           1. Direct numeric encoding

           2. Two-character encoding


           Direct Numeric Encoding

           Alphabetic text can be easily represented using direct
           numeric encoding. The user simply presses the key on
           the telephone keypad that is labeled with the desired
           letter.

           As the letters   and   are not represented anywhere on                          Q     Z                          Q     Z                          Q     Z
           the telephone keypad, you can assign them to the                                                            7                                                            7                                                            7
           (PQRS) and the   (WXYZ) keys, respectively.                          9                          9                          9
           Alternatively, you can assign the   and   keys to key                                             Q     Z                                             Q     Z                                             Q     Z
            , because this key does not contain any letter code.           1           1           1

           As the data being entered is usually variable in
           length, it is useful to require the user to end all
           commands and data by pressing a special key. The pound
           sign key ( ) and the star key ( ) are good choices for                     #                    *                     #                    *                     #                    *
           keystring termination characters.
 


                      The Human Interface in Applications     1-25




           For example, in an order entry system incorporating
           direct numeric encoding, a user who needs to order a
           4-megabyte memory board for the MicroVAX II (part
           number MS630-BB) would press         .                                        6763022#                                        6763022#                                        6763022#

           One problem exists with using direct numeric encoding
           to represent alphabetic and alphanumeric data. If the
           string is ambiguous (has more than one meaning), then
           more than one item might be recalled with the same
           string of digits.

           In the example cited above, the digit string 6763022#
           can represent the:

           o  MS630-AA (1-megabyte memory expansion board for the
              MicroVAX II)

           o  MS630-BA (2-megabyte memory expansion board for the
              MicroVAX II)

           o  MS630-BB (4-megabyte memory expansion board for the
              MicroVAX II)

           In an order entry system, this ambiguity is especially
           dysfunctional.

           One method that you can use to solve the problem of
           ambiguity is to allow the user to select between
           alternatives. In the example cited above, you can use
           the following method to enable the user to select
           items with the same identification code:

                For MS630-AA, press 1.
                For MS630-BA, press 2.
                For MS630-BB, press 3.

           Using this method, the user orders a part by pressing
           the numbered key corresponding to the part.
 


           1-26     The Human Interface in Applications




           Another way that you can allow a user to select from a
           number of alternatives for the same keycode is to step
           through the list using next and previous commands.

           For example:

                Three items found with this identification code.

                For next item, press 1.
                For previous item, press 2.
                To select the current item, press 3.

           If the user presses key   for next item, the first                                   1                                   1                                   1
           item in the list (MS630-AA in this case) will be
           spoken. If the MS630-AA is the part the user wants to
           order, key   should be pressed to place the order for                      3                      3                      3
           that item. This method is preferable in an application
           such as an on-line telephone book when the code 76484#
           could represent many more Smiths than can be
           enumerated using the 12 Touch-Tone keys. Once the user
           makes a selection, the user's choice should be
           verified before any other action is performed with
           this data.

           Direct numeric encoding is probably the best method to
           use for applications with lists of names or many part
           numbers. Its use is not recommended when a large
           amount of data in the database cannot be represented
           uniquely. In this situation, the user would too often
           be required to enter a keystring on the telephone
           keypad, and to select an actual choice from a menu of
           choices represented by the same key string.


           Two-Character Encoding

           In some applications, the alphanumeric text that has
           to be entered cannot be uniquely represented using the
           direct encoding method described above. Also, some
           applications use specific letters in their codes (for
           example, three-character airport codes) that cannot be
 


                      The Human Interface in Applications     1-27




           selected using direct numeric encoding. For these
           applications, you can use two-character encoding.

           Two-character encoding matches the three letters on a
           Touch-Tone key to the three columns of keys on the
           telephone keypad. See Figure 1-1 below.




           Figure 1-1 Touch-Tone Telephone Keypad

           The user presses two keys to select each letter:

           1. The first key is the Touch-Tone key with the
              desired letter.

           2. The second key indicates the specific letter on the
              key.

              There are two methods you can use for the second
              key:

              1. The  ,  , or   key can indicate the position of                     1  2     3                     1  2     3                     1  2     3
                 the letter on the key, or

              2. The leftmost, middle, or rightmost key in the
                 same row as the first key can indicate which
                 column the letter is in.

           To enter digits, the user must first press the
           Touch-Tone key associated with the digit, and then
           press the   key.                     0                     0                     0

              NOTE
              As the length of the text can vary, you should
              assign a key (such as the   key or the   key) to                                        #            *                                        #            *                                        #            *
              indicate the end of data.

           For example: To select     using the first method                                  IBM                                  IBM                                  IBM
           described above, the user would press        .                                                 432261#                                                 432261#                                                 432261#
 


           1-28     The Human Interface in Applications




           To select     using the second method described above,                     IBM                     IBM                     IBM
           the user would press        .                                432264#                                432264#                                432264#

           The second method allows the user to enter multiple
           keystrokes more easily, as the two keystrokes
           representing each letter occur in the same row.

           When you use two-character encoding, you must assign
           the missing  ,  , and                 to the   key. If                       Q  Z      space character        1                       Q  Z      space character        1                       Q  Z      space character        1
           you were to assign   to the   key (PQRS), there would                              Q        7                              Q        7                              Q        7
           be no way for the user to select among the four
           characters represented, as there are only three
           columns on the telephone keypad.

           If necessary, you can use two-character encoding to
           represent punctuation. (You can assign a particular
           punctuation mark to each key.) To enter punctuation,
           the user presses the Touch-Tone key associated with
           the punctuation mark, followed by a special key (such
           as the star key ( )). Note, however, that your goal is                            *                            *                            *
           to keep what the user must input on the keypad as
           simple as possible. Therefore, you should avoid asking
           the user to enter entire sentences.

           The two-character encoding method is a good method to
           use when the user will be entering short strings of
           text (such as three-character airport codes) that are
           not necessarily unique. However, you should avoid
           using this method when the user will be entering long
           strings of text.
 





















           TUNING THE TEXT
 


                                           Tuning the Text     2-3




           This chapter offers suggestions on how to make a
           segment of text more intelligible and
           pleasant-sounding for the end-user. You do not need to
           be a linguistics expert to be able to tune text.

           This chapter focuses on the applications developer who
           has no linguistics training. For more information on
           tuning a segment of text for DECtalk, see the DECtalk
           DTC03 Text-to-Speech Owner's Manual (Chapters 7-19) or
           the DECtalk DTC01 Owner's Manual (Chapters 4, 5).

           You should tune the text only after all other aspects
           of the program are working correctly. Text tuning is
           the      stage in the process of implementing an               last               last               last
           application. This chapter, therefore, assumes that the
           program is fully debugged and is working correctly,
           but that the spoken (DECtalk) output may have some
           mispronunciations or may sound unnatural in places.

              NOTE
              It is advantageous to isolate text from the
              program flow as much as possible so that
              modification of the text will not entail any
              modifications to the code.

           Text tuning is an iterative process. That is, you must
           listen to the text, modify it if necessary, listen to
           it again, modify it again, and so on. (See
           Figure 2-1.)




           Figure 2-1 Text Tuning as an Iterative Process
 


           2-4     Tuning the Text




           2.1  Reformatting Text

           As an initial step, check to see if the text requires
           any reformatting. You may need to reformat previously
           written text or text that is in the form of a special
           database.

           o  Remove all anomalous symbols from the text.

              Look for and remove (or change) any unusual
              punctuation, such as ellipsis, . . . , or double
              question marks, ??.

              Remember that DECtalk reads ASCII characters
              indiscriminately, and will likely read any
              character that appears in the text. Thus       is                                                       . . .                                                       . . .                                                       . . .
              spoken as                       , and    as                        period, period, period      !!                        period, period, period      !!                        period, period, period      !!
                                                  .              exclamation point, exclamation point              exclamation point, exclamation point              exclamation point, exclamation point

           o  Watch also for header characters, such as a line of
              asterisks, hyphens, or other formatting characters.

              Text such as:

                     ***********************************
                     *        BANK OF TIMBUKTU         *
                     ***********************************

              will have each and every asterisk read by DECtalk.

              Although it is possible to write software to ignore
              such formatting characters, it is easier, when
              creating text, to avoid using them altogether.
 


                                           Tuning the Text     2-5




           2.2  The Iterative Stages of Text Tuning

           The iterative stages of text tuning can be broken down
           into the following tasks:

           1. Selecting an appropriate voice and speaking speed

           2. Correcting gross mispronunciations

           3. Fine-tuning text for greater naturalness


           2.2.1  Selecting an Appropriate Voice and Speaking
                  Speed

           Selecting a suitable voice for DECtalk is easy.
           Table 2-1 lists the selectable voices. For further
           information see Section 4.6.2 of this guide, and the
           VAX/VMS Run-Time Library Routines Reference Manual.


           _______________________________________________________
           Table 2-1 Voice Commands           Table 2-1 Voice Commands           Table 2-1 Voice Commands
           _______________________________________________________
           _______________________________________________________           MNEMONIC              DESCRIPTION           MNEMONIC              DESCRIPTION           MNEMONIC              DESCRIPTION

                                 BEST VOICES



           Paul                  standard male

           Harry                 deep male



                                 OTHER AVAILABLE VOICES
 


           2-6     Tuning the Text



           _______________________________________________________
           Table 2-1 (Cont.) Voice Commands           Table 2-1 (Cont.) Voice Commands           Table 2-1 (Cont.) Voice Commands
           _______________________________________________________
           _______________________________________________________           MNEMONIC              DESCRIPTION           MNEMONIC              DESCRIPTION           MNEMONIC              DESCRIPTION

           Betty                 standard female

           Kit                   standard child

           Rita                  deep female

           Frank                 older male

           Wendy                 light female
           _______________________________________________________


           It is advisable to use only the voices with the
           highest intelligibility and naturalness. In DECtalk,
           these voices are Paul and Harry.

           Although a facility exists to create a totally new
           voice, creating a new voice is     advisable.                                          not                                          not                                          not

           Creating a totally new voice:

           o  Is a difficult and time-consuming procedure
              requiring experience, expertise, and resources in
              speech technology.

           o  Produces a different voice (not a better voice),
              and does not significantly improve the speech of
              DECtalk.

           o  Can produce a voice that is not transportable
              between the DTC01 and the DTC03.

           You cannot significantly improve the speech of DECtalk
           by modifying an existing voice.
 


                                           Tuning the Text     2-7




           However, if you do require or desire a voice that is
           only slightly different from one of those listed in
           Table 2-1, you may want to use the voice modification
           facility. For example, you may need to give the voice
           of Paul a slightly deeper sound. You can begin with
           the Paul voice and modify parameters as necessary.
           However, we recommend that most applications use the
           best voices (Paul and Harry).

           If a change of voice in the application is necessary:

           o  Avoid changing a voice too often since it can
              distract the end-user.

           o  Avoid changing a voice in mid-sentence since it
              will result in an unnatural pause.

           To set the speaking voice, use the Run-Time Library
           DTK$SET_VOICE routine. For further information see
           Section 4.6.2 of this guide, and the Run-Time Library
           Routines Reference Manual.

           For further information about modifying voice
           parameters, see the DECtalk DTC01 Owner's Manual
           (Chapter 5) or the DECtalk DTC03 Text-to-Speech
           Owner's Manual (Chapter 9). See also the VAX/VMS
           Run-Time Library Routines Reference Manual, and
           Appendix B of this guide.

           DO:

           o  Use the voice of Paul or Harry.

           DO NOT:

           o  Change voices too often.

           o  Create a totally new voice.

           o  Change voices in mid-sentence.
 


           2-8     Tuning the Text




           Speaking Speed

           Speaking speed is extremely important. Each
           application is different; a normal speaking rate for
           one application can be too slow for another. In
           general, the best speaking speeds range from 160 to
           200 words per minute, with a good average around 180
           words per minute (the default speed).

           To set the speaking rate, use the Run-Time Library
           DTK$SET_VOICE routine. For further information see
           Section 4.6.2 of this guide, and the Run-Time Library
           Routines Reference Manual.

           Slowing down the speaking speed does not increase
           comprehensibility. Moreover, a speed that is too slow
           consumes valuable application time. To a certain
           extent, comprehensibility is connected with
           naturalness, and therefore you should strive for a
           natural speed.

           Occasionally you may need to vary the speed within one
           segment of text. For example, it might be useful to
           make a part number or a telephone number be pronounced
           at a slower speed. Avoid changing the speaking speed
           in mid-sentence because it results in an audible pause
           as DECtalk makes the adjustments. In general, it is
           best to keep the speed standard throughout.

           DO:

           o  Keep the speaking speed between 160 to 200 words
              per minute.

           o  Insert speaking speed parameters in square
              brackets.

           DO NOT:

           o  Change speeds in mid-sentence.
 


                                           Tuning the Text     2-9




           o  Change speeds drastically within one text segment.


           2.2.2  Correcting Gross Mispronunciations

           Most applications using speech synthesis focus on two
           points: intelligibility and naturalness.
           Intelligibility is not a problem because DECtalk
           messages are totally understandable. However, you may
           occasionally find words mispronounced.

           Having chosen an appropriate voice and speed, you are
           ready to listen more carefully to the text to correct
           gross mispronunciations. Mispronunciations are errors
           in the sounds themselves. These errors are also called
           phonemic errors. Here, the priority is
           intelligibility. An end-user who cannot adequately
           understand a message will not be able to use the
           application.

           Mispronunciations can be divided into two types:

           o  Gross mispronunciations

           o  Minor mispronunciations

           Gross mispronunciations are words that are difficult
           or impossible to understand. DECtalk rarely makes this
           type of error because a great deal of linguistic
           intelligence is already built into the system.
           Occasionally, however, and on specific types of words
           (for example, last names, geographical locations,
           industry-specific technical jargon, and so forth),
           DECtalk can mispronounce some words in an application.

           When DECtalk does mispronounce words, the following
           procedure is an easy and precise way to make
           corrections:

           1. Obtain a hardcopy of the text.
 


           2-10     Tuning the Text




              Hardcopy allows you to examine the text word by
              word.

           2. As you listen to the text, mark (in your hardcopy)
              each mispronounced word.

           3. Make all corrections you can at one time.

              Do not correct only one word at a time before
              relistening to the text.

           4. Concentrate on gross mispronunciations only.

           5. Correct the mispronounced words in the text file
              itself in a way that DECtalk will be able to
              pronounce them.

              You may want to spell the word using the phonemic
              alphabet.


           Using the Phonemic Alphabet

           A phonemic alphabet is a special alphabet that makes
           it possible for you to rewrite words so that DECtalk
           will pronounce them correctly. Using a phonemic
           alphabet is similar to writing the word NIGHT as NITE,
           the word THROUGH as THRU, or the word CAT as KAT.

           A phonemic alphabet is useful because of the a one-to-
           one correspondence between written symbols and sounds.

           Always write the phonemic string for a mispronounced
           word between square brackets, [].

           As you put a word into the phonemic alphabet, always
           pay attention to how a word is pronounced, rather than
           how it is spelled. Keep in mind that some letters are
           not pronounced. For example, a final   is usually not                                                E                                                E                                                E
           pronounced, as in HOME, SOME, ALONE. Similarly, a
           beginning   followed by an   is not pronounced, as in                     P                N                     P                N                     P                N
           PNEUMONIA, PNEUMATIC.
 


                                          Tuning the Text     2-11




           Most consonant sounds in the phonemic alphabet are
           represented by the same symbols that we use in normal
           spelling. If, for example, a word begins with the
           sound  , as in BOOT, you can use the symbol     for                 b                                     [b]                 b                                     [b]                 b                                     [b]
           the phonemics. Many other symbols are the same as
           letters we already use:                            .                                   b,d,f,g,k,l,m,n,p,r,s,t,v,z                                   b,d,f,g,k,l,m,n,p,r,s,t,v,z                                   b,d,f,g,k,l,m,n,p,r,s,t,v,z

           Tables  2-2,  2-3, and 2-4 list phonemic equivalents
           with examples. (See also the DECtalk DTC01 or DTC03
           Programmer's Reference Card.)


           _______________________________________________________
           Table 2-2 Phonemic Equivalents - Consonants           Table 2-2 Phonemic Equivalents - Consonants           Table 2-2 Phonemic Equivalents - Consonants
           _______________________________________________________
           _______________________________________________________           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE

           b                      OOT                                 B                                 B                                 B

           d                      OG                                 D                                 D                                 D

           f                      RANK                                 F                                 F                                 F

           g                      OOD                                 G                                 G                                 G

           k                      ILT,  AT                                 K     C                                 K     C                                 K     C

           l                      AMB                                 L                                 L                                 L

           m                      IKE                                 M                                 M                                 M

           n                      OT                                 N                                 N                                 N

           p                      ET                                 P                                 P                                 P

           r                      OOM                                 R                                 R                                 R

           s                      IT                                 S                                 S                                 S

           t                      EST                                 T                                 T                                 T
 


           2-12     Tuning the Text



           _______________________________________________________
           Table 2-2 (Cont.) Phonemic Equivalents - Consonants           Table 2-2 (Cont.) Phonemic Equivalents - Consonants           Table 2-2 (Cont.) Phonemic Equivalents - Consonants
           _______________________________________________________
           _______________________________________________________           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE

           v                      EST                                 V                                 V                                 V

           w                      ET,  HAT                                 W    W                                 W    W                                 W    W

           z                      OO                                 Z                                 Z                                 Z

           jh                     UDGE                                 J                                 J                                 J

           th                      IN                                 TH                                 TH                                 TH

           dh                      IS                                 TH                                 TH                                 TH

           sh                      OW                                 SH                                 SH                                 SH

           ch                      URCH                                 CH                                 CH                                 CH

           zh                    MEA URE (rare)                                    S                                    S                                    S

           hx                     OT                                 H                                 H                                 H

           nx                    SI                                   NG                                   NG                                   NG

           en                    BUTT                                     ON                                     ON                                     ON

           el                    BOTT                                     LE                                     LE                                     LE
           _______________________________________________________


           The vowel symbols are somewhat different from normal
           spelling, but again they are relatively easy to
           master.
 


                                          Tuning the Text     2-13




           _______________________________________________________
           Table 2-3 Phonemic Equivalents--Vowels           Table 2-3 Phonemic Equivalents--Vowels           Table 2-3 Phonemic Equivalents--Vowels
           _______________________________________________________
           _______________________________________________________           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE

           aa                    F THER                                  A                                  A                                  A

           ae                    C T                                  A                                  A                                  A

           ah                    B T                                  U                                  U                                  U

           ao                    B  GHT                                  OU                                  OU                                  OU

           aw                    B  T                                  OU                                  OU                                  OU

           ax                     BOUT (first unstressed sound)                                 A                                 A                                 A

           ay                    B TE                                  I                                  I                                  I

           eh                    L T                                  E                                  E                                  E

           ey                    B KE                                  A                                  A                                  A

           ih                    B T                                  I                                  I                                  I

           iy                    P TE                                  E                                  E                                  E

           ow                    B  T                                  OA                                  OA                                  OA

           oy                    B Y                                  O                                  O                                  O

           uh                    B  K                                  OO                                  OO                                  OO

           uw                    L TE                                  U                                  U                                  U

           yu                    C TE                                  U                                  U                                  U

           ix                    KISS S (second unstressed sound)                                     E                                     E                                     E

           rr                    B RD                                  I                                  I                                  I
           _______________________________________________________
 


           2-14     Tuning the Text




           _______________________________________________________
           Table 2-4 Phonemic Equivalents--Vowels followed by 'r'           Table 2-4 Phonemic Equivalents--Vowels followed by 'r'           Table 2-4 Phonemic Equivalents--Vowels followed by 'r'
           _______________________________________________________
           _______________________________________________________           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE           PHONEMIC SYMBOL       EXAMPLE

           ar                    B                                  AR                                  AR                                  AR

           er                    B                                  EAR                                  EAR                                  EAR

           ir                    B                                  EER                                  EER                                  EER

           or                    B                                  ORE                                  ORE                                  ORE

           ur                    P                                  OOR                                  OOR                                  OOR
           _______________________________________________________


           You are probably familiar with many of these symbols.
           You will have to learn the other symbols, but all are
           fairly mnemonic.
 


                                          Tuning the Text     2-15




           Remember that:

           o  The symbols                             are                          b,d,f,g,k,l,m,n,p,r,s,t,v,z                          b,d,f,g,k,l,m,n,p,r,s,t,v,z                          b,d,f,g,k,l,m,n,p,r,s,t,v,z
              basically the same as their spelling.

           o  The sound  , as in HOT or HARRY is always written                        H                        H                        H
              as [hx].

              For example, [hx'aat], [hx'aeriy].

           o  The last sound in words like SING or BANG is not a
              G!

              It is a special type of  , written as [nx].                                      N                                      N                                      N

              For example, [s'ihnx], [b'aenx].

           o  There is a difference between the first sound of
              the word THIN and the first sound of the word THIS.
              THIN begins with a voiceless sound [th], whereas
              THIS begins with a voiced sound [dh].

              For example, [th'ihn], [dh'ihs].

              Sound these out and you will be able to hear the
              difference.

           o  A special vowel sound in words like BIRD, GIRL,
              ALERT, and SWERVE is written as [rr].

              For example, [b'rrd], [g'rrl], [axl'rrt], [sw'rrv].

           o  The letter   is often pronounced like a  , as in                         S                            Z                         S                            Z                         S                            Z
              HAS, PHASE, EASE, CHASM, PRESERVE, and so on.

              In fact, this distinction often changes the meaning
              of a word. For example, the only difference between
              the words LOOSE and LOSE is the [s]--[z]
              distinction, that is, [l'uws]--[l'uwz].

              Listen carefully to the word, determine whether the
              sound is that of an [s] or a [z], and use the
              appropriate phoneme.
 


           2-16     Tuning the Text




           o  Two symbols are used in unstressed syllables
              (syllables that have no emphasis on them). These
              are the phonemes [ix] and [ax], corresponding to
              the stressed vowel symbols [ih] and [ah],
              respectively.

              Notice the contrast between the stressed and
              unstressed vowels shown in the following table
              (Table 2-5).


           _______________________________________________________
           Table 2-5 Stressed and Unstressed Vowels           Table 2-5 Stressed and Unstressed Vowels           Table 2-5 Stressed and Unstressed Vowels
           _______________________________________________________
           _______________________________________________________           STRESSED VOWELS       UNSTRESSED VOWELS           STRESSED VOWELS       UNSTRESSED VOWELS           STRESSED VOWELS       UNSTRESSED VOWELS

           BIT [b'iht]           ROS E S (second sound)
                                 [r'owzixs]

           BUT [b'aht]           A BOUT (first sound) [axb'awt]
           _______________________________________________________


           With practice, you should be able to use the phonemic
           alphabet effectively.


           Indicating Stress

           Stress, sometimes known as accent, is an important
           part of writing a word in the phonemic alphabet.
           Stress is simply a greater emphasis with which a part
           or syllable of a word is spoken. Every syllable of an
           English word is either stressed or unstressed.

           For example, the word INVALID has three syllables:
           IN-VA-LID. In this case, it is important to know which
           syllable is stressed.

           o  Stress on the first syllable (IN-va-lid) means "a
              person who is confined to a house or a wheelchair."
 


                                          Tuning the Text     2-17




           o  Stress on the second syllable (in-VA-lid) means
              "not valid."

           There is usually only one stress within a word. Thus,
           when you use the phonemic alphabet, you must include
           the stress mark ['] (the apostrophe) in front of
           whichever vowel sound is stressed.

           For example,

                The ['ihnvaxlixd] could not leave the house.
                His visa was [ihnv'aelixd].

           For more information on stress and how it works in
           English, see the DECtalk DTC03 Text-to-Speech Owner's
           Manual (Chapter 7), or the DECtalk DTC01 Owner's
           Manual (Chapter 4).

           See also Section  2.2.3 "Fine-tuning Text for Greater
           Naturalness" (below).

           DO:

           o  Place the stress mark ['] before the stressed vowel
              of the word.

           DO NOT:

           o  Use the phonemic alphabet unnecessarily.
 


           2-18     Tuning the Text




           2.2.3  Fine-Tuning Text for Greater Naturalness

           When you have corrected all gross errors in
           pronunciation, you can listen to the text for
           naturalness. Improving the naturalness of a segment of
           text is called            .                          fine-tuning                          fine-tuning                          fine-tuning

           The obvious difference between speech and written text
           is that written text is typically not written with a
           speaker in mind. As a result,

           o  Words in written text are usually longer and more
              complicated.

           o  Pauses are not taken into consideration even though
              a speaker has to breathe.

           o  Emotion or expressive intonation in written text is
              typically not indicated except by punctuation (for
              example, the exclamation point (!)).

           Ambiguous words (words with more than one meaning)
           cannot be handled without something extra, for
           example, a rise in the voice, a pause, or the like.

           Consider the following sentence:

                John doesn't drink because he's unhappy.

           If we say this sentence with a neutral intonation, it
           means that John does NOT drink. If, on the other hand,
           we emphasize the second syllable (HAP) of "UNHAPPY,"
           and make our voice rise at the very end of the
           sentence, it means that John DOES drink but the reason
           is other than his not being happy.

           The following pair of sentences provides another
           example of ambiguity:
 


                                          Tuning the Text     2-19




                What's that on the road ahead?
                What's that on the road, a head?

           Although these utterances clearly have different
           meanings, the only difference in speech is a pause
           after the word ROAD, and a slight rise at the end of
           the sentence. Therefore, something as simple as a
           pause can add a great deal of naturalness and, more
           importantly, meaning, to your text.

           Fine-tuning the text is done in much the same way as
           the initial correction was accomplished. Initial
           tuning improves the total intelligibility, fine-tuning
           produces greater naturalness.


           Using Synonyms

           Often there is more than one word that can be used in
           the same context to convey the same meaning. Words
           with similar meanings are called synonyms. If a word
           does not sound right, it is often easy to substitute a
           synonym. If you cannot think of a synonym, a thesaurus
           (a dictionary of synonyms) may help.

           For example, in the following sentence, if the phrase
           "line up" sounds a bit strange (perhaps because the
           word "up" is not fully stressed), you can substitute
           the word "justify" for it.

                He couldn't line up words in the right margin.
                He couldn't justify words in the right margin.
 


           2-20     Tuning the Text




           Using Contractions

           As stated above, text that was written to be read is
           often different from text written to be spoken. Using
           contractions (common words that are composites of two
           words) often improves the naturalness of text.
           (Examples of contractions are WON'T for WILL NOT,
           HADN'T for HAD NOT, HE'll for HE WILL, and so on.)

           One of the most common errors in writing applications
           text is in writing a text segment as if it were to be
           read, that is, using formal (even stilted) prose
           containing non-contracted forms. Non-contracted forms
           tend to sound more unnatural simply because they are
           too formal for normal speech (such as that found in
           telephone conversations).


           Using Pauses

           Pauses are extremely important in speech for the
           following reasons:

           o  All speakers need to take an occasional breath.

           o  Pauses are a way of underscoring or emphasizing
              words and clauses, and of creating what are
              generally called "sense groups" (groups of words
              that go together in meaning).

           There are three types of pauses you can insert into a
           segment of text:

           o  Period pauses

           o  Comma pauses

           o  General pauses

           1. Period Pauses
 


                                          Tuning the Text     2-21




              A sentence is an example of a sense group. Always
              make sure that each sentence ends with a period.
              The default period pause in DECtalk is about a half
              second long. Although this pause is reasonable for
              most applications, the pause can be lengthened (or
              shortened in DECtalk DTC03) by using the Run-Time
              Library DTK$SET_VOICE routine.

                 NOTE
                 Remember also that longer pauses tend to
                 waste time, and may create frustration on the
                 part of the end-user. In most applications
                 you should not need to modify the period
                 pause.

              You must sometimes remove the period from text.
              However, you often need to insert a period where it
              might not normally go in written text. For example,
              in the following list, period pauses are needed,
              otherwise DECtalk simply continues reading without
              a pause:

                     1 John Smith
                     2 Mike Jones
                     3 Frank Thompson

              Therefore, you should rewrite this segment as:

                     1. John Smith.
                     2. Mike Jones.
                     3. Frank Thompson.

              DO:

              o  Use period pauses at the end of each sentence.

              o  Use period pauses after each item on a list.

              DO NOT:

              o  Use ellipsis marks, hyphens, or other unusual
                 types of punctuation.
 


           2-22     Tuning the Text




           2. Comma Pauses

              Like the period pause, the comma pause can be
              incremented (or decremented in DECtalk DTC03) using
              the Run-Time Library DTK$SET_VOICE routine.

              It is advisable not to change the length of the
              comma pause since it is optimal for most short
              pauses.

              The comma pause is similar to the period pause, but
              lasts only about one-sixth of a second.

              You may use comma pauses to:

              o  Delineate items in a list.

              o  Separate sense groups (groups of words that go
                 together semantically).

              o  Separate breath groups (groups of words normally
                 spoken in one breath).

                 NOTE
                 To increase the naturalness of a segment of
                 spoken text, you may need to put more commas
                 in the text than would normally be the case.

           3. Pause Phoneme

              A special type of phoneme, called the pause
              phoneme, takes on the property of a pause when
              combined with the length of the pause (in
              milliseconds). The pause phoneme is written as "_"
              (underscore). The length is always written in angle
              brackets.

              For example, if your application needs to pause for
              3 seconds between two sections of text, you could
              insert the following in the appropriate place in
              the text.
 


                                          Tuning the Text     2-23




                    [_<3000>]

           The advantage of using the pause phoneme is that you
           do not have to reset a period pause or comma pause
           duration. However, avoid using the pause phoneme in
           the middle of a sentence, as it can sound unnatural.


           2.3  Conveying Emotion or Emphasis

           DECtalk reads generic serial ASCII text. Although it
           recognizes certain parts of speech (verbs,
           prepositions, and the like), it does not understand
           the meaning of the words it is reading. Human beings,
           on the other hand, not only understand the words they
           are using, but use different inflections of voice
           (pitch rises and falls, loudness, accent, and other
           acoustic aids) to help convey meaning. In DECtalk, it
           is not possible to convey emotion or emphasis on
           significant words or phrases without some help.


           Emphatic Stress

           In the section "Indicating Stress," we discussed the
           use of stress ['] in the phonemic string. The [']
           phoneme is called primary stress, and is the normal
           stress on a word. However, another stress phoneme,
           called emphatic stress, is much more emphatic than
           primary stress and can be useful in an application
           text. Emphatic stress is written as ["].

           You can use the emphatic stress to verbally underscore
           a particular word. You can write emphatic stress in
           the phonemic string just as you write primary stress.
           However, you can more easily write it as ["] placed
           immediately before the word you want to stress. (No
           blank space is allowed between the stress mark ["] and
           the word.)
 


           2-24     Tuning the Text




           Following is an example of emphatic stress. Use the
           phoneme ["] carefully and only when you want to
           strongly stress a particular word.

                Welcome to First National Bank of Timbuktu.
                ["]Welcome to First National Bank of Timbuktu.


           Exclamation Point

           You can also use the exclamation point to stress a
           particular word. The exclamation point is generally
           used at the end of a sentence. However, you can also
           use it at the end of a word.

           For example:

                Welcome to First National Bank of Timbuktu!
                Welcome! to First National Bank of Timbuktu.

           When you use the exclamation point at the end of a
           sentence, the last word in the sentence bears the
           heaviest stress. If you wish to stress a particular
           word, use the ["] phoneme before that word. In
           general, you should not use exclamation points as
           liberally as you use them in normal text.


           Pitch Control Characters

           DECtalk has internal rules that accurately simulate
           the rise and fall intonation of English phrases and
           clauses. However, you may have occasion to override
           these rules to provide more expressiveness to a
           segment of text.
 


                                          Tuning the Text     2-25




           Pitch Phonemes

           There are three pitch control characters that you can
           use to make the voice rise or fall:

           1. the character [/] (pitch rise)

           2. the character [\] (pitch fall)

           3. the character [/\] (pitch rise-fall)

           The [/] phoneme makes the voice go up slightly;
           conversely, the [\] phoneme allows the voice to drop
           slightly. These phonemes are also used together to
           cause a rise-fall pattern in a word or phrase. Like
           the ["] phoneme, you can write the pitch rise [/] and
           pitch fall [\] phonemes in front of a word or phrase
           without having to put the word in phonemics.

           For example:

                 [/\]Hello.


           New Topic or Paragraph Phoneme

           Another way in which you can improve text naturalness
           is to include the new topic phoneme [+]. Essentially,
           the [+] phoneme allows the voice to ramp up as if
           beginning a new thought, exactly what a new paragraph
           does in text. You do not have to limit the use of the
           [+] phoneme to the start of a new paragraph. You can
           use it wherever a new thought occurs. In a typical
           application, the new topic phoneme might indicate
           every different set of choices for the end-user. It
           serves to inject more emotion and delineation into the
           text segment.
 


           2-26     Tuning the Text




           Pitch Parameter

           We speak of people having high-pitched or low-pitched
           voices. We also know about baritones, sopranos, and
           tenors. Pitch refers to the relative "height" of a
           sound or sounds. In physical terms, it refers to
           frequency, measured in Hertz (cycles per second),
           abbreviated as Hz. High sounds are produced at a
           higher (faster) frequency than low sounds.

           Humans usually change pitch often when they speak. If
           they do not, the result is a boring or monotonous way
           of speaking. DECtalk automatically modifies its pitch
           to simulate the speech of a human being. If, however,
           you want to change to a higher or lower pitch, you can
           use the pitch parameter.

           You can specify the pitch parameter in either of two
           ways:

           1. Cycles per second, where the lowest pitch accepted
              by DECtalk is 65.4 Hz and the highest 523.4 Hz

           2. A coded number scheme where 1 is the lowest pitch
              (equivalent to 65.4 Hz), and 37 is the highest
              pitch (equivalent to 523.4 Hz)

           The correct syntax for this command is:

                 [phonemics<,pitch>phonemics]

           The following example uses the word "welcome" to
           illustrate how pitch can help to create more
           expression or emotion:

                 [w'eh<,207>lkax<,82>m]
 


                                          Tuning the Text     2-27




           Here we have inserted two pitches, one on the first
           syllable (at 207 Hz), and one on the second syllable
           (at 82 Hz). We could have also written this command
           with the pitch values 21 and 5, respectively:

                 [w'eh<,21>lkax<,5>m]


           Duration

           Duration refers to the length of a segment or sound.
           DECtalk always calculates these values for you, and
           the result is natural sounding speech. However, there
           may be times when your application calls for a change
           in the length of a segment, although situations like
           this are rare. Unless your application requires a
           change in the length of a vowel or consonant sound,
           leave the default values in and do not change
           duration.

           If you need to change the length of a segment, you
           must do the following:

           1. Write the word or phrase in phonemics.

           2. Make sure that the phonemes and stress are correct.

           3. Put the length in angle brackets after the segment
              you want to lengthen or shorten (almost always a
              vowel sound).

           The correct syntax for this command is as follows:

                 [phonemics<duration>phonemics]

           For example, consider the word "welcome." You have
           already seen what different types of stress and pitch
           can do for expressiveness in this word. Now you can
           also lengthen the first vowel of the word as follows:

                 [w'eh<200>lkaxm]
 


           2-28     Tuning the Text




           Notice that the duration (in this case) follows the
           vowel. There will rarely be a case when you need to
           lengthen or shorten a consonant. Again, a description
           of the duration parameter is here for reference only.
           Unless absolutely necessary, avoid modifying this
           parameter.

           You can combine pitch with duration. (Separate these
           two parameters by a comma.) In this case, the correct
           syntax is:

                 [phonemics<duration, pitch> phonemics]

           The following example again uses the word "welcome" to
           illustrate a combination of duration and pitch. Here
           we have inserted a duration of 200 milliseconds (2/10
           second) on the first syllable, along with a pitch of
           207 Hz. We then insert a duration of 150 milliseconds
           (3/20 second) on the second syllable, along with a
           pitch of 82 Hz:

                 [w'eh<200,207>kax<150,82>m]

           For further information on the pitch and duration
           parameters, see the DECtalk Owner's Manuals. However,
           unless your application requires it, avoid modifying
           either the pitch or the duration parameter.


           Compound Nouns

           A large group of words in English, called compound
           nouns, function as single nouns or adjectives, but are
           (or were originally) composed of two words.

           Examples of compound nouns are: RUSH HOUR, TIMEFRAME,
           NUMBER SIGN, BANK BALANCE, BLUEBIRD, and GREENHOUSE.
           Compound nouns have a unique stress pattern that can
           help make your application text more natural. For
 


                                          Tuning the Text     2-29




           example, consider the word greenhouse in the following
           sentences:

                John lives in a green house.
                Plants live in a greenhouse.

           You will hear that the words GREEN HOUSE and
           GREENHOUSE are not pronounced the same. GREEN HOUSE (a
           house that is green) contains a primary stress on each
           word. (See the section above, "Indicating Stress.")
           However, the compound noun GREENHOUSE contains a
           primary stress on the first syllable and a lesser
           (secondary) stress on the second syllable. If your
           application contains a compound noun, hyphenate the
           two words. For example:

              RUSH-HOUR
              TIME-FRAME
              NUMBER-SIGN
              BANK-BALANCE
              BLUE-BIRD
              GREEN-HOUSE

           This hyphenation makes your text sound more natural.

           If you need to put the compound noun into the phonemic
           alphabet, separate the two words by [#], place a
           primary stress mark over the first word, and a
           secondary stress mark [`] over the second word. (For
           examples, see Table 2-6.)
 


           2-30     Tuning the Text




           _______________________________________________________
           Table 2-6 Putting Compound Nouns into the Phonemic           Table 2-6 Putting Compound Nouns into the Phonemic           Table 2-6 Putting Compound Nouns into the Phonemic
                     Alphabet                     Alphabet                     Alphabet
           _______________________________________________________
           _______________________________________________________           COMPOUND         HYPHENATED       PHONEMIC           COMPOUND         HYPHENATED       PHONEMIC           COMPOUND         HYPHENATED       PHONEMIC

           rush hour        rush-hour        [r'ahsh#`awrr]

           timeframe        time-frame       [t'aym#fr`eym]

           number sign      number-sign      [n'ahmbrr#s`ayn]

           bank balance     bank-balance     [b'aenxk#b`aelaxns]

           bluebird         blue-bird        [bl'uw#b`rrd]

           greenhouse       green-house      [gr'iyn#h`aws]
           _______________________________________________________


           Compound nouns occur frequently in typical application
           text. In banking, for example, words such as BANK
           BALANCE, CHECKING ACCOUNT, SAVINGS ACCOUNT, TELEPHONE
           NUMBER, NUMBER SIGN, and the like, all have the
           compound noun stress pattern. Hyphenating such words
           results in a more natural intonation in the
           application text.


           Numbers

           In general, DECtalk firmware takes care of the
           pronunciation of numbers, and contains a great deal of
           intelligence about how people typically pronounce
           strings of digits. Part numbers, fractions, money,
           dates and times of day, and both cardinal and ordinal
           numbers, are usually pronounced in a way that would be
           useful for an application.
 


                                          Tuning the Text     2-31




           However, DECtalk sees such strings as phone numbers, zip
           codes, and the like, as strings of digits. There is no sim-
           ple way for DECtalk to tell the difference between, for
           example, a zip code like 12345, and a random number (ex-
           cept zip codes beginning with zero). DECtalk, therefore,
           defaults to pronouncing an entire string of digits up to
           9 digits long, and spelling any string longer than 9 dig-
           its. Thus, 999,999,999 is pronounced "nine-hundred and ninety-
           nine million, nine-hundred and ninety-nine thousand, nine-
           hundred and ninety-nine." However, 9999999999 is spoken
           simply as a string of 9's.

           Default pronunciation rules for numbers are listed in
           the DECtalk DTC03 Text-to-Speech Owner's Manual
           (Appendix C) and the DECtalk DTC01 Owner's Manual
           (Appendix A).

           Before you write out a number, it is crucial for you
           as the developer to decide how a number should be
           pronounced. Remember that the pronunciation of numbers
           is often application-specific.


           Telephone Numbers

           Telephone numbers in the United States and Canada are
           pronounced as follows: (D = digit)

                AREA CODE -- PREFIX -- NUMBER

                D D D (pause) D D D (pause) D D D D (long pause)

           DECtalk does not speak telephone numbers written out
           in normal telephone format. For example, (619) 555-
           1212 will not be spoken by DECtalk as desired. It is
           preferable to write telephone numbers out as follows:

                6 1 9, 5 5 5, 1 2 1 2.
 


           2-32     Tuning the Text




           Spaces are left between digits for a "spell out" type
           pronunciation which is normal in this case.

              NOTE
              Use commas for the pause after the area code and
              the prefix to more accurately replicate the drop
              in intonation characteristic of human speech.
              Place a period at the end of the number string.

           If the text of the application normally has 0
           pronounced as "OH," then you can substitute the
           alphabetic character   or   for the number  . For                                o    O                0                                o    O                0                                o    O                0
           example, you can write       or      .                                  6 o 9    6 O 9                                  6 o 9    6 O 9                                  6 o 9    6 O 9

           As with other such pronunciation rules, it is wise to
           run the normal text through DECtalk once, making note
           of any anomalies and idiosyncrasies in pronunciation,
           and then make changes. A great deal of intelligence
           about English usage is built into DECtalk.


           Initialisms

           Many applications contain initialisms ( "words"
           composed of letters or initials that stand for other
           words). For example, FBI, IRS, SDI, RCA, CBS, and NBC
           are just some of the thousands of initialisms in
           common use in English. As applications typically
           contain their own jargon, your text will probably
           contain one or more initialisms.

           If an initialism contains a vowel, DECtalk tries to
           pronounce it. For example, DECtalk pronounces ABC as
           ['aebk]. This pronunciation is clearly unacceptable.

           To make initialisms sound as natural as possible, you
           can either:

           o  Write all initialisms with blanks between the
              letters. For example, write FBI as F B I.
 


                                          Tuning the Text     2-33




           o  Write the initialism using the phonemic alphabet.
              The phonemic alphabet enables you to more
              accurately control stress, something which is very
              important in this case.

              NOTE
              If an initialism contains the letter "A," you
              must surround the A by quotation marks to
              distinguish it from the word A as in A BOOK. For
              example, "A" B C.


           Fine-Tuning Initialisms

           As sequences of common initials are really words, they
           are typically pronounced differently than if they were
           simply a string of letters. The stress pattern for
           two-letter initialisms is usually ['] [']
           (primary-primary). Thus, a blank between the letters
           is sufficient. Write AC, for example, as "A" C.

           The stress pattern for three-letter initialisms is [']
           [`] ['] (primary-secondary-primary).

           Table 2-7 illustrates the stress pattern for
           three-letter initialisms.
 


           2-34     Tuning the Text




           _______________________________________________________
           Table 2-7 Stress Pattern for Three-letter Initialisms           Table 2-7 Stress Pattern for Three-letter Initialisms           Table 2-7 Stress Pattern for Three-letter Initialisms
           _______________________________________________________
           _______________________________________________________           INITIALISM            PHONEMICS           INITIALISM            PHONEMICS           INITIALISM            PHONEMICS

           FBI                   ['ehfb`iy'ay]

           IRS                   ['ay`aar'ehs]

           SDI                   ['ehsd`iy'ay]

           RCA                   ['aars`iy'ey]
           _______________________________________________________


           The User Dictionary

           DECtalk contains a user dictionary that allows you to
           load in words and their respective pronunciations.
           There are two reasons for using this utility:

           1. A word that is important for the application is
              mispronounced by DECtalk.

           2. A common word has an application-specific
              pronunciation that is different from the usual
              pronunciation.

           The user dictionary allows you to load in these words
           so that whenever they appear in the application text,
           DECtalk pronounces them appropriately.

           (See the DECtalk DTC01 Programmer's Reference Manual
           (Chapter 3) or the DECtalk DTC03 Text-to-Speech
           Owner's Manual (Chapter 8) for information on loading
           the user dictionary.) (See also Section 4.6.2,
           "Initializing the Application-Specific Parameters.")
           The software accompanying this guide contains routines
           for loading the user dictionary.
 


                                          Tuning the Text     2-35




           The user dictionary is a Random Access Memory (RAM)
           device. Therefore, the vocabulary stored within the
           dictionary must be downloaded at power-up. If the
           power is turned off for any reason, or if DECtalk is
           hard reset, the information will be lost and must be
           reloaded. (See the DECtalk DTC01 Programmer's
           Reference Manual (Chapter 5) or the DECtalk DTC03
           Text-to-Speech Owner's Manual (Chapter 5).)

           The user dictionary is approximately 8K bytes. An
           average-length word is 8 to 10 bytes and a phonemic
           equivalent is 10 to 12 bytes. Thus, the average entry
           for a word is about 20 bytes. You can enter
           approximately 300 words and their phonemic equivalents
           into the user dictionary.

           If your application needs a large number of special
           pronunciations, run the dictionary from the host
           computer where storage is not an issue. In this case,
           you must write a simple program to essentially
           translate matched words into phonemic symbols before
           they are passed through to DECtalk.

           A word can be entered into the user dictionary by
           using the DECtalk Run-Time Library DTK$LOAD_DICTIONARY
           routine. For further information see Section 4.6.2 of
           this guide, and the VAX/VMS Run-Time Library Routines
           Reference Manual.

           The following examples illustrate three instances
           where the user dictionary is useful:

           1. Example 1 (Gross Mispronunciation)

              The word "DEBENTURE" frequently appears in banking
              applications. DECtalk, however, mispronounces it as
              [d'ehbaxnchrr].
 


           2-36     Tuning the Text




              To correct this mispronunciation, use the
              following:

                 debenture daxb'ehnchrr

           2. Example 2 (Minor Mispronunciation)

              To fine-tune the minor mispronunciation of
              "overview," use the following:

                 overview 'owvrrvyxuw

           3. Example 3 (Application-specific words or
              initialisms)

              Another (and perhaps more common) use of the user
              dictionary is where the pronunciation of a
              particular word or initialism is
              application-specific.

              For example, DECtalk pronounces "GMT" as "GEE EHM
              TEE." Suppose, however, that GMT must instead be
              pronounced "Greenwich Mean Time." You could then
              load the word GMT into the user dictionary using
              the following:

                 GMT gr'ehnixjh*m'iyn#t`aym

              Notice that the spelling sequence contains only the
              initialism (GMT), and that the phoneme sequence
              contains no blanks or white space, even though the
              spelled version "Greenwich Mean Time" does.

           When using the user dictionary, be especially careful
           of the following:

           1. An entry in the user dictionary pre-empts the same
              entry if it occurs in the firmware dictionary, or
              if it would normally have gone through the
              letter-to-sound rules.
 


                                          Tuning the Text     2-37




              For example, if you enter DEC as "Digital Equipment
              Corporation," DECtalk no longer pronounces it as
              "DECK." If you enter ME as "MAINE," then DECtalk no
              longer pronounces it as "MEE."

              Loading in a word that has more than one pronunciation
              has certain liabilities. For example, the following state
              abbreviations are all normal words or abbreviations in
              their own right: AL, CA, HI, ID, IN, MA, ME, OH, OK,
              OR, and PA. When you enter a word into the user dic-
              tionary, be certain that there are no linguistic li-
              abilities. Always ask the question, "Is there any con-
              text where this can be pronounced differently?" Use the
              word only if the answer to the question is no, or if
              the alternate pronunciation is never needed. You should
              avoid entering a word into the user dictionary if it
              is the same as a normal English word that might occur
              in the application text.

           2. An entry in the user dictionary causes DECtalk to
              give that pronunciation to every subsequent token
              of the word.

              For example, if your application calls for IRA to
              be pronounced as "EYE AAR EY" (that is, as
              ['ay`aar'ey]), then the personal first name IRA
              (all upper-case) cannot be pronounced as "EYE-RA"
              (that is, as ['ayrax]).

           3. An entry in the user dictionary is matched with
              certain derived forms for words in text.

              For example, if your user dictionary contains the word
              MUMBLE, then MUMBLING, MUMBLED, MUMBLEHOOD, MUMBLENESS
              and certain others will be matched as well. (The al-
              gorithm essentially strips off the affix and matches
              the dictionary entry.) This device is very useful. However,
              be careful that a dictionary entry will not match some
              other word in its derived form. For example, if you put
 


           2-38     Tuning the Text




              OK in the dictionary as OKLAHOMA, and your text con-
              tains the word OKED, DECtalk considers it to be the past
              tense of OK, and matches it against the entry OKLAHOMA.

           There is a way that you can minimize potential
           problems. When you enter a word (that is, the spelled
           sequence) into the user dictionary in lower-case
           letters, the entry is case-insensitive (it will match
           words written in any case). If, however, you enter a
           word in all upper-case letters, it matches only a word
           written entirely in upper-case letters. (See
           Table 2-8.)


           _______________________________________________________
           Table 2-8 Sample User Dictionary Entry, and Matching           Table 2-8 Sample User Dictionary Entry, and Matching           Table 2-8 Sample User Dictionary Entry, and Matching
                     Entries                     Entries                     Entries
           _______________________________________________________
           _______________________________________________________           ENTRY            WILL MATCH            WILL NOT MATCH           ENTRY            WILL MATCH            WILL NOT MATCH           ENTRY            WILL MATCH            WILL NOT MATCH

           DEC              DEC                   Dec or dec

           Dec              Dec                   DEC or dec

           dec              dec or DEC or Dec
           _______________________________________________________


           If there is any danger of ambiguity or confusion, use
           all upper case letters in the spelling sequence.

           DO NOT

           o  Use blanks within the spelled word.

           o  Use blanks within the phoneme sequence.

           o  Place words with more than one pronunciation into
              the user dictionary.
 


                                          Tuning the Text     2-39




           For an example of text tuning, see the DECtalk DTC03
           Text-to-Speech Owner's Manual (Appendix F) or the
           DECtalk DTC01 Programmer's Reference Manual (Chapter
           3).
 





















           DECTALK AND THE TELEPHONE
 


                                 DECtalk and the Telephone     3-3




           This chapter focuses on the telephone as an integral
           piece of the application. It is necessarily more
           technical than the other chapters in this guide.


           3.1  Developing the Application

           Unlike the Single-line DECtalk (DTC01), the Multi-line
           DECtalk (DTC03) is designed to be used primarily over
           the telephone, and has no speaker. Therefore, as you
           develop an application, you will want to listen to the
           application over a telephone line. For convenience,
           you may want to have one telephone near where you are
           developing the application, and one telephone hooked
           to the DECtalk/host computer. During the development
           process, you can then simply call the application (the
           telephone hooked to the DECtalk/host computer) from
           the nearby telephone.


           3.2  Connecting DECtalk Telephone Lines

           There are two ways to connect the telephone lines:

           1. Your telephone service supplier can connect the
              DECtalk telephone lines using an 8-line RJ21X
              telephone service (in the U.S.), or an 8-line CA21A
              telephone service (in Canada).

              This means that the first 8 pairs of the service
              are connected to the DECtalk modules and the
              remaining 17 pairs are unused. To order telephone
              connections for two racks of DECtalks, you must
              order two 8-line RJ21X or CA21A services. If fewer
              than 8 lines are used, you would specify RJ21X or
              CA21A service with the appropriate number of lines
              connected. (See the DECtalk Text-to-Speech Owner's
              Manual (Chapter 1).)
 


           3-4     DECtalk and the Telephone




              In some cases, the telephone service representative
              may not be familiar with this service. However,
              this service is a standard service, and is indeed
              orderable. You should either ask to speak to the
              supervisor, or ask the service representative to
              consult with the installation group.

           2. While waiting for RJ21X or CA21A service, a quicker
              way to connect the DECtalk telephone lines is to
              use an adapter that converts the 50-pin connector
              on the back of the DECtalk module to a standard
              4-pin modular telephone connector (RJ11). Using an
              adapter can expedite the early stages of the
              application development. (See also the DECtalk
              DTC03 Text-to-Speech System Owner's Manual (Chapter
              1), and Appendix A of this guide.)

              There are at least two companies that manufacture
              these adapters:

              o  Radio Shack (Archer Number 43-270, 4-line tap),
                 and

              o  Mod Tap, Harvard, MA 01451 (order number 512604,
                 8-line tap).

              The Archer 4-line tap is designed so that only
              three of the four connectors are usable for
              DECtalk. This tap was designed for another type of
              service, and only services every fourth line.

              The Mod Tap has 12 RJ11 modular sockets. However,
              only 8 can be used for DECtalk (because of
              DECtalk's 8-line configuration).

              NOTE
              You will ultimately need RJ21X or CA21A
              telephone service. These adapters should be used
              only while you are waiting for this service to
              be connected.
 


                                 DECtalk and the Telephone     3-5




           3.2.1  Hunt Chains

           A hunt chain is a service that acts like automatic
           call forwarding. It takes an inbound call and forwards
           it to a free line, in a predetermined sequence.
           Usually that sequence is from top down. The hunt chain
           facility enables a customer to call one number for the
           same service, regardless of the number of DECtalk
           channels operating.

           For larger installations, you should ask your
           telephone service supplier to load-balance the
           configuration. For example, if the application uses
           three DECtalk systems, they should be ordered 1-2-3,
           1-2-3, and so on, on the hunt sequence so that the
           load is equally divided among the three systems.


           3.3  Determining the Appropriate Number of Lines

           Use Tables <REFERENCE>(tabone\value) through
           <REFERENCE>(tabfour\value) in Appendix C of this guide
           to determine the number of telephone lines required.

           Typically, calls are not randomly distributed
           throughout the day, but have periods of fluctuating
           activity. The number of lines you need depends on the
           number of calls which come in during the peak calling
           period.

           You need to know:

           o  The average length of a call

           o  The call rate at peak times

           o  The acceptable blocking rate

              Blocking rate is defined as the frequency with
              which a user gets a busy signal.
 


           3-6     DECtalk and the Telephone




           An acceptable blocking rate is a subjective parameter,
           and can only be determined by the application. For
           example, if at peak loading you feel that it is
           acceptable for a person to get a busy signal once
           every ten times he or she dials, the blocking rate
           would be 10%.

           If you don't know the statistics for a proposed
           system, be conservative, and approach the system
           understanding that you may need to add more lines if
           the call loading assumptions were incorrect.


           3.4  Receiving Incoming Telephone Calls

           In receiving an incoming telephone call, DECtalk has
           special circuitry to detect the ring signal. The
           signal is standardized, and DECtalk always recognizes
           rings correctly on a standard telephone line. If you
           experience difficulty in detecting a ring, the
           following information should be useful.

           1. The sound which indicates that the remote telephone
              is ringing is not necessarily synchronized with the
              physical ringing of the telephone.

              If you ask DECtalk to pick up the telephone in one
              ring, but you hear two rings before it picks up,
              this is normal. The audible ring you hear is called
              a         , and is not related to the physical ring                ringback                ringback                ringback
              at the telephone end. Often, there is a one-ring
              delay in the system.

           2. The ring signal (40 to 120 volts) is much larger
              than the audio signal.

              On a standard telephone, the audio signal and the
              ring signal are sent over two wires, called     and                                                          tip                                                          tip                                                          tip
                  . As soon as the telephone is picked up, the              ring              ring              ring
              ring signal stops.
 


                                 DECtalk and the Telephone     3-7




           3. In party lines and some small PBXs (Private Branch
              Exchanges), the ring signal is not supplied on the
              tip and ring wires along with the audio.

              In these systems, the ring signal may be supplied
              between tip and ground, ring and ground, or on two
              separate wires.

              o  When the ring signal is supplied between tip and
                 ground or between ring and ground, DECtalk may
                 appear to recognize a ring intermittently. (The
                 voltage DECtalk sees between tip and ring is
                 unpredictable, and may or may not be sufficient
                 to be recognized.)

              o  When the ring signal is supplied on two separate
                 wires, DECtalk does not recognize it at all.

              In all the above cases, an ordinary telephone would
              not function correctly, and neither will DECtalk.
              The only situation where these conditions can exist
              on lines provided by the telephone companies is on
              party lines, which DECtalk is not allowed to
              connect to. All large PBXs supply standard signals;
              only small ones are non-standard.



           3.5  Receiving Telephone Keypad Information

           Your program must enable the keypad to receive
           Touch-Tone information. There are two different kinds
           of keypads:

           o  The standard keypad, found on most telephones, has
              the numbers 0-9, *, and #.
 


           3-8     DECtalk and the Telephone




           o  The military keypad has a fourth column of buttons.

              NOTE
              Nothing can be done to detect rotary dial pulses
              as the information is not passed through most
              central offices.


           3.6  Making Outbound Telephone Calls

           DECtalk can make either Touch-Tone or pulse dial
           outbound telephone calls. You simply ask DECtalk to
           dial while supplying it with the telephone number and
           dialing commands. It is possible to switch from pulse
           dialing to tone dialing within a call, but in general
           this is not advisable. (The Run-Time Library
           DTK$DIAL_PHONE routine makes outbound calls. The valid
           modes are DTK$K_DIAL_PULSE for pulse dialing (the
           default), and DTK$K_DIAL_TONE for tone dialing.)


           3.6.1  Using the Wait Command

           Besides supplying DECtalk with a number to dial, you
           can also supply it with various parameters: for
           example, one parameter to use in normal dialing,
           another parameter to use for call progress detection,
           and a third parameter to use to transfer a call.

           Within the dialing string, you can use the                                                      wait                                                      wait                                                      wait
                   to ask DECtalk to wait for a dial tone. (The           command           command           command
           character   in the dialing text string is referred to                     W                     W                     W
           as the wait command.) You can place a maximum waiting
           period value, in seconds, after the W by enclosing the
           number in angle brackets. The W range is from 10 to
           120 seconds, with a default of 30 seconds.
 


                                 DECtalk and the Telephone     3-9




           It is advisable to wait for a dial tone, or use a
           one-second pause, before dialing a number. Otherwise
           your call could fail for one of the following reasons:

           1. You could start dialing before the system is ready
              to receive the digits.

           2. When you are connected to a PBX where you typically
              dial 9 to get an outside line, you might dial too
              rapidly for the telephone system, and fail on the
              dial attempt.

           If you have dial tones that DECtalk cannot recognize,
           then you should insert a wait command.

           To create a longer wait, you can add more than one
           pause command.

           If a dial tone is not received within one second of
           initiation of a call, then you should use a two- or
           even three-second pause.


           3.6.2  Determining When Connection Has Been
                  Established

           Determining when a call has been established is
           difficult, and varies with the application. DECtalk
           cannot determine with certainty when a call is
           completed and cannot identify that a human has
           answered. It cannot differentiate between a noise and
           someone saying "Hello."

           There are three main ways to determine when a call has
           been established:

           1. Using call progress detection

           2. Using timeouts
 


           3-10     DECtalk and the Telephone




           3. Asking the user to press a Touch-Tone key

           The character   in the dialing text string performs                         X                         X                         X
           call progress detection. The maximum waiting time for
           call progress detection can be specified by a value
           placed between angle brackets. The X range is from 10
           to 120 seconds, with a default of 30 seconds.

           Call progress detection is done by looking at the
           level of noise received on the telephone line. If the
           noise is above a certain threshold, it is considered
           to be ON, and if below the threshold, it is considered
           to be OFF.

           Call progress detection works on all but very noisy or
           very quiet telephone connections. DECtalk listens to
           the noise, and then tries to identify the cadence.
           Nonsensical cadences or sequences of cadences are
           treated as voice. An example of a nonsensical
           arrangement is a ringback, a busy signal, and a
           reorder.

           There are a few cadences that DECtalk decodes as a
           ring:

           1. The most common cadence is the standard ring (a
              cadence of between 0.8 and 2.4 seconds ON, with a
              total time of 5.9 to 6.1 seconds).

           2. A faster ring has the same ON cadence, with a total
              time of between 2.9 and 3.1 seconds.

           3. Two types of split rings are detected as well:

              o  The first split ring has a cadence of between .9
                 and 1.1 seconds ON (both tone sections), with a
                 total time for the first half of between 1.4 and
                 1.5 seconds, and a total time for the second
                 half of between 4.4 and 4.6 seconds.
 


                                DECtalk and the Telephone     3-11




              o  The second split ring has a cadence of between
                 .3 and .8 seconds ON (both tone sections), with
                 a total time for the first half of between .95
                 and 1.05 seconds, and a total time for the
                 second half of between 1.05 and 6.1 seconds.

           A             is a cadence with between .3 and .8             busy signal             busy signal             busy signal
           seconds ON, with a total cadence of between .95 and
           1.05 seconds. A         is a cadence with between .05                           reorder                           reorder                           reorder
           and .3 seconds ON, and a total time of between .45 and
           .55 seconds.)

           The main utility of call progress is the ability to
           abandon a failed call more quickly than with timeouts.
           However, timeouts should be used in all cases as a
           backup.

              NOTE
              The only sure way to determine that a call has
              been established is to ask the user to press a
              Touch-Tone key.


           Using Call Progress Detection

           Call progress detection on the telephone is difficult.
           The signals are made for people, not machines, to
           recognize. Call progress in DECtalk is a compromise;
           improvements are possible. For example, it is
           technically possible to differentiate between a voice
           signal and a non-voice signal. Some automatic calling
           units provide this functionality, but this type of
           functionality is not provided here.

           DECtalk uses cadence detection only. Cadence detection
           looks for an incoming signal and determines what it is
           by its rhythm. It is accurate for identifying dial
           tones, ring signals, busy signals, and so forth, but
           cannot differentiate human speech well. In addition,
           it can take time to make a determination. Rings
 


           3-12     DECtalk and the Telephone




           normally cycle at a six-second rate. If you happen to
           connect when a ring has just completed, it could take
           up to 10 seconds for DECtalk to identify that a ring
           had occurred.

           Call progress in DECtalk is designed to fail in the
           direction of declaring a call complete when it is not,
           rather than to complete a call and not know it.
           Nevertheless, it can sometimes fail to detect
           completion. For example, call progress can fail if
           someone answers the telephone at the same time as the
           ring signal occurs. The chances of this happening are
           rare, however.

           If call completion verification is needed, then you must
           also send a message to ask the user to press a Touch-Tone
           key to indicate that someone is present at the other end
           of the telephone. This message must be repeated in the event
           you get a completion status that was actually noise, and
           the real telephone is still ringing at that moment. In ad-
           dition, you need to consider other issues when dealing with
           outbound calls. If you are dialing into a business, for
           example, you may get a secretary, or be call-forwarded to
           someone who is not familiar with the application, and does
           not know what you want. You must carefully analyze the di-
           alog and the feasibility of such an application.


           When Call Progress Detection Fails

           In some instances, you will not be able to use the
           call progress feature. In certain PBXs the signals do
           not conform to the recommended guidelines, and DECtalk
           will not be able to recognize the signals. In this
           situation, you must rely on timeouts.

           In other cases, you may know that a particular problem
           exists, such as a burst of noise occurring when you
           dial a particular exchange or area. In this situation,
 


                                DECtalk and the Telephone     3-13




           you may wish to dial a part or all of the number
           before enabling call progress detection.


           3.7  Transferring Telephone Calls

           Transferring a call with DECtalk is simple, provided
           that the telephone equipment that DECtalk is connected
           to supports this function. DECtalk transfers a call
           the same way a human does, with a switch hook flash. A
           switch hook flash is accomplished by briefly
           depressing the switch hook on the telephone, and
           releasing it. The telephone system senses this and
           gives you a dial tone. You then dial the same as you
           normally would.

           DECtalk provides a base switch hook flash of 250 millisec-
           onds. In the event that one flash is not sufficient, switch
           hook flashes can be added together to provide a longer flash.
           The intent is to provide 250 milliseconds granularity on
           the length of a switch hook flash with the assumption that
           one flash is not sufficient. The desired length of the flash
           varies from switch to switch, even among the same brand
           of switch, because within most switches the parameter is
           programmable. The preferred way of setting the length is
           to ask someone who is familiar with the switch. If this
           information is not readily available, you can increase the
           length by increments of 250 milliseconds until the switch
           notices a flash. Continue until the switch thinks the tele-
           phone has been hung up, and set the time half way between
           these values. The value will usually be between one-half
           and two seconds.

           To complete a call transfer, DECtalk must be hung up.
 


           3-14     DECtalk and the Telephone




           3.8  Busying Out a Line (DTC03 only)

           DECtalk provides two ways to busy out a line:

           1. Through the DM pushbutton

           2. Through control of EIA pin 18

           Switch SW1 must be closed (on) to enable either the DM
           pushbutton or pin 18. DECtalk is shipped with this pin
           18 functionality disabled. In addition, the local
           telephone company should be notified before enabling
           this functionality. Certain types of switches suffer
           performance degradation if a line is busied out. (See
           the DECtalk DTC03 Text-to-Speech Owner's Manual
           (Chapter 1).)


           3.9  Terminating the Current Telephone Call

           A telephone call can be terminated for one of three
           reasons:

           1. The session has ended in a normal flow and the
              application knows that the session is complete and
              simply hangs up the phone.

           2. The system has not had any interaction with the
              user for a period of time, assumes that the user is
              no longer there, and hangs up.

           3. The telephone detects that the caller has hung up
              and terminates before waiting for a time out.

           The latter two cases are considered abnormal
           terminations.

           To terminate the telephone session, use the DECtalk
           Run-Time Library DTK$HANGUP_PHONE routine.
 


                                DECtalk and the Telephone     3-15




           3.10  Detecting Premature Hangups

           Many, but not all telephone systems, provide a means
           of detecting that a caller has hung up the phone. This
           information is provided by a      (a momentary                                        wink                                        wink                                        wink
           interruption or reversal of the line current).
           Unfortunately, not all systems provide this function.
           Furthermore, taking the telephone off-hook may cause
           winks for a second or two.

           You must determine whether the telephone system
           provides the wink function.

           1. One method for doing this is to have someone call
              you. When the person is ready to hang up, blow into
              the mouthpiece. If you hear a dropout, or gap, in
              the noise in the earpiece, this means that the line
              winked when the calling party hung up.

           2. Another method is to enable wink detection in
              self-test mode, call up DECtalk, hang up, and look
              for a wink reply when you hang up.

           The DECtalk Run-Time Library DTK$ANSWER_PHONE routine
           automatically enables wink detection. There is,
           however, no Run-Time Library routine to disable wink
           detection. Your program can ignore spurious winks by
           ignoring the status of DTK$_WINK that is returned when
           a wink is detected.

           It is important to know that a wink can occur at call
           establishment. Therefore, it is recommended that you
           ignore winks at the very beginning of a session.

           The DECtalk Run-Time Library has a built-in delay
           before enabling winks. In most cases this built-in
           delay should be sufficient.
 


           3-16     DECtalk and the Telephone




              NOTE
              Timeouts should always be used as a backup as
              you should never totally rely on the occurrence
              of winks.
 





















           THE DECTALK DEMONSTRATION PROGRAM
 


                         The DECtalk Demonstration Program     4-3




           4.1  Overview

           The demonstration program, provided in a separate
           volume, is a typical telephone application program
           written for the DECtalk speech synthesizer. Each of
           the VAX COBOL, VAX BASIC, and VAX C implementations
           uses Version 4.5 of the VMS/MicroVMS Run-Time Library
           Support provided for DECtalk. You can quickly modify
           the code to produce a customized demonstration
           program. In addition, you can use much of the code for
           actual application development.

           The demonstration program implements an information
           service demonstration that supplies the Boston weather
           forecast, transportation information, ski conditions,
           and mortgage rate information.

           The program begins by setting up a communication
           channel between DECtalk and the physical device line
           (terminal line), and assigning it a unique device
           identifier.

           The program then initializes application-specific
           parameters. (The program enables the left and right
           square brackets as phonemic delimiters, selects the
           speaking voice and speaking rate, and loads the user
           dictionary.)

           As soon as initialization is completed, the program
           enables the telephone for autoanswer. The
           demonstration program is then ready to accept user
           telephone calls.

           When the user dials into the demonstration program,
           DECtalk answers the telephone on the first ring with
           the following greeting.
 


           4-4     The DECtalk Demonstration Program




                Welcome to the DECtalk Information Services demonstration.
                Please enter your access code followed by the pound key.
                You may enter any number as a test.

           The user is given three attempts to enter a valid
           access code. The demonstration program accepts any
           access code followed by the pound sign key ( ). In an                                                       #                                                       #                                                       #
           actual application, the access code would be verified
           against a database containing valid user access codes
           for the system.

           If the user fails to enter an access code terminated
           by the pound sign key ( ) before the timeout period                                  #                                  #                                  #
           expires, or enters an access code terminated by the
           star key ( ), DECtalk speaks the following message:                     *                     *                     *

                Invalid entry.  Please try again.
                Please enter your access code followed by the pound key.
                You may enter any number as a test.

           If the user fails to enter an access code in three
           attempts, DECtalk notifies the user that access to the
           system is denied.

                Access denied.
                Please check your access code and password and try again.
                Thank you for calling the DECtalk application demonstration program.
                Have a nice day.

           When the user has successfully entered the access
           code, DECtalk then prompts for the associated
           password:

                Please enter your password followed by the pound key.
                You may enter any number as a test.
 


                         The DECtalk Demonstration Program     4-5




           Once again, the user is given three attempts to enter
           a password. The demonstration program accepts any
           password followed by the pound sign key ( ). In an                                                    #                                                    #                                                    #
           actual application, the password would be verified
           against the password associated with the access code
           previously entered. This information would reside in a
           database.

           If the user fails to enter a password terminated by
           the pound sign key ( ) before the timeout period                               #                               #                               #
           expires, or enters a password terminated by the star
           key ( ), DECtalk speaks the following message:                *                *                *

                Invalid entry.  Please try again.
                Please enter your password followed by the pound key.
                You may enter any number as a test.

           If the user fails to enter a password in three
           attempts, DECtalk notifies the user that access to the
           system is denied:

                Access denied.
                Please check your access code and password and try again.
                Thank you for calling the DECtalk application demonstration program.
                Have a nice day.

           When the user has successfully logged into the system,
           DECtalk prompts for a command:

                Please enter a command.
                For help, press 0 followed by the pound key.
                To exit, press the star key.

           This prompt supplies the user with a minimum amount of
           information about using the system. For an expert
           user, this information is sufficient. However, for a
           first-time user of the system, you should provide a
           description of all available commands. When the user
           presses key 0 (the help key), DECtalk speaks the
           following message:
 


           4-6     The DECtalk Demonstration Program




                To hear the current Boston weather forecast, press 1.
                To hear transportation information, press 2.
                To hear mortgage rate information, press 3.
                To hear the current ski conditions, press 4.
                To repeat this message, press 0.
                Terminate your entry with the pound key.
                To exit, press the star key.

           When the user presses a valid Touch-Tone keypad
           command (the star key ( ) or any Touch-Tone key                                  *                                  *                                  *
           followed by the pound sign key ( )), DECtalk speaks                                           #                                           #                                           #
           the text associated with that key.

           o  If the keypad command is not followed by the pound
              sign key, DECtalk waits until the timeout period
              expires. DECtalk then speaks a warning message to
              the user.

           o  If the user presses the pound sign key, the command
              is processed immediately.

           o  If the user enters an invalid command, DECtalk
              speaks the following message:

                   Invalid entry.  Please try again.
                   Please enter a command.
                   For help, press 0 followed by the pound key.
                   To exit, press the star key.

           o  If the user fails to press a key before the timeout
              period expires, DECtalk informs the user, speaks a
              goodbye message, and hangs up the telephone:

                   No key pressed in the timeout period specified.
                   Thank you for calling the DECtalk application demonstration program.
                   Have a nice day.
 


                         The DECtalk Demonstration Program     4-7




           o  If the user exits the system normally (by pressing
              the star key), or if DECtalk detects a wink on the
              telephone line, DECtalk speaks a goodbye message to
              the user, and hangs up the telephone:

                   Thank you for calling the DECtalk application demonstration program.
                   Have a nice day.

              When the current user session ends, the program
              reenables the telephone for autoanswer, and waits
              for the next user to call in.



           4.2  Installing the Demonstration Program

           Use VMSINSTAL, the standard VMS installation facility,
           to install the demonstration software on your system.
           The source code for the demonstration program (in VAX
           COBOL, VAX BASIC, and VAX C), the object code, the
           executable images, the dictionary file, and the header
           files are included on the distribution media in BACKUP
           format.


           4.2.1  DECtalk Demonstration Program Files

           Table 4-1 lists the files contained on the
           distribution media.
 


           4-8     The DECtalk Demonstration Program




           _______________________________________________________
           Table 4-1 DECtalk Demonstration Program Files           Table 4-1 DECtalk Demonstration Program Files           Table 4-1 DECtalk Demonstration Program Files
           _______________________________________________________
           _______________________________________________________           File                       Description           File                       Description           File                       Description

           BASIC_DTDEMO.BAS           Source code for BASIC
                                      demonstration program.

           BASIC_DTDEMO.EXE           Executable image for BASIC
                                      demonstration program.

           BASIC_DTDEM.OBJ            Object code for BASIC
                                      demonstration program.

           BAS.DTDEMO.COM             Startup command file for
                                      BASIC demonstration
                                      program.

           COBOL_DTDEMO.COB           Source code for COBOL
                                      demonstration program.

           COBOL_DTDEMO.EXE           Executable image for COBOL
                                      demonstration program.

           COBOL_DTDEMO.OBJ           Object code for COBOL
                                      demonstration program.

           COB_DTDEMO.COM             Startup command file for
                                      COBOL demonstration
                                      program.

           C_DTDEMO.C                 Source code for C
                                      demonstration program.

           C_DTDEMO.EXE               Executable image for C
                                      demonstration program.

           C_DTDEMO.OBJ               Object code for C
                                      demonstration program.
 


                         The DECtalk Demonstration Program     4-9



           _______________________________________________________
           Table 4-1 (Cont.) DECtalk Demonstration Program Files           Table 4-1 (Cont.) DECtalk Demonstration Program Files           Table 4-1 (Cont.) DECtalk Demonstration Program Files
           _______________________________________________________
           _______________________________________________________           File                       Description           File                       Description           File                       Description

           C_DTDEMO.COM               Startup command file for C
                                      demonstration program.

           DICTIONARY.USER            User dictionary file
                                      containing words for
                                      demonstration program.

           DTDEMO.CFG                 Configuration file for
                                      demonstration program.

           DTKDEF.BAS                 Header file for BASIC
                                      demonstration program.

           DTKDEF.H                   Header file for C
                                      demonstration program.

           DTKDEF.LIB                 Header file for COBOL
                                      demonstration program.

           KITINSTAL.COM              Command file called by
                                      VMSINSTAL.

           README.1ST                 Description of files
                                      contained in the kit.
           _______________________________________________________




           4.3  Running the VMSINSTAL Procedure

           The VMSINSTAL procedure installs the DECtalk
           demonstration program.
 


           4-10     The DECtalk Demonstration Program




           To install the DECtalk demonstration program, follow
           these steps:

              NOTE
              The following is a sample installation dialogue.
              The actual dialogue on your system may vary
              slightly.

           1. Load the distribution tape onto a tape drive.

           2. Log in to the VMS system manager's account (or any
              other privileged account).

           3. Run VMSINSTAL by entering the following command:

                   $ @SYS$UPDATE:VMSINSTAL DTS001 $TAPE1:

              Press the <RETURN> key.

              The following message and prompt will appear on
              your screen:

                   VAX/VMS Software Product Installation Procedure V4.5

                   It is DD--MON--YEAR at HH:MM

                   Enter a question mark (?) at any time for help.

                   * Are you satisfied with the backup of your system disk [YES] ?

              (You should back up your system disk before any
              major software installation. If your system disk
              was backed up recently, press the <RETURN> key or
              type     to continue.)                   YES                   YES                   YES

              The following message and prompt will appear on
              your screen:

                   Please mount the first volume of the set on $TAPE1:

                   * Are you ready?
 


                        The DECtalk Demonstration Program     4-11




              When you have loaded the tape, type     to                                                  YES                                                  YES                                                  YES
              continue.

              The following messages will appear on your screen:

                   The following products will be processed:

                   DTS V0.1

                   Beginning installation of DTS V0.1 at HH:MM

                   Restoring product saveset A . . .

           4. The following prompt will appear on your screen:

                   * target directory for files to be copied. no brackets:

              Type the name of the system directory that you want
              to contain the DECtalk demonstration program files.

              The following messages will appear on your screen:

                   This product creates system directory [directoryname]

                   %VMSINSTAL--I--MOVEFILES, Files will now be moved to their target
                   directories . . .
                   Installation of DTS V0.1 completed at HH:MM

                   VMSTINSTAL procedure done at HH:MM

                 NOTE
                 If you simply enter                       to                                     @SYS$UPDATE:VMSINSTAL                                     @SYS$UPDATE:VMSINSTAL                                     @SYS$UPDATE:VMSINSTAL
                 run VMSINSTAL, the following message will
                 appear on your screen when the installation
                 is complete:

                   Enter the products to be processed from the next distribution volume set.

                   * Products:
 


           4-12     The DECtalk Demonstration Program




              At the Products: prompt, type      and press the                                            EXIT                                            EXIT                                            EXIT
              <RETURN> key, or press <CTRL/Y>.

                 NOTE
                 The phrase press <CTRL/Y> indicates that you
                 should press the key labeled <CTRL> while you
                 simultaneously press the key labeled <Y>.



           4.4  Running the Demonstration Program

           To run the demonstration program, follow these steps:

           1. Set the default directory to the system directory
              specified in the VMSINSTAL procedure.

              Alternatively (for application development and
              testing), you may want to move the files contained
              in the system directory to a non-privileged
              account, and then set the default directory to this
              non-privileged account.

           2. Create a configuration file that contains one line
              per terminal, as shown in the following (sample)
              format:

                   $!
                   $    txa4: dictionary.user  ! DECtalk connected to txa4:
                   $    txa7: dictionary.user  ! DECtalk connected to txa7:
                   $!

              In this example,       and       are the terminal                               txa4:     txa7:                               txa4:     txa7:                               txa4:     txa7:
              lines that DECtalk is connected to, and
                              is the name of the user dictionary.              dictionary.user              dictionary.user              dictionary.user

              Text following an exclamation point is a comment
              and is not processed.
 


                        The DECtalk Demonstration Program     4-13




              A sample configuration file, DTDEMO.CFG, is
              supplied on the tape.

                 NOTE
                 You must specify the terminal line in the
                 configuration file, but the dictionary
                 filename is an optional parameter.

           3. Set the device protection of each of the terminal
              lines specified in the configuration file to WORLD
              READ/WRITE access.

              For example, for the above configuration, type:

                   $ SET PROTECTION=(W:RW)/DEVICE txa4:

                   $ SET PROTECTION=(W:RW)/DEVICE txa7:

              (Consult the DCL manual for more detailed
              information.)

              On some machines running VMS, if the terminal line
              connected to the DECtalk module is running at
              high-speed (9600 baud) and the terminal line is not
              allocated, the program fails with a Device Timeout
              error.

                 NOTE
                 After the terminal line has been allocated,
                 the program may still fail with a Device
                 Timeout error the first time it is run.
                 However, it should not fail the second, and
                 subsequent times it is run.

              An alternative but less desirable solution to a
              "Device Timeout" error is to lower the baud rate on
              both the terminal line and the DECtalk module.

           4. Set up an operator terminal to receive the "OPER12"
              type of message.
 


           4-14     The DECtalk Demonstration Program




              To have error messages logged to the console
              terminal, or any other terminal, type the
              REPLY/ENABLE command at the DCL ($) prompt:

                   $ REPLY/ENABLE=OPER12

                 NOTE
                 To enter the REPLY/ENABLE command, you must
                 have OPERATOR privileges.

           5. Run the appropriate command procedure for the
              programming language you choose.

              Type     of the following commands:                   one                   one                   one

                   $ @COB_DTDEMO configuration.file
                   $ @BAS_DTDEMO configuration.file
                   $ @C_DTDEMO configuration.file

              where                    is the name of the                    configuration.file                    configuration.file                    configuration.file
              configuration file. (COB_DTDEMO is the COBOL
              startup command file, BAS_DTDEMO is the BASIC
              startup command file, and C_DTDEMO is the C startup
              command file.)

              When you have completed this step, the
              demonstration program should be up and running. You
              can then call one of the DECtalk modules to verify
              operation, and to familiarize yourself with the
              operation and functionality of the DECtalk
              demonstation application program.

              The demonstration program runs on each terminal
              line as a detached job. The process name given to
              each process is DEMO_TXyy, where TXyy is the
              terminal line that the DECtalk module is connected
              to.
 


                        The DECtalk Demonstration Program     4-15




              All messages written to SYS$OUTPUT and SYS$ERROR
              are written to an error file, TXyy.err, where again
              TXyy is the terminal line that the DECtalk module
              is connected to.



           4.5  Terminating the DECtalk Demonstration Program

           The DECtalk demonstration program terminates
           automatically when a fatal error occurs, unless the
           error is a timeout error that is generated when the
           system timer expires. (The system time is set by
           SYS$SETIMR.) A timeout error causes the demonstration
           program to hang until you stop the process.

           To stop a process, either normally or because a
           non-recoverable error has occurred, type the following
           command:

                $ stop DEMO_TXyy

           DEMO_TXyy (where TXyy is the terminal line that the
           DECtalk module is connected to) is the name of the
           process. If you issue this command while a customer is
           currently engaged with the demonstration program, the
           customer will be disconnected and the demonstration
           program will terminate.

           After the process has been stopped, type the following
           DCL command:

                $ copy TXyy: nla0:

           where, again, TXyy: is the name of the terminal line
           that the DECtalk is connected to.
 


           4-16     The DECtalk Demonstration Program




           After about 5 seconds, press either <CTRL/C> or
           <CTRL/Y>. (The copy command hangs until either of
           these commands is entered.) Pressing <CTRL/C> or
           <CTRL/Y> should cause the DCL prompt ($) to be
           displayed again.

              NOTE
              The phrase press <CTRL/C> indicates that you
              should press the key labeled <CTRL> while you
              simultaneously press the key labeled <C>.
              Similarly, press <CTRL/Y> indicates that you
              should press the key labeled <CTRL> while you
              simultaneously press the key labeled <Y>.

           The copy command gets rid of any characters that may
           have been being transmitted when the STOP TXyy command
           was entered.


           4.6  Detailed Description

           4.6.1  Initializing the Hardware

           An application program cannot send any commands or
           text to DECtalk before a communication channel between
           the DECtalk module and the physical device line
           (terminal line) is established.

           To establish this communication channel, the
           application program must:

           1. Obtain the name of the physical device (terminal
              line) to which the DECtalk module is connected.

              The demonstration program obtains the name of the
              physical device from the configuration file when
              you run the appropriate command procedure.

           2. Invoke the DTK$INITIALIZE Run-Time Library routine
              with the name of the physical device.
 


                        The DECtalk Demonstration Program     4-17




              This routine sets up the communication channel
              between the DECtalk module and the physical device,
              and assigns a unique identifier to it. This
              initialization is performed once for each DECtalk
              module/physical device connection.


           4.6.2  Initializing the Application-Specific
                  Parameters

           The initialization of application-specific parameters
           should also take place before DECtalk begins accepting
           user telephone calls. The actual DECtalk parameters
           that are initialized vary among applications. However,
           the parameters that are initialized should be
           initialized when the application is invoked as well as
           everytime the DECtalk module has power-cycled. (See
           Section 4.6.4 "Verifying DECtalk Operation.")

           The initialization of the application-specific
           parameters in the demonstration program is performed
           by a single subroutine.

           This subroutine:

           o  Enables square bracket phonemic mode

           o  Selects the speaking voice and speaking rate

           o  (Optionally) loads the user dictionary


           Square Bracket Phonemic Mode

           Square bracket phonemic mode allows you to send
           phonemic text, as well as commands that modify the
           voice characteristics, enclosed in square brackets,
           rather than as escape sequences.
 


           4-18     The DECtalk Demonstration Program




           The demonstration program enables '[' and ']' as
           phonemic delimiters by invoking the DTK$SET_MODE
           Run-Time Library routine with the mode set to
           DTK$M_SQUARE.

           It is not necessary to enable '[' and ']' as phonemic
           delimiters in a DECtalk application. Phonemic text can
           be sent to DECtalk using the DTK$SPEAK_PHONEMIC_TEXT
           Run-Time Library routine. The voice characteristics
           can be modified by issuing a call to the DTK$SET_VOICE
           Run-Time Library routine.

           However, when the square brackets are used as phonemic
           delimiters, it is possible to get into phonemic mode
           unintentionally. DECtalk is left in a state where it
           tries to interpret text phonemically when:

           o  The text contains an unexpected left square bracket
              '[', or does not contain a right square bracket ']'
              after a phonemic entry

           o  The telephone keypad is enabled in autostop keypad
              mode, and a Touch-Tone key signal is received while
              DECtalk is speaking phonemic text delimited by '['
              and ']'

           If DECtalk is left in phonemic mode, it garbles speech
           (because it is trying to interpret text phonemically),
           discarding phonemically illegal letters. DECtalk keeps
           track of the number of extra left square brackets in
           the input relative to the number of right square
           brackets it has seen. If text happens to have one or
           more extra left square brackets, you may need to type
           several right square brackets to get DECtalk out of
           phoneme-input mode.
 


                        The DECtalk Demonstration Program     4-19




           Speaking Voice and Speaking Rate

           The demonstration program initializes the default
           speaking voice and the default speaking rate by
           issuing a call to the DTK$SET_VOICE Run-Time Library
           routine. The program selects the normal male voice
           (DTK$K_VOICE_MALE) as the speaking voice, and 180
           words per minute as the speaking rate.
 


           4-20     The DECtalk Demonstration Program




           The valid voice settings are given below in Table 4-2.
           Although several voices are available, the most
           intelligible voices are:

           o  DTK$K_VOICE_MALE

           o  DTK$K_VOICE_FEMALE

           o  DTK$K_VOICE_DEEP_MALE


           _______________________________________________________
           Table 4-2 Valid Voice Mode Settings           Table 4-2 Valid Voice Mode Settings           Table 4-2 Valid Voice Mode Settings
           _______________________________________________________
           _______________________________________________________           VOICE                           SETTING               MNEMONIC           VOICE                           SETTING               MNEMONIC           VOICE                           SETTING               MNEMONIC

           DTK$K_VOICE_MALE                Standard male voice   Paul

           DTK$K_VOICE_FEMALE              Standard female       Betty
                                           voice

           DTK$K_VOICE_CHILD               Standard child        Kit
                                           voice

           DTK$K_VOICE_DEEP_MALE           Deep male voice       Harry

           DTK$K_VOICE_DEEP_FEMALE         Deep female voice     Rita

           DTK$K_VOICE_OLDER_MALE          Older male voice      Frank

           DTK$K_VOICE_LIGHT_FEMALE        Light female voice    Wendy
           _______________________________________________________


           Valid speaking rate settings for DECtalk range from
           120 to 350 words per minute. It is recommended that
           the default speaking be set between 160 and 200 words
           per minute. Any slower or faster speaking rate
           decreases the intelligibility of the spoken text.
 


                        The DECtalk Demonstration Program     4-21




           Loading the User Dictionary

           The user dictionary can be optionally loaded as part
           of the application-specific parameter initialization.
           The user dictionary is primarily used for processing
           abbreviations (initialisms) and for providing
           equivalents of unusual or application-specific words.
           (For a description of the format of the user
           dictionary, see Section  2.2.3 of this guide.)

           The demonstration program loads the user dictionary if
           you specify a file name in the configuration file
           (which is invoked when you run the command procedure).

           If you do not specify a filename in the command
           string, the demonstration program assumes that no
           words are to be loaded into the user dictionary. (See
           Section  2.2.3 of this guide.)

           The subroutine used to load words and phonemic
           pronunciations into the user dictionary reads a line
           of text from the file specified, and parses the text
           line storing the word to be defined and its phonemic
           pronunciation into separate arrays. Then, the word and
           phonemic pronunciation are loaded into the user
           dictionary using the DTK$LOAD_DICTIONARY Run-Time
           Library routine.

           After DECtalk receives a DTK$LOAD_DICTIONARY command,
           it returns one of three status messages:

           1. A successful status message is returned if the word
              has been loaded into the user dictionary.

           2. If the word is not loaded into the user dictionary,
              one of two error messages is returned:

              o  The DTK$_TOOLONG error message indicates the
                 dictionary entry was too long.
 


           4-22     The DECtalk Demonstration Program




              o  The DTK$_NOROOM message indicates there is no
                 room left in the user dictionary.

                 Generally, neither of these errors is fatal.
                 However, this demonstration program treats both
                 errors as fatal errors. In developing a
                 customized demonstration program or application
                 program, if words cannot be loaded into the user
                 dictionary, it is better for you to be notified
                 before the application goes on-line so that the
                 error can be corrected. Once the error
                 generating either of these error messages is
                 corrected, it should not occur again.

           The DTK$LOAD_DICTIONARY command does not have a
           timeout parameter as part of its argument list.
           However, the demonstration program sets a watchdog
           timer prior to loading the user dictionary.

           As many words are being loaded into the user
           dictionary, the demonstration program sets the
           watchdog timer before entering a loop that loads all
           the word/phonemic pronunciation pairs from a specified
           file into the user dictionary. (Alternatively, the
           program could have been designed to set the watchdog
           timer prior to loading each word into the user
           dictionary.)

           If all the word/phonemic pronunciation pairs are not
           loaded into the user dictionary in this time period
           (no status returned from DECtalk) a timeout error is
           reported and an error message is output to the
           operator's terminal. If all entries are successfully
           loaded into the user dictionary, then the watchdog
           timer is canceled.
 


                        The DECtalk Demonstration Program     4-23




           4.6.3  Receiving Incoming Telephone Calls

           When the hardware and the application-specific
           parameters have been initialized, DECtalk can be
           enabled to answer incoming telephone calls.

           The demonstration program enables DECtalk for
           autoanswer by calling the DTK$ANSWER_PHONE Run-Time
           Library routine.

           In the call to DTK$ANSWER_PHONE, you can specify up to
           three optional parameters:

           1. You can specify the number of rings that DECtalk
              waits before answering the telephone.

              By default, DECtalk answers the telephone on the
              first ring. Digital Equipment Corporation
              recommends that you use the default number of
              rings. Answering the telephone after a greater
              number of rings decreases utilization of the
              telephone line.

           2. You can specify a greeting message to be spoken to
              the user when the telephone is answered.

           3. You can specify a timeout parameter.

              Specifying this parameter causes the
              DTK$ANSWER_PHONE routine to return a timeout status
              if the telephone has not been answered in the
              specified timeout period.

              If a timeout occurs, your program can use the
              timeout feature to verify the operation of the
              DECtalk hardware, and correct any problems that may
              exist. (See Section 4.6.4, "Verifying DECtalk
              Operation.")
 


           4-24     The DECtalk Demonstration Program




           When a user calls the demonstration, DECtalk answers
           the telephone after it has waited the number of rings
           specified in the DTK$ANSWER_PHONE Run-Time Library
           call.

           A greeting message, if specified, is spoken to the
           user.

           In some applications, the use of autostop keypad mode
           may not be suitable. The DTK$ANSWER_PHONE Run-Time
           Library routine automatically enables autostop keypad
           mode and wink detection. Therefore, if your
           application does not require autostop keypad mode, it
           can be disabled by one of the following methods:

           o  Turning on the keypad without autostop mode.

              Your program can call the DTK$SET_KEYPAD_MODE
              Run-Time Library routine with the mode set to
              DTK$K_KEYPAD_ON.

           o  Disabling the keypad altogether.

              Your program can call the DTK$SET_KEYPAD_MODE
              Run-Time Library routine with the mode set to
              DTK$K_KEYPAD_OFF.

              The call to DTK$SET_KEYPAD_MODE should be made
              immediately after the call to DTK$ANSWER_PHONE has
              returned.

           The DTK$ANSWER_PHONE routine also automatically
           enables wink detection. Winks are caused by transients
           on the telephone line. On most telephone exchanges,
           one or more winks are generated when a caller hangs up
           the telephone on DECtalk. However, winks can be
           generated at other times during the telephone call.
 


                        The DECtalk Demonstration Program     4-25




           The demonstration program naively assumes that winks
           indicate that the caller has hung up. Therefore,
           detection of a wink causes DECtalk to hang up the
           telephone and terminate the current user session. If
           spurious winks occur that do not indicate that the
           caller has hung up, this assumption could cause
           DECtalk to hang up on the caller before the session
           has completed.


           4.6.4  Verifying DECtalk Operation

           Since the DTK$ANSWER_PHONE Run-Time Library routine
           allows you to specify an optional timeout parameter,
           your program can verify the operation of the DECtalk
           hardware and the connection between the DECtalk and
           the terminal line. Specifying this parameter causes
           the DTK$ANSWER_PHONE routine to return a timeout
           status if the telephone has not been answered in the
           specified timeout period.

           Unfortunately, the current version of the Run-Time Library
           Support for DECtalk does not provide the "Extended Device
           Status Request" command. Therefore, there is no sure way
           to determine whether the DECtalk module has power-cycled.
           Thus, the demonstration program reinitializes the application-
           specific parameters every time a telephone call has not
           been received in a certain time period (that is, the time-
           out status has been received). Normally, your program could
           check the status of the DECtalk module at this point, and
           reinitialize the application-specific parameters only if
           the module has power-cycled. (See also Appendix B of this
           guide.)
 


           4-26     The DECtalk Demonstration Program




           4.6.5  Verifying the User's Access

           When DECtalk has answered the telephone, and has
           spoken the greeting message, you may need to verify
           the caller as a valid system user.

           The demonstration program allows the caller three
           attempts to enter a valid access code. If a valid
           access code is entered on any of the three attempts,
           the program then gives the caller three attempts to
           enter a password. If the caller fails to enter a valid
           access code or a valid password in the specified
           timeout period, the caller is notified that access has
           been denied, and DECtalk hangs up its telephone.

           The demonstration program accepts any combination of
           key digits followed by the pound sign key as valid
           access codes and valid passwords. Access is denied
           only if a user fails to enter an access code and
           password within the specified timeout period, or
           terminates the access code or password with the star
           key.

           If DECtalk detects a wink while the program is waiting
           for the user to press a string of Touch-Tone keys, it
           speaks a goodbye message to the user, and hangs up the
           telephone.


           4.6.6  Creating Voice Menus

           Although voice menus vary between applications, they gen-
           erally follow a similar format. Usually the caller is prompted
           to enter a command on the telephone keypad. When the caller
           has pressed one or more Touch-Tone keys to enter a selec-
           tion, some action is taken. Usually, either a passage of
           text is spoken to the user, information is retrieved from
           a database, formatted, and spoken to the user, another sub-
           menu is entered, or the current menu is exited. Then the
           caller is prompted for another command. This sequence of
 


                        The DECtalk Demonstration Program     4-27




           events continues until the caller presses the exit key,
           hangs up the telephone, or until an error occurs.

           The demonstration program receives commands from the
           caller until either the exit key is pressed, a wink is
           detected, or a timeout or some other error occurs. It
           is an example of a single-level menu system.

           Before prompting for a command, any keys that the user
           may have typed ahead are read from the type-ahead
           buffer using a very short (1-second) timeout. (Expert
           users of a system enter commands before being prompted
           for a command.) If a valid command has been received
           from the caller, it is processed immediately.
           Otherwise, DECtalk speaks the menu.

           The demonstration program uses the following two
           methods to send text to DECtalk. (Both methods use the
           DTK$SPEAK_TEXT Run-Time Library routine.)

           1. The program sends menu prompts to DECtalk with the
              DTK$K_WAIT mode specified.

              If the telephone keypad is enabled in autostop
              keypad mode, the caller can stop DECtalk from
              speaking by pressing a Touch-Tone key on the
              keypad.

           2. The program sends other text to DECtalk by first
              disabling autostop keypad mode. After DECtalk has
              spoken the text, the program reenables autostop
              keypad mode.

              This method ensures that all the specified text is
              spoken.

              NOTE
              The demonstration program uses the
              DTK$SPEAK_TEXT routine to speak prompts, instead
              of the built-in prompting mechanism provided in
              the DTK$READ_KEYSTROKE routine, so that the time
 


           4-28     The DECtalk Demonstration Program




              to wait for keypad input can be determined
              independently from the amount of time needed to
              speak the prompt.

           After DECtalk has spoken a prompt to the caller, a
           subroutine is called to receive Touch-Tone input from
           the caller. Each Touch-Tone key is read individually
           by issuing a call to the DTK$READ_KEYSTROKE routine.
           If the user fails to press a keystring terminator, ( )                                                               #                                                               #                                                               #
           or ( ), DTK$READ_KEYSTROKE returns the keystring after               *               *               *
           the timeout period has expired.

           The DTK$ANSWER_PHONE routine enables the telephone
           keypad in autostop keypad mode. In this mode, when the
           user presses a Touch-Tone key, DECtalk stops speaking
           as soon as possible and discards pending speech. No
           text will be spoken by DECtalk until it receives a
           command to restart speech.

              NOTE
              If the left and right square brackets are
              enabled as phonemic delimiters, and a Touch-Tone
              key is received in the middle of speaking
              phonemic text, DECtalk will be left in phonemic
              mode when speaking is reenabled.

           In the demonstration program, the RESTART routine
           sends a right square bracket ']', sets the speaking
           mode to DTK$K_SPEAK, and resets the voice parameters
           using the DTK$SET_SPEECH_MODE routine.

           When the keystring has been returned, it is verified
           as a valid command. The valid commands accepted by the
           demonstration program include the star key ( ), or any                                                       *                                                       *                                                       *
           Touch-Tone key followed by the pound sign key ( ).                                                          #                                                          #                                                          #
 


                        The DECtalk Demonstration Program     4-29




           If the caller forgets to press the pound sign key, the ap-
           plication waits until the timeout period expires, and a
           warning message has been spoken, before processing the com-
           mand. If the caller enters multiple commands (for exam-
           ple,         ), the text associated with Touch-Tone key                1# 2# 3#                1# 2# 3#                1# 2# 3#
             will be completely spoken, then the text associated with           1           1           1
           Touch-Tone key  , followed by the text associated with Touch-                          2                          2                          2
           Tone key  . The pound sign key is used as a command ter-                    3                    3                    3
           minator much like a carriage return is used when enter-
           ing text at a terminal. If the caller presses      , an                                                         123 #                                                         123 #                                                         123 #
           "invalid command" message is spoken, and the caller is prompted
           for a new command.

           When you are developing a multi-layer application
           program, allow users to get to any layer without
           having to listen to the prompts for each menu at each
           layer. For example, if a caller presses      , the                                                   1# 2#                                                   1# 2#                                                   1# 2#
           action associated with item   (in the menu selected by                                       2                                       2                                       2
           item   from the main menu) should be taken                1                1                1
           immediately.

           The method described above for reading Touch-Tone keys
           (reading the type-ahead buffer, prompting if nothing
           is entered, and then reading keypad commands) provides
           this functionality, and is demonstrated in the GET KEY
           STRING routine. To create multi-level menus, you
           should incorporate the technique shown in the MENU and
           PROCESS MENU ENTRY routines in the appropriate
           locations in the menu tree.
 


           4-30     The DECtalk Demonstration Program




           4.6.7  Terminating the Current Telephone Call

           The current telephone call can be terminated for a
           number of reasons. For example, the caller may have
           pressed the exit key from the main menu; DECtalk may
           have detected a wink (indicating the caller may have
           hung up); or a timeout on keypad input or some other
           error has occurred.

           When the current user session ends, the
           DTK$HANGUP_PHONE Run-Time Library routine first sends
           a closing message to the user, and then hangs up the
           telephone. In the demonstration program, this function
           is performed in the END CALL routine.


           4.6.8  Generating Statistical Reports

           It may be desirable in your application to provide the
           capability to generate statistical reports about the
           usage of the system. If your program keeps information
           about successfully completed telephone calls, the
           starting time of the call, the ending time of the
           call, the call number and the length of the call, you
           can generate many interesting reports about the usage
           of the system.

           Some examples of reports that you can generate from
           this information are:

           1. The number of calls completed on each DECtalk unit
              for each day of the week or each day of the month.

           2. The number of calls completed on each DECtalk unit
              during each hour of the day.

           3. The maximum and minimum length of telephone calls,
              and the average length of a telephone call.
 


                        The DECtalk Demonstration Program     4-31




           You can also generate additional reports containing
           information about the users of the system and the
           particular transactions that they perform. However,
           the demonstration program does not have this
           functionality.
 





















           APPENDIX  A




           TELEPHONICS SPECIFICATIONS
 


                                Telephonics Specifications     A-1




           Busy signal:           Busy signal:           Busy signal:

              A cadence with between .3 and .8 seconds ON, with a
              total cadence of between .95 and 1.05 seconds. (See
              also        .)                   _______                   Reorder

           Call Progress Detection:           Call Progress Detection:           Call Progress Detection:

              Call progress detection is done by looking at the
              level of noise received on the telephone line. If
              the noise is above a certain threshold, it is
              considered to be ON, and if below the threshold, it
              is considered to be OFF. Call progress detection
              works on all but very noisy or very quiet telephone
              connections.

              DECtalk listens to the noise, and then tries to
              identify the cadence. Nonsensical cadences or
              sequences of cadences are treated as voice. An
              example of a nonsensical arrangement is ringback, a
              busy signal, and a reorder.

           Certification:           Certification:           Certification:

              DECtalk is certified to comply with part 68 of the
              FCC rules. The FCC certification number is
              A0994Q-71152-AN-E. DECtalk is also certified by the
              Canadian Department of Communications. The DOC
              certification number is 1921422A.

           Connection:           Connection:           Connection:

              DECtalk telephone lines are connected using RJ21X
              telephone service (in the U.S.), or CA21A telephone
              service (in Canada). The first 8 pairs of the
              service are connected to the DECtalk modules; the
              remaining 17 pairs are unconnected. To order
              telephone connections for two racks of DECtalks,
              you must order two 8-line RJ21X or CA21A services.
 


           A-2     Telephonics Specifications




           Dial tone:           Dial tone:           Dial tone:

              A signal with more than 2.4 seconds ON.

           DTMF Dialing:           DTMF Dialing:           DTMF Dialing:

              DECtalk can generate outgoing calls using DTMF
              dialing. DTMF dialing uses the standard DTMF
              frequencies. (See                  (below) for a                                ____ ___________                                Tone Frequencies
              list of frequencies.) There is a .1% maximum
              frequency error. The digits are generated with a
              digit time of 160 milliseconds and an interdigit
              time of 60 milliseconds. The generated DTMF levels
              are around -3.5 dBm for the column tones and -6.3
              dBm for the row tones, with a maximum twist of 3.3
              dB. The total digit power (both tones combined) is
              between -1.4 and -1.8 dBm.

           Noise Bursts:           Noise Bursts:           Noise Bursts:

              A noise burst of under 50 milliseconds is ignored.
              If the noise persists for greater than 50
              milliseconds it is taken as a true event. If this
              noise burst is long enough, then the cadence will
              be thrown off and the telephone is assumed to have
              been answered. Common sources of noise bursts are
              telephone relays in a crossbar or step-by-step
              office.

           Pulse Dialing:           Pulse Dialing:           Pulse Dialing:

              DECtalk can generate outgoing calls using pulse
              (rotary) dialing. The digits are pulse dialed at a
              rate of 10 pulses per second.

                 The average Make interval is 40 milliseconds.
                 The average Break interval is 60 milliseconds.
                 The interdigit interval is 0.8 seconds.
 


                                Telephonics Specifications     A-3




           Reorder:           Reorder:           Reorder:

              A cadence with between .05 and .3 seconds ON, and a
              total time of between .45 and .55 seconds. DECtalk
              treats reorder as a busy telephone line. (See also
                         .)              ____ ______              Busy signal

           Ring Characteristics:           Ring Characteristics:           Ring Characteristics:

              DECtalk detects rings with frequencies between 15.3
              and 68.0 Hz, and voltages between 40 and 150 volts
              rms. Ringing must be present for at least 500
              milliseconds to be detected. If autoanswer is set
              for more than one ring and ringing is absent for 10
              or more seconds, the ring counter is reset to 0.

           Ringback:           Ringback:           Ringback:

              There are several cadences that DECtalk decodes as
              a ring.

              1. The most common cadence is the standard ring (a
                 cadence of between 0.8 and 2.4 seconds ON, with
                 a total time of 5.9 to 6.1 seconds).

              2. A faster ring has the same ON cadence, with a
                 total time of between 2.9 and 3.1 seconds.

              3. Two types of split ringbacks are detected.

                 o  The first split ring has a cadence of between
                    .9 and 1.1 seconds ON (both tone sections),
                    with a total time for the first half of
                    between 1.4 and 1.5 seconds, and a total time
                    for the second half of between 4.4 and 4.6
                    seconds.

                 o  The second split ring has a cadence of
                    between .3 and .8 seconds ON (both tone
                    sections), with a total time of between .95
                    and 1.05 for the first half, and a total time
 


           A-4     Telephonics Specifications




                    for the second half of between 1.05 and 6.1
                    seconds.

           Ringer Equivalence:           Ringer Equivalence:           Ringer Equivalence:

              The DECtalk ring detection circuit has a ringer
              equivalence of 0.2B as determined by FCC test
              methods, and a load number of 5B by DOC test
              methods.

           Tone Detection:           Tone Detection:           Tone Detection:

              DECtalk will detect Dual-Tone Multiple Frequency
              (DTMF) signals. These are the signals that a
              Touch-Tone telephone creates.

              A digipulse phone has keys like a Touch-Tone phone
              but does not create DTMF signals. Digipulse
              (Rotary) signaling is not detected by DECtalk.

           Tone Dropouts:           Tone Dropouts:           Tone Dropouts:

              A tone dropout of under 50 milliseconds is ignored.
              A dropout that persists for greater than 50
              milliseconds is taken as a true event. Again, if
              the dropout is long enough, then it is assumed that
              the previous cadence was not network signaling but
              someone saying "Hello."

              A possible source of tone dropout is an old
              telephone ringer with too many broken gear teeth.

           Tone Frequencies:           Tone Frequencies:           Tone Frequencies:

              DTMF tones that are within +/- 1.5% (+/- 2 Hz) of
              the nominal frequency will be accepted. DTMF tones
              that are outside of 3.5% of the nominal frequency
              will be rejected.
 


                                Telephonics Specifications     A-5




              DTMF tones are a combination of two tones, one from
              the row group, and one from the column group. The
              row group tones are 697 Hz, 770 Hz, 852 Hz, and 941
              Hz going from the top row (containing 1) to the
              bottom row (containing 0). The column group tones
              are 1209 Hz, 1336 Hz, 1477 Hz, and 1633 Hz going
              from the leftmost row (containing 1) to the
              rightmost digit row (containing 3), and then
              including the extra row of keys that are
              occasionally present in PBX installations (often
              labeled "A," "B," "C," and "D.")

           Tone Levels:           Tone Levels:           Tone Levels:

              The DTMF tones must typically be at least 12 dB
              louder than the noise floor and 16 dB louder than
              any other tone within the DTMF detection band. Each
              frequency of the DTMF tone must be between -5 and
              -34 dBm when measured at the DECtalk telephone line
              connections.

           Tone Timing:           Tone Timing:           Tone Timing:

              A DTMF tone must be present for at least 55
              milliseconds before it is recognized. A tone burst
              of less than 35 milliseconds is ignored as a noise
              burst. The DTMF tone should be absent for at least
              55 milliseconds before the next DTMF tone is
              detected. A tone absence of less than 40
              milliseconds is detected as a drop out and is
              ignored.

           Voice Signal Level:           Voice Signal Level:           Voice Signal Level:

              DECtalk provides a maximum of -9 dBm to the
              telephone line, according to telephone regulations.
              This level is measured into a 600 ohm load with a
              three second average. The average level of DECtalk
              is between -9 and -11 dBm, and depends upon the
              voice which is selected.
 





















           APPENDIX  B




           VAX/VMS RTL SUPPORT FOR DECTALK
 


                           VAX/VMS RTL Support for DECtalk     B-1




           B.1  Overview

           All three implementations of the demonstration program (BASIC,
           COBOL and C) use Version 4.5 of the VAX/VMS Run-Time Library
           Support for DECtalk (the DTK$ facility) to communicate with
           DECtalk. The DTK$ facility of VAX/VMS Run-Time Library pro-
           vides a simple interface between the application program
           and the DECtalk device, and consists of procedures that
           control the functions of the DECtalk device. These pro-
           cedures control DECtalk's characteristics, as well as pro-
           vide functional controls (such as reading keys pressed on
           the telephone keypad or hanging up the telephone).

           The functions currently supported by Version 4.5 of
           the DTK$ facility of the VAX/VMS Run-Time Library are
           listed below in Table B-1. For a more detailed
           description of these routines, see the VAX/VMS Run-Time
           Library Routines Reference Manual.


           _______________________________________________________
           Table B-1 VAX/VMS RTL DTK$ Supported Functions           Table B-1 VAX/VMS RTL DTK$ Supported Functions           Table B-1 VAX/VMS RTL DTK$ Supported Functions
           _______________________________________________________
           _______________________________________________________           Routine                         Function           Routine                         Function           Routine                         Function

           DTK$ANSWER_PHONE                Wait for telephone to
                                           ring and answer it

           DTK$DIAL_PHONE                  Dial specified number
                                           on the phone

           DTK$HANGUP_PHONE                Hang up the phone

           DTK$INITIALIZE                  Initialize DECtalk
                                           connection

           DTK$LOAD_DICTIONARY             Define word in DECtalk
                                           user dictionary
 


           B-2     VAX/VMS RTL Support for DECtalk



           _______________________________________________________
           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions
           _______________________________________________________
           _______________________________________________________           Routine                         Function           Routine                         Function           Routine                         Function

           DTK$READ_KEYSTROKE              Read a key pressed on
                                           the telephone keypad

           DTK$READ_STRING                 Read a series of keys
                                           from the keypad

           DTK$RETURN_LAST_INDEX           Return last index
                                           spoken

           DTK$SET_INDEX                   Insert index at
                                           current position

           DTK$SET_KEYPAD_MODE             Enable or disable
                                           telephone keypad

           DTK$SET_LOGGING_MODE            Set or reset logging
                                           mode

           DTK$SET_MODE                    Set or reset mode

           DTK$SET_SPEECH_MODE             Turn speaking on or
                                           off

           DTK$SET_TERMINAL_MODE           Set or reset terminal
                                           mode

           DTK$SET_VOICE                   Set speaking voice
                                           characteristics

           DTK$SPEAK_FILE                  Send text file to
                                           DECtalk device

           DTK$SPEAK_PHONEMIC_TEXT         Send phonemic text to
                                           DECtalk device
 


                           VAX/VMS RTL Support for DECtalk     B-3



           _______________________________________________________
           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions           Table B-1 (Cont.) VAX/VMS RTL DTK$ Supported Functions
           _______________________________________________________
           _______________________________________________________           Routine                         Function           Routine                         Function           Routine                         Function

           DTK$SPEAK_TEXT                  Send text to DECtalk
                                           device

           DTK$TERMINATE                   Terminate DECtalk
                                           connection
           _______________________________________________________




           B.2  Restrictions of VAX/VMS Version 4.5 Run-Time
                Library Support for DECtalk

           While Version 4.5 of the DTK$ facility of the VAX/VMS
           Run-Time Library does provide a simple interface
           between the application program and DECtalk, there are
           some restrictions. These restrictions are listed
           below.

              NOTE
              It is anticipated that the restrictions listed
              below, with the exception of the problem with
              the wink detection function, will be fixed in
              Version 4.6. When Version 4.6 becomes available,
              consult the release notes.

           1. In Version 4.5 of the DTK$ facility, a problem
              exists with the DTKDEF module in STARLET.OLB.

              When reading keys, DECtalk returns the ASCII values
              of the keys that are pressed. However, the
              definitions in DTKDEF are not the ASCII values.
              Table B-2 lists the current DTKDEF definitions.
 


           B-4     VAX/VMS RTL Support for DECtalk




           _______________________________________________________
           Table B-2 Current DTKDEF Definitions           Table B-2 Current DTKDEF Definitions           Table B-2 Current DTKDEF Definitions
           _______________________________________________________
                                                       Current                                                       Current                                                       Current
                                                       value                                                       value                                                       value
           Pressed on            Returned from         defined in           Pressed on            Returned from         defined in           Pressed on            Returned from         defined in
           _______________________________________________________           telephone             DECtalk               DTKDEF           telephone             DECtalk               DTKDEF           telephone             DECtalk               DTKDEF

           0                     48                    0

           1                     49                    1

           2                     50                    2

           3                     51                    3

           4                     52                    4

           5                     53                    5

           6                     54                    6

           7                     55                    7

           8                     56                    8

           9                     57                    9
           _______________________________________________________


              Star and pound sign both have correct values. Only
              the values for keypad numbers 0 through 9, DTK$K_
              TRM_ZERO through DTK$K_TRM_NINE, are incorrect.
              These definitions must be redefined, or compensated
              for, in this version of the Run-Time Library.

           2. The current version of the Run-Time Library Support
              for DECtalk does not provide a command to check the
              status of DECtalk.
 


                           VAX/VMS RTL Support for DECtalk     B-5




              With this command, an application program could
              determine whether or not the DECtalk module had
              power-cycled since the last time its status was
              requested. If the DECtalk module has power-cycled,
              application-specific parameters (speaking voice,
              speaking rate, words loaded into the user
              dictionary) should be reinitialized. By
              periodically checking the status of the DECtalk
              module, and reinitializing if the unit has
              power-cycled, an application would not have to be
              brought down to replace failed DECtalk modules.

              As a temporary solution, the demonstration program
              reinitializes application-specific parameters
              whenever a telephone call has not been received in
              15 minutes. These parameters may not need to be
              reinitialized every 15 minutes. However, with this
              alternative, DECtalk modules can still be replaced
              without terminating the application. The
              application will have to be brought down if the
              DECtalk module fails at some time other than when
              it is waiting for a telephone call.

           3. Some of the DTK$ Run-Time Library functions that
              read and return a status condition from DECtalk do
              not have a timeout associated with their read from
              DECtalk. These DTK$ Run-Time Library functions
              include:

                 DTK$ANSWER_PHONE
                 DTK$DIAL_PHONE
                 DTK$HANGUP_PHONE
                 DTK$LOAD_DICTIONARY
                 DTK$READ_KEYSTROKE
                 DTK$READ_STRING
                 DTK$RETURN_LAST_INDEX
                 DTK$SET_KEYPAD_MODE
                 DTK$SPEAK_FILE
                 DTK$SPEAK_PHONEMIC_TEXT
                 DTK$SPEAK_TEXT
 


           B-6     VAX/VMS RTL Support for DECtalk




              As all these commands wait to receive status informa-
              tion from DECtalk, it is possible to send a command to
              a "dead" DECtalk module and never receive a response.
              This situation causes the application program to hang.
              Hence, no more telephone calls are serviced by this DECtalk
              module. As the DECtalk modules may be on a hunt chain,
              if the telephone is active (busy) when the failure oc-
              curs, incoming calls are forwarded to the next avail-
              able DECtalk module. Therefore, only the user of the
              failed DECtalk module would be lost. However, if the
              telephone is not active (that is, if it is not busy),
              the telephone rings indefinitely when a user calls, be-
              cause hunt chains only forward calls if the current line
              is busy. This situation could bring down the whole sys-
              tem if the failed DECtalk module is the first module
              in the hunt chain. Since the system operator is never
              warned of the failure, it could be a long time before
              the problem is detected.

              A watchdog timer provides a more reliable means of
              error detection. While the situation described
              above still exists, the system operator can at
              least be warned if a failure occurs in the
              communication between the DECtalk module and the
              CPU. To alleviate the problem described above, a
              watchdog timer can be set in the application
              program (using the SYS$SETIMR system service) prior
              to issuing the DTK$ routine call.

              In the VAX C demonstration program provided, the
              maximum value that a system timer can be set to is
              86400 seconds (24 hours). The BASIC and COBOL
              programs do not have a maximum timer value, but it
              is recommended that you keep the timer value less
              than 24 hours.

              If a response is received from DECtalk within the
              timeout period specified, the watchdog timer is
              canceled (using the system service SYS$CANTIM). If
              the timer expires, a warning message, displayed at
              the operator's terminal, details the error number
 


                           VAX/VMS RTL Support for DECtalk     B-7




              and the device line that is connected to the
              failing DECtalk module.

              This warning message informs the operator that
              there is something wrong with the connection
              between the DECtalk module and the physical device
              (such as a failed DECtalk module, a disconnected
              RS232 cable, a disconnected power cord, and so
              forth) and that the error should be corrected.

              To correct any error that may occur in the
              communication between the DECtalk module and the
              physical device, perform the following procedures:

              o  Terminate the current job

              o  Correct the error

              o  Restart the job

              In the demonstration program, a watchdog timer is
              set prior to issuing a call to all DTK$ Run-Time
              Library routines.

           4. The current version of DTK$READ_STRING Run-Time
              Library routine does not work correctly.

              If a series of Touch-Tone keys is pressed on the
              Touch-Tone keypad, and a terminating character
              (pound sign key or star key) is not pressed, the
              series of Touch-Tone keys pressed is returned after
              the timeout period specified, along with a
              terminator code of DTK$K_TRM_TIMEOUT. On the next
              call to DTK$READ_STRING the last Touch-Tone key
              returned in the previous call to DTK$READ_STRING is
              always included as the first Touch-Tone key in the
              series of keys returned.

              The demonstration program provides a subroutine
              (GET_KEY_STRING) that can be used to read single
              keys, a series of Touch-Tone keys terminated by the
              pound sign key or the star key. It is similar in
              functionality to the DTK$READ_STRING routine.
 


           B-8     VAX/VMS RTL Support for DECtalk




           5. The version of the DTK$SPEAK_FILE command provided
              in Version 4.5 of the DTK$ facility does not
              terminate speaking the file when a Touch-Tone key
              is pressed while the keypad is enabled in autostop
              keypad mode.

              Instead, it stops speaking the current line of text
              in the file and then resumes speaking on the next
              line of text. If it is necessary to hear an entire
              file of text spoken without interruption, the
              telephone keypad mode should be either disabled, or
              enabled without autostop mode. (Use the
              DTK$SET_KEYPAD_MODE command.) Then, the file can be
              spoken using the DTK$SPEAK_FILE command.

              If it is necessary to hear a file of text spoken
              and allow the user to terminate the speaking of the
              file, use the following procedure:

              o  First, enable the keypad without autostop mode.
                 (Use the DTK$SET_KEYPAD_MODE command.)

              o  Then, the application program should read a line
                 from the file of text and send it to DECtalk
                 using the DTK$SPEAK_TEXT command in DTK$K_IMMED
                 mode.

              o  As soon as the DTK$SPEAK_TEXT command returns,
                 the DTK$READ_KEYSTROKE command should be issued
                 with a one-second timeout.

              o  If no Touch-Tone keys are received, continue
                 sending the text lines of the file using
                 DTK$SPEAK_TEXT.

              o  If a Touch-Tone key is received, terminate
                 sending text to DECtalk by issuing the
                 DTK$SET_SPEECH_MODE command with the mode
                 specified as DTK$K_HALT.
 


                           VAX/VMS RTL Support for DECtalk     B-9




              Using this procedure, speaking of the file is not
              stopped immediately (as it would be using autostop
              keypad mode). However, it will stop at the end of
              the current line of text.

           6. If DECtalk detects a wink on the first call of
              DTK$READ_KEYSTROKE or DTK$READ_STRING, you get an
              access violation, and the application program
              fails.

              When DECtalk receives a Touch-Tone key, the problem
              no longer occurs.

              You can avoid this problem by calling each DECtalk
              module and pressing a Touch-Tone key, then hanging
              up to allow users to call. This procedure fixes the
              wink problem for as long as the application is
              running. If the application fails and must be
              restarted, you should call each DECtalk module
              again, press a Touch-Tone key, and hang up.

                 NOTE
                 In some facilities, calling each DECtalk
                 module every time the application is
                 restarted may not be practical. In this case,
                 you may simply want to watch for a DECtalk
                 module that has failed because of this wink
                 problem, and then restart the application.

           7. The value of timeout in DTK$DIAL_PHONE, if used,
              must be a minimum of 10 seconds.
