//=============================================================================
//
//  Module:         P3DConverter - Poseidon engine 1997
//
//  Credits:        https://community.bistudio.com/wiki/P3D_File_Format_-_MLOD
//
//  Author:         GameSpy
//
//  Date:           Started 03.02.2025
//
//=============================================================================
// P3D - 3D model format
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "poseidon.h"

void ReadP3DData(FILE *file, struct P3D *p3d)
{
    fread(&p3d->data, sizeof(struct P3DData), 1, file);

    #ifdef _DEBUG          
    printf("Debug: nPoints: %d\n",  p3d->data.nPoints);
    printf("Debug: nNormals: %d\n", p3d->data.nFaceNormals);
    printf("Debug: nFaces: %d\n",   p3d->data.nFaces);
    #endif
}

//=============================================================================

void ReadP3DPoints(FILE *file, struct P3D *p3d, struct RVHeader *rvh)
{
    int i;

    p3d->point = (struct P3DPoint *)malloc(p3d->data.nPoints * sizeof(struct P3DPoint));

    switch (rvh->Signature)  
    {
        case SP3X_SIGNATURE:   
            fseek(file, 4, SEEK_CUR); //Skip unused data
            fread(p3d->point, sizeof(struct P3DPoint), p3d->data.nPoints, file);
            #ifdef _DEBUG
            for (i = 0; i < p3d->data.nPoints; i++)
            {
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, 
                       p3d->point[i].position.XYZ[0], 
                       p3d->point[i].position.XYZ[1], 
                       p3d->point[i].position.XYZ[2]);
            }
            #endif
            break;
        case SP3D_SIGNATURE:
            for (i = 0; i < p3d->data.nPoints; i++)
            {
                fread(&p3d->point[i].position, sizeof(p3d->point[i].position), 1, file);
                #ifdef _DEBUG
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, 
                       p3d->point[i].position.XYZ[0], 
                       p3d->point[i].position.XYZ[1], 
                       p3d->point[i].position.XYZ[2]);
                #endif
            }      
            break;
        default:
            exit(1);
    }
}

//=============================================================================

void ReadP3DFaceNormals(FILE *file, struct P3D *p3d)
{
    #ifdef _DEBUG
    int i;  
    #endif   

    p3d->triplet = (struct P3DTriplet *)malloc(p3d->data.nFaceNormals * sizeof(struct P3DTriplet));    
    fread(p3d->triplet, sizeof(struct P3DTriplet), p3d->data.nFaceNormals, file);

    #ifdef _DEBUG
    for (i = 0; i < p3d->data.nFaceNormals; i++) 
    {
        printf("Debug: Triplet [%d]: Position=(%f, %f, %f)\n", i, 
               p3d->triplet[i].XYZ[0], 
               p3d->triplet[i].XYZ[1], 
               p3d->triplet[i].XYZ[2]);
    }
    #endif
}

//=============================================================================

void ReadP3DLodFaces(FILE *file, struct P3D *p3d, struct RVHeader *rvh)
{
    int i,j;

    p3d->lodface = (struct P3DLodFace *)malloc(p3d->data.nFaces * sizeof(struct P3DLodFace));

    for (i = 0; i < p3d->data.nFaces; i++)  
    {
        fread(&p3d->lodface[i].TextureName, sizeof(p3d->lodface[i].TextureName), 1, file);
        fread(&p3d->lodface[i].FaceType, sizeof(p3d->lodface[i].FaceType), 1, file);
        for (j = 0; j < 4; j++) 
        {
            fread(&p3d->lodface[i].p3dvertextable[j], sizeof(struct P3DVertexTable), 1, file);
        }
        if (rvh->Signature != SP3D_SIGNATURE)
        {
            fread(&p3d->lodface[i].FaceFlags, sizeof(p3d->lodface[i].FaceFlags), 1, file);
            #ifdef _DEBUG
            printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d, FaceFlags=%d\n", i, 
                   p3d->lodface[i].TextureName, 
                   p3d->lodface[i].FaceType, 
                   p3d->lodface[i].FaceFlags);
            #endif
        }
        #ifdef _DEBUG
        else
        {
           printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d\n", i, 
                   p3d->lodface[i].TextureName, 
                   p3d->lodface[i].FaceType);    
        }
        for (j = 0; j < 4; j++) 
        {
            printf("  VertexTable [%d]: PointsIndex=%d, NormalsIndex=%d, U=%f, V=%f\n",
                   j,
                   p3d->lodface[i].p3dvertextable[j].PointsIndex,
                   p3d->lodface[i].p3dvertextable[j].NormalsIndex,
                   p3d->lodface[i].p3dvertextable[j].U,
                   p3d->lodface[i].p3dvertextable[j].V);
        }
        #endif
    }
}

//=================================Optional===================================

