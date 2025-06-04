///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#include <wx_prec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "wxhtml.h"
#include "htmlpars.h"

const float TEXT_FACTOR = 1.0F;
const ADJUST = 2;

static char buf[1024*4];
static char tag[1024*31];		      // Has to be big for comments

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Parse(wxDC* dc)
{
	in_progress++;

	ListOf includes_src, includes_save;
	const float DEFAULT_RIGHT_MARGIN = (float)(drawing_w - SCROLLBAR_WIDTH);
	char* dst = buf;
	char last_ch = ' ';
	const char* src = text;
	char* save_src = 0;
	int quoting = 0;

	x = left_margin = default_left_margin = DEFAULT_LEFT_MARGIN;
	right_margin = default_right_margin = DEFAULT_RIGHT_MARGIN;
	y = left_margin;

	finished = 0, tagged = 0, coded = 0, preformatted = 0;
	tabs = 0, p_center = 0, p_right = 0, quote_level = 0, nobr = 0, ignore = 0;
	right_limit_y = left_limit_y = 0.0F;
	addressing_max_h = max_h = 0.0F;
	quiet = 0, overflow = 0, center = 0, right = 0, addressing = 0, framing = 0;
	states = 0; mapping = 0, tabling = 0, comment = 0, nowrap = heading = wsmling = 0;
	left_floating = right_floating = 0;
	bgsound = wallpaper = "";
	bgloop = 1;
	extent_x = 0.0F;			//drawing_w;// + SCROLLBAR_WIDTH;
	extent_y = 0.0F;			//drawing_h;// + SCROLLBAR_WIDTH;
	previous_object = 0;
	have_body = FALSE;
	is_wsml = FALSE;
	have_frames = FALSE;
	bad_include = FALSE;
	deferred_images = FALSE;
	line_nbr = 1;

	fsz = basefont = n0;

	if (plain_text)
	{
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz-1, wxMODERN, style, weight));
		mode = FIXED;
	}
	else
	{
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz, family, style, weight));
		mode = NONE;
	}

	// Default character size???

	float tmp_w, tmp_h;
	dc->GetTextExtent("X", &tmp_w, &tmp_h);
	tmp_h *= TEXT_FACTOR;
	char_width = tmp_w, char_height = tmp_h;

