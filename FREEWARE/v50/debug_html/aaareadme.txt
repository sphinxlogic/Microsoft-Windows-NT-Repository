DEBUG HTML
----------
This collection of command procedures create a set of debugging files for 
'typical' web pages and will work on VAX or Alpha.

The DCL may not completely bug-free or may not even be very useful in your
situation, but should be a template for performing validation testing on
web sites. debug-make.com calls each of the remaining files to
create the following. Note that all filenames are lowercased by the
procedure, primarily because my host server is a unix system. Although
these files are generated and used on a VMS system, I also need to use then
on a case sensitive system too. The pages I use these on are text rich and 
mainly HTML-2 without frames. 

The procedures are not particularly tidy and could probably be made more
efficient, a run can take a several hours with the huge number of files and
references on my site. The web site these have assisted with have over 2000
images and 250 html files with many internal and external links. The
command procedures make the maintenance and testing much easier, it couldn't
be done manually.

All files are referenced by a file called debug.htm which is copied to the 
directory being tested if it is not there.

links to all images - image-links.htm
----IMAGE-LINKS.COM------------------
This provides a clickable alphabetic indexed list of all GIF and JPG files 
and includes creation dates and size in k. It is also an example of an 
automatic HTML generator with embedded anchors.

load unused GIF or JPG links - unused-gif.htm and unused-jpg.htm
----UNUSED-GIF.COM UNUSED-JPG.COM-------------------------------
This is a page of all the pictures which exist in the directory, but 
none of the HTML files refer to them, so they are never used. (The command 
procedure that creates this has a boolean to create a clickable list only). 
This can take a while to execute as it locates each image file then 
searches all the HTML files for each.


link unused any - unused-any.htm
----UNUSED-ANY.COM--------------
This contains a list of all files in the directory which are not referred 
to by HTML files. This can take a very long time if there are many files, 
all the html files are searched for every single file in the directory.

links not found or located locally or online testing - check-ref.htm
----CHECK-REF.COM---------------------------------------------------
All the HTML files are scanned for HREF and SRC and any which cannot be 
matched with a local file are placed here. Most likely they are external 
references, but they may be local typos or missing files. It also checks
for mixed case, which can cause problems for unix web servers if it is a
typo. SOme other errors are also detected, missing quotes or empty 
references. Each HTML file is read in turn and the reference is isolated to a 
file specification which is tested for existence. The code in fact does 
screen a couple of common references which are not files e.g. mailto.
Execution should not take too long as all files are read in only once.

links to all HTML files - link-html.htm
----LINK-HTML.COM----------------------
As suggested by the title. Very quick execution as the results of an 
F$SEARCH are placed into a HTM file.

links to all files - link-all.htm
----LINK-ALL.COM-----------------
This makes everything 'clickable' with creation dates. This is fairly quick 
as well.

references within HTML files - references.htm
----REFERENCES.COM-------------------------
This test cross references all files. Taking each file, it lists what other 
HTML files also reference it. It has a preset low threshold, and lower than 
this it displays the total count in red. It is designed to find HTML pages 
that are not linked to the main body of the pages, or have few links to 
them overall. This can take a while, it locates each file in turn and 
searches the remaining files for references.

HTML not containing reference - notcontained.htm
------------------------------------------------
This simply examines all HTML files for a reference to, say index.html. It 
allows you to check that all files link back to the homepage for example, 
or are referenced by an indexing file. This is fairly quick as each HTML 
file is searched once in turn.

HTML not referenced by file - notreferenced.htm
-----------------------------------------------
This is the opposite of the previous test, it check to see which files are 
not linked by, say, the main index page. This can take a little longer as 
it searches all files once for each HTML file.

While running these procedures file access conflict errors are normal.

Pressing control-y at any time is safe and results in only a partial htm 
file.

How to use
----------
Either use directory from the freeware disk, or copy all the .COM files and 
the debug.htm file to a directory of your choice. Simply @ or submit with 
parameters the debug-make file, it will find all the others itself.

You may individually call each file, or submit it to batch with the correct 
parameters, or edit the debug-make.com to run the components you wish.

That's it, I hope you find something of use here. If you develop something 
interesting you think I should keep with the distribution then send the 
updates to me. I'll maintain a current distribution on 
http://www.python.demon.co.uk and I would appreciate you dropping me a note 
if you find this useful.

Nic Clews nic@python.demon.co.uk
