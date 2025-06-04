--  CALLMON Examples
--
--  File:     EXAMPLE_COLLECTION_.ADA
--  Author:   Thierry Lelegard
--  Version:  1.0
--  Date:     24-JUL-1996
--
--  Abstract: Example package which traces the usage of Ada collections.
--


package Example_Collection is

    --  This function returns a string which uniformely identifies
    --  the current task in the application.

    function Task_Name return String;

    --  This generic routine returns the "image" of an integer value,
    --  in an hexadecimal string.

    generic
        type Element is range <>;
        Default_Width : in Positive;
    function Integer_Hexa_Image (
        Item  : in Element;
        Width : in Positive := Default_Width)
        return String;

    --  This generic routine returns the "image" of an access value,
    --  in an hexadecimal string.

    generic
        type Element is limited private;
        type Access_Element is access Element;
        Default_Width : in Positive;
    function Access_Hexa_Image (
        Item  : in Access_Element;
        Width : in Positive := Default_Width)
        return String;

end Example_Collection;