void ReadP3DSupplement(FILE *file, struct P3D *p3d, struct RVHeader *rvh)
{
    #ifdef _DEBUG
    int i;  
    #endif    
    int totalBools = 0;
    int totalIndexes = 0;

    fread(&rvh->Signature, sizeof(int), 1, file);
    if (rvh->Signature != SS3D_SIGNATURE)
    {
        fprintf(stderr, "Alert: Wrong signature! (0x%X instead of 'SS3D').\n", rvh->Signature);
        exit(1);
    }

    fread(&p3d->supply.nPoints, sizeof(int), 1, file);
    fread(&p3d->supply.nFaces, sizeof(int), 1, file);
    fread(&p3d->supply.nNormals, sizeof(int), 1, file);
    fread(&p3d->supply.nBytes, sizeof(int), 1, file);

    totalBools = p3d->supply.nPoints + p3d->supply.nFaces + p3d->supply.nNormals; //Check poseidon.h !!!
    p3d->supply.TinyBools = (unsigned char *)malloc(totalBools * sizeof(unsigned char));
    fread(p3d->supply.TinyBools, sizeof(unsigned char), totalBools, file);

    totalIndexes = p3d->supply.nBytes / 4; //Check poseidon.h !!!
    p3d->supply.Indexes = (int *)malloc(totalIndexes * sizeof(int));
    fread(p3d->supply.Indexes, sizeof(int), totalIndexes, file);

    #ifdef _DEBUG
    printf("Debug: Signature: 0x%X\n", p3d->supply.Signature);
    printf("Debug: nPoints: %d\n",     p3d->supply.nPoints);
    printf("Debug: nFaces: %d\n",      p3d->supply.nFaces);
    printf("Debug: nNormals: %d\n",    p3d->supply.nNormals);
    printf("Debug: nBytes: %d\n",      p3d->supply.nBytes);
    for (i = 0; i < totalBools; i++)
    {
        printf("Debug: TinyBools[%d]: %d\n", i, p3d->supply.TinyBools[i]);
    }
    for (i = 0; i < totalIndexes; i++)
    {
        printf("Debug: Indexes[%d]: %d\n", i, p3d->supply.Indexes[i]);
    }
    #endif
}

//=================================Optional=END===============================
// WRP/WVR - World map format
//============================================================================

void ReadWVRTexture(FILE *file, struct WVR *wvr)
{
    #ifdef _DEBUG
    int i;  
    #endif  

    fread(wvr->texture.Elevations, sizeof(wvr->texture.Elevations), 1, file);
    fread(wvr->texture.TextureIndex, sizeof(wvr->texture.TextureIndex), 1, file);
    fread(wvr->texture.TextureName, sizeof(wvr->texture.TextureName), 1, file);

    #ifdef _DEBUG
    for (i = 0; i < 256; i++)
    {
        printf("Debug: Texture[%d]: %s\n", i, wvr->texture.TextureName[i]);
    }
    #endif    
}

//=============================================================================

void ReadWVRModels(FILE *file, struct WVR *wvr)
{
    #ifdef _DEBUG
    int i;  
    #endif  
    
    wvr->model = (struct WVRModel *)malloc(2233 * sizeof(struct WVRModel));
    fread(wvr->model, sizeof(struct WVRModel), 2233, file);

    #ifdef _DEBUG
    for (i = 0; i < 2233; i++)
    {
        printf("Debug: Model[%d]: Position=(%f, %f, %f), Heading=%f, ModelName=%s\n",
               i,
               wvr->model[i].position.XYZ[0], 
               wvr->model[i].position.XYZ[1], 
               wvr->model[i].position.XYZ[2],
               wvr->model[i].Heading,
               wvr->model[i].ModelName);
    }
    #endif
   //0x34E4C
}

//=============================================================================

void ReadWVRNet(FILE *file, struct WVR *wvr)
{
    int nNets = 0;
    int nSubNets = 0;
    
    wvr->net.subnet = (struct WVRSubNet *)malloc(nSubNets * sizeof(struct WVRSubNet));

    while (1)
    {
        fread(&wvr->net.netheader, sizeof(struct WVRNetHeader), 1, file); 

        if (strcmp(wvr->net.netheader.NetName, "EndOfNets") == 0)
        {
            break;
        }
        
        #ifdef _DEBUG
        printf("Debug: Net[%d]:\n", nNets);
        printf("  Texture: %s\n", wvr->net.netheader.NetName);
        printf("  Type: %ld\n", wvr->net.netheader.Type);
        printf("  Position: (%f, %f, %f)\n", wvr->net.netheader.position.XYZ[0], 
                                             wvr->net.netheader.position.XYZ[1], 
                                             wvr->net.netheader.position.XYZ[2]);
        printf("  Scale: %f\n", wvr->net.netheader.Scale);
        #endif

        while (1)
        {
            fread(&wvr->subnet, sizeof(struct WVRSubNet), 1, file);

            if (wvr->subnet.X == 0.0 && wvr->subnet.Y == 0.0)
            {
                break;
            }

            #ifdef _DEBUG
            printf("    SubNet[%d]: X=%f, Y=%f\n", nSubNets, wvr->subnet.X, wvr->subnet.Y);
            printf("      Position: (%f, %f, %f)\n", 
                          wvr->subnet.OptionalData.position.XYZ[0],
                          wvr->subnet.OptionalData.position.XYZ[1],
                          wvr->subnet.OptionalData.position.XYZ[2]);
            nSubNets++;
            #endif
        }
        nNets++;
    }
}