--  CALLMON - A Call Monitor for OpenVMS Alpha
--
--  File:     CALLMON_.ADA
--  Author:   Thierry Lelegard
--  Version:  1.0
--  Date:     24-JUL-1996
--
--  Abstract: Public Interface of CALLMON
--


with Lib;
with System;
with Condition_Handling;

package Callmon is

    subtype Cond_Value_Type is Condition_Handling.Cond_Value_Type;

    subtype Address is System.Address;

    subtype Integer_8  is System.Integer_8;
    subtype Integer_16 is System.Integer_16;
    subtype Integer_32 is System.Integer_32;
    subtype Integer_64 is System.Integer_64;

    subtype Invo_Handle_Type is Lib.Invo_Handle_Type;

    --
    -- Procedure argument list.
    -- According to OpenVMS Alpha Calling Standard, an argument list
    -- is made of 0 to 255 64-bits arguments.
    --

    subtype Argument_Count is Natural range 0..255;
    subtype Argument_Index is Argument_Count range 1..Argument_Count'Last;

    type Argument_List_Type is array (Argument_Index range <>) of Integer_64;
    subtype Maximum_Argument_List_Type is Argument_List_Type (Argument_Index);

    --
    -- Argument to pre-processing and post-processing procedure.
    --

    type Arguments_Type is
    record
        Post_Processing : Boolean;
        Call_It         : Boolean;
        Result_R0       : Integer_64;
        Result_R1       : Integer_64;
        Result_F0       : Integer_64;
        Result_F1       : Integer_64;
        Arg_Count       : Integer_64;
        Arg_List        : Maximum_Argument_List_Type;
    end record;

    for Arguments_Type use
    record
        Post_Processing at  0 range 0 .. 0;
        Call_It         at  4 range 0 .. 0;
        Result_R0       at  8 range 0 .. 63;
        Result_R1       at 16 range 0 .. 63;
        Result_F0       at 24 range 0 .. 63;
        Result_F1       at 32 range 0 .. 63;
        Arg_Count       at 40 range 0 .. 63;
        Arg_List        at 48 range 0 .. 255 * 64 - 1;
    end record;

    for Arguments_Type'Size use 261 * 64;

    --
    --  CALLMON Initialization (optional, automatically done otherwise)
    --

    procedure Initialize (Status : out Cond_Value_Type);
    procedure Initialize;

    pragma Interface (C, Initialize);

    pragma Import_Valued_Procedure (
        Internal        => Initialize,
        External        => "CALLMON$INITIALIZE",
        Parameter_Types => (Cond_Value_Type),
        Mechanism       => (Value));

    pragma Import_Procedure (
        Internal        => Initialize,
        External        => "CALLMON$INITIALIZE",
        Parameter_Types => (null));

    --
    --  Intercept a routine.
    --

    procedure Intercept (
        Status       : out Cond_Value_Type;
        Routine_Name : in  String;
        Pre_Routine  : in  Address := System.Null_Address;
        Post_Routine : in  Address := System.Null_Address);

    procedure Intercept (
        Routine_Name : in  String;
        Pre_Routine  : in  Address := System.Null_Address;
        Post_Routine : in  Address := System.Null_Address);

    pragma Interface (C, Intercept);

    pragma Import_Valued_Procedure (
        Internal        => Intercept,
        External        => "CALLMON$INTERCEPT_DSC",
        Parameter_Types => (Cond_Value_Type, String, Address, Address),
        Mechanism       => (Value, Descriptor (S), Value, Value));

    pragma Import_Procedure (
        Internal        => Intercept,
        External        => "CALLMON$INTERCEPT_DSC",
        Parameter_Types => (String, Address, Address),
        Mechanism       => (Descriptor (S), Value, Value));

    --
    -- Declaring a pre-/post-processing routine:
    --
    -- procedure Interceptor (
    --     Arguments           : in out Callmon.Arguments_Type;
    --     Caller              : in     Callmon.Invo_Handle_Type;
    --     Routine_Name        : in     String;
    --     Intercepted_Routine : in     Callmon.Address;
    --     Jacket_Routine      : in     Callmon.Address);
    --
    -- pragma Export_Procedure (
    --     Internal  => Interceptor,
    --     External  => "",
    --     Mechanism => (Reference, Value, Descriptor (S), Value, Value));
    --

    --
    -- Intercept all routines in an image.
    --

    procedure Intercept_All (
        Status       : out Cond_Value_Type;
        Image_Name   : in  String;
        Pre_Routine  : in  Address := System.Null_Address;
        Post_Routine : in  Address := System.Null_Address;
        Override     : in  Boolean := True);

    procedure Intercept_All (
        Image_Name   : in  String;
        Pre_Routine  : in  Address := System.Null_Address;
        Post_Routine : in  Address := System.Null_Address;
        Override     : in  Boolean := True);

    pragma Interface (C, Intercept_All);

    pragma Import_Valued_Procedure (
        Internal        => Intercept_All,
        External        => "CALLMON$INTERCEPT_ALL_DSC",
        Parameter_Types => (Cond_Value_Type, String, Address, Address, Boolean),
        Mechanism       => (Value, Descriptor (S), Value, Value, Value));

    pragma Import_Procedure (
        Internal        => Intercept_All,
        External        => "CALLMON$INTERCEPT_ALL_DSC",
        Parameter_Types => (String, Address, Address, Boolean),
        Mechanism       => (Descriptor (S), Value, Value, Value));

    --
    -- Disassemble a range of instructions.
    --

    procedure Disassemble (
        Start_Address  : in Address;
        Size_In_Bytes  : in Natural;
        Displayed_PC   : in Address;
        Output_Routine : in Address := System.Null_Address;
        User_Data      : in Address := System.Null_Address);

    pragma Interface (C, Disassemble);

    pragma Import_Procedure (
        Internal        => Disassemble,
        External        => "CALLMON$DISASSEMBLE_DSC",
        Parameter_Types => (Address, Natural, Address, Address, Address),
        Mechanism       => (Value, Value, Value, Value, Value));

    --
    -- Declaring an output routine for the disassembler.
    --
    -- procedure Output (
    --     PC        : in Callmon.Address;
    --     Opcode    : in String;
    --     Operands  : in String;
    --     User_Data : in Callmon.Address);
    --
    -- pragma Export_Procedure (
    --     Internal  => Output,
    --     External  => "",
    --     Mechanism => (Value, Descriptor (S), Descriptor (S), Value));
    --

    --
    -- Dumping the list of active images.
    --

    procedure Dump_Image_Tree (Status : out Cond_Value_Type);
    procedure Dump_Image_Tree;

    pragma Interface (C, Dump_Image_Tree);

    pragma Import_Valued_Procedure (
        Internal        => Dump_Image_Tree,
        External        => "CALLMON$DUMP_IMAGE_TREE",
        Parameter_Types => (Cond_Value_Type),
        Mechanism       => (Value));

    pragma Import_Procedure (
        Internal        => Dump_Image_Tree,
        External        => "CALLMON$DUMP_IMAGE_TREE",
        Parameter_Types => (null));


    --
    -- Dumping the list of known routines.
    --

    type Dump_Routine_Type is (
        All_Routines,
        Intercepted_Only,
        Uninterceptable_Only,
        Unrelocatable_Only);

    for Dump_Routine_Type use (
        All_Routines         => 0,
        Intercepted_Only     => 1,
        Uninterceptable_Only => 2,
        Unrelocatable_Only   => 3);

    procedure Dump_Routine_Tree (
        Status : out Cond_Value_Type;
        Class  : in  Dump_Routine_Type);

    procedure Dump_Routine_Tree (
        Class  : in  Dump_Routine_Type);

    pragma Interface (C, Dump_Routine_Tree);

    pragma Import_Valued_Procedure (
        Internal        => Dump_Routine_Tree,
        External        => "CALLMON$DUMP_ROUTINE_TREE",
        Parameter_Types => (Cond_Value_Type, Dump_Routine_Type),
        Mechanism       => (Value, Value));

    pragma Import_Procedure (
        Internal        => Dump_Routine_Tree,
        External        => "CALLMON$DUMP_ROUTINE_TREE",
        Parameter_Types => (Dump_Routine_Type),
        Mechanism       => (Value));

end Callmon;
