        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C R E A T E   E X P O R T   S T R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !       The user wants to export the data to a structure
        !       I need to create the definition file, build a temp file
        !       with the setup commands to define the fields and build
        !       the data file.  then I need to open the export structure
        !       for output
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine create_export_str
          message building_message$
          export_ch = _channel
          open #export_ch : name 'sys$scratch:guide_export', unique, access output
          ask #export_ch : name export_str_cmd$
          print #export_ch : '$define intouch_setup no'
          print #export_ch : '$define/user sys$output nla0:'
          print #export_ch : '$intouch/source tti_run:guide_build_export_structure'
          {export_def}
        end routine