loop:

	while (!cancel && *src)
	{
		if (*src == '\r')
		{
			src++;
			continue;
		}

		if (word_break && !tagged && !coded && (dst != buf) &&
			((*src == ' ') || (*src == '\t') ||
				(!preformatted && (*src == '\n'))))
		{
			*dst = 0;
			float tmp_w, tmp_h;
			dc->GetTextExtent(buf, &tmp_w, &tmp_h);

			if ((x + tmp_w) < right_margin)
			{
				*dst++ = ' ';
				*dst = 0;
				dc->GetTextExtent(buf, &tmp_w, &tmp_h);
				tmp_h *= TEXT_FACTOR;
				Hold(new HtmlText(this, x, y, tmp_w, tmp_h, buf));
				addressing_max_h = wxMax(addressing_max_h, tmp_h);
				if (tmp_h > max_h) max_h = tmp_h;
				x += tmp_w;
				dst = buf;
				src++;
				last_ch = ' ';
				continue;
			}
		}

		// Check for line-wrap...

		if ( /*!plain_text &&*/ !preformatted && !tagged && !coded &&
			!nobr && !quiet && !comment && !addressing && !nowrap &&
			(dst != buf) && ((*src == ' ') || (*src == '\t') ||
			(*src == '\n') || (*src == '<')) )
		{
			*dst = 0;

			float tmp_w, tmp_h;
			dc->GetTextExtent(buf, &tmp_w, &tmp_h);
			tmp_h *= TEXT_FACTOR;
			if (tmp_h > max_h) max_h = tmp_h;

			if ((x + tmp_w) >= right_margin)
			{
				// Find a suitable break-point...

				while ((*dst != ' ') && (*dst != '-') &&
							(*dst != '/') && (dst != buf))
					--dst;

				*dst = 0;

				if ((x != left_margin) || (dst != buf))
				{
					overflow = 1;
					src--;

					while ((*src != ' ') && (*src != '-') &&
							(*src != '/') && (*src != '\n') &&
							(*src != '\t') && *src)
						--src;

					src++;
				}
			}
		}

		if (!tagged && !coded && overflow)
		{
			extent_x = wxMax(extent_x, x);

			overflow = 0;

			while (!plain_text && !preformatted && *src &&
						((*src == ' ') || (*src == '\t') ||
							(*src == '\r') || (*src == '\n')))
					src++;

			*dst = 0;

			// Hack, get rid of trailing spaces...
			while ((dst != buf) && (dst[-1] == ' '))
				--dst;

			*dst = 0;

			float tmp_w, tmp_h;

			if (dst != buf)
			{
				dc->GetTextExtent(buf, &tmp_w, &tmp_h);
				tmp_h *= TEXT_FACTOR;
				Hold(new HtmlText(this, x, y, tmp_w, tmp_h, buf));
				addressing_max_h = wxMax(addressing_max_h, tmp_h);
				dst = buf;
			}
			else
				tmp_h = char_height;

			Release(dc);

			if (tmp_h > max_h) max_h = tmp_h;

			if ((y >= left_limit_y) && !tabs)
			{
				left_floating = 0;
				left_limit_y = 0.0F;
				left_margin = default_left_margin;
			}

			if (y >= right_limit_y)
			{
				right_floating = 0;
				right_limit_y = 0.0F;
				right_margin = default_right_margin;
			}

			y += max_h;
			max_h = 0.0F;

			if (!left_floating && left_limit_y)
			{
				y = left_limit_y + ADJUST;
				left_limit_y = 0.0F;
			}

			x = left_margin;
			if (tabs && 0) 
			{
				x += tabs * TAB_SIZE * char_width;
				x += char_width;
			}

			// Dodgy bug-fix...
			//if (x >= right_margin) x = left_margin;

			// At the end of the visible region?

			if (!finished && (y > (((float)starting_y*VSCROLL_INCREMENT) + drawing_h + 100)))
			{
				finished = 1;
				//wxYield();
			}

			// If printing, determine if we need a page break...

			if (dc != GetDC())
			{
				if (y > drawing_h)
				{
					dc->EndPage();
					dc->StartPage();
					y = 0.0F;
				}
			}

			// Eat whitespace?
			if (!plain_text && !preformatted)
			{
				while (*src && ((*src == ' ') || (*src == '\t') ||
							(*src == '\r') || (*src == '\n')))
					src++;
			}

			last_ch = ' ';
		}

		// Only care about new-line if preformatted-data...

		else if (*src == '\n')
		{
			if (!line_nbr++ % 100)
			{
				//wxYield();
			}

			src++;

			if ((tagged || coded) && quoting) continue;

			if (plain_text)
				overflow = 1;
			else if (preformatted)
			{
				*dst = 0;

				float tmp_w, tmp_h;

				if (dst != buf)
				{
					dc->GetTextExtent(buf, &tmp_w, &tmp_h);
					tmp_h *= TEXT_FACTOR;
					Hold(new HtmlText(this, x, y, tmp_w, tmp_h, buf));
					addressing_max_h = wxMax(addressing_max_h, tmp_h);
					x += tmp_w;
				}
				else
					tmp_h = char_height;

				Release(dc);

				extent_x = wxMax(extent_x, x);

				if (tmp_h > max_h) max_h = tmp_h;
				y += max_h;
				max_h = 0.0F;
				dst = buf;

				if (!left_floating && left_limit_y)
				{
					y = left_limit_y + ADJUST;
					left_limit_y = 0.0F;
				}

				x = left_margin;
				if (tabs && 0)
				{
					x += tabs * TAB_SIZE * 2 * char_width;
					x += char_width;
				}

				// Dodgy bug-fix...
				//if (x >= right_margin) x = left_margin;
			}
			else if ((last_ch != ' ') && ((x != left_margin) || (dst != buf)))
			{
				*dst++ = ' ';
				last_ch = ' ';
			}
			else
				last_ch = 0;
		}

		// Start of tag or code...
		else if (!plain_text && !comment &&
				((*src == '<') || ((*src == '&') && !tagged)) &&
				(isalpha(src[1]) || (src[1] == '#') ||
				(src[1] == '!') || (src[1] == '/')))
		{
			*dst = 0;

			if ((dst != buf) && !quiet)
			{
				float tmp_w, tmp_h;
				dc->GetTextExtent(buf, &tmp_w, &tmp_h);
				tmp_h *= TEXT_FACTOR;
				Hold(new HtmlText(this, x, y, tmp_w, tmp_h, buf));
				addressing_max_h = wxMax(addressing_max_h, tmp_h);
				if (tmp_h > max_h) max_h = tmp_h;
				x += tmp_w;
			}

			if (*src == '<')
				tagged = 1;
			else if (*src == '&')
				coded = 1;

			src++;
			dst = tag;

			last_ch = 0;
		}

		// End of code...
		else if (coded && (*src == ';'))
		{
			coded = 0;
			src++;
			*dst = 0;
			dst = buf;

			// Break it up coz MSVC 4.0 spits the dummy...

			if (tag[0] == '#')
				*dst++ = char(atoi(tag+1));
			else if (!strcmp(tag, "lt"))
				*dst++ = '<';
			else if (!strcmp(tag, "gt"))
				*dst++ = '>';
			else if (!strcmp(tag, "quot"))
				*dst++ = '"';
			else if (!strcmp(tag, "amp"))
				*dst++ = '&';
			else if (!strcmp(tag, "nbsp"))
				*dst++ = (unsigned char)160;
			else if (!strcmp(tag, "iexcl"))
				*dst++ = (unsigned char)161;
			else if (!strcmp(tag, "cent"))
				*dst++ = (unsigned char)162;
			else if (!strcmp(tag, "pound"))
				*dst++ = (unsigned char)163;
			else if (!strcmp(tag, "curren"))
				*dst++ = (unsigned char)164;
			else if (!strcmp(tag, "yen"))
				*dst++ = (unsigned char)165;
			else if (!strcmp(tag, "brvbar"))
				*dst++ = (unsigned char)166;
			else if (!strcmp(tag, "brkbar"))
				*dst++ = (unsigned char)166;
			else if (!strcmp(tag, "sect"))
				*dst++ = (unsigned char)167;
			else if (!strcmp(tag, "uml"))
				*dst++ = (unsigned char)168;
			else if (!strcmp(tag, "die"))
				*dst++ = (unsigned char)168;
			else if (!strcmp(tag, "copy"))
				*dst++ = (unsigned char)169;
			else if (!strcmp(tag, "ordf"))
				*dst++ = (unsigned char)170;
			else if (!strcmp(tag, "laquo"))
				*dst++ = (unsigned char)171;
			else if (!strcmp(tag, "not"))
				*dst++ = (unsigned char)172;
			else if (!strcmp(tag, "shy"))
				*dst++ = (unsigned char)173;
			else if (!strcmp(tag, "reg"))
				*dst++ = (unsigned char)174;
			else if (!strcmp(tag, "macr"))
				*dst++ = (unsigned char)175;

			if (!strcmp(tag, "hibar"))
				*dst++ = (unsigned char)175;
			else if (!strcmp(tag, "deg"))
				*dst++ = (unsigned char)176;
			else if (!strcmp(tag, "plusm"))
				*dst++ = (unsigned char)177;
			else if (!strcmp(tag, "sup2"))
				*dst++ = (unsigned char)178;
			else if (!strcmp(tag, "sup3"))
				*dst++ = (unsigned char)179;
			else if (!strcmp(tag, "acute"))
				*dst++ = (unsigned char)180;
			else if (!strcmp(tag, "micro"))
				*dst++ = (unsigned char)181;
			else if (!strcmp(tag, "para"))
				*dst++ = (unsigned char)182;
			else if (!strcmp(tag, "middot"))
				*dst++ = (unsigned char)183;
			else if (!strcmp(tag, "cedil"))
				*dst++ = (unsigned char)184;
			else if (!strcmp(tag, "sup1"))
				*dst++ = (unsigned char)185;
			else if (!strcmp(tag, "ordm"))
				*dst++ = (unsigned char)186;
			else if (!strcmp(tag, "raquo"))
				*dst++ = (unsigned char)187;
			else if (!strcmp(tag, "frac14"))
				*dst++ = (unsigned char)188;
			else if (!strcmp(tag, "frac12"))
				*dst++ = (unsigned char)189;
			else if (!strcmp(tag, "frac34"))
				*dst++ = (unsigned char)190;
			else if (!strcmp(tag, "iquest"))
				*dst++ = (unsigned char)191;
			else if (!strcmp(tag, "Agrave"))
				*dst++ = (unsigned char)192;
			else if (!strcmp(tag, "Aacute"))
				*dst++ = (unsigned char)193;
			else if (!strcmp(tag, "Acirc"))
				*dst++ = (unsigned char)194;
			else if (!strcmp(tag, "Atilde"))
				*dst++ = (unsigned char)195;
			else if (!strcmp(tag, "Auml"))
				*dst++ = (unsigned char)196;
			else if (!strcmp(tag, "Aring"))
				*dst++ = (unsigned char)197;
			else if (!strcmp(tag, "AElig"))
				*dst++ = (unsigned char)198;
			else if (!strcmp(tag, "Ccedil"))
				*dst++ = (unsigned char)199;
			else if (!strcmp(tag, "Egrave"))
				*dst++ = (unsigned char)200;
			else if (!strcmp(tag, "Eacute"))
				*dst++ = (unsigned char)201;
			else if (!strcmp(tag, "Ecirc"))
				*dst++ = (unsigned char)202;
			else if (!strcmp(tag, "Euml"))
				*dst++ = (unsigned char)203;
			else if (!strcmp(tag, "Igrave"))
				*dst++ = (unsigned char)204;
			else if (!strcmp(tag, "Iacute"))
				*dst++ = (unsigned char)205;
			else if (!strcmp(tag, "Icirc"))
				*dst++ = (unsigned char)206;
			else if (!strcmp(tag, "Iuml"))
				*dst++ = (unsigned char)207;
			else if (!strcmp(tag, "ETH"))
				*dst++ = (unsigned char)208;
			else if (!strcmp(tag, "Dstrok"))
				*dst++ = (unsigned char)208;
			else if (!strcmp(tag, "Ntilde"))
				*dst++ = (unsigned char)209;
			else if (!strcmp(tag, "Ograve"))
				*dst++ = (unsigned char)210;
			else if (!strcmp(tag, "Oacute"))
				*dst++ = (unsigned char)211;
			else if (!strcmp(tag, "Ocirc"))
				*dst++ = (unsigned char)212;

			if (!strcmp(tag, "Otilde"))
				*dst++ = (unsigned char)213;
			else if (!strcmp(tag, "Ouml"))
				*dst++ = (unsigned char)214;
			else if (!strcmp(tag, "times"))
				*dst++ = (unsigned char)215;
			else if (!strcmp(tag, "Oslash"))
				*dst++ = (unsigned char)216;
			else if (!strcmp(tag, "Ugrave"))
				*dst++ = (unsigned char)217;
			else if (!strcmp(tag, "Uacute"))
				*dst++ = (unsigned char)218;
			else if (!strcmp(tag, "Ucirc"))
				*dst++ = (unsigned char)219;
			else if (!strcmp(tag, "Uuml"))
				*dst++ = (unsigned char)220;
			else if (!strcmp(tag, "Yacute"))
				*dst++ = (unsigned char)221;
			else if (!strcmp(tag, "THORN"))
				*dst++ = (unsigned char)222;
			else if (!strcmp(tag, "szlig"))
				*dst++ = (unsigned char)223;
			else if (!strcmp(tag, "agrave"))
				*dst++ = (unsigned char)224;
			else if (!strcmp(tag, "aacute"))
				*dst++ = (unsigned char)225;
			else if (!strcmp(tag, "acirc"))
				*dst++ = (unsigned char)226;
			else if (!strcmp(tag, "atilde"))
				*dst++ = (unsigned char)227;
			else if (!strcmp(tag, "auml"))
				*dst++ = (unsigned char)228;
			else if (!strcmp(tag, "aring"))
				*dst++ = (unsigned char)229;
			else if (!strcmp(tag, "aelig"))
				*dst++ = (unsigned char)230;
			else if (!strcmp(tag, "ccedil"))
				*dst++ = (unsigned char)231;
			else if (!strcmp(tag, "egrave"))
				*dst++ = (unsigned char)232;
			else if (!strcmp(tag, "eacute"))
				*dst++ = (unsigned char)233;
			else if (!strcmp(tag, "ecirc"))
				*dst++ = (unsigned char)234;
			else if (!strcmp(tag, "euml"))
				*dst++ = (unsigned char)235;
			else if (!strcmp(tag, "igrave"))
				*dst++ = (unsigned char)236;
			else if (!strcmp(tag, "iacute"))
				*dst++ = (unsigned char)237;
			else if (!strcmp(tag, "icirc"))
				*dst++ = (unsigned char)238;
			else if (!strcmp(tag, "iuml"))
				*dst++ = (unsigned char)239;
			else if (!strcmp(tag, "eth"))
				*dst++ = (unsigned char)240;
			else if (!strcmp(tag, "ntilde"))
				*dst++ = (unsigned char)241;
			else if (!strcmp(tag, "ograve"))
				*dst++ = (unsigned char)242;
			else if (!strcmp(tag, "oacute"))
				*dst++ = (unsigned char)243;
			else if (!strcmp(tag, "ocirc"))
				*dst++ = (unsigned char)244;
			else if (!strcmp(tag, "otilde"))
				*dst++ = (unsigned char)245;
			else if (!strcmp(tag, "ouml"))
				*dst++ = (unsigned char)246;
			else if (!strcmp(tag, "divide"))
				*dst++ = (unsigned char)247;
			else if (!strcmp(tag, "oslash"))
				*dst++ = (unsigned char)248;
			else if (!strcmp(tag, "ugrave"))
				*dst++ = (unsigned char)249;
			else if (!strcmp(tag, "uacute"))
				*dst++ = (unsigned char)250;
			else if (!strcmp(tag, "ucirc"))
				*dst++ = (unsigned char)251;
			else if (!strcmp(tag, "uuml"))
				*dst++ = (unsigned char)252;
			else if (!strcmp(tag, "yacute"))
				*dst++ = (unsigned char)253;
			else if (!strcmp(tag, "thorn"))
				*dst++ = (unsigned char)254;
			else if (!strcmp(tag, "yuml"))
				*dst++ = (unsigned char)255;

			last_ch = 0;
		}

#if 1
		else if (!IsCompiling() && !comment && tagged && (src[0] == '!') &&
					(src[1] == '-') && (src[2] == '-'))
		{
			src += 3;
			comment = 1;
		}
		else if (comment && (src[0] == '-') &&
					(src[1] == '-') && (src[2] == '>'))
		{
			src += 3;
			comment = 0;
		}
#endif
		else if (comment)
		{
	      	src++;
		}
		else if (tagged && (*src == '>'))
		{
			last_ch = 0;
			tagged = 0;
			src++;
			*dst = 0;
			dst = buf;

			if (ParseSome(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseSome1(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseSome2(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseSome3(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseTable(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseFrame(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseForm(dc, dst, buf, tag));
			else if (!ignore && !wsmling && ParseUser(dc, dst, buf, tag));

#if ALLOW_SITE_MODE
			else if (!ignore && ParseSite(dc, dst, buf, tag));
			
			else if (!ignore && (IsCompiling() || wsmling) && !strncmpi(tag, "INCLUDE", 7))
			{
				string key;
				string name;
				const char* ptr = tag+7;

				while (GetAttributeValuePair(ptr, key, name))
				{
					if (!strcmpi(key.data(), "SRC"))
					{
						string localname = ""; 
						GetURL(name, TRUE, localname, FALSE);
						char* tmp_buf = (char*)ReadFile(localname);

						if (tmp_buf)
						{
							includes_src.Append((void*)src);
							includes_save.Append(save_src);
							src = save_src = tmp_buf;
							goto loop;
						}
						else
						{
							wxMessageBox((char*)(string("Bad include: ")+localname).data());
							bad_include = TRUE;
						}
					}
				}
			}
#endif

			else if (!ignore && !IsCompiling() && !strncmpi(tag, "!--#include", 11))
			{
				string key;
				string name;
				const char* ptr = tag+11;

				while (GetAttributeValuePair(ptr, key, name))
				{
					if (!strcmpi(key.data(), "file") || !strcmpi(key.data(), "virtual"))
					{
						string localname = "";

						GetURL(name, TRUE, localname, FALSE);
						char* tmp_buf = (char*)ReadFile(localname);

						if (tmp_buf)
						{
							includes_src.Append((void*)src);
							includes_save.Append(save_src);
							src = save_src = tmp_buf;
							goto loop;
						}
					}
				}
			}
			else
			{
				Hold(new HtmlMarkup(this, x, y, tag));
			}
		}
		else if (!plain_text && !preformatted && (*src == '\t'))
		{
			if ((last_ch != ' ') && ((x != left_margin) || (dst != buf)))
			{
				*dst++ = ' '; src++;
				last_ch = ' ';
			}

			while (*src && ((*src == ' ') || (*src == '\t') ||
						(*src == '\r') || (*src == '\n')))
				src++;
		}
		else if (!plain_text && !preformatted && (*src == ' '))
		{
			if ((last_ch != ' ') && ((x != left_margin) || (dst != buf)))
			{
				*dst++ = ' '; src++;
				last_ch = ' ';
			}

			while (*src && ((*src == ' ') || (*src == '\t') ||
						(*src == '\r') || (*src == '\n')))
				src++;
		}
		else if (*src == '"')
		{
			last_ch = *dst++ = *src++;
			quoting = !quoting;
		}
		else
		{
			last_ch = *dst++ = *src++;
		}
	}


	if (dst != buf)
	{
		*dst = 0;
		float tmp_w, tmp_h;
		dc->GetTextExtent(buf, &tmp_w, &tmp_h);
		tmp_h *= TEXT_FACTOR;
		Hold(new HtmlText(this, x, y, tmp_w, tmp_h, buf));
		addressing_max_h = wxMax(addressing_max_h, tmp_h);
		x += tmp_w;
		if (tmp_h > max_h) max_h = tmp_h;
		y += max_h;

		extent_x = wxMax(extent_x, x);

		if (left_floating && left_limit_y)
		{
			y = left_limit_y + ADJUST;
			left_limit_y = 0.0F;
		}
	}

	Release(dc);

	// Include processing...

	if (includes_src.Count())
	{
		delete [] save_src;
		void* a;
		includes_src.Remove(a);
		src = (char*)a;

		void* aa;
		includes_save.Remove(aa);
		save_src = (char*)aa;
		goto loop;
	}

	extent_y = wxMax(y, wxMax(left_limit_y, right_limit_y));// + SCROLLBAR_WIDTH;

	center = p_center = 0;
	finished = 0;

	thru_count++;
	draw_count++;

	while (states)
	{
		State* save = states;
		states = states->next;
		delete save;
	}

	mode = 0;
	fsz = n0;

	in_progress--;
}


