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

//========================Arma 1========================

//=============================================================================
// RVHeader - Real Virtuality header (master)
//=============================================================================

struct RVHeader //All headers should reference to RVHeader, for a better and easier code experience.
{
    int Signature;
    int Unknown;
    int Unknown1;
};

#endif //HEADER_H