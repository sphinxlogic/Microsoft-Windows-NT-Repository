C
C  SYSUAF RECORD FORMAT
C  
C  SYSUAF.DAT is an INDEX file made up of the following fields
C
C  There are currently 4 keys:  key 0 - username
C                               key 1 - UIC
C                               key 2 - extended uic
C                               key 3 - owner
C
C  Most strings are counted strings, with the first position the length
C  
C  The following information was translated from a C file found on
C  the SPRING 90 DECUS TAPE [VAX90A.MEADOWS.UAF]UAF.H
C
C  COVNERTED BY R. EDEN 5/8/91
C
C DEFINE VARIOUS CONSTANTS
C
      PARAMETER       UAF$C_USER_ID   =1      ! main user ID record   
      PARAMETER       UAF$C_VERSION   =1      ! this version  
      PARAMETER       UAF$C_KEYED_PART=52     ! ISAM keys come this far       
      PARAMETER       UAF$C_AD_II     =0      ! AUTODIN-II 32bit crc code     
      PARAMETER       UAF$C_PURDY     =1      ! Purdy polynomial over salted input    
      PARAMETER       UAF$C_PURDY_V   =2      ! Purdy polynomial+variable length username     
      PARAMETER       UAF$K_FIXED     =644    ! length of fixed portion       
      PARAMETER       UAF$C_FIXED     =644    ! length of fixed portion       
      PARAMETER       UAF$K_LENGTH    =1412
      PARAMETER       UAF$C_LENGTH    =1412

C
C START OF UAF RECORD
C
      structure       /UAF_RECORD/
      UNION
      MAP
        character*1     uaf$b_rtype         ! HEADER INFORMATION
        character*1     uaf$b_version   
        integer*2       uaf$w_usrdatoff 

        character*32    uaf$t_username      ! USER INFORMATION
        union
            map
                integer*4       uaf$l_uic       
                endmap
            map
                integer*2       uaf$w_mem       
                integer*2       uaf$w_grp       
                endmap
            endunion
        integer*4       uaf$l_sub_id            
        integer*4       uaf$q_parent_id(2)      
        character*32    uaf$t_account   
        character*32    uaf$t_owner     
        character*32    uaf$t_defdev        ! ACCOUNT DEFAULTS       
        character*64    uaf$t_defdir    
        character*64    uaf$t_lgicmd    
        character*32    uaf$t_defcli    
        character*32    uaf$t_clitables 

        real*8          uaf$q_pwd          ! PASSWORD INFORMATION
        real*8          uaf$q_pwd2   
        integer*2       uaf$w_logfails  
        integer*2       uaf$w_salt      
        character*1     uaf$b_encrypt   
        character*1     uaf$b_encrypt2          
        character*1     uaf$b_pwd_length        
        character*1     uaf$fill_1

        real*8          uaf$q_expiration    ! VARIOUS DATES    
        real*8          uaf$q_pwd_lifetime   
        real*8          uaf$q_pwd_date      
        real*8          uaf$q_pwd2_date
        real*8          uaf$q_lastlogin_i
        REAL*8		uaf$q_lastlogin_n

        integer*4       uaf$q_priv(2)       ! PRIVILAGES    
        integer*4       uaf$q_def_priv(2)       
        character*20    uaf$r_min_class         
        character*20    uaf$r_max_class         

        integer*4       uaf$l_flags         ! FLAGS AND ACCESS INFORMATION
        character*3     uaf$b_network_access_p  
        character*3     uaf$b_network_access_s  
        character*3     uaf$b_batch_access_p    
        character*3     uaf$b_batch_access_s    
        character*3     uaf$b_local_access_p    
        character*3     uaf$b_local_access_s    
        character*3     uaf$b_dialup_access_p   
        character*3     uaf$b_dialup_access_s   
        character*3     uaf$b_remote_access_p   
        character*3     uaf$b_remote_access_s   
        character*12    uaf$fill_2
        character*1     uaf$b_primedays         
        character*1     uaf$fill_3

        character*1     uaf$b_pri               ! QUOTAS
        character*1     uaf$b_quepri            
        integer*2       uaf$w_maxjobs           
        integer*2       uaf$w_maxacctjobs       
        integer*2       uaf$w_maxdetach         
        integer*2       uaf$w_prccnt            
        integer*2       uaf$w_biolm             
        integer*2       uaf$w_diolm             
        integer*2       uaf$w_tqcnt             
        integer*2       uaf$w_astlm             
        integer*2       uaf$w_enqlm             
        integer*2       uaf$w_fillm             
        integer*2       uaf$w_shrfillm          
        integer*4       uaf$l_wsquota           
        integer*4       uaf$l_dfwscnt           
        integer*4       uaf$l_wsextent          
        integer*4       uaf$l_pgflquota         
        integer*4       uaf$l_cputim            
        integer*4       uaf$l_bytlm             
        integer*4       uaf$l_pbytlm            
        integer*4       uaf$l_jtquota           
        integer*2       uaf$w_proxy_lim         
        integer*2       uaf$w_proxies           
        integer*2       uaf$w_account_lim       
        integer*2       uaf$w_accounts          
        ENDMAP
      MAP
        CHARACTER*1412 STRING                     ! RECORD AS STRING FOR I/O
        ENDMAP
        ENDUNION
        endstructure

