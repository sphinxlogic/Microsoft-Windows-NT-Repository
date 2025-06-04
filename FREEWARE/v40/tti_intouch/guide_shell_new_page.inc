          if  not making_report  then  exit routine
          page_counter% = page_counter% + 1
          gosub show_recs_printed
          gosub show_pages_printed
          gosub show_eoj
          print #out_ch: chr$(12);
          gosub check_title_substitutes
          page$ = "Page " + str$(page_counter%)
          p% = len(page$)
          t% = len(title$(1))
          rpt_width% = max(report_width%,t%+22%)
          rpt_width% = max(rpt_width%, len(title$(2)))
          rpt_width% = max(rpt_width%, len(title$(3)))
          z% = (rpt_width% - t%) / 2
          if  z% < 13%  then  z% = 13% ! leave room for the date
          print #out_ch: today$; 
            print #out_ch: tab(z% + 1); title$(1);
            print #out_ch: tab(rpt_width% - p% + 1); page$; chr$(0)
            line_counter% = 1
          for i% = 2 to titles%
            z% = (rpt_width% - len(title$(i%)))/2
            if  z% < 1%  then  z% = 1%
            print #out_ch: tab(z% + 1); title$(i%); chr$(0)
            line_counter% = line_counter% + 1
          next i%
          print #out_ch: chr$(0)
          line_counter% = line_counter% + 1
          {COLHEADS}
          first_page_line% = true
        end routine



21100   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C H E C K   T I T L E   S U B S T I T U T E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine check_title_substitutes
          when exception in
            {titles}
          use
            continue
          end when
