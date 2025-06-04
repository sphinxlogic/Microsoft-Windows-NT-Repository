///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
//
// Edit History
//
//	  Started November 1995 by Andrew Davison.
//
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

#include "wxhtml.h"
#include "htmlpars.h"
#include "htmltabl.h"

const float DEFAULT_BORDER_WIDTH = 1.0F;
const float DEFAULT_CELL_SPACING = 2.0F;
const float DEFAULT_CELL_PADDING = 1.0F;
const float EXTRA_X_SPACING = 2.0F;
const float EXTRA_Y_SPACING = 4.0F;

ListOf table_list;

///////////////////////////////////////////////////////////////////////////////

void TableCell::Adjust(float x, float y)
{
	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->SetLeft(x + obj->GetLeft());
			obj->SetTop(y + obj->GetTop());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void TableCell::GetDimensions(Table* t)
{
	float min_width = 0.0F;
	width = height = 0.0F;

	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		float line_width = 0.0F, line_height = 0.0F;
		Bool first = TRUE;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;

			// Accumulate the line width...

			//line_width += obj->GetWidth();
			line_width = obj->GetLeft() + obj->GetWidth();
			line_height = wxMax(line_height, obj->GetHeight());

			// Find the MINIMUM cell width...

			if (first)
			{
				float first_width;

				if (obj->IsText())
				{
					HtmlText* tt = (HtmlText*)obj;
					string text = tt->GetText();

					// Find the width of the first WORD in the line...

					const char* ptr = strchr(text.data(), ' ');

					if (ptr && (ptr != text.data()))
					{
						string s(text.data(), ptr - text.data());
						float w, h;

						obj->HtmlObject::SetFont(&t->mem_dc);
						t->mem_dc.GetTextExtent((char*)s.data(), &w, &h);
						first_width = w;
					}
					else
						first_width = obj->GetWidth();

					min_width = wxMax(min_width, first_width);
					first = FALSE;
				}
				else if (obj->IsImage())
				{
					first_width = obj->GetWidth();
					min_width = wxMax(min_width, first_width);
					first = FALSE;
				}
			}
		}

		// Find the MAXIMUM cell width...

		width = wxMax(width, line_width);
		height += line_height;

		//tags->Append((wxObject*)item);
	}

	if (col_span == 1)
	{
		t->max_width[col_nbr] =
			wxMax(t->max_width[col_nbr], width + (2 * EXTRA_X_SPACING));

		t->min_width[col_nbr] =
			wxMax(t->min_width[col_nbr], min_width + (2 * EXTRA_X_SPACING));
	}
#if 1
	else
	{
		float spanned_width = 0.0F;

		for (int i = 0; i < col_span; i++)
			spanned_width += t->max_width[col_nbr + i];

		// If a cell overflows it's spannees, then spread the
		// difference over the spanned cells...

		if (width > spanned_width)
		{                                           
			float delta = (width - spanned_width) / col_span;
			for (int i = 0; i < col_span; i++)
				t->max_width[col_nbr + i] += delta;
		}
	}
#endif

	if (row_span == 1)
		t->current_row->max_height =
			wxMax(t->current_row->max_height, height + (2 * EXTRA_Y_SPACING));
}

///////////////////////////////////////////////////////////////////////////////

//#include <iostream.h>
//static ofstream debug("x.out");

