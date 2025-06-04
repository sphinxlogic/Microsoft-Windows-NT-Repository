      SUBROUTINE QSORT (LIST,N)                                                 
C***********************************************************************        
C                                                                               
C   Author         : Curtis Rempel                                              
C   Language       : FORTRAN 77                                                 
C   System         : MTS - *FORTRANVS                                           
C   Written        : 87/02/27                                                   
C                                                                               
C   PARAMETERS     : LIST - an array of elements to be sorted          
C                  : N    - the number of elements in the array                 
C                                                                               
C   COMMENTS       : Allows sorting up to 1,000,000 elements.
C                                                                               
C                  : Uses a partitioning subroutine, PARTIT, to                 
C                    subdivide the list.                                        
C                                                                               
C***********************************************************************        
                                                                                
      INTEGER*2 N

      INTEGER*4  HEAD,TAIL,LHEAD,LTAIL,HHEAD,HTAIL,HEADS(20),TAILS(20),         
     *           NSTACK
                                                             
      CHARACTER*13 LIST(*)
                                                                                
      NSTACK = 0                                                                
                                                                                
      HEAD = 1                                                                  
      TAIL = N                                                                  
                                                                                
10    IF (NSTACK .GT. 0) THEN                                                   
          HEAD = HEADS (NSTACK)                                                 
          TAIL = TAILS (NSTACK)                                                 
          NSTACK = NSTACK - 1                                                   
      END IF                                                                    
                                                                                
20    IF (HEAD .LT. TAIL) THEN                                                  
          CALL PARTIT (LIST,HEAD,TAIL,LHEAD,LTAIL,HHEAD,HTAIL)                  
          IF ( (LTAIL - LHEAD) .LT. (HTAIL - HHEAD) ) THEN                      
               IF (NSTACK .GE. 20) THEN                                         
                    WRITE (6,*) '** STACK OVERFLOW **'                          
                    STOP                                                        
               END IF                                                           
               NSTACK = NSTACK + 1                                              
               HEADS (NSTACK) = HHEAD                                           
               TAILS (NSTACK) = HTAIL                                           
               HEAD = LHEAD                                                     
               TAIL = LTAIL                                                     
          ELSE IF (NSTACK .GT. 20) THEN                                         
                    WRITE (6,*) '** STACK OVERFLOW **'                          
                    STOP                                                        
          ELSE                                                                  
               NSTACK = NSTACK + 1                                              
               HEADS (NSTACK) = LHEAD                                           
               TAILS (NSTACK) = LTAIL                                           
               HEAD = HHEAD                                                     
               TAIL = HTAIL                                                     
          END IF                                                                
                                                                                
          GOTO 20                                                               
                                                                                
      END IF                                                                    
                                                                                
      IF (NSTACK .NE. 0) GOTO 10                                                
                                                                                
      END                                                                       
                                                                                
                                                                                
      SUBROUTINE PARTIT (LIST,HEAD,TAIL,LHEAD,LTAIL,HHEAD,HTAIL)                
                                                                                
      INTEGER*4  HEAD,TAIL,LHEAD,LTAIL,HHEAD,HTAIL                              
      INTEGER*4  I,J
                                                 
      CHARACTER*13 PIVOT,LIST(*),TEMP                               
                                                                                
      PIVOT = LIST ((HEAD+TAIL)/2)                                              
                                                                                
      I = HEAD - 1                                                              
      J = TAIL + 1                                                              
                                                                                
10    I = I + 1                                                                 
      IF (LIST(I) .LT. PIVOT) GOTO 10                                           
                                                                                
20    J = J - 1                                                                 
      IF (LIST(J) .GT. PIVOT) GOTO 20                                           
                                                                                
      IF (I .LT. J) THEN                                                        
          TEMP = LIST (I)                                                       
          LIST (I) = LIST (J)                                                   
          LIST (J) = TEMP                                                       
      END IF                                                                    
                                                                                
      IF (I .LT. J) GOTO 10                                                     
                                                                                
      LHEAD = HEAD                                                              
      HTAIL = TAIL                                                              
                                                                                
      IF (I .EQ. J) THEN                                                        
          LTAIL = J - 1                                                         
          HHEAD = I + 1                                                         
      ELSE                                                                      
          LTAIL = J                                                             
          HHEAD = I                                                             
      END IF                                                                    
                                                                                
      END                                                                       
