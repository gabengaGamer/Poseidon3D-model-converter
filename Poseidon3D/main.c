//=============================================================================
//
//  Program:        P3DConverter - DEMO
//
//  Credits:        https://community.bistudio.com/wiki/P3D_File_Format_-_MLOD
//
//  Author:         GameSpy
//
//  Date:           Started 16.01.2025
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "poseidon.h"

void ReadHeader(FILE *file, struct P3DHeader *p3dheader, struct WVRHeader *wvrheader)
{
    fread(&p3dheader->Signature, sizeof(p3dheader->Signature), 1, file);

    switch (p3dheader->Signature) 
    {
        case SP3X_SIGNATURE:    
            fread(&p3dheader->MajorVersion, sizeof(p3dheader->MajorVersion), 1, file);
            if (p3dheader->MajorVersion != MAJOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature...
            {
                fprintf(stderr, "Alert: Wrong Major version! (0x%X instead of 0x1C).\n", p3dheader->MajorVersion);
                exit(1);
            }
            fread(&p3dheader->MinorVersion, sizeof(p3dheader->MinorVersion), 1, file);
            if (p3dheader->MinorVersion != MINOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature... 
            {
                fprintf(stderr, "Alert: Wrong Minor version! (0x%X instead of 0x99).\n", p3dheader->MinorVersion);
                exit(1);
            }
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n",   p3dheader->Signature);
            printf("Debug: Major version: %d\n", p3dheader->MajorVersion);
            printf("Debug: Minor version: %d\n", p3dheader->MinorVersion);
            #endif
            break;
        case SP3D_SIGNATURE:
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n", p3dheader->Signature);
            #endif
            break;
        case WVR1_SIGNATURE:
            fread(&wvrheader->Xsize, sizeof(wvrheader->Xsize), 1, file);
            if (wvrheader->Xsize != 128) //In fact, this makes no sense, since the program successfully read the signature...
            {
                fprintf(stderr, "Alert: Wrong Xsize value! (%X instead of 128).\n", wvrheader->Xsize);
                exit(1);
            }
            fread(&wvrheader->Ysize, sizeof(wvrheader->Ysize), 1, file);
            if (wvrheader->Ysize != 128) //In fact, this makes no sense, since the program successfully read the signature... 
            {
                fprintf(stderr, "Alert: Wrong Ysize value! (%X instead of 128).\n", wvrheader->Ysize);
                exit(1);
            }
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n", p3dheader->Signature);
            printf("Debug: Xsize value: %d\n", wvrheader->Xsize);
            printf("Debug: Ysize value: %d\n", wvrheader->Ysize);
            #endif
            break;    
        default:
            fprintf(stderr, "Alert: Wrong signature! (0x%X).\n", p3dheader->Signature);    
            exit(1);
    }
}

//=============================================================================

void ReadP3DData(FILE *file, struct P3DData *p3ddata)
{
    fread(p3ddata, sizeof(struct P3DData), 1, file);

    #ifdef _DEBUG          
    printf("Debug: nPoints: %d\n",  p3ddata->nPoints);
    printf("Debug: nNormals: %d\n", p3ddata->nFaceNormals);
    printf("Debug: nFaces: %d\n",   p3ddata->nFaces);
    #endif
}

//=============================================================================

void ReadP3DPoints(FILE *file, struct P3DPoint *p3dpoint, struct P3DHeader *p3dheader, int vertex_cnt)
{
    int i;
    
    switch (p3dheader->Signature) 
    {
        case SP3X_SIGNATURE:   
            fseek(file, 4, SEEK_CUR); //Skip unused data
            fread(p3dpoint, sizeof(struct P3DPoint), vertex_cnt, file);
            #ifdef _DEBUG
            for (i = 0; i < vertex_cnt; i++) 
            {
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, p3dpoint[i].position.XYZ[0], p3dpoint[i].position.XYZ[1], p3dpoint[i].position.XYZ[2]);
            }
            #endif
            break;
        case SP3D_SIGNATURE:
            for (i = 0; i < vertex_cnt; i++)
            {
                fread(&p3dpoint[i].position, sizeof(p3dpoint[i].position), 1, file);
                #ifdef _DEBUG
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, p3dpoint[i].position.XYZ[0], p3dpoint[i].position.XYZ[1], p3dpoint[i].position.XYZ[2]);
                #endif
            }      
            break;
        default:
            exit(1);
    }
}

