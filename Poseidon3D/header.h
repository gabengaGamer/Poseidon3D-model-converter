#ifndef HEADER_H
#define HEADER_H

//=============================================================================
// DATA SIGNS
//=============================================================================

//========================Poseidon3D Demo========================

#define SP3X_SIGNATURE 0x58335053
#define SP3D_SIGNATURE 0x44335053
#define SS3D_SIGNATURE 0x44335353
#define WVR1_SIGNATURE 0x52565731

#define MAJOR_VERSION  0x0000001C
#define MINOR_VERSION  0x00000099

//========================Real Virtuality========================

#define ODOL_SIGNATURE 0x4C4F444F

#define VERSION_07 0x00000007 // Arra 1
#define VERSION_40 0x00000028 // Arra Armed Assault
#define VERSION_43 0x0000002B // Arra 2a
#define VERSION_47 0x0000002F // Arra 2b LZO
#define VERSION_48 0x00000030 // Arra 2c
#define VERSION_49 0x00000031 // Arra Arrowhead
#define VERSION_50 0x00000032 // Arra Arrowhead DLC
#define VERSION_52 0x00000034 // TOH
#define VERSION_53 0x00000035 // DayZ
#define VERSION_54 0x00000036 // DayZ
#define VERSION_56 0x00000038 // Arra 3
#define VERSION_58 0x0000003A // Arra 3
#define VERSION_59 0x0000003B // Arra 3
#define VERSION_60 0x0000003C // Arra 3
#define VERSION_64 0x00000040 // Arra 3
#define VERSION_67 0x00000043 // Arra 3
#define VERSION_68 0x00000044 // Arra 3
#define VERSION_69 0x00000045 // Arra 3
#define VERSION_70 0x00000046 // Arra 3
#define VERSION_71 0x00000047 // Arra 3
#define VERSION_72 0x00000048 // Arra 3
#define VERSION_73 0x00000049 // Arra 3
#define VERSION_75 0x0000004B // Arra 3

//=============================================================================
// RVHeader - Real Virtuality header (master)
//=============================================================================

struct RVHeader //All headers should reference to RVHeader, for a better and easier code experience.
{
    int Signature;
    int Unknown;
    int Unknown1;
    int Unknown2;
    int Unknown3; //Maybe useless
};

#endif //HEADER_H