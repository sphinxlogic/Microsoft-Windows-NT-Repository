2A1F:0100 1E            PUSH    DS
2A1F:0101 31C0          XOR     AX,AX
2A1F:0103 8ED8          MOV     DS,AX
2A1F:0105 BE1704        MOV     SI,0417
2A1F:0108 8A04          MOV     AL,[SI]
2A1F:010A 24DF          AND     AL,DF   (Change this mask for Caps & Scroll
2A1F:010C 8804          MOV     [SI],AL                       Lock as well)
2A1F:010E 1F            POP     DS
2A1F:010F CD20          INT     20