//=============================================================================

void ReadP3DFaceNormals(FILE *file, struct P3DTriplet *p3dtriplet, int fnormals_cnt)
{
    #ifdef _DEBUG
    int i;  
    #endif    

    fread(p3dtriplet, sizeof(struct P3DTriplet), fnormals_cnt, file);

    #ifdef _DEBUG
    for (i = 0; i < fnormals_cnt; i++) 
    {
        printf("Debug: Triplet [%d]: Position=(%f, %f, %f)\n", i, p3dtriplet[i].XYZ[0], p3dtriplet[i].XYZ[1], p3dtriplet[i].XYZ[2]);
    }
    #endif
}

//=============================================================================

void ReadP3DLodFaces(FILE *file, struct P3DLodFace *p3dlodface, struct P3DHeader *p3dheader, int lodfaces_cnt)
{
    int i;

    for (i = 0; i < lodfaces_cnt; i++) 
    {
        int j;
        fread(&p3dlodface[i].TextureName, sizeof(p3dlodface[i].TextureName), 1, file);
        fread(&p3dlodface[i].FaceType, sizeof(p3dlodface[i].FaceType), 1, file);

        for (j = 0; j < 4; j++) 
        {
            fread(&p3dlodface[i].P3DVertexTables[j], sizeof(struct P3DVertexTable), 1, file);
        }
        if (p3dheader->Signature != SP3D_SIGNATURE)
        {
            fread(&p3dlodface[i].FaceFlags, sizeof(p3dlodface[i].FaceFlags), 1, file);
            #ifdef _DEBUG
            printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d, FaceFlags=%d\n", i, p3dlodface[i].TextureName, p3dlodface[i].FaceType, p3dlodface[i].FaceFlags);
            #endif
        }
        #ifdef _DEBUG
        else
        {
           printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d\n", i, p3dlodface[i].TextureName, p3dlodface[i].FaceType);    
        }
        for (j = 0; j < 4; j++) 
        {
            printf("  VertexTable [%d]: PointsIndex=%d, NormalsIndex=%d, U=%f, V=%f\n",
                   j,
                   p3dlodface[i].P3DVertexTables[j].PointsIndex,
                   p3dlodface[i].P3DVertexTables[j].NormalsIndex,
                   p3dlodface[i].P3DVertexTables[j].U,
                   p3dlodface[i].P3DVertexTables[j].V);
        }
        #endif
    }
}

//=================================Optional===================================

void ReadP3DSupplement(FILE *file, struct P3DSupplement *p3dsupply) //Optional block of code, but I still prefer to handle it.
{
    #ifdef _DEBUG
    int i;  
    #endif    
    int totalBools;
    int totalIndexes;

    fread(&p3dsupply->Signature, sizeof(int), 1, file);
    if (p3dsupply->Signature != SS3D_SIGNATURE)
    {
        fprintf(stderr, "Alert: Wrong signature! (0x%X instead of 'SS3D').\n", p3dsupply->Signature);
        exit(1);
    }

    fread(&p3dsupply->nPoints, sizeof(int), 1, file);
    fread(&p3dsupply->nFaces, sizeof(int), 1, file);
    fread(&p3dsupply->nNormals, sizeof(int), 1, file);
    fread(&p3dsupply->nBytes, sizeof(int), 1, file);

    totalBools = p3dsupply->nPoints + p3dsupply->nFaces + p3dsupply->nNormals; //Check poseidon.h !!!
    p3dsupply->TinyBools = (unsigned char *)malloc(totalBools * sizeof(unsigned char));
    fread(p3dsupply->TinyBools, sizeof(unsigned char), totalBools, file);

    totalIndexes = p3dsupply->nBytes / 4; //Check poseidon.h !!!
    p3dsupply->Indexes = (int *)malloc(totalIndexes * sizeof(int));
    fread(p3dsupply->Indexes, sizeof(int), totalIndexes, file);

    #ifdef _DEBUG
    printf("Debug: Signature: 0x%X\n", p3dsupply->Signature);
    printf("Debug: nPoints: %d\n",     p3dsupply->nPoints);
    printf("Debug: nFaces: %d\n",      p3dsupply->nFaces);
    printf("Debug: nNormals: %d\n",    p3dsupply->nNormals);
    printf("Debug: nBytes: %d\n",      p3dsupply->nBytes);
    for (i = 0; i < totalBools; i++)
    {
        printf("Debug: TinyBools[%d]: %d\n", i, p3dsupply->TinyBools[i]);
    }
    for (i = 0; i < totalIndexes; i++)
    {
        printf("Debug: Indexes[%d]: %d\n", i, p3dsupply->Indexes[i]);
    }
    #endif
}

