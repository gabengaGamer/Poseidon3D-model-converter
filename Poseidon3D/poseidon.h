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

#define WVR1_SIGNATURE 0x52565731

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
// P3D - 3D model format
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
    int Signature;   //Always "SS3D" and only for "SP3X"
    int nPoints;     //Identically to P3DData "nPoints"
    int nFaces;      //Identically to P3DData "nFaces"
    int nNormals;    //Identically to P3DData "nFaceNormals"
    int nBytes;      //Always "0"
    char *TinyBools; //Formula: nPoints+nFaces+nNormals | usualy contain 0 or 1 values
    int *Indexes;    //Formula: nBytes/4
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
// WRP/WVR - world map format
//=============================================================================

struct WVRHeader
{
    int Signature; //"1WVR"
    int Xsize;     //(=128) cell dimension (wvr4 is 256)
    int Ysize;     //(=128) cell dimension
};

//=============================================================================

struct WVRTexture
{
    short Elevations[128][128];   // in centimetres. see 4WVR documentation
    short TextureIndex[128][128]; // Each 'index' refers to a filename below. Range 0..255 4WVR is 1..511
    char TextureName[256][32];    //"LandText\\mo.pac\0LandText.pi.pac.........."
};

//=============================================================================

struct WVRModel //64 byte structure
{
    struct     P3DTriplet position; // in gridsize scale. eg *50.0 meters
    float      Heading;             // in Cartesian degrees NOT polar radians 
    char       ModelName[48];       // "Data3d\Smrk.p3d (WVR4 is 76)
};

//=============================================================================

struct WVRNetHeader
{
    char     NetName[24];         //"LandText\Silnice.pac" null termed
    long     Unknown;             //0x00cd9100 or 0x00d4c600
    long     Unknown1;            //0x00bfd400
    long     Unknown2;            //0x00000047
    long     Unknown3;            //0x00000000
    long     Unknown4;            //0x0069fbb0;
    long     Type;                //0,1 or 2
    struct   P3DTriplet position; //[0.152,0.15,0.1] typical
    float    Scale;               //3.5, 4.5 or 5.5
};

//=============================================================================

struct WVRSubNet
{
    float X,Y;                      // Grid size of 50
    struct
    {
        struct P3DTriplet position; // Very similar content to header triplet
        float Stepping;
        unsigned long Unknown;      // 0x0046931A
        unsigned long Unknown1;     // 0x00980778 or 0x733760
    } OptionalData;                 // Included only if X || Y
};

//=============================================================================

struct WVRNet
{
    struct WVRNetHeader NetHeader;
    struct WVRSubNet SubNets[];
};

#endif // POSEIDON_H

/*
//=============================================================================
// GRAVEYARD
//=============================================================================

struct WVRTexture
{
    short **Elevations;         // in centimetres. see 4WVR documentation
    short **TextureIndex;       // Each 'index' refers to a filename below. Range 0..255 4WVR is 1..511
    char TextureName[256][32];    //"LandText\\mo.pac\0LandText.pi.pac.........."
};
*/