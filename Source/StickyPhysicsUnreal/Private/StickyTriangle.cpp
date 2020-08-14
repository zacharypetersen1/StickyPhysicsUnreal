// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyTriangle.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"

FStickyTriangle::FStickyTriangle()
{
}

FStickyTriangle::FStickyTriangle(FVector VertexPositionA, FVector VertexPositionB, FVector VertexPositionC, const USceneComponent* SetOwningComponent)
{
	VertexPositions.Add(VertexPositionA);
	VertexPositions.Add(VertexPositionB);
	VertexPositions.Add(VertexPositionC);
	OwningComponent = SetOwningComponent;
}

FVector FStickyTriangle::GetFaceNormal()
{
	return FaceNormal;
}

FVector FStickyTriangle::GetInterpolatedNormal(const FVector &AtThisLocation)
{
	FVector BaryCentricCoords = FMath::ComputeBaryCentric2D(
		AtThisLocation,
		VertexPositions[0],
		VertexPositions[1],
		VertexPositions[2]
	);
	return VertexNormals[0] * BaryCentricCoords[0] +
		VertexNormals[1] * BaryCentricCoords[0] +
		VertexNormals[2] * BaryCentricCoords[0];
}

FVector FStickyTriangle::ProjectDirectionOntoTriangle(const FVector &Direction)
{
	return FVector::VectorPlaneProject(Direction, FaceNormal);
}

FVector FStickyTriangle::ProjectLocationOntoTriangle(const FVector &Location)
{
	return FVector::VectorPlaneProject(Location - VertexPositions[0], FaceNormal) + VertexPositions[0];
}

bool FStickyTriangle::ProjectRayOntoBoundaries(FVector &ResultLocation, const FRay &Ray)
{
	return false;
}

void FStickyTriangle::DebugDraw(const UWorld* World, const FColor &Color)
{
	for (int i = 0; i < 3; i++)
	{
		FTransform OwnerTransform = OwningComponent->GetComponentTransform();
		DrawDebugLine(World, 
			OwnerTransform.GetLocation() + OwnerTransform.TransformVector(VertexPositions[i]), 
			OwnerTransform.GetLocation() + OwnerTransform.TransformVector(VertexPositions[(i + 1) % 3]),
			Color, false, -1.0f, 0, 5.0f);
	}
}
