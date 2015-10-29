#!/bin/bash
#generate the accelerator simulation modules
mono AccGen.exe -path:../../src/modules/LCAcc ./*.type

#example generate the benchmark modules
mono ApGen.exe VectorAddSample.txt VectorAddSampleLCacc
cp VectorAddSampleLCacc.h ../Benchmarks/TDLCA_VectorAddSample/

#generate benchmark modules that use the accelerators
mono ApGen.exe Deblur1Mega.txt Deblur1LCacc
mono ApGen.exe Deblur2Mega.txt Deblur2LCacc
mono ApGen.exe Blur1Mega.txt Blur1LCacc
mono ApGen.exe Blur2Mega.txt Blur2LCacc
mono ApGen.exe Blur3Mega.txt Blur3LCacc
mono ApGen.exe Denoise1Mega.txt Denoise1LCacc
mono ApGen.exe Denoise2Mega.txt Denoise2LCacc
mono ApGen.exe Registration1Mega.txt Registration1LCacc
mono ApGen.exe Segmentation1Mega.txt Segmentation1LCacc
mono ApGen.exe BlackScholes.txt BlackScholesLCacc
mono ApGen.exe Swaptions1.txt Swaptions1LCacc 
mono ApGen.exe Swaptions2.txt Swaptions2LCacc 
mono ApGen.exe Swaptions3.txt Swaptions3LCacc 
mono ApGen.exe Swaptions4.txt Swaptions4LCacc 
mono ApGen.exe StreamCluster1.txt StreamCluster1LCacc
mono ApGen.exe StreamCluster3.txt StreamCluster3LCacc
mono ApGen.exe StreamCluster4.txt StreamCluster4LCacc
mono ApGen.exe StreamCluster5.txt StreamCluster5LCacc
mono ApGen.exe StreamCluster6.txt StreamCluster6LCacc
mono ApGen.exe LPCIP.txt LPCIPLCacc
mono ApGen.exe TexSynth1.txt TexSynth1LCacc
mono ApGen.exe TexSynth2.txt TexSynth2LCacc
mono ApGen.exe RobLoc.txt RobLocLCacc
mono ApGen.exe DispMapCompSAD.txt DispMapCompSADLCacc
mono ApGen.exe DispMapIntegSum1.txt DispMapIntegSum1LCacc
mono ApGen.exe DispMapIntegSum2.txt DispMapIntegSum2LCacc
mono ApGen.exe DispMapFindDisp.txt DispMapFindDispLCacc
mono ApGen.exe Jacobians.txt JacobiansLCacc
mono ApGen.exe SphericalCoords.txt SphericalCoordsLCacc

#copy benchmark modules into the directory
cp Blur*LCacc.h ../Benchmarks/TDLCA_Deblur_Modified/
cp Denoise1LCacc.h ../Benchmarks/TDLCA_Deblur_Modified/
cp Deblur1LCacc.h ../Benchmarks/TDLCA_Deblur_Modified/
cp Deblur2LCacc.h ../Benchmarks/TDLCA_Deblur_Modified/
cp Denoise1LCacc.h ../Benchmarks/TDLCA_Denoise/Denoise1LCacc.h
cp Denoise2LCacc.h ../Benchmarks/TDLCA_Denoise/Denoise2LCacc.h
cp Blur*LCacc.h ../Benchmarks/TDLCA_Registration_Modified/
cp Registration1LCacc.h ../Benchmarks/TDLCA_Registration_Modified/Registration1LCacc.h
cp Segmentation1LCacc.h ../Benchmarks/TDLCA_Segmentation/Segmentation1LCacc.h
cp BlackScholesLCacc.h ../Benchmarks/TDLCA_BlackScholes/
cp StreamCluster*LCacc.h ../Benchmarks/TDLCA_StreamCluster/
cp Swaptions*LCacc.h ../Benchmarks/TDLCA_Swaptions/
cp LPCIPLCacc.h ../Benchmarks/TDLCA_LPCIP_Desc/
cp TexSynth1LCacc.h ../Benchmarks/TDLCA_Texture_Synthesis/
cp TexSynth2LCacc.h ../Benchmarks/TDLCA_Texture_Synthesis/
cp RobLocLCacc.h ../Benchmarks/TDLCA_Robot_Localization/
cp DispMapCompSADLCacc.h ../Benchmarks/TDLCA_Disparity_Map/
cp DispMapFindDispLCacc.h ../Benchmarks/TDLCA_Disparity_Map/
cp DispMapIntegSum1LCacc.h ../Benchmarks/TDLCA_Disparity_Map/
cp DispMapIntegSum2LCacc.h ../Benchmarks/TDLCA_Disparity_Map/
cp JacobiansLCacc.h ../Benchmarks/TDLCA_EKF_SLAM/
cp SphericalCoordsLCacc.h ../Benchmarks/TDLCA_EKF_SLAM/
