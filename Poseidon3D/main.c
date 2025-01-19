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
#include "poseidon.h"

void ReadP3DHeader(FILE *file, struct P3DHeader *header)
{
    fread(&header->Signature, sizeof(header->Signature), 1, file);

    switch (header->Signature) 
    {
        case SP3X_SIGNATURE:    
            fread(&header->MajorVersion, sizeof(header->MajorVersion), 1, file);
            if (header->MajorVersion != MAJOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature...
            {
                fprintf(stderr, "Alert: Wrong Major version! (0x%X instead of 0x1C).\n", header->MajorVersion);
                exit(1);
            }
            fread(&header->MinorVersion, sizeof(header->MinorVersion), 1, file);
            if (header->MinorVersion != MINOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature... 
            {
                fprintf(stderr, "Alert: Wrong Minor version! (0x%X instead of 0x99).\n", header->MinorVersion);
                exit(1);
            }
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n",   header->Signature);
            printf("Debug: Major version: %d\n", header->MajorVersion);
            printf("Debug: Minor version: %d\n", header->MinorVersion);
            #endif
            break;
        case SP3D_SIGNATURE:
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n", header->Signature);
            #endif
            break;
        default:
            fprintf(stderr, "Alert: Wrong signature! (0x%X).\n", header->Signature);	
            exit(1);
    }
}

//=============================================================================

void ReadP3DData(FILE *file, struct P3DData *data)
{
    fread(data, sizeof(struct P3DData), 1, file);

    #ifdef _DEBUG          
    printf("Debug: nPoints: %d\n",  data->nPoints);
    printf("Debug: nNormals: %d\n", data->nFaceNormals);
    printf("Debug: nFaces: %d\n",   data->nFaces);
    #endif
}

//=============================================================================

void ReadP3DPoints(FILE *file, struct P3DPoint *point, struct P3DHeader *header, int vertex_cnt)
{
    int i;
    
    switch (header->Signature) 
    {
        case SP3X_SIGNATURE:   
            fseek(file, 4, SEEK_CUR); //Skip unused data
            fread(point, sizeof(struct P3DPoint), vertex_cnt, file);
            #ifdef _DEBUG
            for (i = 0; i < vertex_cnt; i++) 
            {
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, point[i].position.XYZ[0], point[i].position.XYZ[1], point[i].position.XYZ[2]);
            }
            #endif
            break;
        case SP3D_SIGNATURE:
            for (i = 0; i < vertex_cnt; i++)
            {
                fread(&point[i].position, sizeof(point[i].position), 1, file);
                #ifdef _DEBUG
                printf("Debug: Point [%d]: Position=(%f, %f, %f)\n", i, point[i].position.XYZ[0], point[i].position.XYZ[1], point[i].position.XYZ[2]);
                #endif
            }      
            break;
        default:
            exit(1);
    }
}

//=============================================================================

void ReadP3DFaceNormals(FILE *file, struct P3DTriplet *triplet, int fnormals_cnt)
{
    #ifdef _DEBUG
    int i;  
    #endif    

    fread(triplet, sizeof(struct P3DTriplet), fnormals_cnt, file);

    #ifdef _DEBUG
    for (i = 0; i < fnormals_cnt; i++) 
    {
        printf("Debug: Triplet [%d]: Position=(%f, %f, %f)\n", i, triplet[i].XYZ[0], triplet[i].XYZ[1], triplet[i].XYZ[2]);
    }
    #endif
}

//=============================================================================

void ReadP3DLodFaces(FILE *file, struct P3DLodFace *lodFace, struct P3DHeader *header, int lodfaces_cnt)
{
    int i;

    for (i = 0; i < lodfaces_cnt; i++) 
    {
        int j;
        fread(&lodFace[i].TextureName, sizeof(lodFace[i].TextureName), 1, file);
        fread(&lodFace[i].FaceType, sizeof(lodFace[i].FaceType), 1, file);

        for (j = 0; j < 4; j++) 
        {
            fread(&lodFace[i].P3DVertexTables[j], sizeof(struct P3DVertexTable), 1, file);
        }
        if (header->Signature != SP3D_SIGNATURE)
        {
            fread(&lodFace[i].FaceFlags, sizeof(lodFace[i].FaceFlags), 1, file);
			#ifdef _DEBUG
            printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d, FaceFlags=%d\n", i, lodFace[i].TextureName, lodFace[i].FaceType, lodFace[i].FaceFlags);
			#endif
		}
		#ifdef _DEBUG
		else
		{
		   printf("Debug: LodFace [%d]: TextureName=%s, FaceType=%d\n", i, lodFace[i].TextureName, lodFace[i].FaceType);	
		}
        for (j = 0; j < 4; j++) 
        {
            printf("  VertexTable %d: PointsIndex=%d, NormalsIndex=%d, U=%f, V=%f\n",
                   j,
                   lodFace[i].P3DVertexTables[j].PointsIndex,
                   lodFace[i].P3DVertexTables[j].NormalsIndex,
                   lodFace[i].P3DVertexTables[j].U,
                   lodFace[i].P3DVertexTables[j].V);
        }
        #endif
    }
}

//=================================Optional===================================

void ReadP3DSupplement(FILE *file, struct P3DSupplement *supply) //Optional block of code, but I still prefer to handle it.
{
    #ifdef _DEBUG
    int i;  
    #endif    
    int totalBools;
    int totalIndexes;

    fread(&supply->Signature, sizeof(int), 1, file);
    if (supply->Signature != SS3D_SIGNATURE)
    {
        fprintf(stderr, "Alert: Wrong signature! (0x%X instead of 'SS3D').\n", supply->Signature);
        exit(1);
    }

    fread(&supply->nPoints, sizeof(int), 1, file);
    fread(&supply->nFaces, sizeof(int), 1, file);
    fread(&supply->nNormals, sizeof(int), 1, file);
    fread(&supply->nBytes, sizeof(int), 1, file);

    totalBools = supply->nPoints + supply->nFaces + supply->nNormals; //Check poseidon.h !!!
    supply->TinyBools = (unsigned char *)malloc(totalBools * sizeof(unsigned char));
    fread(supply->TinyBools, sizeof(unsigned char), totalBools, file);

    totalIndexes = supply->nBytes / 4; //Check poseidon.h !!!
    supply->Indexes = (int *)malloc(totalIndexes * sizeof(int));
    fread(supply->Indexes, sizeof(int), totalIndexes, file);

    #ifdef _DEBUG
    printf("Debug: Signature: 0x%X\n", supply->Signature);
    printf("Debug: nPoints: %d\n",     supply->nPoints);
    printf("Debug: nFaces: %d\n",      supply->nFaces);
    printf("Debug: nNormals: %d\n",    supply->nNormals);
    printf("Debug: nBytes: %d\n",      supply->nBytes);
    for (i = 0; i < totalBools; i++)
    {
        printf("Debug: TinyBools[%d]: %d\n", i, supply->TinyBools[i]);
    }
    for (i = 0; i < totalIndexes; i++)
    {
        printf("Debug: Indexes[%d]: %d\n", i, supply->Indexes[i]);
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

void UnloadData(struct P3DPoint *point, struct P3DTriplet *triplet, struct P3DLodFace *lodFace, struct P3DSupplement supply)
{
    if (point) free(point);
    if (triplet) free(triplet);
    if (lodFace) free(lodFace);

    if (supply.TinyBools) free(supply.TinyBools);
    if (supply.Indexes) free(supply.Indexes);
}

//=============================================================================

int main(int argc, char *argv[])   
{
    const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    struct P3DHeader     header;
    struct P3DData       data;
    struct P3DSupplement supply;

    struct P3DPoint   *point;
    struct P3DTriplet *triplet;
    struct P3DLodFace *lodFace;
    
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

    ReadP3DHeader(f_in, &header);
    ReadP3DData(f_in, &data);
    
    point = (struct P3DPoint *)malloc(data.nPoints * sizeof(struct P3DPoint));
    triplet = (struct P3DTriplet *)malloc(data.nFaceNormals * sizeof(struct P3DTriplet));
    lodFace = (struct P3DLodFace *)malloc(data.nFaces * sizeof(struct P3DLodFace));

    ReadP3DPoints(f_in, point, &header, data.nPoints);    
    ReadP3DFaceNormals(f_in, triplet, data.nFaceNormals);
    ReadP3DLodFaces(f_in, lodFace, &header, data.nFaces);  
    
    if (header.Signature == SP3D_SIGNATURE || header.Signature == SP3X_SIGNATURE)
    {
        ReadP3DSupplement(f_in, &supply); 
    }
    
    WriteOBJFile(f_out, point, data.nPoints, triplet, data.nFaceNormals, lodFace, data.nFaces);

    UnloadData(point, triplet, lodFace, supply);
    
    fclose(f_in);
    fclose(f_out);
    return 0;
}