--  CALLMON Examples
--
--  File:     EXAMPLE_COLLECTION.ADA
--  Author:   Thierry Lelegard
--  Version:  1.0
--  Date:     24-JUL-1996
--
--  Abstract: Example package which traces the usage of Ada collections.
--
--            All routines prefixed with ADA$ which are intercepted in
--            this package come from module ADA$ACCESS, in the DEC Ada
--            Run-Time Library for OpenVMS Alpha V6.2. The description
--            of these routine are copied from file ADAACCESS.LIS in
--            directory [V62.ADARTL.LIS] on the OpenVMS Alpha listing
--            CD-ROMs.
--


with Callmon;

with Text_IO;
with System;
with Get_Task_Info;
with Unchecked_Conversion;
with Condition_Handling;

package body Example_Collection is

    -- Local renaming or external types and operators.

    function "=" (X, Y : in Callmon.Integer_64) return Boolean
    renames System."=";

    function "<" (X, Y : in Callmon.Integer_32) return Boolean
    renames System."<";

    --  This generic routine fetches an object at an address. The address
    --  if given by an integer type. If the address is null, return a
    --  default value.

    generic
        type Address is range <>;
        type Target is private;
        Default_Value : in Target;
    function Fetch_From_Address (
        Location : in Address;
        Default  : in Target := Default_Value)
        return Target;


    ----------------------------------------------------------------------------
    --
    --  This generic routine returns the "image" of an integer value,
    --  in an hexadecimal string.
    --
    ----------------------------------------------------------------------------

    function Integer_Hexa_Image (
        Item  : in Element;
        Width : in Positive := Default_Width)
        return String is

        Digit  : constant String := "0123456789ABCDEF";
        Image  : String (1..Width);
        Remain : Element := Item;

    begin

        for N in reverse Image'Range loop
            Image (N) := Digit (Digit'First + Natural (Remain mod 16));
            Remain := Remain / 16;
        end loop;

        return Image;

    end Integer_Hexa_Image;


    ----------------------------------------------------------------------------
    --
    --  Hexadecimal image of 32-bits and 64-bits integers.
    --
    ----------------------------------------------------------------------------

    function Hexa_Image is new Integer_Hexa_Image (
        Element => Callmon.Integer_32,
        Default_Width => 8);

    function Hexa_Image is new Integer_Hexa_Image (
        Element => Callmon.Integer_64,
        Default_Width => 8); -- P0 addresses are 32-bits only


    ----------------------------------------------------------------------------
    --
    --  This generic routine returns the "image" of an access value,
    --  in an hexadecimal string.
    --
    ----------------------------------------------------------------------------

    function Access_Hexa_Image (
        Item  : in Access_Element;
        Width : in Positive := Default_Width)
        return String is

        function To_Integer_32 is
        new Unchecked_Conversion (Access_Element, Callmon.Integer_32);

    begin

        return Hexa_Image (To_Integer_32 (Item), Width);

    end Access_Hexa_Image;


    ----------------------------------------------------------------------------
    --
    --  This function returns a string which uniformely identifies
    --  the current task in the application.
    --
    ----------------------------------------------------------------------------

    function Task_Name return String is

        package Integer_IO is new Text_IO.Integer_IO (Integer);

        Image : String (1..4);

    begin

        Integer_IO.Put (Image, Get_Task_Info.Get_Current_Task_Id);
        return "Task" & Image;

    end Task_Name;


    ----------------------------------------------------------------------------
    --
    --  This generic routine fetches an object at an address. The address
    --  if given by an integer type. If the address is null, return a
    --  default value.
    --
    ----------------------------------------------------------------------------

    function Fetch_From_Address (
        Location : in Address;
        Default  : in Target := Default_Value)
        return Target is

        function Fetch is new System.Fetch_From_Address (Target);

    begin

        if Location = 0 then
            return Default;
        else
            return Fetch (
                System.To_Address (System.Unsigned_Longword (Location)));
        end if;

    end Fetch_From_Address;


    ----------------------------------------------------------------------------
    --
    --  When a parameter is passed by reference, the Integer_64 in
    --  the argument list is the address of the actual parameter.
    --  This function fetches a 32-bits integer passed by reference.
    --
    ----------------------------------------------------------------------------

    function Fetch_Integer_32 is new Fetch_From_Address (
        Address       => Callmon.Integer_64,
        Target        => Callmon.Integer_32,
        Default_Value => 0);


    ----------------------------------------------------------------------------
    --
    --  Interception of ADA$CREATE_COLLECTION_2
    --
    ----------------------------------------------------------------------------
    --
    --
    --  FUNCTIONAL DESCRIPTION:
    -- 
    --     Called by the Ada compiler when it wishes to create 
    --     a collection corresponding to some access type, i.e.
    --     when the access type definition is elaborated.
    -- 
    --     This routine is passed a list header upon which the
    --     RTL chains collections created in the current frame 
    --     so they can be released at normal exit or exception
    --     propagation.
    -- 
    --     Bounds are passed for the size of objects in the collection.
    --     These must be correct.  It is desireable that they also
    --     be as tight as possible because they are used to choose the 
    --     allocation algorithms.
    -- 
    --     There are two outputs.  One output is that information about
    --     this new collection is inserted on the collection list of the
    --     scope.  Another output is the collection descriptor
    --     associated with the access type.   The collection descriptor is
    --     used as a parameter when other operations related to this access
    --     type are performed.
    -- 
    --     A limit on the size of the collection may be specified, and 
    --     an alignment requirement can be specified.
    -- 
    --     Ada source:
    -- 
    --         type SOME_TYPE is access OTHER_TYPE; 
    --         -- ADA$CREATE_COLLECTION
    -- 
    -- 
    --  FORMAL PARAMETERS:
    --
    --     MIN_SIZE (in)              Minimum size of any object of the 
    --                                type.  (0 to MAX_INT).
    --  
    --                                If negative, PROGRAM_ERROR is raised.
    --
    --     MAX_SIZE (in)              Maximum size of any object of the type
    --                                (0 to MAX_INT).
    -- 
    --                                NOTE: later attempting to allocate an
    --                                object whose size exceeds these limits
    --                                is erroneous and will be assumed to
    --                                indicate a corrupted program.
    -- 
    --                                If negative, PROGRAM_ERROR is raised.
    --                                If MAX_SIZE < MIN_SIZE, PROGRAM_ERROR
    --                                is raised.
    --
    --     COLLECTION_LIST (in out)   Address of the collection list header
    --                                for this scope.
    -- 
    --                                The collection list header is a longword
    --                                allocated in the context of the current
    --                                scope.  It must be initially nulled by
    --                                the compiler before ADA$HANDLER is 
    --                                established.
    --
    --     COLLECTION_DESC (out)      Address of the collection descriptor for
    --                                this collection. 
    --  
    --                                The collection descriptor is initialized
    --                                by this routine.  Collection descriptors
    --                                must never be copied and are always passed
    --                                by reference. (Internally, it consists of
    --                                a link to the next collection in the
    --                                scope, a longword zone, and other
    --                                information).
    --
    --  OPT_STORAGE_SIZE (in)         Address of a longword giving an optional 
    --                                restriction on the size of the collection.
    --                                The STORAGE_SIZE is given in bytes.
    -- 
    --                null address    => "no storage limit".
    --                Value of <= 0   => "no storage allocated".
    --
    --  OPT_ALIGNMENT (in)            Address of a longword giving an optional
    --                                byte alignment specification for objects
    --                                of the collection. This number must be
    --                                equal to 2**N for some N.  To illustrate,
    --                                the value 8 implies quadword alignment.
    -- 
    --      null address              => longword alignment.
    --      Value > 0 and Value < 4   => longword alignment.
    --      Value < 0 or  Value > 512 => PROGRAM_ERROR is raised
    --      Value not power-of-2      => PROGRAM_ERROR is raised
    --
    --
    --  IMPLICIT INPUTS:
    -- 
    --      NONE
    -- 
    --  IMPLICIT OUTPUTS:
    -- 
    --      NONE
    -- 
    --  ROUTINE VALUE:
    -- 
    --      NONE
    -- 
    --  SIDE EFFECTS:
    -- 
    --      CONSTRAINT_ERROR is raised if the specified collection size will
    --                       exceed MAX_INT when it is rounded up to the next
    --                       block.
    --      STORAGE_ERROR    is raised if a COLLECTION_SIZE was specified
    --                       and insufficient virtual space exists.
    --      PROGRAM_ERROR    is raised if parameters violate required limits.
    --
    --
    ----------------------------------------------------------------------------
    -- 
    --  The following procedure is a post-processing routine for
    --  ADA$CREATE_COLLECTION_2
    -- 
    ----------------------------------------------------------------------------

    procedure Post_Create_Collection_2 (
        Arguments           : in out Callmon.Arguments_Type;
        Caller              : in     Callmon.Invo_Handle_Type;
        Routine_Name        : in     String;
        Intercepted_Routine : in     Callmon.Address;
        Jacket_Routine      : in     Callmon.Address) is

        function Storage_Limit return String is
            Address : Callmon.Integer_64 := Arguments.Arg_List (5);
            Value   : Callmon.Integer_32 := Fetch_Integer_32 (Address);
        begin
            if Address = 0 then
                return "no storage limit";
            elsif Value < 0 then
                return "no storage";
            else
                return "storage limit:" & 
                   Callmon.Integer_32'Image (Value) & " bytes";
            end if;
        end Storage_Limit;

    begin

        Text_IO.Put_Line (Task_Name &
            ": [trace] Created collection " &
            Hexa_Image (Arguments.Arg_List (4)) &
            ", min size:" &
            Callmon.Integer_64'Image (Arguments.Arg_List (1)) &
            ", max:" &
            Callmon.Integer_64'Image (Arguments.Arg_List (2)) &
            " bytes" & Ascii.CR & Ascii.LF & "          List: " &
            Hexa_Image (Arguments.Arg_List (3)) &
            ", " & Storage_Limit & ", alignment:" &
            Callmon.Integer_32'Image (
                Fetch_Integer_32 (Arguments.Arg_List (6), Default => 4)) &
            " bytes");

    end Post_Create_Collection_2;

    pragma Export_Procedure (
        Internal  => Post_Create_Collection_2,
        External  => "",
        Mechanism => (Reference, Value, Descriptor (S), Value, Value));


    ----------------------------------------------------------------------------
    --
    --  Interception of ADA$DELETE_COLLECTIONS
    --
    ----------------------------------------------------------------------------
    -- 
    --
    --  FUNCTIONAL DESCRIPTION:
    --
    --     Called by the Ada compiler or Ada exception handler
    --     when a scope containing any collections is being left.
    --
    --     This routine releases the storage for all collections
    --     dependent on that scope.
    --
    --     Ada source:
    --
    --         end;
    --         -- ADA$DELETE_COLLECTIONS
    -- 
    --
    --  FORMAL PARAMETERS:
    --
    --     COLLECTION_LIST (in out)  Address of the list header describing all
    --                               collections in the scope.
    --
    --                               This longword will be nulled by this
    --                               routine. This indicates that the freeing
    --                               has been already been done when a second
    --                               cleanup is attempted on exception or
    --                               unwind.
    -- 
    --
    --  IMPLICIT INPUTS:
    -- 
    --     NONE
    -- 
    --  IMPLICIT OUTPUTS:
    -- 
    --     NONE
    -- 
    --  ROUTINE VALUE:
    -- 
    --     NONE
    -- 
    --  SIDE EFFECTS:
    -- 
    --     NONE
    -- 
    -- 
    ----------------------------------------------------------------------------
    -- 
    --  The following procedure is a pre-processing routine for
    --  ADA$DELETE_COLLECTIONS
    -- 
    ----------------------------------------------------------------------------

    procedure Pre_Delete_Collections (
        Arguments           : in out Callmon.Arguments_Type;
        Caller              : in     Callmon.Invo_Handle_Type;
        Routine_Name        : in     String;
        Intercepted_Routine : in     Callmon.Address;
        Jacket_Routine      : in     Callmon.Address) is

    begin

        Text_IO.Put_Line (Task_Name &
            ": [trace] Delete collection in list " &
            Hexa_Image (Arguments.Arg_List (1)));

    end Pre_Delete_Collections;

    pragma Export_Procedure (
        Internal  => Pre_Delete_Collections,
        External  => "",
        Mechanism => (Reference, Value, Descriptor (S), Value, Value));


    ----------------------------------------------------------------------------
    --
    --  Interception of ADA$ALLOCATE
    --
    ----------------------------------------------------------------------------
    --
    --  Description of ADA$ALLOCATE, module ADA$ACCESS, V6.2
    -- 
    --
    --  FUNCTIONAL DESCRIPTION:
    --
    --     Called by the Ada compiler when it wishes to allocate a 
    --     dynamic object to be referenced by an object of an access type.
    --
    --     This routine allocates some number of requested bytes of 
    --     storage for a dynamic object.  The address of the dynamic
    --     object is stored in the access object (a longword).
    --     The caller must pass in the address of the collection descriptor
    --     that is associated with the access type definition.
    --
    --     Ada source:
    --
    --        X := new SOME_TYPE 
    --        -- ADA$ALLOCATE    
    --
    --
    --  FORMAL PARAMETERS:
    --
    --     SIZE (in)             Size of the object to be allocated (bytes).
    --
    --     COLLECTION_DESC (in)  The address of the collection descriptor for
    --                           the collection which the object belongs to.
    --
    --
    --  IMPLICIT INPUTS:
    --
    --     NONE
    --
    --  IMPLICIT OUTPUTS:
    --
    --     NONE
    --
    --  ROUTINE VALUE:
    --
    --     ACCESS_VALUE, the address of the dynamic object that was created.
    --                   A null value may be returned if it is a collection
    --                   of null objects.
    --
    --  SIDE EFFECTS:
    --
    --     PROGRAM_ERROR is raised if the object size is out of bounds
    --                   of MIN_SIZE, MAX_SIZE for the collection.
    --                   It is also raised if internal data structures fail a
    --                   consistency check (out-of-bounds addressing by user
    --                   is assumed).
    --
    --     STORAGE_ERROR is raised if the collection size had a 
    --                   STORAGE_SIZE rep spec and the new object can't be 
    --                   allocated, or, there was no bound, but there is no 
    --                   virtual space available.
    --
    --
    ----------------------------------------------------------------------------
    -- 
    --  The following procedure is a post-processing routine for ADA$ALLOCATE.
    -- 
    ----------------------------------------------------------------------------

    procedure Post_Allocate (
        Arguments           : in out Callmon.Arguments_Type;
        Caller              : in     Callmon.Invo_Handle_Type;
        Routine_Name        : in     String;
        Intercepted_Routine : in     Callmon.Address;
        Jacket_Routine      : in     Callmon.Address) is

    begin

        Text_IO.Put_Line (Task_Name &
            ": [trace] Allocated" & 
            Callmon.Integer_64'Image (Arguments.Arg_List (1)) &
            " bytes at " &
            Hexa_Image (Arguments.Result_R0) &
            " in collection " &
            Hexa_Image (Arguments.Arg_List (2)));

    end Post_Allocate;

    pragma Export_Procedure (
        Internal  => Post_Allocate,
        External  => "",
        Mechanism => (Reference, Value, Descriptor (S), Value, Value));


    ----------------------------------------------------------------------------
    --
    --  Interception of ADA$DEALLOCATE
    --
    ----------------------------------------------------------------------------
    --
    --  Description of ADA$DEALLOCATE, module ADA$ACCESS, V6.2
    -- 
    --
    --  FUNCTIONAL DESCRIPTION:
    --
    --     Called by the Ada compiler when it wishes to deallocate
    --     an access object.
    --
    --     This routine releases the storage for the object to the
    --     collection, and sets the access object to null.
    --
    --     Ada source:
    --
    --        UNCHECKED_DEALLOCATION_FOR_TYPE_OF_X (X);
    --        -- ADA$DEALLOCATE
    --
    --  FORMAL PARAMETERS:
    --
    --     COLLECTION_DESC (in)    The address of the collection descriptor of
    --                             the collection which the object belongs to.
    --
    --     ACCESS_OBJECT (in out)  The address of the access object pointing
    --                             to the dynamic object.
    --
    --                             The access object is set to the null
    --                             access value by this routine.
    --
    --
    --  IMPLICIT INPUTS:
    --
    --     NONE
    --
    --  IMPLICIT OUTPUTS:
    --
    --     NONE
    --
    --  ROUTINE VALUE:
    --
    --     NONE
    --
    --  SIDE EFFECTS:
    --
    --     NONE
    --
    --
    ----------------------------------------------------------------------------
    -- 
    --  The following procedure is a pre-processing routine for ADA$DEALLOCATE.
    -- 
    ----------------------------------------------------------------------------

    procedure Pre_Deallocate (
        Arguments           : in out Callmon.Arguments_Type;
        Caller              : in     Callmon.Invo_Handle_Type;
        Routine_Name        : in     String;
        Intercepted_Routine : in     Callmon.Address;
        Jacket_Routine      : in     Callmon.Address) is

    begin

        Text_IO.Put_Line (Task_Name &
            ": [trace] Deallocate " &
            Hexa_Image (Fetch_Integer_32 (Arguments.Arg_List (2))) &
            " from collection " &
            Hexa_Image (Arguments.Arg_List (1)));

    end Pre_Deallocate;

    pragma Export_Procedure (
        Internal  => Pre_Deallocate,
        External  => "",
        Mechanism => (Reference, Value, Descriptor (S), Value, Value));


--------------------------------------------------------------------------------
--
--  Package elaboration.
--  Should be executed before the creation of any task.
--
--------------------------------------------------------------------------------

begin

    Callmon.Intercept ("ADA$CREATE_COLLECTION_2",
        Post_Routine => Post_Create_Collection_2'Address);

    Callmon.Intercept ("ADA$DELETE_COLLECTIONS",
        Pre_Routine => Pre_Delete_Collections'Address);

    Callmon.Intercept ("ADA$ALLOCATE",
        Post_Routine => Post_Allocate'Address);

    Callmon.Intercept ("ADA$DEALLOCATE",
        Pre_Routine => Pre_Deallocate'Address);

end Example_Collection;
