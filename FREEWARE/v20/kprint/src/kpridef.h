globalvalue KPRI$_CODE_CHECK;
globalvalue KPRI$_LEVEL_II;
globalvalue KPRI$_EXPANDED;
globalvalue KPRI$_KEISEN;
globalvalue KPRI$_OPENIN;
globalvalue KPRI$_OPENOUT;
globalvalue KPRI$_CLOSEIN;
globalvalue KPRI$_CLOSEOUT;
globalvalue KPRI$_READERR;
globalvalue KPRI$_WRITEERR;
globalvalue KPRI$_INVALID_COMMAND;
globalvalue KPRI$_INVALID_PARAM;

enum CMD_TYPE {
	CMD_UNKNOWN,
	CMD_BLANK,
	CMD_COMMENT,
	CMD_FONT_PITCH,
	CMD_FONT_SIZE,
	CMD_FONT_TYPE,
	CMD_HEADER,
	CMD_HORIZONTAL,
	CMD_ITALIC,
	CMD_LEFT_MARGIN,
	CMD_PITCH,
	CMD_SKIP,
	CMD_SPACING,
	CMD_SIXEL,
	CMD_UPRIGHT,
	CMD_VERTICAL,
	CMD_END_OF_COMMAND
    };

#define YES			1
#define NO			0

#define TAB			'\t'
#define CR			'\r'
#define LF			'\n'
#define FORM_FEED		'\f'
#define ESC			'\033'

#define JSY$C_EOS		0xFFFF

#define is_expanded(ch)		((ch & 0xff00) >= 0xA100 && (ch & 0x00ff) <= 0x7E)
#define is_level_II(ch)		((ch & 0xff00) >= 0xD000 && (ch & 0x00ff) >= 0xA1)
