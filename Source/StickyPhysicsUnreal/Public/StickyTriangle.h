// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class STICKYPHYSICSUNREAL_API FStickyTriangle
{
private:
	const USceneComponent* OwningComponent;	// Component that owns Triangle
	TArray<FVector> VertexNormals;
	FVector FaceNormal;
public:
	TArray<FVector> VertexPositions;
	FStickyTriangle();
	FStickyTriangle(FVector VertexPositionA, FVector VertexPositionB, FVector VertexPositionC, const USceneComponent* SetOwningTransform);
	FVector GetFaceNormal();
	FVector GetInterpolatedNormal(const FVector &AtThisLocation);
	FVector ProjectDirectionOntoTriangle(const FVector &Direction);
	FVector ProjectLocationOntoTriangle(const FVector &Location);
	bool ProjectRayOntoBoundaries(FVector &ResultLocation, const FRay &Ray);
	void DebugDraw(const UWorld* World, const FColor &Color);
};
