/*
 * $Id: EpsViewer.h 3.38.2.5 1992/11/10 02:08:00 woo Exp $
 *
 */

#import <appkit/Application.h>
#import <appkit/graphics.h>
#import <dpsclient/dpsclient.h>
#import <appkit/Window.h>

@interface EpsViewer:Application
{
	id theNewWin;
}

- windowCreate:(NXCoord)width Height:(NXCoord)height;
- (NXRect *)nextRectForWidth:(NXCoord)width Height:(NXCoord)height;

@end


