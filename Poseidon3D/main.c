//=============================================================================
//
//  Program:        P3DConverter
//
//  Credits:        https://community.bistudio.com/
//
//  Author:         GameSpy
//
//  Date:           Started 16.01.2025
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "module/poseidon.h"

void ReadHeader(FILE *file, struct RVHeader *rvh)
{
    fread(&rvh->Signature, sizeof(rvh->Signature), 1, file);

    switch (rvh->Signature) 
    {
        case SP3X_SIGNATURE:    
            fread(&rvh->Unknown, sizeof(rvh->Unknown), 1, file);
            if (rvh->Unknown != MAJOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature...
            {
                fprintf(stderr, "Alert: Wrong Major version! (0x%X instead of 0x1C).\n", rvh->Unknown);
                exit(1);
            }
            fread(&rvh->Unknown1, sizeof(rvh->Unknown1), 1, file);
            if (rvh->Unknown1 != MINOR_VERSION) //In fact, this makes no sense, since the program successfully read the signature... 
            {
                fprintf(stderr, "Alert: Wrong Minor version! (0x%X instead of 0x99).\n", rvh->Unknown1);
                exit(1);
            }
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n",   rvh->Signature);
            printf("Debug: Major version: %d\n", rvh->Unknown);
            printf("Debug: Minor version: %d\n", rvh->Unknown1);
            #endif
            break;
        case SP3D_SIGNATURE:
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n", rvh->Signature);
            #endif
            break;
        case WVR1_SIGNATURE:
            fread(&rvh->Unknown, sizeof(rvh->Unknown), 1, file);
            if (rvh->Unknown != 128) //In fact, this makes no sense, since the program successfully read the signature...
            {
                fprintf(stderr, "Alert: Wrong Xsize value! (%X instead of 128).\n", rvh->Unknown);
                exit(1);
            }
            fread(&rvh->Unknown1, sizeof(rvh->Unknown1), 1, file);
            if (rvh->Unknown1 != 128) //In fact, this makes no sense, since the program successfully read the signature... 
            {
                fprintf(stderr, "Alert: Wrong Ysize value! (%X instead of 128).\n", rvh->Unknown1);
                exit(1);
            }
            #ifdef _DEBUG
            printf("Debug: Signature: 0x%X\n", rvh->Signature);
            printf("Debug: Xsize value: %d\n", rvh->Unknown);
            printf("Debug: Ysize value: %d\n", rvh->Unknown1);
            #endif
            break;    
        default:
            fprintf(stderr, "Alert: Wrong signature! (0x%X).\n", rvh->Signature);    
            exit(1);
    }
}

//=============================================================================

void WriteOBJFile(FILE *f_out, struct P3D *p3d) 
{
    int i,j;
    int voffs = 1;

    for (i = 0; i < p3d->data.nPoints; i++) 
    {
        fprintf(f_out, "v %f %f %f\n", p3d->point[i].position.XYZ[0], 
                                       p3d->point[i].position.XYZ[1], 
                                       p3d->point[i].position.XYZ[2]);
    }

    for (i = 0; i < p3d->data.nFaceNormals; i++) 
    {
        fprintf(f_out, "vn %f %f %f\n", p3d->triplet[i].XYZ[0], 
                                        p3d->triplet[i].XYZ[1], 
                                        p3d->triplet[i].XYZ[2]);
    }

    for (i = 0; i < p3d->data.nFaces; i++) 
    {       
        int facetype = (p3d->lodface[i].FaceType == 3) ? 3 : 4; //Skip fourth block if we don't need it. Check FaceType!!!     
        for (j = 0; j < facetype; j++) 
        {
            fprintf(f_out, "vt %f %f\n", p3d->lodface[i].p3dvertextable[j].U, 
                                         p3d->lodface[i].p3dvertextable[j].V);
        }
    }

    for (i = 0; i < p3d->data.nFaces; i++)
    {
        int facetype = (p3d->lodface[i].FaceType == 3) ? 3 : 4; //Skip fourth block if we don't need it. Check FaceType!!!  
        fprintf(f_out, "f");
        for (j = 0; j < facetype; j++) 
        {
            fprintf(f_out, " %d/%d/%d", 
                p3d->lodface[i].p3dvertextable[j].PointsIndex+voffs, 
                p3d->lodface[i].p3dvertextable[j].PointsIndex+voffs, 
                p3d->lodface[i].p3dvertextable[j].NormalsIndex+voffs
            );
        }
        fprintf(f_out, "\n");
    }
}

//=============================================================================

void InitData(struct P3D *p3d, struct WVR *wvr)
{
    p3d->point = NULL;
    p3d->triplet = NULL;
    p3d->lodface = NULL;

    p3d->supply.TinyBools = NULL;
    p3d->supply.Indexes = NULL;

    wvr->model = NULL;
    wvr->net.subnet = NULL;
}

//=============================================================================

void UnloadData(struct P3D *p3d, struct WVR *wvr)
{
    if (p3d->point)   { free(p3d->point);   p3d->point = NULL; }
    if (p3d->triplet) { free(p3d->triplet); p3d->triplet = NULL; }
    if (p3d->lodface) { free(p3d->lodface); p3d->lodface = NULL; }

    if (p3d->supply.TinyBools) { free(p3d->supply.TinyBools); p3d->supply.TinyBools = NULL; }
    if (p3d->supply.Indexes)   { free(p3d->supply.Indexes);   p3d->supply.Indexes = NULL; }

    if (wvr->model)      { free(wvr->model);      wvr->model = NULL; }
    if (wvr->net.subnet) { free(wvr->net.subnet); wvr->net.subnet = NULL; }
}

//=============================================================================

int main(int argc, char *argv[])   
{
    const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;
    
    struct RVHeader rvh;
    
    struct P3D p3d;
    struct WVR wvr;
    
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

    InitData(&p3d, &wvr);
    ReadHeader(f_in, &rvh);

    if (rvh.Signature != WVR1_SIGNATURE)
    {
        ReadP3DData(f_in, &p3d);
        ReadP3DPoints(f_in, &p3d, &rvh);
        ReadP3DFaceNormals(f_in, &p3d);
        ReadP3DLodFaces(f_in, &p3d, &rvh);
        
        if (rvh.Signature == SP3D_SIGNATURE || rvh.Signature == SP3X_SIGNATURE)
        {
            ReadP3DSupplement(f_in, &p3d, &rvh);
        }
        
        WriteOBJFile(f_out, &p3d);
    }
    else
    {
        ReadWVRTexture(f_in, &wvr);
        ReadWVRModels(f_in, &wvr);
        ReadWVRNet(f_in, &wvr);
    }

    UnloadData(&p3d, &wvr);
    
    fclose(f_in);
    fclose(f_out);
    return 0;
}