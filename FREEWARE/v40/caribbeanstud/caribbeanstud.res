/*
 * (c) Copyright 1997, Qun Zhang.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Qun Zhang not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Qun Zhang make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE ABOVE-NAMED DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE ABOVE-NAMED BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
 
! toplevel

CaribbeanStud.minBet: 10
CaribbeanStud.betUnit: 1
CaribbeanStud.bankroll: 200
CaribbeanStud.jackpot: 34000000
CaribbeanStud.minjackpot: 3000000
CaribbeanStud.players: 2

CaribbeanStud.title: Caribbean Stud Poker
CaribbeanStud*fontList: -adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1

! Layout
CaribbeanStud*Layout.marginWidth: 0
CaribbeanStud*Layout.marginHeight: 0
CaribbeanStud*Layout.packing: 1
CaribbeanStud*Layout.entryAlignment: 1
CaribbeanStud*Layout.adjustLast: true
CaribbeanStud*Layout.adjustMargin: true
CaribbeanStud*Layout.attachments.rightAttachment: 1 0 1
CaribbeanStud*Layout.attachments.leftAttachment: 1 0 1;
CaribbeanStud*Layout.attachments.bottomAttachment: 2 0 0
CaribbeanStud*Layout.attachments.rightAttachment.topAttachment: 1 0 1


! DealButton
CaribbeanStud*DealButton.labelString: Deal


CaribbeanStud*ExceptionDialog.dialogTitle: Information

! ProgressiveButton
CaribbeanStud*ProgressiveButton.labelString: Progressive
CaribbeanStud*ProgressiveButton.marginTop: 5
CaribbeanStud*ProgressiveButton.marginBottom: 5

! AnteScroll
CaribbeanStud*AnteScroll.titleString: Your Ante Bet
CaribbeanStud*AnteScroll.width: 240

! BetButton
CaribbeanStud*BetButton.labelString: Call
CaribbeanStud*BetButton.fontList: -adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1
CaribbeanStud*BetButton.marginHeight:2
CaribbeanStud*BetButton.marginWidth: 2

! FoldButton
CaribbeanStud*FoldButton.labelString: Fold 
CaribbeanStud*FoldButton.fontList: -adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1
CaribbeanStud*FoldButton.marginHeight:2
CaribbeanStud*FoldButton.marginWidth: 2

!YouHaveLabel
CaribbeanStud*YouHaveLabel*.fontList: -adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1
CaribbeanStud*YouHaveLabelValue*.marginRight: 8
CaribbeanStud*YouHaveLabelValue.fontList: -adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1

!JackPotLabel
CaribbeanStud*JackPotLabel*fontList: -adobe-helvetica-bold-r-normal--20-*-*-*-*-*-iso8859-1
CaribbeanStud*JackPotLabel.alignment: ALIGNMENT_BEGINNING

! HelpRulesButton
CaribbeanStud*HelpRulesButton.labelString: Rules...

! HelpRulesDialog
CaribbeanStud*HelpRulesDialog.pushpin: in
CaribbeanStud*HelpRulesDialog.dialogTitle: Rules...
CaribbeanStud*HelpRulesDialog.defaultPosition: False
CaribbeanStud*HelpRulesDialog.x: 328
CaribbeanStud*HelpRulesDialog.y: 142


CaribbeanStud*HelpRulesScrolledText.rows: 24
CaribbeanStud*HelpRulesScrolledText.columns: 80
CaribbeanStud*HelpRulesScrolledText.fontList: 9x15
CaribbeanStud*HelpRulesScrolledText.editable: False
CaribbeanStud*HelpRulesScrolledText.editMode: MULTI_LINE_EDIT
CaribbeanStud*HelpRulesScrolledText.autoShowCursorPosition: True
CaribbeanStud*HelpRulesScrolledText.wordWrap: True
CaribbeanStud*HelpRulesScrolledText.marginWidth: 8
CaribbeanStud*HelpRulesScrolledText.scrollHorizontal: False
CaribbeanStud*HelpRulesScrolledText.value: From BOYD GAMING Gaming Guide:\n\
\n\
	To begin the game, each player makes an ante bet.\n\
	If you choose to be in the progressive jackpot, an additional side\n\
	wager of 1.00 is required. A coin exceptor is provided for each\n\
	player.\n\
\n\
	All players will receive five cards face down. The dealer will also\n\
	receive five cards, with the exception of the last card, face up.\n\
	If a player feels his hand will beat the dealers hand and wishes to\n\
	continue playing, the player must place an additional wager of \n\
	exactly TWICE the ante bet. If a player feels he cannot beat the\n\
	dealers's hand, the player will fold his hand and surrender ante bet.\n\
\n\
	To open, the dealer must have an Ace/King or higher to continue play.\n\
	Once the dealer has exposed his hand and fails to have Ace/King or\n\
	higher, he cannot open and hand will end. The dealer will collect \n\
	the cards and pay the ante bet to those players who remained in \n\
	hand. If the dealer's hand has an ace/king or higher and the players\n\
	hand is higher than the dealer's hand, the player qualifies for the\n\
	following bonus pay-out:\n\
\n\
	Even Money on all Ante Bets.\n\
\n\
	1 Pair ......... Even Money\n\
	2 Pair ............. 2 to 1\n\
	3 of a Kind ........ 3 to 1\n\
	Straight............ 4 to 1\n\
	Flush .............. 5 to 1\n\
	Full House ......... 7 to 1\n\
	4 of a Kind ....... 20 to 1\n\
	Straight Flush .... 50 to 1\n\
	Royal Flush ...... 100 to 1\n\
\n\
\n\
	Regardless of the dealers hand, players that wagered on the side \n\
	progressive bet and received one of the following hands will be paid:\n\
\n\
	Royal Flush ....... 100% of Progressive Meter\n\
	Straight Flush ..... 10% of Progressive Meter\n\
	4 of a Kind ........ $100\n\
	Full House .........  $75\n\
	Flush ..............  $50\n\

