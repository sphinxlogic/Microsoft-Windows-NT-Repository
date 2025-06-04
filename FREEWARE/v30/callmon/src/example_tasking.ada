--  CALLMON Examples
--
--  File:     EXAMPLE_TASKING.ADA
--  Author:   Thierry Lelegard
--  Version:  1.0
--  Date:     24-JUL-1996
--
--  Abstract: Example program using Ada tasking.
--


with Text_IO;
with Unchecked_Deallocation;
with Example_Collection;

pragma Elaborate (Example_Collection);

procedure Example_Tasking is

    --
    -- This task type exercises collections.
    --

    task type Tester_Task is
        entry Initialize (Count : in Positive);
    end Tester_Task;

    --
    -- Creates several collection exercisers.
    --

    Testers : array (1..5) of Tester_Task;

    --
    -- This procedure illustrates the creation and global deallocation
    -- of a collection.
    --

    generic
        type Element is private;
    procedure Collection_Life (Count : in Positive);

    --
    -- Various kinds of types to exercise.
    --

    subtype Type_12 is String (1..12);
    subtype Small_Index is Positive range 1..10;
    subtype Large_Index is Positive range 1..1000;

    type Small_Variant_Type (Size : Small_Index := 1) is
    record
        Fixed    : String (1..1000);
        Variable : String (1..Size);
    end record;

    type Large_Variant_Type (Size : Large_Index := 1) is
    record
        Fixed    : String (1..1000);
        Variable : String (1..Size);
    end record;

    --
    -- Generic body of Collection_Life.
    --

    procedure Collection_Life (Count : in Positive) is
    begin

        -- Write header information

        if Element'Constrained then
            Text_IO.Put_Line (Example_Collection.Task_Name & 
                ": [appli] Starting collection life, object size is" &
                Integer'Image (Element'Size / 8) & " bytes");
        else
            Text_IO.Put_Line (Example_Collection.Task_Name & 
                ": [appli] Starting collection life, object is unconstrained");
        end if;

        -- Birth and death of a collection...

        declare

            type Access_Element is access Element;

            procedure Free is new Unchecked_Deallocation
                (Element, Access_Element);

            function Hexa_Image is new Example_Collection.Access_Hexa_Image
                (Element, Access_Element, Default_Width => 8);

            Pointer : Access_Element;

        begin

            -- Allocate many elements

            for N in 1..Count loop
                Pointer := new Element;
                Text_IO.Put_Line (Example_Collection.Task_Name & 
                    ": [appli] Allocated at " & Hexa_Image (Pointer));
                delay 0.2;
            end loop;

            -- Deallocate last one only

            Free (Pointer);
        end;

        -- Write trailer information

        Text_IO.Put_Line (Example_Collection.Task_Name & 
            ": [appli] After collection life");

    end Collection_Life;

    --
    -- Generate collections for various kinds of types
    --

    procedure Collection_Life_12 is new Collection_Life (Type_12);
    procedure Collection_Life_Small is new Collection_Life (Small_Variant_Type);
    procedure Collection_Life_Large is new Collection_Life (Large_Variant_Type);

    --
    -- This task type exercises collections.
    --

    task body Tester_Task is
        This_Count : Positive;
    begin
        accept Initialize (Count : in Positive) do
            This_Count := Count;
        end Initialize;
        Collection_Life_12 (This_Count);
        Collection_Life_Small (This_Count);
        Collection_Life_Large (This_Count);
    end Tester_Task;

--
-- Main program
--

begin
    for N in Testers'Range loop
        Testers (N).Initialize (N);
    end loop;
end Example_Tasking;
