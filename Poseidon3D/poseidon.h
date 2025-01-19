#ifndef POSEIDON_H
#define POSEIDON_H

//=============================================================================
// DATA SIGNS
//=============================================================================

#define SP3X_SIGNATURE 0x58335053
#define SP3D_SIGNATURE 0x44335053
#define SS3D_SIGNATURE 0x44335353
#define MAJOR_VERSION  0x0000001C
#define MINOR_VERSION  0x00000099

//=============================================================================
// FACE FLAGS
//=============================================================================

#define DISABLE_SHADOW 0x00000010 //Disable shadow

#define DISABLE_TMERGING 0x01000000 //Disable texture merging

#define ZBIAS_NONE   0x00000000 //None Z-bias
#define ZBIAS_LOW    0x00000100 //Low Z-Bias
#define ZBIAS_MIDDLE 0x00000200 //Middle Z-Bias
#define ZBIAS_HIGH   0x00000300 //High Z-Bias  

#define LIGHTING_BOTH     0x00000020 //Both sides render
#define LIGHTING_POSITION 0x00000080 //Position render
#define LIGHTING_FLAT     0x00200000 //Flat render
#define LIGHTING_REVERSED 0x00100000 //Reversed (transparent) render

#define USER_MVALUE 0xfe000000 //User additional mark value (0..127)

//=============================================================================
// PROTOTYPING
//=============================================================================


//=============================================================================
// EXTERNING
//=============================================================================

    
//=============================================================================
// STRUCTURIZING
//=============================================================================

struct P3DHeader
{
    int Signature;    //"SP3X" or "P3DM" or "SP3D" (demo)
    int MajorVersion; //28 (x1C)
    int MinorVersion; //0x99 (SP3X) or 0x100 (P3DM)
};

//=============================================================================

struct P3DData
{
    int nPoints;
    int nFaceNormals; //Perpendicular
    int nFaces;
};

//=============================================================================

struct P3DSupplement //Optional
{
    int Signature;            //Always "SS3D" and only for "SP3X"
    int nPoints;              //Identically to P3DData "nPoints"
    int nFaces;               //Identically to P3DData "nFaces"
    int nNormals;             //Identically to P3DData "nFaceNormals"
    int nBytes;               //Always "0"
    unsigned char *TinyBools; //Formula: nPoints+nFaces+nNormals | usualy contain 0 or 1 values
    int *Indexes;             //Formula: nBytes/4
};

//=============================================================================

struct P3DTriplet
{
    float XYZ[3];
};

//=============================================================================

struct P3DPoint
{
	struct P3DTriplet position;
    //float XYZ[3];
    int   PointFlags; //-> check FACE FLAGS
};

//=============================================================================

struct P3DVertexTable
{
    int PointsIndex;
    int NormalsIndex;
    float U,V;
};

//=============================================================================

struct P3DLodFace
{
    char   TextureName[32];
    int    FaceType;                          // 3 - Triangle or 4 - Quad
    struct P3DVertexTable P3DVertexTables[4];
    int    FaceFlags;                         //-> check FACE FLAGS
};

//=============================================================================

#endif // POSEIDON_H