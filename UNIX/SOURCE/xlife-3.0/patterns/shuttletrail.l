## FOUND IN (shuttletrail.life):
#I :rpa 69 -95 0 1 321
#I :rakep20f -22 -102 0 -1 25
#I :rpb 98 27 0 1 19
#I :rpc -3 134 0 1 20
#I :rpd -2 -8 0 1 21

## FOUND IN (rpb.life):
#B rpb
#I :rakep20b 47 -45 0 1 0
#I :rakep20f -29 -33 0 -1 6
#E

## FOUND IN (rakep20b.life):
#B rakep20b
#C structured description of period 20 rake (backward gliders)

#C pat          x   y  rot  flip  delay
#C ---          -   -  ---  ----  -----
#I :puffertrain 0 0 0 1 0
#I :ss.s -9 -7 0 1 0
#I :ss.s -17 9 0 1 3
#E

## FOUND IN (puffertrain.life):
#B puffertrain
#C structured description of period 20 puffertrain

#C pat      x   y  rot  flip  delay
#C ---      -   -  ---  ----  -----
#I :gen.20 0 0 0 1 0
#I :ss.s 1 -7 0 1 3
#I :ss.s 1 7 0 1 3
#E

## FOUND IN (ss.life):
#B ss.s
#P
*..*.
....*
*...*
.****
#E

## FOUND IN (gen.life):
#B gen.20
#P
*..
**.
.**
**.
#E

## FOUND IN (rpc.life):
#B rpc
#I :rakep20f -79 -60 0 1 0
#I :rakep20b 152 -48 0 -1 1
#E

## FOUND IN (rpd.life):
#B rpd
#I :rakep20b 80 -92 0 1 0
#I :rakep20f -2 -2 0 1 4
#E

## FOUND IN (rakep20f.life):
#B rakep20f
#C structured description of period 20 rake (forward gliders)

#C pat          x   y  rot  flip  delay
#C ---          -   -  ---  ----  -----
#I :puffertrain 0 0 0 1 0
#I :ss.s -9 -7 0 1 0
#I :ss.s -17 9 0 1 1
#E

## FOUND IN (rpa.life):
#B rpa
#I :rakep20f -67 9 0 -1 3
#I :rakep20b -1 -4 0 1 0
#E