//=================================Optional=END===============================

void WriteOBJFile(FILE *f_out, struct P3DPoint *points, int nPoints, struct P3DTriplet *normals, int nNormals, struct P3DLodFace *faces, int nFaces) 
{
    int i;
    int voffs = 1;
    
    for (i = 0; i < nPoints; i++) 
    {
        fprintf(f_out, "v %f %f %f\n", points[i].position.XYZ[0], points[i].position.XYZ[1], points[i].position.XYZ[2]);
    }

    for (i = 0; i < nNormals; i++) 
    {
        fprintf(f_out, "vn %f %f %f\n", normals[i].XYZ[0], normals[i].XYZ[1], normals[i].XYZ[2]);
    }
    
    for (i = 0; i < nFaces; i++) 
    {
        int j;
        int facetype = (faces[i].FaceType == 3) ? 3 : 4; //Skip fourth block if we don't need it. Check FaceType!!!
        
        for (j = 0; j < facetype; j++) 
        {
            fprintf(f_out, "vt %f %f\n", faces[i].P3DVertexTables[j].U, faces[i].P3DVertexTables[j].V);
        }
    }
    
    for (i = 0; i < nFaces; i++) 
    {
        int j;
        int facetype = (faces[i].FaceType == 3) ? 3 : 4; //Skip fourth block if we don't need it. Check FaceType!!!

        fprintf(f_out, "f");
        for (j = 0; j < facetype; j++) 
        {
            fprintf(f_out, " %d/%d/%d", 
                faces[i].P3DVertexTables[j].PointsIndex+voffs, 
                faces[i].P3DVertexTables[j].PointsIndex+voffs, 
                faces[i].P3DVertexTables[j].NormalsIndex+voffs
            );
        }
        fprintf(f_out, "\n");
    }
}

//=============================================================================

void UnloadData(struct P3DPoint *p3dpoint, struct P3DTriplet *p3dtriplet, struct P3DLodFace *p3dlodface, struct P3DSupplement p3dsupply, struct WVRModel *wvrmodel)
{
    if (p3dpoint) free(p3dpoint);
    if (p3dtriplet) free(p3dtriplet);
    if (p3dlodface) free(p3dlodface);
    
    if (wvrmodel) free(wvrmodel);

    if (p3dsupply.TinyBools) free(p3dsupply.TinyBools);
    if (p3dsupply.Indexes) free(p3dsupply.Indexes);
}

//=============================================================================

void ReadWVRTexture(FILE *file, struct WVRTexture *wvrtexture)
{
    int i;
    fread(wvrtexture->Elevations, sizeof(wvrtexture->Elevations), 1, file);
    fread(wvrtexture->TextureIndex, sizeof(wvrtexture->TextureIndex), 1, file);
    fread(wvrtexture->TextureName, sizeof(wvrtexture->TextureName), 1, file);

#ifdef _DEBUG
    for (i = 0; i < 256; i++)
    {
        printf("Debug: Texture[%d]: %s\n", i, wvrtexture->TextureName[i]);
    }
#endif    
}

//=============================================================================

void ReadWVRModels(FILE *file, struct WVRModel *wvrmodel)
{
    int i;
    fread(wvrmodel, sizeof(struct WVRModel), 2233, file);

#ifdef _DEBUG
    for (i = 0; i < 2233; i++)
    {
        printf("Debug: Model[%d]: Position=(%f, %f, %f), Heading=%f, ModelName=%s\n",
               i,
               wvrmodel[i].position.XYZ[0], wvrmodel[i].position.XYZ[1], wvrmodel[i].position.XYZ[2],
               wvrmodel[i].Heading,
               wvrmodel[i].ModelName);
    }
#endif
   //0x34E4C
}

//=============================================================================

