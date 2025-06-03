#include <stdio.h>
#include <syidef.h>
#include "BANNER_PE"
#include "PEM_DEF"
#include "LANUDEF"


globaldef struct PE_BUS	    BUS_ARRAY [ PE_BUS$C_BUS_COUNT ];
globaldef struct PE_VC	    VC_ARRAY [ PE_VC$C_VC_COUNT ];

main ()
   {
    int rate, newkb, x_pos, y_pos, lx_pos;
    int width, x, y, x1, y1, y_max;
    int bus_error_pos, bus_name_pos, bus_percent_pos, bus_graph_pos, bus_graph_width;
    int vc_rexmt_pos, vc_rercv_pos, vc_name_pos, vc_percent_pos, vc_graph_pos, vc_graph_width;
    int vcs_displayed;
    char string[] = "                ";
    int status, iocnt, len; 
    long int bus_index;
    long int vc_index;
    struct PE_BUS *bus;
    struct PE_VC *vc;
    unsigned long int total_lan_bandwidth;
    unsigned long int lan_utilization;
    struct PE_VC *vc_display_array [ 100 + 1 ];
    long int hash_index;

    status = REMOTE_PE_COUNT ();

    if (status != 1)
	{
	printf("\nPE access routine returned bad status.\n");
	printf("PE Monitor was built for VMS 5.5\n");
	printf("No PE statistics can be displayed\n");
	exit();
	}

/*
 * For each BUS, update the statistics for the display.
 */

    lan_utilization = 0;
    for ( bus_index = 0; bus_index < PE_BUS$C_BUS_COUNT; bus_index++ )
      {

        bus = & BUS_ARRAY [ bus_index ];
        if ( bus -> PE_BUS$T_NAME [ 0 ] != 0 )
          {

			/*  Highlight the BUS if it is offline.  */

            if (( bus -> PE_BUS$L_FLAGS & BUS$M_ONLINE ) == 0 )
              bus -> PE_BUS$L_HIGHLIGHT_BUS = 1;
            else
              bus -> PE_BUS$L_HIGHLIGHT_BUS = 0;

			/*  Compute the BUS utilization.  */

            if ( bus -> PE_BUS$L_XMT_LAST != 0 )
              {
                bus -> PE_BUS$L_XMT_PERCENT = bus -> PE_BUS$L_XMT_BYTES - bus -> PE_BUS$L_XMT_LAST;
                bus -> PE_BUS$L_XMT_LAST = bus -> PE_BUS$L_XMT_BYTES;
                bus -> PE_BUS$L_RCV_PERCENT = bus -> PE_BUS$L_RCV_BYTES - bus -> PE_BUS$L_RCV_LAST;
                bus -> PE_BUS$L_RCV_LAST = bus -> PE_BUS$L_RCV_BYTES;

			/*  Compute the LAN utilization.  */

                if (( bus -> PE_BUS$L_FLAGS & BUS$M_LDL ) == 0 )
                  lan_utilization = lan_utilization + bus -> PE_BUS$L_XMT_PERCENT + bus -> PE_BUS$L_RCV_PERCENT;

              }
            else
              {
                bus -> PE_BUS$L_XMT_LAST = bus -> PE_BUS$L_XMT_BYTES;
                bus -> PE_BUS$L_XMT_PERCENT = 0;
                bus -> PE_BUS$L_RCV_LAST = bus -> PE_BUS$L_RCV_BYTES;
                bus -> PE_BUS$L_RCV_PERCENT = 0;
              }

            bus -> PE_BUS$L_LAN_UTILIZATION = 0;


          }
      }

		/*  Compute the utilization of each BUS.  */

    total_lan_bandwidth = 0;
    if ( lan_utilization != 0 )
      for ( bus_index = 0; bus_index < PE_BUS$C_BUS_COUNT; bus_index++ )
        {

          bus = & BUS_ARRAY [ bus_index ];
          if ( bus -> PE_BUS$T_NAME [ 0 ] != 0 )
            {

			/*  LAN segment utilization associated with this LAN adapter.  */

	      if ( bus -> PE_BUS$L_LAN_TYPE == VCIB$K_DLL_CSMACD )
                {
                  bus -> PE_BUS$L_LAN_UTILIZATION = ( bus -> PE_BUS$L_LAN_UTILIZATION / 10000000. );
                  total_lan_bandwidth = total_lan_bandwidth + 10000000;
                }
              else
                if ( bus -> PE_BUS$L_LAN_TYPE == VCIB$K_DLL_FDDI )
                  {
                    bus -> PE_BUS$L_LAN_UTILIZATION = ( bus -> PE_BUS$L_LAN_UTILIZATION / 100000000. );
                    total_lan_bandwidth = total_lan_bandwidth + 100000000;
                  }

			/*  Node load supported by this transmitter.  */

              bus -> PE_BUS$L_XMT_PERCENT = ( 100. * bus -> PE_BUS$L_XMT_PERCENT ) / lan_utilization;

			/*  Node load supported by this receiver.  */

              bus -> PE_BUS$L_RCV_PERCENT = ( 100. * bus -> PE_BUS$L_RCV_PERCENT ) / lan_utilization;

            }
        }

/*
 * Clear the Virtual Circuit display array.
 */

    for ( vc_index = 0; vc_index <= 100; vc_index++ )
      vc_display_array [ vc_index ] = 0;

   }
