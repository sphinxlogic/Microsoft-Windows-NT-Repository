    %IF %DECLARED(%BASIC$QUADWORD_DECLARED) = 0 %THEN
	RECORD BASIC$QUADWORD
		LONG FILL(2)
	END RECORD
    %LET %BASIC$QUADWORD_DECLARED = 1
    %END %IF
    %IF %DECLARED(%BASIC$OCTAWORD_DECLARED) = 0 %THEN
	RECORD BASIC$OCTAWORD
		LONG FILL(4)
	END RECORD
    %LET %BASIC$OCTAWORD_DECLARED = 1
    %END %IF
     !  Definitions for RTL DECtalk Management
    
    ! 
    ! 	Phone keypad keys.
    ! 
      DECLARE LONG CONSTANT DTK$K_TRM_ZERO = 48
      DECLARE LONG CONSTANT DTK$K_TRM_ONE = 49
      DECLARE LONG CONSTANT DTK$K_TRM_TWO = 50
      DECLARE LONG CONSTANT DTK$K_TRM_THREE = 51
      DECLARE LONG CONSTANT DTK$K_TRM_FOUR = 52
      DECLARE LONG CONSTANT DTK$K_TRM_FIVE = 53
      DECLARE LONG CONSTANT DTK$K_TRM_SIX = 54
      DECLARE LONG CONSTANT DTK$K_TRM_SEVEN = 55
      DECLARE LONG CONSTANT DTK$K_TRM_EIGHT = 56
      DECLARE LONG CONSTANT DTK$K_TRM_NINE = 57
      DECLARE LONG CONSTANT DTK$K_TRM_A = 65
      DECLARE LONG CONSTANT DTK$K_TRM_B = 66
      DECLARE LONG CONSTANT DTK$K_TRM_C = 67
      DECLARE LONG CONSTANT DTK$K_TRM_D = 68
      DECLARE LONG CONSTANT DTK$K_TRM_NUMBER_SIGN = 35
      DECLARE LONG CONSTANT DTK$K_TRM_ASTERISK = 42
    !  Conditions
      DECLARE LONG CONSTANT DTK$K_TRM_CANCELLED = 508  !  I/O cancelled 
      DECLARE LONG CONSTANT DTK$K_TRM_TIMEOUT = 509  !  Timeout period expired
      DECLARE LONG CONSTANT DTK$K_TRM_BUFFER_FULL = 510  !  Buffer is full
      DECLARE LONG CONSTANT DTK$K_TRM_UNKNOWN = 511  !  Unknown terminator
    ! 
    ! 	Phone dialing constants.
    ! 
      DECLARE LONG CONSTANT DTK$K_DIAL_TONE = 0
      DECLARE LONG CONSTANT DTK$K_DIAL_PULSE = 1
    ! 
    ! 	DECtalk types.
    ! 
      DECLARE LONG CONSTANT DTK$K_DTC_UNKNOWN = 0
      DECLARE LONG CONSTANT DTK$K_DTC_01 = 1
      DECLARE LONG CONSTANT DTK$K_DTC_03 = 2
    ! 
    ! 	Keypad modes.
    ! 
      DECLARE LONG CONSTANT DTK$K_KEYPAD_OFF = 0
      DECLARE LONG CONSTANT DTK$K_KEYPAD_ON = 1
      DECLARE LONG CONSTANT DTK$K_KEYPAD_AUTO = 2
    ! 
    ! 	Voices.
    ! 
      DECLARE LONG CONSTANT DTK$K_VOICE_MALE = 0
      DECLARE LONG CONSTANT DTK$K_VOICE_FEMALE = 1
      DECLARE LONG CONSTANT DTK$K_VOICE_CHILD = 2
      DECLARE LONG CONSTANT DTK$K_VOICE_DEEP_MALE = 3
      DECLARE LONG CONSTANT DTK$K_VOICE_DEEP_FEMALE = 4
      DECLARE LONG CONSTANT DTK$K_VOICE_OLDER_MALE = 5
      DECLARE LONG CONSTANT DTK$K_VOICE_LIGHT_FEMALE = 6
    ! 
    ! 	Speaking modes.
    ! 
      DECLARE LONG CONSTANT DTK$K_SPEAK = 0
      DECLARE LONG CONSTANT DTK$K_STOP = 1
      DECLARE LONG CONSTANT DTK$K_HALT = 2
    ! 
    ! 	Return modes.
    ! 
      DECLARE LONG CONSTANT DTK$K_IMMED = 0  !  Return immedately after sending text.
      DECLARE LONG CONSTANT DTK$K_WAIT = 1  !  Wait until text is completely spoken be returning.
      DECLARE LONG CONSTANT DTK$K_STATUS = 2  !  Wait for text to be spoken and return phone status.
    ! 
    !         DEFINE BIT MASKS AND VALUES FOR MODE ATTRIBUTES
    ! 
      DECLARE LONG CONSTANT DTK$M_SQUARE = x'00000001'
      DECLARE LONG CONSTANT DTK$M_ASCII = x'00000002'
      DECLARE LONG CONSTANT DTK$M_MINUS = x'00000004'
      DECLARE LONG CONSTANT DTK$M_EUROPE = x'00000008'
      DECLARE LONG CONSTANT DTK$M_SPELL = x'00000010'
      DECLARE LONG CONSTANT DTK$M_NORMAL = 0  !  no bits set 
      DECLARE LONG CONSTANT DTK$K_SQUARE = 0  !  Mode square bitvector index
      DECLARE LONG CONSTANT DTK$K_ASCII = 1  !  asky mode
      DECLARE LONG CONSTANT DTK$K_MINUS = 2  !  Say dash as minus bitvector index
      DECLARE LONG CONSTANT DTK$K_EUROPE = 3  !  European type numbers
      DECLARE LONG CONSTANT DTK$K_SPELL = 4  !  Spell all words
      DECLARE LONG CONSTANT DTK$M_HOST = x'00000001'
      DECLARE LONG CONSTANT DTK$M_SPEAK_ALL = x'00000002'
      DECLARE LONG CONSTANT DTK$M_EDITED = x'00000004'
      DECLARE LONG CONSTANT DTK$M_HARD = x'00000008'
      DECLARE LONG CONSTANT DTK$M_SETUP = x'00000010'
      DECLARE LONG CONSTANT DTK$M_FILTER = x'00000020'
      DECLARE LONG CONSTANT DTK$K_HOST = 0  !  Send all chars typed to host.
      DECLARE LONG CONSTANT DTK$K_SPEAK_ALL = 1  !  Speak all chars typed.
      DECLARE LONG CONSTANT DTK$K_EDITED = 2  !  Line edit all chars typed.
      DECLARE LONG CONSTANT DTK$K_HARD = 3  !  Do local echo in hardcopy format.
      DECLARE LONG CONSTANT DTK$K_SETUP = 4  !  Speak all chars displayed in setup mode.
      DECLARE LONG CONSTANT DTK$K_FILTER = 5  !  Don't send DECtalk specific sequences.
      DECLARE LONG CONSTANT DTK$M_TEXT = x'00000001'
      DECLARE LONG CONSTANT DTK$M_PHONEME = x'00000002'
      DECLARE LONG CONSTANT DTK$M_RAWHOST = x'00000004'
      DECLARE LONG CONSTANT DTK$M_INHOST = x'00000008'
      DECLARE LONG CONSTANT DTK$M_OUTHOST = x'00000010'
      DECLARE LONG CONSTANT DTK$M_ERROR = x'00000020'
      DECLARE LONG CONSTANT DTK$M_TRACE = x'00000040'
      DECLARE LONG CONSTANT DTK$M_DEBUG = x'00000080'
      DECLARE LONG CONSTANT DTK$K_TEXT = 0  !  Log ASCII text.
      DECLARE LONG CONSTANT DTK$K_PHONEME = 1  !  Log phonemic text.
      DECLARE LONG CONSTANT DTK$K_RAWHOST = 2  !  Log all chars without inspection.
      DECLARE LONG CONSTANT DTK$K_INHOST = 3  !  Log text read from host.
      DECLARE LONG CONSTANT DTK$K_OUTHOST = 4  !  Log text sent to host.
      DECLARE LONG CONSTANT DTK$K_ERROR = 5  !  Log error messages.
      DECLARE LONG CONSTANT DTK$K_TRACE = 6  !  Log sequences symbolically
      DECLARE LONG CONSTANT DTK$K_DEBUG = 7  !  Reserved
      DECLARE LONG CONSTANT DTK$S_DTKDEF = 1
      record DTKDEF
         variant
    ! 
    ! 	DEFINE BITS, MASKS, AND FIELDS FOR THE CONTROL MODES.
    ! 
            case
               group DTK$R_DTKMODE_BITS
                   !  Set mode square on
                   !  asky mode
                   !  Say dash as minus
                   !  Europe 
                   !  Spell all words
                  BYTE SQUARE_bits   ! COMMENT ADDED BY SDL - SQUARE_bits contains bits SQUARE through fill_0
               end group DTK$R_DTKMODE_BITS
            case
               group DTK$R_DTKTERM_BITS
                   !  Send all chars typed to host.
                   !  Speak all chars typed.
                   !  Line edit all chars typed.
                   !  Do local echo in hardcopy format.
                   !  Speak all chars displayed in setup mode.
                   !  Don't send DECtalk specific sequences.
                  BYTE HOST_bits   ! COMMENT ADDED BY SDL - HOST_bits contains bits HOST through fill_1
               end group DTK$R_DTKTERM_BITS
            case
               group DTK$R_DTKLOG_BITS
                   !  Log ASCII text.
                   !  Log phonemic text.
                   !  Log all chars without inspection.
                   !  Log text read from host.
                   !  Log text sent to host.
                   !  Log error messages.
                   !  Log sequences symbolically
                   !  Reserved
                  BYTE TEXT_bits   ! COMMENT ADDED BY SDL - TEXT_bits contains bits TEXT through DEBUG
               end group DTK$R_DTKLOG_BITS
         end variant
      end record DTKDEF
    