void ReadWVRNet(FILE *file, struct WVRNet *wvrnet, struct WVRSubNet *wvrsubnet)
{
    #ifdef _DEBUG
    int nNets;
    int nSubNets;
    #endif

    while (1)
    {
        fread(&wvrnet->NetHeader, sizeof(struct WVRNetHeader), 1, file);

        if (strcmp(wvrnet->NetHeader.NetName, "EndOfNets") == 0)
        {
            break;
        }

        #ifdef _DEBUG
        printf("Debug: Net[%d]:\n", nNets);
        printf("  Texture: %s\n", wvrnet->NetHeader.NetName);
        printf("  Type: %ld\n", wvrnet->NetHeader.Type);
        printf("  Position: (%f, %f, %f)\n", wvrnet->NetHeader.position.XYZ[0], wvrnet->NetHeader.position.XYZ[1], wvrnet->NetHeader.position.XYZ[2]);
        printf("  Scale: %f\n", wvrnet->NetHeader.Scale);
        #endif

        while (1)
        {
            fread(&wvrsubnet, sizeof(struct WVRSubNet), 1, file);

            if (wvrsubnet->X == 0.0 && wvrsubnet->Y == 0.0)
            {
                break;
            }

            #ifdef _DEBUG
            printf("    SubNet[%d]: X=%f, Y=%f\n", nSubNets, wvrsubnet->X, wvrsubnet->Y);
            printf("      Position: (%f, %f, %f)\n", 
                          wvrsubnet->OptionalData.position.XYZ[0],
                          wvrsubnet->OptionalData.position.XYZ[1],
                          wvrsubnet->OptionalData.position.XYZ[2]);
                          nSubNets++;
            #endif
        }
        #ifdef _DEBUG
        nSubNets = 0;
        nNets++;
        #endif
    }
}

//=============================================================================

int main(int argc, char *argv[])   
{
    const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    struct P3DHeader     p3dheader;
    struct P3DData       p3ddata;
    struct P3DSupplement p3dsupply;

    struct P3DPoint   *p3dpoint;
    struct P3DTriplet *p3dtriplet;
    struct P3DLodFace *p3dlodface;
    
    struct WVRHeader    wvrheader;
    struct WVRTexture   wvrtexture;
    struct WVRNet       wvrnet;
    struct WVRSubNet    wvrsubnet;
    
    struct WVRModel *wvrmodel;
    
    input_file = argv[1];
    output_file = "output.obj";

    if (argc < 2) 
    {
        printf("Info: Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    f_in = fopen(input_file, "rb");
    if (!f_in) 
    {
        fprintf(stderr, "Alert: Error loading <%s!\n", argv[1]);
        return 1;
    }

    f_out = fopen(output_file, "w");
    if (!f_out) 
    {
        fprintf(stderr, "Alert: Error loading output file!\n");
        fclose(f_in);
        return 1;
    }

    ReadHeader(f_in, &p3dheader, &wvrheader);

    if (p3dheader.Signature != WVR1_SIGNATURE )
    {
        ReadP3DData(f_in, &p3ddata);
    
        p3dpoint = (struct P3DPoint *)malloc(p3ddata.nPoints * sizeof(struct P3DPoint));
        p3dtriplet = (struct P3DTriplet *)malloc(p3ddata.nFaceNormals * sizeof(struct P3DTriplet));
        p3dlodface = (struct P3DLodFace *)malloc(p3ddata.nFaces * sizeof(struct P3DLodFace));

        ReadP3DPoints(f_in, p3dpoint, &p3dheader, p3ddata.nPoints);    
        ReadP3DFaceNormals(f_in, p3dtriplet, p3ddata.nFaceNormals);
        ReadP3DLodFaces(f_in, p3dlodface, &p3dheader, p3ddata.nFaces);  
    
        if (p3dheader.Signature == SP3D_SIGNATURE || p3dheader.Signature == SP3X_SIGNATURE)
        {
            ReadP3DSupplement(f_in, &p3dsupply); 
        }
        
        WriteOBJFile(f_out, p3dpoint, p3ddata.nPoints, p3dtriplet, p3ddata.nFaceNormals, p3dlodface, p3ddata.nFaces);
    }
    else
    {
        ReadWVRTexture(f_in, &wvrtexture);
    
        wvrmodel = (struct WVRModel *)malloc(2233 * sizeof(struct WVRModel));
    
        ReadWVRModels(f_in, wvrmodel);
        ReadWVRNet(f_in, &wvrnet, &wvrsubnet);    
    }
    
    UnloadData(p3dpoint, p3dtriplet, p3dlodface, p3dsupply, wvrmodel);
    
    fclose(f_in);
    fclose(f_out);
    return 0;
}