void Table::AutoLayout(wxHtml* canvas, float drawing_width)
{
	if (spec_width)
	{
		if (pc)
		{
			drawing_width *= spec_width;
			drawing_width /= 100;
		}
		else
			drawing_width = spec_width;
	}

	int i, cnt = 0;

	for (i = 0; i < max_cols; i++)
	{
		if (col_spec_width[i] > 0)
		{
			if (col_pc[i])
			{
				max_width[i] *= col_spec_width[i];
				max_width[i] /= 100;
			}
			else
				max_width[i] = col_spec_width[i];

			cnt++;
		}
	}

	// Calculate minimum and maximum table widths...

	float w1 = 0.0F, w2 = 0.0F;

	for (i = 0; i < max_cols; i++)
	{
		w1 += min_width[i];
		w2 += max_width[i];
	}

	// Does it fit?

	if (w2 < drawing_width)
	{
		if (spec_width)
		{                                 
			float ratio = drawing_width / w2;

			for (int ii = 0; ii < max_cols; ii++)
				max_width[ii] *= ratio;
		}
	}
	else
	{
		// No, so adjust everything...

		//drawing_width -= (2*Border()) + Spacing() + (max_cols*2*Padding()) + (max_cols*Spacing());

		float W = drawing_width - w1;
		float D = w2 - w1;

		for (i = 0; i < max_cols; i++)
		{
			//if (col_spec_width[i] == 0)
			{
				float m = min_width[i];
				float M = max_width[i];
				float d = M - m;
                               
				m += (d * W) / (D?D:1);

				max_width[i] = m - (2*Padding() + Spacing());
			}
		}
	}

	// Go thru each row...

	ListOfIterator ilist = rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;
			float cell_width = 0.0F, cell_height = 0.0F;

			// Calculate cell width...

			for (int ii = 0; ii < cell->col_span; ii++)
				cell_width += max_width[cell->col_nbr+ii];

			// Go thru each line...

			ListOfIterator ilist3 = cell->tags;
			void* a3;

			for (int ok3 = ilist3.First(a3); ok3; ok3 = ilist3.Next(a3))
			{
				wxHtml::Tag* item = (wxHtml::Tag *)a3;
				float x = 0.0F, line_height = 0.0F, actual_height = 0.0F;
				int cnt = 0;
				Bool first = TRUE;

				if (item->objects.Count() > 1)
				{
					//cell->left = TRUE;
					//cell->center = cell->right = FALSE;

					//cell->top = TRUE;
					//cell->middle = cell->bottom = FALSE;
				}

				// Go thru each object...

				ListOfIterator ilist4 = item->objects;
				void* a4;

				for (int ok4 = ilist4.First(a4); ok4; ok4 = ilist4.Next(a4))
				{
					HtmlObject* obj = (HtmlObject*)a4;
					obj->SetTop(item->y);

					if (first)
					{
						x = obj->GetLeft();
						first = FALSE;
					}

					// Recursively apply to tables...

					if (obj->IsTable())
					{
						obj->SetLeft(x);
						Table* table = ((HtmlTable*)obj)->GetTable();
						table->AutoLayout(canvas, cell_width);
						table->Process();
						max_width[cell->col_nbr] = wxMax(max_width[cell->col_nbr], table->width);
						actual_height = wxMax(actual_height, obj->GetHeight());
					}
					else if (obj->IsImage())
					{
						HtmlImage* image = (HtmlImage*)obj;

						if (!image->IsLeftAligned() && !image->IsRightAligned())
							actual_height = wxMax(actual_height, obj->GetHeight());
					}
					else
						actual_height = wxMax(actual_height, obj->GetHeight());

					line_height = wxMax(line_height, obj->GetHeight());

					float w = obj->GetWidth();

					// Check for cell wrap...

					if ((x + w) > cell_width)
					{
						cell->left = TRUE;
						cell->center = cell->right = FALSE;

						cell->top = TRUE;
						cell->middle = cell->bottom = FALSE;

						// Create a new line...

						wxHtml::Tag* titem = new wxHtml::Tag;
						titem->center = item->center;
						titem->p_center = item->p_center;
						titem->y = item->y + actual_height;
						titem->left_margin = item->left_margin;
						titem->right_margin = item->right_margin;
						titem->left_floating = item->left_floating;
						titem->right_floating = item->right_floating;
						item = titem;

						if (ilist3.PeekNext())
							ilist3.InsertAfter(titem);
						else
							cell->tags.Append(titem);

						// If text then try to split it...

						line_height = 0.0F;

						if (obj->IsText())
						{
							HtmlText* tt = (HtmlText*)obj;
							string text = tt->GetText();

							// Find the end of the first WORD in the line, a word
							// is deemed to be delimited by a space or hyphen...

							const char* ptr = text.data();
							Bool not_wrapped = TRUE;

							obj->HtmlObject::SetFont(&mem_dc);

							do
							{
								const char* ptr2 = strchr(ptr, ' ');

								if (!ptr2)
									ptr2 = strchr(ptr, 0);
								else
									ptr2++;

								string s(ptr, ptr2 - ptr);
								float _w = 0.0F, _h = 0.0F;

								mem_dc.GetTextExtent((char*)s.data(), &_w, &_h);

								// Found a wrap point?

								if (x && ((x + _w) > cell_width))
								{
									not_wrapped = FALSE;
									cell->width = cell_width;

									// Fix old object...

									string s2(text.data(), ptr - text.data());
									mem_dc.GetTextExtent((char*)s2.data(), &_w, &_h);
									tt->SetText(s2, _w);

									// Fix new object...

									mem_dc.GetTextExtent(ptr, &_w, &_h);
									x = item->left_margin;
									line_height = _h;
									obj = new HtmlText(canvas, x, titem->y, _w, _h, ptr);
									x += _w;
									titem->objects.Append(obj);

									break;
								}

								// Try the next word...

								x += _w;
								ptr = ptr2;
							}
							  while (*ptr);

							if (!cnt && not_wrapped) continue;
						}
						else
						{
							if (!cnt) continue;
						}

						// Transfer remaining objects to the new line...

						while (ilist4.Next(a4))
						{
							obj = (HtmlObject*)a4;
							obj->SetLeft(x);
							obj->SetTop(titem->y);
							x += obj->GetWidth();
							line_height = wxMax(line_height, obj->GetHeight());
							titem->objects.Append(obj);
							ilist4.Delete();
							//ilist4.Next(a4);
						}

						break;
					}
					else
					{
						x += w;
					}

					//cnt++;
				}

				cell_height += line_height;
			}

			row->max_height = wxMax(row->max_height, cell_height+5);
		}
	}
}


